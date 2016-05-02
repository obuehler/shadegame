/*
* cocos2d-x   http://www.cocos2d-x.org
*
* Copyright (c) 2010-2011 - cocos2d-x community
*
* Portions Copyright (c) Microsoft Open Technologies, Inc.
* All Rights Reserved
*
* Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#include "platform/CCPlatformConfig.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WINRT

#ifndef __AUDIO_ENGINE_WINRT_H_
#define __AUDIO_ENGINE_WINRT_H_

#define NEAR near

#include <unordered_map>

#include "base/CCRef.h"
#include "AudioEngine.h"
#include "AudioCachePlayer.h"

NS_CC_BEGIN
    namespace experimental{
#define MAX_AUDIOINSTANCES 32

 class CC_DLL AudioEngineImpl : public cocos2d::Ref
 {
 public:
     AudioEngineImpl();
     ~AudioEngineImpl();

     bool init();
     int play2d(const std::string &fileFullPath, bool loop, float volume);
     void setVolume(int audioID, float volume);
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
     void setFinishCallback(int audioID, const std::function<void(int, const std::string &)> &callback);
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

 private:
     std::unordered_map<int, AudioPlayer>  _audioPlayers;
     std::unordered_map<std::string, AudioCache> _audioCaches;
     std::vector<AudioCache*> _toRemoveCaches;

     std::mutex _threadMutex;
     std::vector<int> _toRemoveAudioIDs;
     bool _lazyInitLoop;
     int _currentAudioID;
 };
}

NS_CC_END
#endif // __AUDIO_ENGINE_WINRT_H_
#endif

