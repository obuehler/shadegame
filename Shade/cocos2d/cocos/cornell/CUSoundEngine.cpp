//
//  CUSoundEngine.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a robust sound engine for Cocos2d applications.  It is built on top of
//  the new experimental AudioEngine, making it a lot more user friendly.  It provides all of
//  the functionality of SimpleAudioEngine except pan and pitch control.  As there is no true
//  cross platform support for either of these (they are usually ignored on non-Apple platforms)
//  this should not be a problem.
//
//  IMPORTANT: It is absolutely crucial on iOS that all sounds have exactly the same format.
//  The same file format, the same sampling rate, the same number of channels.  Any change in
//  format requires a reconfiguration of the mixer graph, and this can cause clipping and/or
//  distortion for rapid-fire sound effects.
//
//  Author: Walker White
//  Version: 12/10/15
//
#include <cocos2d.h>
#include <random>
#include "CUSoundEngine.h"
#include "CUSound.h"
#include <audio/include/SimpleAudioEngine.h>


NS_CC_BEGIN

/** The built-in key for the active music */
#define MUSIC_KEY   "__MUSIC__"

/** Reference to the sound engine singleton */
SoundEngine* SoundEngine::_gEngine = nullptr;


#pragma mark -
#pragma mark Static Accessors

/**
 * Starts the singleton sound engine.
 *
 * Once this method is called, the method getInstance() will no longer return
 * null.  Calling the method multiple times (without calling stop) will have
 * no effect.
 */
void SoundEngine::start() {
    if (_gEngine != nullptr) {
        return;
    }
    _gEngine = new SoundEngine();
    if (!_gEngine->init()) {
        delete _gEngine;
        _gEngine = nullptr;
        CCASSERT(false,"Sound engine failed to initialize");
    }
}

/**
 * Starts the singleton sound, releasing all resources.
 *
 * Once this method is called, the method getInstance() will return null.
 * Calling the method multiple times (without calling stop) will have no effect.
 */
void SoundEngine::stop() {
    if (_gEngine == nullptr) {
        return;
    }
    delete _gEngine;
    _gEngine = nullptr;
}


#pragma mark -
#pragma mark Allocation
/**
 * Initializes the sound engine.
 *
 * This method starts up the experimental AudioEngine and creates the custom
 * profiles for this implementation.
 *
 * @return true if the sound engine was successfully initialized.
 */
bool SoundEngine::init() {
    if (AENG::lazyInit()) {
        _musicProfile = new experimental::AudioProfile();
        _musicProfile->name = "MUSIC_PROFILE";
        _musicProfile->maxInstances = 2;
        
        _effectProfile = new experimental::AudioProfile();
        _effectProfile->name = "EFFECT_PROFILE";
        _effectProfile->maxInstances = AENG::getMaxAudioInstance()-2;
        
        _musicData = new SoundPacket(MUSIC_KEY,nullptr,false,1.0f);
        
        return true;
    }
    return false;
}

/**
 * Releases all resources for this singleton sound engine.
 *
 * If you need to use the engine again, you must call init().
 */
void SoundEngine::dispose() {
    // Clear the queues
    stopAll();

    delete _musicData;
    delete _musicProfile;
    delete _effectProfile;
    
    for(auto it = _effectData.begin(); it != _effectData.end(); ++it) {
        delete it->second;
        it->second = nullptr;
    }
    _effectData.clear();
    
    experimental::AudioEngine::end();
}

/**
 * Clears the music queue, but does not release any other resources.
 */
void SoundEngine::clearQueue() {
    while (!_mqueue.empty()) {
        SoundPacket* data = _mqueue.front();
        _mqueue.pop_front();
        delete data;
    }
}


#pragma mark -
#pragma mark Audio Helpers
/**
 * Plays the music for the associated sound packet
 *
 * This does not change the _musicData attribute.  It simply plays the sound and
 * attaches the callback function.
 *
 * @param  data     the settings for the sound effect to play
 */
