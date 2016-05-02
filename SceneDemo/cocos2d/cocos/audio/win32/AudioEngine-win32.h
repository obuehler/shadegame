/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.

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
#include "platform/CCPlatformConfig.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#ifndef __AUDIO_ENGINE_INL_H_
#define __AUDIO_ENGINE_INL_H_

#include <unordered_map>

#include "base/CCRef.h"
#include "AudioCache.h"
#include "AudioPlayer.h"

NS_CC_BEGIN
    namespace experimental{
#define MAX_AUDIOINSTANCES 32

class CC_DLL AudioEngineImpl : public cocos2d::Ref
{
public:
    AudioEngineImpl();
    ~AudioEngineImpl();
    
    bool init();
    int play2d(const std::string &fileFullPath ,bool loop ,float volume);
    void setVolume(int audioID,float volume);
    void setLoop(int audioID, bool loop);
    bool pause(int audioID);
    bool resume(int audioID);
    bool stop(int audioID);
    void stopAll();
    float getDuration(int audioID);
    
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
    float getDuration(const std::string& filePath);
    
    float getCurrentTime(int audioID);
    bool setCurrentTime(int audioID, float time);
    void setFinishCallback(int audioID, const std::function<void (int, const std::string &)> &callback);
    
    void uncache(const std::string& filePath);
    void uncacheAll();
    AudioCache* preload(const std::string& filePath, std::function<void(bool)> callback);
    
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
    
    void update(float dt);
    
private:
    void _play2d(AudioCache *cache, int audioID);
    
    ALuint _alSources[MAX_AUDIOINSTANCES];
    
    //source,used
    std::unordered_map<ALuint, bool> _alSourceUsed;
    
    //filePath,bufferInfo
    std::unordered_map<std::string, AudioCache> _audioCaches;
    
    //audioID,AudioInfo
    std::unordered_map<int, AudioPlayer>  _audioPlayers;
    
    std::mutex _threadMutex;
    
    std::vector<AudioCache*> _toRemoveCaches;
    std::vector<int> _toRemoveAudioIDs;
    
    bool _lazyInitLoop;
    
    int _currentAudioID;
    
};
}
NS_CC_END
#endif // __AUDIO_ENGINE_INL_H_
#endif

