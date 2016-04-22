/****************************************************************************
 Copyright (c) 2014-2015 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#ifndef __AUDIO_ENGINE_INL_H_
#define __AUDIO_ENGINE_INL_H_

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <string>
#include <unordered_map>
#include "base/CCRef.h"
#include "base/ccUtils.h"

#define MAX_AUDIOINSTANCES 24

#define ERRORLOG(msg) log("fun:%s,line:%d,msg:%s",__func__,__LINE__,#msg)

NS_CC_BEGIN
    namespace experimental{
class AudioEngineImpl;

class AudioPlayer
{
public:
    AudioPlayer();
    ~AudioPlayer();

    bool init(SLEngineItf engineEngine, SLObjectItf outputMixObject,const std::string& fileFullPath, float volume, bool loop);

    bool _playOver;
    bool _loop;
    SLPlayItf _fdPlayerPlay;
private:
    SLObjectItf _fdPlayerObject;
    SLSeekItf _fdPlayerSeek;
    SLVolumeItf _fdPlayerVolume;

    float _duration;
    int _audioID;
    int _assetFd;
    float _delayTimeToRemove;

    std::function<void (int, const std::string &)> _finishCallback;

    friend class AudioEngineImpl;
};

class AudioEngineImpl : public cocos2d::Ref {
public:
    /**
     * Creates a new Android-specific implementation of AudioEngine
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
     * This method creates a new OpenSL sound engine.
     *
     * @return true if the AudioEngine initialized successfully.
     */
    bool init();
    
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
    int play2d(const std::string &fileFullPath ,bool loop ,float volume);
    
	/**
     * Sets the volume (0 to 1) of the sound for the given ID.
     *
     * @param  audioID  the ID of the sound to check.
     * @param  volume   the volume (0 to 1) of the sound for the given ID.
     */
    void setVolume(int audioID,float volume);
    
	/**
     * Sets whether the sound for the given ID is in an indefinite loop.
     *
     * If loop is false, then the sound will stop at its natural loop point.
     *
     * @param  audioID  the ID of the sound to check.
     * @param  loop     whether the sound for the given ID is in an indefinite loop.
     */
    void setLoop(int audioID, bool loop);
    
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
    void pause(int audioID);
    
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
    void resume(int audioID);
    
	/**
     * Stops the sound for the given ID, making that ID available.
     *
     * When a sound is stopped manually, no callback functions are called, and all existing
     * callbacks are deleted.
     *
     * @param  audioID  the ID of the sound to stop.
     */
    void stop(int audioID);
    
	/**
     * Stop all sounds in the audio engine, making the audio IDs available.
     */
    void stopAll();
    
	/**
     * Returns the duration of the sound for the given ID.
     *
     * @param  audioID  the ID of the sound to check.
     *
     * @return the duration of the sound for the given ID.
     */
    float getDuration(int audioID);
    
	/**
     * Returns the duration of the sound for the given file name.
     *
     * As Android cannot preload, this will always return TIME_UNKNOWN.
     *
     * @param  filePath the file name of the sound to check.
     *
     * @return the duration of the sound for the given file name.
     */
    float getDuration(const std::string& filePath);
    
	/**
     * Returns the current position of the sound for the given ID, in seconds.
     *
     * @param  audioID  the ID of the sound to check.
     *
     * @return the current position of the sound for the given ID, in seconds.
     */
    float getCurrentTime(int audioID);
    
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
    bool setCurrentTime(int audioID, float time);
    
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

    /**
     * Unloads the given sound asset from the engine.
     *
     * This has no affect on Android, as sounds are not preloaded.
     *
     * @param  filePath the path to the sound file to load
     */
    void uncache(const std::string& filePath) {}

   	/**
     * Unloads all cached sound assets
     *
     * This has no affect on Android, as sounds are not preloaded.
     */
	void uncacheAll() {}
    
	/**
     * Loads an sound asset asynchronously.
     * 
     * This method will always fail on Android
     *
     * @param  filePath the path to the sound file to load
     * @param  callback the callback to execute when sound is loaded.
     *
     * @return an audio cache that may not yet be fully loaded
     */
    void preload(const std::string& filePath, std::function<void(bool)> callback);
    
	/**
     * Returns the loaded status of the sound associated with filePath.
     *
     * This function returns -1, 0, or 1.  A value of 1 means that it is successfully loaded.
     * A value of 0 means that it is still loading.  Finally a value of -1 means that it tried
     * to load, but failed.
     *
     * This method will always fail on Android.
     *
     * @param  filePath the path to the sound file to load
     *
     * @return the loaded status of the sound associated with filePath.
     */
    int isLoaded(const std::string& filePath) { return -1; }

	/**
     * Performs a regular clean-up of the AudioEngine.
     *
     * This method is used to execute the callback functions to clean-up after any 
     * asynchronous execution. This method is guaranteed to execute in the primary Cocos2d 
     * thread, so no synchronization code should be necessary.
     */
    void update(float dt);
    
    
private:
    // engine interfaces
    SLObjectItf _engineObject;
    SLEngineItf _engineEngine;

    // output mix interfaces
    SLObjectItf _outputMixObject;

    //audioID,AudioInfo
    std::unordered_map<int, AudioPlayer>  _audioPlayers;

    int currentAudioID;
    
    bool _lazyInitLoop;
};

#endif // __AUDIO_ENGINE_INL_H_
 }
NS_CC_END

#endif