void SoundEngine::playMusic(SoundPacket* data) {
    if (data->_sndid != -1) {
        AENG::stop(data->_sndid);
    }
    data->_sndid = AENG::play2d(data->_sound->getSource(), data->_loop, data->_volume, _musicProfile);
    AENG::setFinishCallback(data->_sndid,[=](int id, const std::string& file) { this->gcMusic(id); });
}

/**
 * Plays the sound effect for the associated sound packet
 *
 * This does not change the _effectData attribute.  It simply plays the sound and
 * attaches the callback function.
 *
 * There are a limited number of channels available for sound effects.  If you
 * go over the number available, the sound will not play unless force is true.
 * In that case, it will grab the channel from the longest playing sound effect.
 *
 * @param  data     the settings for the sound effect to play
 * @param  force    whether to force another sound to stop.
 */
void SoundEngine::playEffect(SoundPacket* data, bool force) {
    bool full = _effectIDs.size() >= _effectProfile->maxInstances;
    if (full && !force) {
        return;
    } else if (full) {
        stopEffect(_equeue.front());
    }

    _equeue.push_back(data->_key);
    int sndid = AENG::play2d(data->_sound->getSource(), data->_loop, data->_volume, _effectProfile);
    data->_sndid = sndid;

    // Getting an invalid id.
    CCASSERT(data->_sndid >= 0, "Invalid Audio ID returned");

    _effectIDs.emplace(data->_key,data->_sndid);
    _effectData.emplace(data->_sndid,data);
    
    // This works because callbacks are in same thread.
    std::string key = data->_key;
    AENG::setFinishCallback(data->_sndid,[=](int id, const std::string& file) {
        this->gcEffect(id, key);
    });
}

/**
 * Callback function for when a music channel finishes
 *
 * This method is called when the active music completes.  If there is any music
 * waiting in the queue, it plays it immediately (using the second channel for
 * a smooth transition).  Otherwise, it sets _musicData to inactive.
 *
 * @param  id   the sound id for the completed sound
 */
void SoundEngine::gcMusic(int id) {
    _musicData->_sndid = -1;
    
    _musicData->_sound->release();
    _musicData->_sound = nullptr;

    if (!_mqueue.empty()) {
        SoundPacket* data = _mqueue.front();
        _mqueue.pop_front();
        _musicData->set(*data);
        _musicData->_sndid = -1;
        delete data;
        playMusic(_musicData);
    }
}

/**
 * Callback function for when a sound effect channel finishes
 *
 * This method is called when the active sound effect completes. It garbage
 * collects the sound effect, allowing its key to be reused.
 *
 * @param  id   the sound id for the completed sound
 * @param  key  the reference key for the completed sound
 */
void SoundEngine::gcEffect(int id, const std::string key) {
    // Nothing to do if already collected.
    // It looks from the code of AudioEngine that this is not necessary.
    // But there are race conditions without it.
    if (_effectIDs.find(key) == _effectIDs.end() || _effectIDs[key] != id) {
        return;
    }
    
    Sound* sound = _effectData[id]->_sound;
    sound->release();
    
    delete _effectData[id];
    _effectData.erase(id);
    _effectIDs.erase(key);

	auto it = _equeue.begin();
	while (it != _equeue.end()) {
		if (*it == key) {
			it = _equeue.erase(it);
			break;
		}
		else {
			it++;
			
		}
	}
}

/**
 * Returns the SoundState value equivalent to the AudioState value.
 *
 * @param  state    the original AudioState value
 *
 * @return the SoundState value equivalent to the AudioState value.
 */
SoundEngine::SoundState SoundEngine::convertAudioState(AENG::AudioState state) {
    switch (state) {
        case AENG::AudioState::ERROR:
            return SoundState::INACTIVE;
        case AENG::AudioState::INITIALZING:
            return SoundState::LOADING;
        case AENG::AudioState::PLAYING:
            return SoundState::PLAYING;
        case AENG::AudioState::PAUSED:
            return SoundState::PAUSED;
    }
    return SoundState::INACTIVE;
}


