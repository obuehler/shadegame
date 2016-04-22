//
//  CUSoundEngine.h
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
#ifndef __CU_SOUND_ENGINE_H__
#define __CU_SOUND_ENGINE_H__

#include <audio/include/AudioEngine.h>

NS_CC_BEGIN

class Sound;

#define AENG experimental::AudioEngine

/**
 * Class provides a singleton sound manager
 *
 * This class is a more robust sound manager on top of the new experimental AudioEngine. In
 * particular, it allows the user to specify sound instances by predefined key.  This cuts
 * down on the overhead of managing the sound identifier. It also provides advanced support
 * for stringing together music loops.
 *
 * This class has all of the functionality of SimpleAudioEngine except for pan and pitch
 * support. As there is no true cross platform support for either of these (they are usually
 * ignored on non-Apple platforms) this should not be a problem.
 *
 * You cannot create new instances of this class.  Instead, you should access the singleton
 * through the three static methods: start(), stop() and getInstance().
 */
class CC_DLL SoundEngine {
public:
    
#pragma mark -
#pragma mark Sound Packet
    /**
     * Class provides a encapsulation of an active sound channel.
     *
     * This class allows us to internalize sound management, instead of requiring the
     * application layer to keep track of a lot of integers and continuously query when
     * sounds complete.
     */
    class SoundPacket {
    public:
        /**
         * Returns the reference key for this active sound channel.
         *
         * @return the reference key for this active sound channel.
         */
        const std::string getKey() const    { return _key; }
        
        /**
         * Returns the sound file for this active sound channel.
         *
         * @return the sound file for this active sound channel.
         */
        const Sound* getSound() const  { return _sound; }
        
        /**
         * Creates a sound packet that is a copy of the given packet.
         *
         * @param  data the packet to copy
         */
        SoundPacket(const SoundPacket& data) :
        SoundPacket(data._key, data._sound, data._loop, data._volume) {}
        
        /** 
         * Disposes of a sound packet (does nothing as it owns no resources)
         */
        ~SoundPacket() { }
        
    protected:
        /** The reference key for this active sound channel. */
        std::string _key;
        
        /** The sound file for this active sound channel */
        Sound* _sound;
        
        /** Whether this active sound channel is in a loop. */
        bool  _loop;
        
        /** The volume of this active sound channel. */
        float _volume;
        
        /** The sound identifier for the experimental AudioEngine */
        int _sndid;
        
        /**
         * Creates a sound packet with the given data.
         *
         * @param  k    the reference key
         * @param  s    the sound file
         * @param  l    whether to loop the sound
         * @param  v    the sound volume
         */
        SoundPacket(const std::string& k, Sound* s, bool l, float v) :
        _sndid(-1), _sound(s), _key(k), _loop(l), _volume(v) {}
        
        /**
         * Creates an empty, inactive sound data
         */
        SoundPacket() : _sndid(-1), _sound(nullptr), _key(""), _loop(false), _volume(0.0f) {}
        
        /**
         * Assigns this packet to be a copy of the given packet.
         *
         * @param  data the packet to copy
         */
        void set(const SoundPacket& data) {
            _sound = data._sound; _key = data._key; _loop = data._loop; _volume = data._volume;
        }
        
        /** Allow SoundEngine direct access */
        friend class SoundEngine;
    };
    
    
#pragma mark -
#pragma mark Sound State
    
    /**
     * This enumeration provides a public alternative to AudioState
     *
     * We want to avoid any direct contact with the experimental AudioEngine.
     */
    enum class SoundState {
        /** This sound channel is not actually active */
        INACTIVE,
        /** This sound is still being loaded */
        LOADING,
        /** This sound is active and currently playing */
        PLAYING,
        /** This sound is active but is currently paused */
        PAUSED
    };
    
    
#pragma mark -
#pragma mark Sound Engine
protected:
    /** Reference to the sound engine singleton */
    static SoundEngine* _gEngine;
    
    /** The sound profile for music channels (we reserve 2 for smooth loop transitions) */
    experimental::AudioProfile* _musicProfile;
    /** The sound profile for sound effect channels (uses the leftover channels) */
    experimental::AudioProfile* _effectProfile;
    
    /** The sound packet for the active music channel */
    SoundPacket* _musicData;
    /** The queue for subsequent sound loops */
    std::deque<SoundPacket*> _mqueue;
    
    /** The sound packets for all active effects */
    std::unordered_map<int,SoundPacket*> _effectData;
    /** Map keys to identifiers */
    std::unordered_map<std::string,int> _effectIDs;
    /** The queue for subsequent sound loops */
    std::deque<std::string> _equeue;
    
    
#pragma mark Allocation
    /**
     * Creates, but does not initialize the singleton sound engine
     *
     * The engine must be initialized before is can be used.
     */
    SoundEngine() : _musicData(nullptr), _musicProfile(nullptr), _effectProfile(nullptr) {}
    
    /**
     * Disposes of the singleton sound engine.
     *
     * This destructor releases all of the resources associated with this sound engine.
     */
    ~SoundEngine() { dispose(); }
    
