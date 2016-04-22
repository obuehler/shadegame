//
//  AudioEngine-inl.h
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

#ifndef __AUDIO_ENGINE_INL_H_
#define __AUDIO_ENGINE_INL_H_

#include <unordered_map>
#include "AudioPlayer.h"
#include "base/CCRef.h"


NS_CC_BEGIN
/** Forward reference to Cocos2D scheduler */
class Scheduler;

namespace experimental {

/** Number of simultaneous sound instances supported by mixer graph */
#define MAX_AUDIOINSTANCES 24

/**
 * Forward reference to the AVAudioEngine.
 *
 * This is a C++ wrapper for AVAudioEngine.  It is necessary for a PIMPL implementation of
 * AudioEngineImpl.  It is just POD; there is no associated constructor. It has a lone
 * field, which is as follows:
 *
 *    AVAudioEngine* engine; // Reference to AVAudioEngine
 */
struct AVEngineInstance;

/** Forward reference to AudioCache (used as buffers) */
class AudioCache;

#pragma mark -
#pragma mark AudioEngine

/**
 * Apple-specific implementation of AudioEngine
 *
 * The class implements all of the methods required by AudioEngine to support platform-specific
 * functional on Apple platforms (both OS X and iOS). This implementation differs from the original
 * Cocos2d implementation in that it uses AVAudioEngine instead of OpenAL.  This makes if cleaner
 * and more thread safe (the original AudioEngineImpl appeared to suffer from race conditions).
 * It also makes it future-proof, as OpenAL is deprecated in iOS 9.
 *
 * IMPORTANT: For best performance, it is absolutely crucial that all sounds have exactly the
 * same format. The same file format, the same sampling rate, the same number of channels.  Any
 * change in format requires a reconfiguration of the mixer graph, and this can cause distortion
 * for rapid-fire sound effects.
 */
class AudioEngineImpl : public Ref {
protected:
    /** AudioCache instances for loaded sounds */
    std::unordered_map<std::string, AudioCache*> _caches;
    /** Wrapped reference to the AVAudioEngine */
    AVEngineInstance* _engine;
    /** The player nodes in the mixer graph */
    AudioPlayer* _player[MAX_AUDIOINSTANCES];
    /** Reference to the Cocos2d for clean-up and callbacks */
    Scheduler*   _scheduler;
    /** Reference to the next AudioID to assign (if available) */
    unsigned int _nextID;
    
    /** The number of units loading */
    unsigned int _loading;
    /** The number of buffers playing */
    unsigned int _playing;
    
    
#pragma mark Internal Helpers
    /**
     * Reboots the AVAudioEngine, reporting any errors as appropriate
     *
     * @return true if reboot was successful
     */
    bool boot();
    
    /**
     * Shuts down the AVAudioEngine, preventing any further playback.
     */
    void shutdown();
    
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
    int play(AudioCache* buffer, float volume=1.0, bool loop=false);

    /**
     * Returns the AudioPlayer associated with the given ID.
     *
     * @param  audioID  the id of a playing sound.
     *
     * @return the AudioPlayer associated with the given ID.
     */
    AudioPlayer* getInstance(int audioID) { return _player[audioID]; }

    
public:
#pragma mark Allocation
    /**
     * Creates a new Apple-specific implementation of AudioEngine
     * 
     * This method does not initialize the AudioEngine.  It only initializes the
     * attributes to their defaults.  To start the AudioEngine, you must call init().
     */
    AudioEngineImpl();
    
    /**
     * Disposes of the AudioEngine, releasing all resources.
     */
    ~AudioEngineImpl();
    
    /**
     * Returns true if the AudioEngine initialized successfully.
     *
     * This method creates an instance of AVAudioEngine and constructs the initial mixer graph.
     * As long as everything hooks together successfully, this will return true.  If it fails,
     * it will report an error in addition to returning false.
     *
     * @return true if the AudioEngine initialized successfully.
     */
    bool init();

    
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
    AudioCache* preload(const std::string& filePath, std::function<void(bool)> callback);
    
    /**
     * Returns a cached sound asset for immediate use.
     *
     * Unlike preload, this method loads the sound asset synchronously, making it available 
     * for immediate use.  This should only be used when there is an attemp to play a sound
     * that was not preloaded.
     *
     * @param  filePath the path to the sound file to load
     *
     * @return cached instance of the loaded sound asset
     */
    AudioCache* load(const std::string& filePath);
    
    /**
     * Unloads the given sound asset from the list of AudioCaches
     *
     * @param  filePath the path to the sound file to load
     */
    void uncache(const std::string& filePath);
    
