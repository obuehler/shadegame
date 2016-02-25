//
//  AudioCache.cpp
//  Cornell Extensions to Cocos2D
//
//  This module is a rewrite of the Cocos2d AudioCache to use AVFramework instead of OpenAL.
//  It provides a C++ wrapper of AVAudioFile and AVAudioPCMBuffer. It represents a preloaded
//  sound asset.  In our implementation, all assets are cached until unloaded.
//
//  This implementation is very similar to the original AudioCache except that we only attach
//  load callbacks, but not play callbacks.  Play callbacks ahould be attached to AudioPlayer.
//
//  This module uses the PIMPL pattern for bridging C++ and Objective-C.  The header is clean
//  of any Objective-C types, wrapping them in structs.
//
//  Author: Walker White
//  Version: 1/12/16
//
#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#import <AVFoundation/AVFoundation.h>

#include "AudioCache.h"
#include "platform/CCFileUtils.h"
#include "base/ccUtils.h"

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


#pragma mark -
#pragma mark Audio Cache

/**
 * Reads the data from the sound file
 *
 * When finished, this method sets the status to STATUS_LOADED, not STATUS_READY. The
 * AudioEngine must confirm it is ready and then set the status via the provided
 * method setStatus().
 *
 * For asynchronous loading, the method should be passed to a support thread in the
 * cross-platform AudioEngine, using the method addTask().
 */
void AudioCache::readData() {
    _status = STATUS_READING;
    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(_path);
    NSURL* url = [NSURL fileURLWithPath: [NSString stringWithUTF8String:fullpath.c_str()]];
    
    // Create objective C wrapper
    _data = new AVAudioSource();
    _data->file = nil;
    _data->pcmb = nil;
    
    // Read the file and process any errors
    NSError* error = nil;
    _data->file = [[AVAudioFile alloc] initForReading:url error:&error];
    if (error != nil) {
        CCLOG("Failed to load audio file %s: %s", _path.c_str(), [[error localizedDescription] UTF8String]);
        _data->file = nil;
        _status = STATUS_FAILED;
        return;
    }
    
    // Allocate the buffer
    _data->pcmb = [[AVAudioPCMBuffer alloc] initWithPCMFormat:_data->file.processingFormat
                                                frameCapacity:(AVAudioFrameCount)_data->file.length];
    [_data->file readIntoBuffer:_data->pcmb error:&error];
    if (error != nil) {
        CCLOG("Failed to load audio file %s: %s", _path.c_str(), [[error localizedDescription] UTF8String]);
        _data->file = nil;
        _data->pcmb = nil;
        _status = STATUS_FAILED;
        return;
    }
    
    // Retain objects and notify the AudioEngine we are done
    [_data->file retain];
    [_data->pcmb retain];
    _status = STATUS_LOADED;
}

/**
 * Disposes the audio data for this cache.
 *
 * This method differs from the destructor in that the file path is preserved, so the
 * cache can be reloaded if necessary.
 */
void AudioCache::dispose() {
    if (_data->pcmb != nil) {
        [_data->pcmb release];
        _data->pcmb = nil;
    }
    if (_data->file != nil) {
        [_data->file release];
        _data->file = nil;
    }
    delete _data;
    _data = nullptr;
    _status = STATUS_EMPTY;
}


#pragma mark -
#pragma mark Source Attributes

/**
 * Returns the file type for the source file
 *
 * The file type is returned as a string identifying the file suffix (e.g. "mp3",
 * "caf", "ogg", and so on).
 *
 * @return the file type for the source file
 */
std::string AudioCache::getFileType() const {
    size_t pos = _path.rfind(".");
    if (pos == std::string::npos) {
        return "UNKNOWN";
    }
    return _path.substr(pos+1);
}

/**
 * Returns the length of this audio source in seconds.
 *
 * @return the length of this audio source in seconds.
 */
double AudioCache::getDuration() const {
    if (getSampleRate() == 0) {
        return 0.0;
    }
    
    return (double)(getLength()/getSampleRate());
}

/**
 * Returns the sample rate of this audio source.
 *
 * @return the sample rate of this audio source.
 */
double AudioCache::getSampleRate() const {
    return _data->pcmb.format.sampleRate;
}

/**
 * Returns the frame length of this audio source.
 *
 * The frame length is the duration * the same rate.
 *
 * @return the frame length of this audio source.
 */
int64_t AudioCache::getLength() const {
    return _data->file.length;
}

/**
 * Returns the number of channels used by this audio source
 *
 * A value of 1 means mono, while 2 means sterio.  Depending on the file format,
 * other channels are possible.
 *
 * @return the number of channels used by this audio source
 */
int AudioCache::getChannels() const {
    return _data->pcmb.format.channelCount;
}


#pragma mark -
#pragma mark Asynchronous Loading

/**
 * Invokes all callbacks with the given success parameter.
 *
 * The callback functions will be removed once execution is completed.
 *
 * @param  success  whether loading was successful
 */
void AudioCache::invokeLoadCallbacks(bool success) {
    for(auto it = _loadCallbacks.begin(); it != _loadCallbacks.end(); ++it) {
        (*it)(success);
    }
    _loadCallbacks.clear();
}
#endif
