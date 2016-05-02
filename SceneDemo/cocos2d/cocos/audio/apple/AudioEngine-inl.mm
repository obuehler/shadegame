//
//  AudioEngine-inl.cpp
//  Cornell Extensions to Cocos2D
//
//  This module is a rewrite of the Cocos2d AudioEngineImpl to use AVFramework instead of OpenAL.
//  It provides a C++ wrapper of AVAudioEngine. It provides a simple graph of MAX_AUDIOINSTANCES
//  many AVAudioPlayerNodes.
//
//  This implementation is very similar to the original AudioEngineImpl with a few extra methods
//  and a lot less multi-threading.  It was unfortunately necessary for two reasons.  First, OpenAL
//  is deprecated in iOS 9, and its support is very flaky (some clipping and distortion).  This
//  affected both SimpleAudioEngine and the experimental AudioEngine.  Second, the experimental
//  AudioEngine apparently has a lot of issues with multi-threading.  We regularly encountered race
//  conditions during early testing.  Rather than fix that implementation, it was easier to create
//  a new one using AVFoundation.
//
//  This module uses the PIMPL pattern for bridging C++ and Objective-C.  The header is clean
//  of any Objective-C types, wrapping them in light-weight classes.
//
//  Author: Walker White
//  Version: 1/12/16
//
#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#include "AudioEngine-inl.h"

#import <AVFoundation/AVFoundation.h>

#include "AudioCache.h"
#include "AudioPlayer.h"

#include "audio/include/AudioEngine.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"
#include "base/ccUtils.h"

/** The default volume for all players */
#define DEFAULT_VOLUME  0.8

USING_NS_CC;
using namespace cocos2d::experimental;

/**
 * A C++ wrapper for AVAudioFile and AVAudioPCMBuffer.
 *
 * This struct is necessary for a PIMPL implementation of AudioCache.  It is just POD; there is no
 * associated constructor. The fields of the struct are as follows:
 *
 *     AVAudioPCMBuffer* pcmb;  // The uncompressed source data
 *     AVAudioFile*      file;  // File reference for AVAudioEngine.
 */
struct cocos2d::experimental::AVAudioSource {
    /** The uncompressed source data */
    AVAudioPCMBuffer* pcmb;
    /** File reference for AVAudioEngine */
    AVAudioFile*      file;
};


/**
 * A C++ wrapper for AVAudioPlayerNode and AVAudioFormat.
 *
 * This struct is necessary for a PIMPL implementation of AudioPlayer.  It is just POD; there is no
 * associated constructor. The fields of the struct are as follows:
 *
 *     AVAudioPlayerNode* player; // The player node for the AVAudioEngine
 *     AVAudioFormat*     format; // The sound format for the mixer graph
 */
struct cocos2d::experimental::AVPlayerInstance {
    /** The player node for AVAudioEngine */
    AVAudioPlayerNode* player;
    /** The sound format for the mixer graph */
    AVAudioFormat*     format;
};

/**
 * A C++ wrapper for AVAudioEngine.
 *
 * Tis struct is necessary for a PIMPL implementation of AudioEngineImpl. It is just POD; there is no
 * associated constructor. It has a lone field, which is as follows:
 *
 *    AVAudioEngine* engine; // Reference to AVAudioEngine
 */
struct cocos2d::experimental::AVEngineInstance {
    /** Reference to AVAudioEngine */
    AVAudioEngine* engine;
};


#pragma mark -
#pragma mark Audio Engine
/**
 * Creates a new Apple-specific implementation of AudioEngine
 *
 * This method does not initialize the AudioEngine.  It only initializes the
 * attributes to their defaults.  To start the AudioEngine, you must call init().
 */
AudioEngineImpl::AudioEngineImpl() :
_engine(nullptr),
_scheduler(nullptr),
_playing(0),
_loading(0),
_nextID(0) {
    for(int ii = 0; ii < MAX_AUDIOINSTANCES; ii++) {
        _player[ii] = new AudioPlayer();
    }
}