    /**
     * Unloads all cached sound assets
     */
    void uncacheAll();
    
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
    int isLoaded(const std::string& filePath);
    
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
    void setFinishCallback(int audioID, const std::function<void (int, const std::string &)> &callback);

    
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
    int play2d(const std::string &fileFullPath, bool loop, float volume);
    
    /**
     * Pauses the sound associated with the given ID.
     *
     * This method assumes that audioID is valid, and does not do any error checking.
     * If the sound is already paused, this method will fail.
     *
     * @param  audioID  the ID of the sound to pause.
     *
     * @return true if the sound is successfully paused.
     */
    bool pause(int audioID)     { return _player[audioID]->pause(); }

    /**
     * Resumes the sound associated with the given ID.
     *
     * This method assumes that audioID is valid, and does not do any error checking.
     * If the sound is not paused, this method will fail.
     *
     * @param  audioID  the ID of the sound to pause.
     *
     * @return true if the sound is successfully resumed.
     */
    bool resume(int audioID)    { return _player[audioID]->resume(); }
    
    /**
     * Stops the sound for the given ID, making that ID available.
     *
     * When a sound is stopped manually, no callback functions are called, and all existing
     * callbacks are deleted.
     *
     * @param  audioID  the ID of the sound to stop.
     */
    void stop(int audioID)      { _player[audioID]->stop(); }

    
#pragma mark Playback Attributes
    /**
     * Returns the duration of the sound for the given ID.
     *
     * @param  audioID  the ID of the sound to check.
     *
     * @return the duration of the sound for the given ID.
     */
    float getDuration(int audioID) const    { return _player[audioID]->getDuration(); }

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
    float getDuration(const std::string& filePath) const;

    /**
     * Returns the volume (0 to 1) of the sound for the given ID.
     *
     * @param  audioID  the ID of the sound to check.
     *
     * @return the volume (0 to 1) of the sound for the given ID.
     */
    float getVolume(int audioID) const      { return _player[audioID]->getVolume(); }
    
    /**
     * Sets the volume (0 to 1) of the sound for the given ID.
     *
     * @param  audioID  the ID of the sound to check.
     * @param  volume   the volume (0 to 1) of the sound for the given ID.
     */
    void  setVolume(int audioID, float volume)      { _player[audioID]->setVolume(volume); }

    /**
     * Returns true if the sound for the given ID is in an indefinite loop.
     *
     * @param  audioID  the ID of the sound to check.
     *
     * @return true if the sound for the given ID is in an indefinite loop.
     */
    bool getLoop(int audioID) const         { return _player[audioID]->getLoop(); }
    
    /**
     * Sets whether the sound for the given ID is in an indefinite loop.
     *
     * If loop is false, then the sound will stop at its natural loop point.
     *
     * @param  audioID  the ID of the sound to check.
     * @param  loop     whether the sound for the given ID is in an indefinite loop.
     */
    void setLoop(int audioID, bool loop)    { _player[audioID]->setLoop(loop); }
    
    /**
     * Returns the current position of the sound for the given ID, in seconds.
     *
     * @param  audioID  the ID of the sound to check.
     *
     * @return the current position of the sound for the given ID, in seconds.
     */
    float getCurrentTime(int audioID) const { return _player[audioID]->getCurrentTime(); }
    
    /**
     * Sets the current position of the sound for the given ID, in seconds.
     *
     * If the sound is paused, this will do nothing until the player is resumed.
     * Otherwise, this will stop and restart the sound at the new position.
     *
     * @param  audioID  the ID of the sound to check.
     * @param  time     the new position of the player in the audio source
     *
     * @return true if the sound position was successfully changed.
     */
    bool  setCurrentTime(int audioID, float time)   { _player[audioID]->setCurrentTime(time); return true; }

    
#pragma mark Engine Control
    /**
     * Pause all sounds in the audio engine
     */
    void pauseAll();

    /**
     * Resume all paused sounds in the audio engine
     */
    void resumeAll();
    
    /**
     * Stop all sounds in the audio engine, making the audio IDs available.
     */
    void stopAll();

	/**
     * Performs a regular clean-up of the AudioEngine.
     *
     * This method is used to execute the callback functions to clean-up after any 
     * asynchronous execution. This method is guaranteed to execute in the primary Cocos2d 
     * thread, so no synchronization code should be necessary.
     */
    void update(float dt);
};
    
}
NS_CC_END
#endif // __AUDIO_ENGINE_INL_H_
#endif