    /**
     * Initializes the sound engine.
     *
     * This method starts up the experimental AudioEngine and creates the custom
     * profiles for this implementation.
     *
     * @return true if the sound engine was successfully initialized.
     */
    bool init();
    
    /**
     * Releases all resources for this singleton sound engine.
     *
     * If you need to use the engine again, you must call init().
     */
    void dispose();
    
    /**
     * Clears the music queue, but does not release any other resources.
     */
    void clearQueue();
    
    
#pragma mark Audio Helpers
    /**
     * Plays the music for the associated sound packet
     *
     * This does not change the _musicData attribute.  It simply plays the sound and
     * attaches the callback function.
     *
     * @param  data     the settings for the sound effect to play
     */
    void playMusic(SoundPacket* data);
    
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
    void playEffect(SoundPacket* data, bool force=false);
    
    /**
     * Returns the SoundState value equivalent to the AudioState value.
     *
     * @param  state    the original AudioState value
     *
     * @return the SoundState value equivalent to the AudioState value.
     */
    static SoundState convertAudioState(AENG::AudioState state);
    
    /**
     * Callback function for when a music channel finishes
     *
     * This method is called when the active music completes.  If there is any music
     * waiting in the queue, it plays it immediately (using the second channel for
     * a smooth transition).  Otherwise, it sets _musicData to inactive.
     *
     * @param  id   the sound id for the completed sound
     */
    void gcMusic(int id);
    
    /**
     * Callback function for when a sound effect channel finishes
     *
     * This method is called when the active sound effect completes. It garbage
     * collects the sound effect, allowing its key to be reused.
     *
     * @param  id   the sound id for the completed sound
     * @param  key  the reference key for the completed sound
     */
    void gcEffect(int id, const std::string key);
    
    
public:
#pragma mark Static Accessors
    /**
     * Returns the singleton instance of the sound engine.
     *
     * If the sound engine has not been started, then this method will return null.
     *
     * @return the singleton instance of the sound engine.
     */
    static SoundEngine* getInstance() { return _gEngine; }
    
    /**
     * Starts the singleton sound engine.
     *
     * Once this method is called, the method getInstance() will no longer return
     * null.  Calling the method multiple times (without calling stop) will have
     * no effect.
     */
    static void start();
    
    /**
     * Starts the singleton sound, releasing all resources.
     *
     * Once this method is called, the method getInstance() will return null.
     * Calling the method multiple times (without calling stop) will have no effect.
     */
    static void stop();
    
    
#pragma mark -
#pragma mark Music Management
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
    void playMusic(Sound* sound, bool loop=false, float volume=1.0);
    
    /**
     * Returns a sound packet with the settings of the background music
     *
     * If there is no active background music, this method returns null.
     *
     * @return a sound packet with the settings of the background music
     */
    const Sound* currentMusic() const { return (_musicData->_sndid != -1 ? _musicData->_sound : nullptr); }
    
    /**
     * Returns the current state of the background music
     *
     * @return the current state of the background music
     */
    SoundState getMusicState() const;
    
    /**
     * Returns true if the background music is in a continuous loop.
     *
     * If there is no active background music, this method will return false.
     *
     * @return true if the background music is in a continuous loop.
     */
    bool isMusicLoop() const    { return _musicData->_sndid != -1 && _musicData->_loop; }
    
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
    void setMusicLoop(bool loop);  // Clears the queue
    
    /**
     * Returns the volume of the background music
     *
     * If there is no active background music, this method will return 0.
     *
     * @return the volume of the background music
     */
    bool getMusicVolume() const { return (_musicData->_sndid != -1 ? _musicData->_volume : 0.0f); }
    
    /**
     * Sets the volume of the background music
     *
     * If there is no active background music, this method will raise an error.
     *
     * @param  volume   the volume of the background music
     */
    void setMusicVolume(float volume);
    
    /**
     * Returns the duration of the background music
     *
     * This method does not take into account whether the music is on a loop. It also does
     * not include the duration of any music waiting in the queue. If there is no active
     * background music, this method will return 0.
     *
     * @return the duration of the background music
     */
    float getMusicDuration() const  {
        return _musicData->_sndid != -1 ? AENG::getDuration(_musicData->_sndid) : 0.0f;
    }
    
    /**
     * Returns the elapsed time of the background music.
     *
     * The elapsed time is the current position of the music from the beginning.  It
     * does not include any time spent on a continuous loop. If there is no active
     * background music, this method will return 0.
     *
     * @return the elapsed time of the background music
     */
    float getMusicElapsed() const {
        return _musicData->_sndid != -1 ? AENG::getCurrentTime(_musicData->_sndid) : 0.0f;
    }
    