/**
 * Disposes of the AudioEngine, releasing all resources.
 */
AudioEngineImpl::~AudioEngineImpl() {
    if (_engine != nullptr) {
        [_engine->engine stop];
    }
    for(int ii = 0; ii < MAX_AUDIOINSTANCES; ii++) {
        delete _player[ii];
    }
    if (_engine != nullptr) {
        [_engine->engine release];
        _engine->engine = nil;
        delete _engine;
        _engine = nullptr;
    }
}

/**
 * Returns true if the AudioEngine initialized successfully.
 *
 * This method creates an instance of AVAudioEngine and constructs the initial mixer graph.
 * As long as everything hooks together successfully, this will return true.  If it fails,
 * it will report an error in addition to returning false.
 *
 * @return true if the AudioEngine initialized successfully.
 */
bool AudioEngineImpl::init() {
    _engine = new AVEngineInstance();
    _engine->engine = [[AVAudioEngine alloc] init];
    if (_engine->engine != nil) {
        [_engine->engine retain];
        _engine->engine.mainMixerNode.volume = DEFAULT_VOLUME;
        for(int ii = 0; ii < MAX_AUDIOINSTANCES; ii++) {
            // Make the player
            _player[ii] = new AudioPlayer();
            _player[ii]->init(_engine);
        }
        
        // The old engine turned this on and off.  I find it easier to short circuit the loops
        // Hence the reason for _loading and _playing
        _scheduler = Director::getInstance()->getScheduler();
        _scheduler->schedule(CC_SCHEDULE_SELECTOR(AudioEngineImpl::update), this, 0.05f, false);
        return boot();
    }
    return false;
}

/**
 * Reboots the AVAudioEngine, reporting any errors as appropriate
 *
 * @return true if reboot was successful
 */
bool AudioEngineImpl::boot() {
    NSError* error = nil;
    [_engine->engine startAndReturnError:&error];
    if (error != nil) {
        std::string message([[error localizedDescription] UTF8String]);
        CCLOG("Failed to initialize sound engine: %s", message.c_str());
        return false;
    }
    return true;
}

/**
 * Shuts down the AVAudioEngine, preventing any further playback.
 */
void AudioEngineImpl::shutdown() {
    [_engine->engine stop];
}


#pragma mark -
#pragma mark Asset Loading

/**
 * Loads an sound asset asynchronously.
 *
 * The sound will not be available immediately. Instead, it will load in a separate thread.
 * When it is ready, the provided callback (if any) will be executed to notify the user
 * of success. The callback function takes a single argument indicated whether loading was
 * successfully completed.
 *
 * @param  filePath the path to the sound file to load
 * @param  callback the callback to execute when sound is loaded.
 *
 * @return an audio cache that may not yet be fully loaded
 */
AudioCache* AudioEngineImpl::preload(const std::string& filePath, std::function<void(bool)> callback) {
    AudioCache* audioCache = nullptr;
    
    auto it = _caches.find(filePath);
    if (it == _caches.end()) {
        audioCache = new AudioCache(filePath);
        audioCache->retain();
        
        _caches[filePath] = audioCache;
        if (callback) {
            audioCache->addLoadCallback(callback);
        }
        _loading++;
        AudioEngine::addTask(std::bind(&AudioCache::readData, audioCache));
    } else {
        audioCache = it->second;
        if (callback) {
            callback(true);
        }
    }
    return audioCache;
}

/**
 * Returns a cached sound asset for immediate use.
 *
 * Unlike preload, this method loads the sound asset synchronously, making it available
 * for immediate use.  This should be only used when there is an attemp to play a sound
 * that was not preloaded.
 *
 * @param  filePath the path to the sound file to load
 *
 * @return cached instance of the loaded sound asset
 */
AudioCache* AudioEngineImpl::load(const std::string& filePath) {
    AudioCache* audioCache = nullptr;
    
    auto it = _caches.find(filePath);
    if (it == _caches.end()) {
        audioCache = new AudioCache(filePath);
        audioCache->retain();
        
        // No need to set _loading, as done immediately.
        _caches[filePath] = audioCache;
        audioCache->readData();
        audioCache->setStatus(STATUS_READY);
    } else {
        audioCache = it->second;
    }
    
    return audioCache;
}