#pragma mark -
#pragma mark Music Mangement
/**
 * Plays given sound file as background music.
 *
 * Music is handled differently from sound effects.  Only one sound effect can be
 * treated as music at a time.  However, it is possible to queue music files for
 * immediate playback once the active sound is finished.
 *
 * This method immediately plays the provided sound. Hence it overrides and clears
 * the music queue. To safely play a sound without affecting the music queue, use
 * the method queueMusic instead.
 *
 * @param  filename the sound file to play as music
 * @param  loop     whether to loop the music continuously
 * @param  volume   the music volume
 *
 * @retain the sound asset (until completion)
 */
void SoundEngine::playMusic(Sound* sound, bool loop, float volume) {
    CCASSERT(sound, "Attempt to play nonexistent sound");
    clearQueue();
    sound->retain();
    _musicData->_sound  = sound;
    _musicData->_loop   = loop;
    _musicData->_volume = volume;
    playMusic(_musicData);
}

/**
 * Returns the current state of the background music
 *
 * @return the current state of the background music
 */
SoundEngine::SoundState SoundEngine::getMusicState() const {
    if (_musicData->_sndid == -1) {
        return SoundState::INACTIVE;
    }
    return convertAudioState(AENG::getState(_musicData->_sndid));
}

/**
 * Sets whether the background music is on a continuous loop.
 *
 * If loop is true, this will clear the active music queue (as a continuous loop cannot
 * be followed by later music).
 *
 * If there is no active background music, this method will raise an error.
 *
 * @param  loop  whether the background music should be on a continuous loop
 */
void SoundEngine::setMusicLoop(bool loop) {
    CCASSERT(_musicData->_sndid != -1, "Attempt to modify nonexistent sound");
    AENG::setLoop(_musicData->_sndid, loop);
    if (loop) {
        clearQueue();
    }
    _musicData->_loop = loop;
}

/**
 * Sets the volume of the background music
 *
 * If there is no active background music, this method will raise an error.
 *
 * @param  volume   the volume of the background music
 */
void SoundEngine::setMusicVolume(float volume) {
    CCASSERT(_musicData->_sndid != -1, "Attempt to modify nonexistent sound");
    AENG::setVolume(_musicData->_sndid, volume);
    _musicData->_volume = volume;
}

/**
 * Sets the elapsed time of the background music.
 *
 * The elapsed time is the current position of the music from the beginning.  It
 * does not include any time spent on a continuous loop.
 *
 * If there is no active background music, this method will raise an error.
 *
 * @param  time  the new position of the background music
 */
void SoundEngine::setMusicElapsed(float time) {
    CCASSERT(_musicData->_sndid != -1, "Attempt to modify nonexistent sound");
    AENG::setCurrentTime(_musicData->_sndid, time);
}

/**
 * Sets the time remaining for the background music.
 *
 * The time remaining is just duration-elapsed.  It does not take into account whether
 * the music is on a loop.  is the current position of the music from the beginning.  It
 * does not include any time spent on a continuous loop. It does not include the duration
 * of any music waiting in the queue.
 *
 * If there is no active background music, this method will raise an error.
 *
 * @param  time  the new time remaining of the background music
 */
void SoundEngine::setMusicRemaining(float time) {
    CCASSERT(_musicData->_sndid != -1, "Attempt to modify nonexistent sound");
    float total = AENG::getDuration(_musicData->_sndid);
    total = (time > total ? 0 : time-total);
    AENG::setCurrentTime(_musicData->_sndid, total);
}

/**
 * Stops the background music
 *
 * This method clears the queue of any further music.
 *
 * @release all queued music assets
 */
void SoundEngine::stopMusic() {
    CCASSERT(_musicData->_sndid != -1, "Attempt to stop nonexistent sound");
    AENG::stop(_musicData->_sndid);
    _musicData->_sndid = -1;
    _musicData->_sound->release();
    _musicData->_sound = nullptr;

    // Clear the queue
    while (!_mqueue.empty()) {
        SoundPacket* data = _mqueue.front();
        _mqueue.pop_front();
        data->_sound->release();
        delete data;
    }
}

/**
 * Pauses the background music, allowing it to be resumed later.
 *
 * This method has no effect on the music queue.
 */
void SoundEngine::pauseMusic() {
    CCASSERT(_musicData->_sndid != -1, "Attempt to pause nonexistent sound");
    AENG::pause(_musicData->_sndid);
}