    /**
     * Returns the time remaining for the background music.
     *
     * The time remaining is just duration-elapsed.  This method does not take into account
     * whether the music is on a loop. It also does not include the duration of any music
     * waiting in the queue. If there is no active background music, this method will return 0.
     *
     * @return the time remaining for the background music
     */
    float getMusicRemaining() const {
        return _musicData->_sndid != -1 ?
        AENG::getDuration(_musicData->_sndid)-AENG::getCurrentTime(_musicData->_sndid) :
        0.0f;
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
    void setMusicElapsed(float time);
    
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
    void setMusicRemaining(float time);
    
    /**
     * Stops the background music
     *
     * This method clears the queue of any further music.
     *
     * @release all queued music assets
     */
    void stopMusic();
    
    /**
     * Pauses the background music, allowing it to be resumed later.
     *
     * This method has no effect on the music queue.
     */
    void pauseMusic();
    
    /**
     * Resumes the background music assuming that it was paused previously.
     */
    void resumeMusic();
    
    /**
     * Restarts the current background music from the beginning
     *
     * This method has no effect on the music queue.
     */
    void restartMusic();
    
    
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
    void queueMusic(Sound* sound, bool loop=false, float volume=1.0);
    
    /**
     * Returns the sound information for current the music queue
     *
     * @return the sound information for current the music queue
     */
    const std::vector<const Sound*> getMusicQueue() const;
    
    /**
     * Returns the size of the current the music queue
     *
     * @return the size of the current the music queue
     */
    size_t getMusicQueueSize() const { return _mqueue.size(); }
    
    /**
     * Skips ahead in the music queue.
     *
     * The value skip is the number of songs to skip over. A value of 0 will simply
     * skip over the active music to the next element of the queue. Each value above
     * 0 will skip over one more element in the queue.  If this skipping empties
     * the queue, then no music will play.
     *
     * @param  steps    number of elements to skip in the queue
     */
    void skipMusicQueue(unsigned int steps=0);
    
    
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
    void playEffect(std::string key, Sound* sound, bool loop=false, float volume=1.0f, bool force=false);
    
    /**
     * Returns the number of channels available for sound effects.
     *
     * There are a limited number of channels available for sound effects.  If you
     * go over the number available, you cannot play another sound unless you force it.
     * In that case, it will grab the channel from the longest playing sound effect.
     *
     * @return the number of channels available for sound effects.
     */
    int getAvailableChannels() { return _effectProfile->maxInstances-(int)_effectIDs.size(); }
    
    /**
     * Returns true if the key is associated with an active channel.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return true if the key is associated with an active channel.
     */
    bool isActiveEffect(std::string key) const { return _effectIDs.find(key) != _effectIDs.end(); }
    
    /**
     * Returns the current state of the sound effect
     *
     * If the key does not correspond to a channel, this method returns INACTIVE.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the current state of the sound effect
     */
    SoundState getEffectState(std::string key) const;
    
    /**
     * Returns true if the sound effect is in a continuous loop.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return true if the sound effect is in a continuous loop.
     */
    bool isEffectLoop(std::string key) const;
    
    /**
     * Sets whether the sound effect is in a continuous loop.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     * @param  loop     whether the sound effect is in a continuous loop
     */
    void setEffectLoop(std::string key, bool loop);
    
    /**
     * Returns the current volume of the sound effect.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @return the current volume of the sound effect
     */
    float getEffectVolume(std::string key) const;
    
    /**
     * Sets the current volume of the sound effect.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     * @param  volume   the current volume of the sound effect
     */
    void setEffectVolume(std::string key, float volume);
    
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
    float getEffectDuration(std::string key) const;
    
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
    float getEffectElapsed(std::string key) const;
    
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
    float getEffectRemaining(std::string key) const;
    
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
    void setEffectElapsed(std::string key, float time);
    
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
    void setEffectRemaining(std::string key, float time);
    
    /**
     * Stops the sound effect for the given key, removing it.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     *
     * @release the stopped sound asset
     */
    void stopEffect(std::string key);
    
    /**
     * Pauses the sound effect for the given key.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     */
    void pauseEffect(std::string key);
    
    /**
     * Resumes the sound effect for the given key.
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     */
    void resumeEffect(std::string key);
    
    /**
     * Restarts the sound effect from the beginning
     *
     * If the key does not correspond to a channel, this method raises an error.
     *
     * @param  key      the reference key for the sound effect
     */
    void restartEffect(std::string key);
    
    /**
     * Stops all sound effects, removing them from the engine.
     *
     * @release all active sounds assets
     */
    void stopAllEffects();
    
    /**
     * Pauses all sound effects, allowing them to be resumed later.
     */
    void pauseAllEffects();
    
    /**
     * Resumes all paused sound effects.
     */
    void resumeAllEffects();
    
    
#pragma mark -
#pragma mark Global Management
    /**
     * Stops all sounds, both music and sound effects.
     *
     * This effectively clears the sound engine.
     *
     * @release all active sounds assets
     */
    void stopAll();
    
    /**
     * Pauses all sounds, both music and sound effects, allowing them to be resumed later.
     */
    void pauseAll();
    
    /**
     * Resumes all paused sounds, both music and sound effects.
     */
    void resumeAll();
};

NS_CC_END
#endif /* defined(__CU_SOUND_ENGINE_H__) */