/**
 * Unloads the given sound asset from the list of AudioCaches
 *
 * @param  filePath the path to the sound file to load
 */
void AudioEngineImpl::uncache(const std::string& filePath) {
    AudioCache* buffer = _caches[filePath];
    _caches.erase(filePath);
    buffer->release();
}

/**
 * Unloads all cached sound assets
 */
void AudioEngineImpl::uncacheAll() {
    for(auto it = _caches.begin(); it != _caches.end(); ++it) {
        it->second->release();
        it->second = nullptr;
    }
    _caches.clear();
}

/**
 * Returns the loaded status of the sound associated with filePath.
 *
 * This function returns -1, 0, or 1.  A value of 1 means that it is successfully loaded.
 * A value of 0 means that it is still loading.  Finally a value of -1 means that it tried
 * to load, but failed.
 *
 * @param  filePath the path to the sound file to load
 *
 * @return the loaded status of the sound associated with filePath.
 */
int AudioEngineImpl::isLoaded(const std::string& filePath) {
    auto it = _caches.find(filePath);
    if (it != _caches.end()) {
        return (it->second->getStatus() == STATUS_READY ? 1 : 0);
    }
    return -1;
}

/**
 * Sets the callback function for when the sound for audioID is completed.
 *
 * At first glance, this function looks like it should be subjected to a race condition.
 * You have to play a sound to get an audio ID, so you can only assign the callback
 * after the sound is completed.  However, the callbacks are executed in the update()
 * method which is primary Cocos2d thread.  Therefore it is safe to call this function
 * immediately after calling play2d().
 *
 * The callback function takes two arguments.  The first is the audio ID attached
 * to the completed sound.  The second is the file name of the sound.
 *
 * @param  audioID  the id of the sound to attach a callback.
 * @param  callback the callback function to execute when the sound is completed.
 */
void AudioEngineImpl::setFinishCallback(int audioID, const std::function<void (int, const std::string &)> &callback) {
    // No race.  same thread
    std::string name = _player[audioID]->getBuffer()->getSource();
    _player[audioID]->addPlayCallback([=](void) {
        callback(audioID,name);
    });
}


#pragma mark -
#pragma mark Playback Control
/**
 * Plays the given sound file and returns a sound id if successful.
 *
 * This method attempts to grab a free player instance to play the given
 * AudioCache.  The sound will be played with the given volume and loop setting.
 * If successful, it returns the unique audio id (an int between 0 and
 * MAX_INSTANCES).  Otherwise, it returns INVALID_AUDIO_ID.
 *
 * @param  filePath the path to the sound file to play
 * @param  volume   the volume ot play the sound
 * @param  loop     whether to loop the sound indefinitely
 *
 * @return the audio id associated with this sound
 */
int AudioEngineImpl::play2d(const std::string &filePath ,bool loop ,float volume) {
    AudioCache* audioCache = load(filePath);
    return play(audioCache,volume,loop);
}

/**
 * Plays the given AudioCache buffer and returns a sound id if successful.
 *
 * This method attempts to grab a free player instance to play the given
 * AudioCache.  The sound will be played with the given volume and loop setting.
 * If successful, it returns the unique audio id (an int between 0 and
 * MAX_INSTANCES).  Otherwise, it returns INVALID_AUDIO_ID.
 *
 * @param  buffer   the audio source to play
 * @param  volume   the volume ot play the sound
 * @param  loop     whether to loop the sound indefinitely
 *
 * @return the audio id associated with this buffer
 */