/**
 * Resumes the background music assuming that it was paused previously.
 */
void SoundEngine::resumeMusic() {
    CCASSERT(_musicData->_sndid != -1, "Attempt to resume nonexistent sound");
    AENG::resume(_musicData->_sndid);
}

/**
 * Restarts the current background music from the beginning
 *
 * This method has no effect on the music queue.
 */
void SoundEngine::restartMusic() {
    CCASSERT(_musicData->_sndid != -1, "Attempt to restart nonexistent sound");
    experimental::AudioEngine::setCurrentTime(_musicData->_sndid, 0.0f);
}

#pragma mark Music Queue
/**
 * Adds the given sound file the background music queue
 *
 * Music is handled differently from sound effects.  Only one sound effect can be
 * treated as music at a time.  However, it is possible to queue music files for
 * immediate playback once the active sound is finished.
 *
 * If the queue is empty and there is no active music, this method will play
 * the music immediately.  Otherwise, it will add the music to the queue, and
 * it will play as soon as it is removed from the queue.  Only the last element
 * of the queue can be in a loop (otherwise the queue will experience starvation).
 * Therefore, this method will disable the loop option of the active music and
 * any music in the queue.
 *
 * @param  filename the sound file to play as music
 * @param  loop     whether to loop the music continuously
 * @param  volume   the music volume
 *
 * @retain the sound asset (until completion)
 */
void SoundEngine::queueMusic(Sound* sound, bool loop, float volume) {
    CCASSERT(sound, "Attempt to play nonexistent sound");
    if (_musicData->_sndid == -1 && _mqueue.empty()) {
        playMusic(sound,loop,volume);
        return;
    }
    
    sound->retain();
    SoundPacket* data = new SoundPacket(MUSIC_KEY,sound,loop,volume);
    _mqueue.push_back(data);
    AENG::setLoop(_musicData->_sndid, false);
}

/**
 * Returns the sound information for current the music queue
 *
 * @return the sound information for current the music queue
 */
const std::vector<const Sound*> SoundEngine::getMusicQueue()  const {
    std::vector<const Sound*> result;
    for(auto it = _mqueue.begin(); it != _mqueue.begin(); ++it) {
        SoundPacket* packet = *it;
        result.push_back(packet->_sound);
    }
    return result;
}

/**
 * Skips ahead in the music queue.
 *
 * The value skip is the number of songs to skip over. A value of 0 will simply
 * skip over the active music to the next element of the queue. Each value above
 * 0 will skip over one more element in the queue.  If this skipping empties
 * the queue, then no music will play.
 *
 * @param  steps    number of elements to skip in the queue
 *
 * @release a music assets stopped or skipped
 */
void SoundEngine::skipMusicQueue(unsigned int steps) {
    while (steps > 0 && !_mqueue.empty()) {
        SoundPacket* data = _mqueue.front();
        _mqueue.pop_front();
        data->_sound->release();
        delete data;
        steps--;
    }
    if (_musicData->_sndid != -1) {
        AENG::stop(_musicData->_sndid);
        gcMusic(_musicData->_sndid);
    }
}


#pragma mark -
#pragma mark Sound Effect Management
/**
 * Plays given sound effect, and associates it with the specified key.
 *
 * Sound effects are associated with a reference key.  This allows the application
 * to easily reference the sound state without having to internally manage pointers
 * to the AudioEngine.
 *
 * If the key is already associated with an active sound channel, this method will
 * stop the existing sound and replace it with this one.  It is the responsibility
 * of the application layer to manage key usage.
 *
 * There are a limited number of channels available for sound effects.  If you
 * go over the number available, the sound will not play unless force is true.
 * In that case, it will grab the channel from the longest playing sound effect.
 *
 * @param  key      the reference key for the sound effect
 * @param  filename the sound effect file to play
 * @param  loop     whether to loop the sound effect continuously
 * @param  volume   the sound effect volume
 * @param  force    whether to force another sound to stop.
 *
 * @retain the sound asset (until completion)
 */
void SoundEngine::playEffect(std::string key, Sound* sound, bool loop, float volume, bool force) {
    CCASSERT(sound, "Attempt to play nonexistent sound");
    if (isActiveEffect(key)) {
        if (force) {
            stopEffect(key);
        } else {
            return;
        }
    }
    
    sound->retain();
    playEffect(new SoundPacket(key,sound,loop,volume),force);
}

/**
 * Returns the current state of the sound effect
 *
 * If the key does not correspond to a channel, this method returns INACTIVE.
 *
 * @param  key      the reference key for the sound effect
 *
 * @return the current state of the sound effect
 */
SoundEngine::SoundState SoundEngine::getEffectState(std::string key) const {
    if (!isActiveEffect(key)) {
        return SoundState::INACTIVE;
    }
    int sndid = _effectIDs.find(key)->second;
    return convertAudioState(AENG::getState(sndid));
}

/**
 * Returns true if the sound effect is in a continuous loop.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 *
 * @return true if the sound effect is in a continuous loop.
 */
bool SoundEngine::isEffectLoop(std::string key) const {
    CCASSERT(isActiveEffect(key), "Query for inactive sound");
    int id = _effectIDs.find(key)->second;
    SoundPacket* data = _effectData.at(id);
    return data->_loop;
}

/**
 * Sets whether the sound effect is in a continuous loop.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 * @param  loop     whether the sound effect is in a continuous loop
 */
void SoundEngine::setEffectLoop(std::string key, bool loop) {
    CCASSERT(isActiveEffect(key), "Query for inactive sound");
    SoundPacket* data = _effectData[_effectIDs.find(key)->second];
    data->_loop = loop;
    AENG::setLoop(data->_sndid, loop);
}

/**
 * Returns the current volume of the sound effect.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 *
 * @return the current volume of the sound effect
 */
float SoundEngine::getEffectVolume(std::string key) const {
    CCASSERT(isActiveEffect(key), "Query for inactive sound");
    int id = _effectIDs.find(key)->second;
    SoundPacket* data = _effectData.at(id);
    return data->_volume;
}

/**
 * Sets the current volume of the sound effect.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 * @param  volume   the current volume of the sound effect
 */
void SoundEngine::setEffectVolume(std::string key, float volume) {
    CCASSERT(isActiveEffect(key), "Query for inactive sound");
    SoundPacket* data = _effectData[_effectIDs.find(key)->second];
    data->_volume = volume;
    AENG::setVolume(data->_sndid, volume);
}

/**
 * Returns the duration of the sound effect
 *
 * This method does not take into account whether the sound effect is on a loop.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 *
 * @return the duration of the sound effect
 */
float SoundEngine::getEffectDuration(std::string key) const {
    CCASSERT(isActiveEffect(key), "Query for inactive sound");
    int id = _effectIDs.find(key)->second;
    SoundPacket* data = _effectData.at(id);
    return AENG::getDuration(data->_sndid);
}

/**
 * Returns the elapsed time of the sound effect
 *
 * The elapsed time is the current position of the sound from the beginning.  It
 * does not include any time spent on a continuous loop.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 *
 * @return the elapsed time of the sound effect
 */
float SoundEngine::getEffectElapsed(std::string key) const {
    CCASSERT(isActiveEffect(key), "Query for inactive sound");
    int id = _effectIDs.find(key)->second;
    SoundPacket* data = _effectData.at(id);
    return AENG::getCurrentTime(data->_sndid);
}

/**
 * Returns the time remaining for the sound effect.
 *
 * The time remaining is just duration-elapsed.  This method does not take into account
 * whether the sound is on a loop.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 *
 * @return the time remaining for the sound effect
 */
float SoundEngine::getEffectRemaining(std::string key) const {
    CCASSERT(isActiveEffect(key), "Query for inactive sound");
    int id = _effectIDs.find(key)->second;
    SoundPacket* data = _effectData.at(id);
    return AENG::getDuration(data->_sndid)-AENG::getCurrentTime(data->_sndid);
}

/**
 * Sets the elapsed time of the sound effect
 *
 * The elapsed time is the current position of the sound from the beginning.  It
 * does not include any time spent on a continuous loop.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 * @param  time     the new position of the sound effect
 */