int AudioEngineImpl::play(AudioCache* buffer, float volume, bool loop) {
    // Get first available position
    bool shadow = false;
    int audioID = -1;
    int count = 0;
    for(int ii = _nextID; audioID == -1 && count < MAX_AUDIOINSTANCES; ii = (ii+1) % MAX_AUDIOINSTANCES) {
        if (!_player[ii]->isAttached()) {
            audioID = ii;
        }
        count++;
    }
    
    // Search for those queued for deletion if necessary
    count = 0;
    for(int ii = _nextID; audioID == -1 && ii < MAX_AUDIOINSTANCES; ii = (ii+1) % MAX_AUDIOINSTANCES) {
        if (!_player[ii]->isDeleted()) {
            audioID = ii;
            shadow = true;
        }
        count++;
    }
    
    // Fail if nothing available
    if (audioID == -1) {
        CCLOG("No available instances");
        return AudioEngine::INVALID_AUDIO_ID;
    }

    if (!_engine->engine.running) {
        boot();
    }

    if (shadow) {
        _player[audioID]->attachShadow(buffer,volume,loop);
    } else {
        _player[audioID]->attach(buffer,volume,loop);
        _player[audioID]->play();
    }
    
    _nextID = (_nextID+1) % MAX_AUDIOINSTANCES;
    _playing++;
    return audioID;
}


#pragma mark -
#pragma mark Playback Attributes

/**
 * Returns the duration of the sound for the given file name.
 *
 * This method will only return a value if the file name is successfully (pre)loaded.
 * Otherwise, it will return TIME_UNKNOWN.
 *
 * @param  filePath the file name of the sound to check.
 *
 * @return the duration of the sound for the given file name.
 */
float AudioEngineImpl::getDuration(const std::string& filePath) const {
    auto it = _caches.find(filePath);
    if (it != _caches.end()) {
        return it->second->getDuration();
    }
    return AudioEngine::TIME_UNKNOWN;
}


#pragma mark -
#pragma mark Engine Control
/**
 * Pause all sounds in the audio engine
 */
void AudioEngineImpl::pauseAll() {
    for(int ii = 0; ii < MAX_AUDIOINSTANCES; ii++) {
        if (_player[ii]->isAttached() && !_player[ii]->isPaused()) {
            _player[ii]->pause();
        }
    }
}

/**
 * Resume all paused sounds in the audio engine
 */
void AudioEngineImpl::resumeAll() {
    for(int ii = 0; ii < MAX_AUDIOINSTANCES; ii++) {
        if (_player[ii]->isAttached() && _player[ii]->isPaused()) {
            _player[ii]->resume();
        }
    }
}

/**
 * Stop all sounds in the audio engine, making the audio IDs available.
 */
void AudioEngineImpl::stopAll() {
    for(int ii = 0; ii < MAX_AUDIOINSTANCES; ii++) {
        if (_player[ii]->isAttached()) {
            _player[ii]->stop();
        }
    }
}

/**
 * Performs a regular clean-up of the AudioEngine.
 *
 * This method is used to execute the callback functions to clean-up after any asychronous execution.
 * This method is guaranteed to execute in the primary Cocos2d thread, so no synchronization code
 * should be necessary.
 */
void AudioEngineImpl::update(float dt) {
    // Process loading.
    if (_loading > 0) {
        for(auto it = _caches.begin(); it != _caches.end(); ++it) {
            AudioCache* buffer = it->second;
            int status = buffer->getStatus();
            if (status == STATUS_FAILED) {
                buffer->invokeLoadCallbacks(false);
                buffer->setStatus(STATUS_EMPTY);
                _loading--;
            } else if (status == STATUS_LOADED) {
                buffer->invokeLoadCallbacks(true);
                buffer->setStatus(STATUS_READY);
                _loading--;
            }
        }
    }
    
    // Process playing
    if (_engine != nullptr && _playing > 0) {
        for(int ii = 0; ii < MAX_AUDIOINSTANCES; ii++) {
            if (_player[ii]->isDeleted()) {
                _player[ii]->detach();
                _playing--;
                if (_player[ii]->isSelfDeleted()) {
                    AudioEngine::remove(ii);
                    _player[ii]->resetDelete();
                }
            }
        }
        
    }
}

#endif