void SoundEngine::setEffectElapsed(std::string key, float time) {
    CCASSERT(isActiveEffect(key), "Query for inactive sound");
    SoundPacket* data = _effectData[_effectIDs.find(key)->second];
    AENG::setCurrentTime(data->_sndid,time);
}

/**
 * Sets the time remaining for the sound effect.
 *
 * The time remaining is just duration-elapsed.  This method does not take into account
 * whether the sound is on a loop.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 * @param  time     the new time remaining for the sound effect
 */
void SoundEngine::setEffectRemaining(std::string key, float time) {
    CCASSERT(isActiveEffect(key), "Query for inactive sound");
    SoundPacket* data = _effectData[_effectIDs.find(key)->second];
    float total = AENG::getDuration(data->_sndid);
    total = (time > total ? 0 : time-total);
    AENG::setCurrentTime(data->_sndid, total);
}

/**
 * Stops the sound effect for the given key, removing it.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 *
 * @release the stopped sound asset
 */
void SoundEngine::stopEffect(std::string key) {
    CCASSERT(isActiveEffect(key), "Attempt to modify inactive sound");
    SoundPacket* data = _effectData[_effectIDs.find(key)->second];
    int sid = data->_sndid;
    gcEffect(sid, data->_key);
    AENG::stop(sid);
}

/**
 * Pauses the sound effect for the given key.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 */
void SoundEngine::pauseEffect(std::string key) {
    CCASSERT(isActiveEffect(key), "Attempt to modify inactive sound");
    SoundPacket* data = _effectData[_effectIDs.find(key)->second];
    AENG::pause(data->_sndid);
}

/**
 * Resumes the sound effect for the given key.
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 */
void SoundEngine::resumeEffect(std::string key) {
    CCASSERT(isActiveEffect(key), "Attempt to modify inactive sound");
    SoundPacket* data = _effectData[_effectIDs.find(key)->second];
    AENG::resume(data->_sndid);
}

/**
 * Restarts the sound effect from the beginning
 *
 * If the key does not correspond to a channel, this method raises an error.
 *
 * @param  key      the reference key for the sound effect
 */
void SoundEngine::restartEffect(std::string key) {
    CCASSERT(isActiveEffect(key), "Query for inactive sound");
    SoundPacket* data = _effectData[_effectIDs.find(key)->second];
    AENG::setCurrentTime(data->_sndid,0.0f);
}

/**
 * Stops all sound effects, removing them from the engine.
 * 
 * @release all active sounds assets
 */
void SoundEngine::stopAllEffects() {
    for(auto it = _effectIDs.begin(); it != _effectIDs.end(); ++it) {
        AENG::stop(it->second);
        _effectData[it->second]->_sound->release();
        delete _effectData[it->second];
        _effectData[it->second] = nullptr;
    }
    _effectIDs.clear();
    _equeue.clear();
}

/**
 * Pauses all sound effects, allowing them to be resumed later.
 */
void SoundEngine::pauseAllEffects() {
    for(auto it = _effectIDs.begin(); it != _effectIDs.end(); ++it) {
        AENG::pause(it->second);
    }
}

/**
 * Resumes all paused sound effects.
 */
void SoundEngine::resumeAllEffects() {
    for(auto it = _effectIDs.begin(); it != _effectIDs.end(); ++it) {
        AENG::resume(it->second);
    }
}

#pragma mark -
#pragma mark Global Management
/**
 * Stops all sounds, both music and sound effects.
 *
 * This effectively clears the sound engine.
 *
 * @release all active sounds assets
 */
void SoundEngine::stopAll() {
    if (_musicData->_sndid != -1) {
        stopMusic();
    }
    stopAllEffects();
}

/**
 * Pauses all sounds, both music and sound effects, allowing them to be resumed later.
 */
void SoundEngine::pauseAll() {
    if (_musicData->_sndid != -1) {
        pauseMusic();
    }
    pauseAllEffects();
}

/**
 * Resumes all paused sounds, both music and sound effects.
 */
void SoundEngine::resumeAll() {
    if (_musicData->_sndid != -1) {
        resumeMusic();
    }
    resumeAllEffects();
}

NS_CC_END