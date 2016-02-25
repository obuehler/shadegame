//
//  AudioPlayer.cpp
//  Cornell Extensions to Cocos2D
//
//  This module is a rewrite of the Cocos2d AudioPlayer to use AVFramework instead of OpenAL.
//  It provides a C++ wrapper of AVAudioPlayerNode. It represents a node in the mixer graph
//  of AVAudioEngine.  In our engine, we have a fixed number of nodes at all times.
//
//  This implementation is very similar to the original AudioPlayer except that we attach
//  play callbacks to this class, not AudioCache.  As it should be.
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

#include "AudioPlayer.h"
#include "AudioCache.h"
#include "base/ccUtils.h"

/** The default volume for all players */
#define DEFAULT_VOLUME  0.8
/** The minimum volume for a player ("stopping" the sound) */
#define MINIMUM_VOLUME  0.0

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
#pragma mark Audio Player

/**
 * Creates a new AudioPlayer
 *
 * This method simply initializes the default values of the attributes.  It does not place
 * the player in the mixer graph.  You must call the method init() to initialize this
 * player.
 */
AudioPlayer::AudioPlayer() :
_engine(nullptr),
_player(nullptr),
_buffer(nullptr),
_shadowBuffer(nullptr),
_loop(false),
_shadowLoop(false),
_paused(false),
_volume(0.0),
_shadowVolume(0.0),
_pauseTime(0.0),
_startTime(0.0),
_shadowTime(0.0),
_autodelete(false),
_currStamp(0),
_nextStamp(1) {
}

/**
 * Attaches this player to the AVAudioEngine mixer graph.
 *
 * @param  engine   wrapped instance of the AVAudioEngine
 */
void AudioPlayer::init(AVEngineInstance* engine) {
    _engine = engine;
    _player = new AVPlayerInstance();
    _player->player = [[AVAudioPlayerNode alloc] init];
    [_player->player retain];
    _player->player.volume = 0.001;
    _player->format = nil;

    // Add it to the mixer graph
    // No format for now.  May change format later.
    [_engine->engine attachNode:_player->player];
    [_engine->engine connect:_player->player to:_engine->engine.mainMixerNode format:nil];
}

/**
 * Removes this player from the mixer graph.
 *
 * This method differs from the destructor in that the player can be reattached with a
 * subsequent call to init().
 */
void AudioPlayer::dispose() {
    if (_engine == nullptr) {
        return;
    }
    
    if (_player->player != nil) {
        // Remove if from the mixer graph
        [_engine->engine disconnectNodeOutput:_player->player];
        [_engine->engine detachNode:_player->player];
        
        [_player->player release];
        _player->player = nil;
    }
    if (_player->format != nil) {
        [_player->format release];
        _player->format = nil;
    }
    
    delete _player;
    _player = nullptr;
    _buffer = nullptr;
    _engine = nullptr;
}


#pragma mark -
#pragma mark Buffer Attachment
/**
 * Attaches an AudioCache and readies it to play with the given volume and loop setting.
 *
 * The buffer can be played immediately. We have decoupled this functionality from play()
 * to allow a buffer to be stopped and started without reseting the buffer.
 *
 * @param  buffer   the audio source to play
 * @param  volume   the volume ot play the sound
 * @param  loop     whether to loop the sound indefinitely
 */
void AudioPlayer::attach(AudioCache* buffer, float volume, bool loop) {
    CCASSERT(_buffer == nullptr, "Attaching to an occupied audio instance");
    _paused = false;
    _buffer = buffer;
    _volume = volume;
    _loop = loop;

    _pauseTime = 0.0;
    _startTime = 0.0;
    
    _buffer->retain();
}

/**
 * Queues an AudioCache to play as soon as the current one is detached.
 *
 * This method is to allow the AudioEngine the illusion that a player is immediately
 * available once it is stopped.  This is not the case without clipping; the shadowed
 * buffer will start playing the next animation frame.
 *
 * @param  buffer   the audio source to play
 * @param  volume   the volume ot play the sound
 * @param  loop     whether to loop the sound indefinitely
 */
void AudioPlayer::attachShadow(AudioCache* buffer, float volume, bool loop) {
    CCASSERT(_shadowBuffer == nullptr, "Attaching to an occupied shadow instance");
    _shadowBuffer = buffer;
    _shadowVolume = volume;
    _shadowLoop = loop;
    _shadowTime = 0.0;

    _shadowBuffer->retain();
}

/**
 * Swaps the queued AudioCache with the active buffer, readying it for play
 *
 * The shadowBuffer is set to nullptr, allowing further sounds to be queued.
 */
void AudioPlayer::swapShadow() {
    _paused = false;
    _buffer = _shadowBuffer;
    _volume = _shadowVolume;
    _loop = _shadowLoop;
    
    _pauseTime = 0.0;
    _startTime = _shadowTime;
    _shadowBuffer = nullptr;
}


#pragma mark -
#pragma mark Playback Control

/**
 * Plays the current AudioCache buffer immediately.
 */
void AudioPlayer::play() {
    CCASSERT(_buffer != nullptr, "Attempt to play without an attached buffer");

    // We may still be playing with volume down (CLIPPING WORKAROUND)
    if (_player->player.playing) {
        [_player->player stop];
    }
    
    // Reconfigure graph if there is a format change
    AVAudioFormat* format = _buffer->getData()->pcmb.format;
    if (_player->format == nil || ![_player->format isEqual:format]) {
        _player->format = format;
        [format retain];
        
        [_engine->engine disconnectNodeOutput:_player->player];
        [_engine->engine connect:_player->player to:_engine->engine.mainMixerNode format:format];
    }
    
    // Capture the current timestamp by value
    unsigned long stamp = _nextStamp;
    id callback = ^( void ) {
        this->selfDelete(stamp);
    };
    
    // Queue up the sound for as soon as possible
    if (_loop) {
        if (_startTime > 0) {
            AVAudioFramePosition framePosition = _startTime * _buffer->getSampleRate();
            AVAudioFrameCount frameLength = (AVAudioFrameCount)(_buffer->getLength() - framePosition);
            [_player->player scheduleSegment:_buffer->getData()->file startingFrame:framePosition frameCount:frameLength
                                      atTime:nil completionHandler:nil];
        }
        [_player->player scheduleBuffer:_buffer->getData()->pcmb atTime:nil
                                options:AVAudioPlayerNodeBufferLoops completionHandler:callback];
    } else {
        if (_startTime > 0) {
            AVAudioFramePosition framePosition = _startTime * _buffer->getSampleRate();
            AVAudioFrameCount frameLength = (AVAudioFrameCount)(_buffer->getLength() - framePosition);
            [_player->player scheduleSegment:_buffer->getData()->file startingFrame:framePosition frameCount:frameLength
                                      atTime:nil completionHandler:callback];
        } else {
            [_player->player scheduleBuffer:_buffer->getData()->pcmb atTime:nil
                                    options:AVAudioPlayerNodeBufferInterrupts completionHandler:callback];
        }
    }
    _player->player.volume = _volume;
    [_player->player play];
}

/**
 * Pauses the current AudioCache.
 *
 * The AudioCache is not marked for deletion and will pick up from where it stopped
 * when the sound is resumed.  If the sound is already paused, this method will fail.
 *
 * @return true if the sound is successfully paused
 */
bool AudioPlayer::pause() {
    CCASSERT(_buffer != nullptr, "Attempt to pause without an attached buffer");
    if (_paused) {
        return false;
    }
    
    // Capture time at the pause.
    double sampleRate = _buffer->getSampleRate();
    if (sampleRate == 0) {
        _pauseTime = 0.0;
    } else {
        NSTimeInterval time = (NSTimeInterval)[_player->player playerTimeForNodeTime:_player->player.lastRenderTime].sampleTime;
        _pauseTime = time/sampleRate;
    }
    _startTime = _pauseTime;
    _paused = true;
    [_player->player pause];
    return true;
}

/**
 * Resumes the current AudioCache.
 *
 * If the sound was previously paused, this pick up from where it stopped.  If the
 * sound is not paused, this method will fail.
 *
 * @return true if the sound is successfully resumed
 */
bool AudioPlayer::resume() {
    CCASSERT(_buffer != nullptr, "Attempt to resume without an attached buffer");
    if (!_paused) {
        return false;
    }
    
    // Reschedule if time changed while paused.
    if (_startTime != _pauseTime) {
        setCurrentTime(_startTime, true);
    }
    
    _paused = false;
    [_player->player play];
    return true;
}

/**
 * Stops the current AudioCache, marking it for deletion.
 *
 * When a sound is stopped manually, no callback functions are called, and all existing
 * callbacks are deleted.
 *
 * The player will not be available for a new sound immediately.  The buffer will
 * not detach until the next animation frame.  If the player is needed immediately,
 * the sound should be attached to the shadow buffer.
 */
void AudioPlayer::stop() {
    CCASSERT(_buffer != nullptr, "Attempt to stop without an attached buffer");
    _player->player.volume = MINIMUM_VOLUME;
    _playCallbacks.clear();
    _currStamp = _nextStamp;  // Deletes without self-delete
}


#pragma mark -
#pragma mark Playback Attributes

/**
 * Returns the length of this sound being played, in seconds.
 *
 * If there is a shadow buffer present, this method will apply to the shadow buffer
 * instead.
 *
 * @return the length of this sound being played, in seconds.
 */
float AudioPlayer::getDuration() const {
    if (_shadowBuffer != nullptr) {
        return _shadowBuffer->getDuration();
    } else if (_buffer == nullptr) {
        return 0.0f;
    }
    
    return _buffer->getDuration();
}

/**
 * Returns the current position of the sound being played, in seconds.
 *
 * If there is a shadow buffer present, this method will apply to the shadow buffer
 * instead.
 *
 * @return the current position of the sound being played, in seconds.
 */
float AudioPlayer::getCurrentTime() const {
    if (_paused) {
        return _pauseTime;
    } else if (_shadowBuffer != nullptr) {
        return _startTime;
    } else if (_buffer == nullptr) {
        return 0.0f;
    }
    
    double sampleRate = _buffer->getSampleRate();
    if (sampleRate == 0) {
        return 0;
    }
    
    NSTimeInterval time = (NSTimeInterval)[_player->player playerTimeForNodeTime:_player->player.lastRenderTime].sampleTime;
    return time/sampleRate;
}

/**
 * Sets the current posiiton of the sound being played, in seconds.
 *
 * If the sound is paused, this will do nothing until the player is resumed.
 * Otherwise, this will stop and restart the sound at the new position.
 *
 * If there is a shadow buffer present, this method will apply to the shadow buffer
 * instead.
 *
 * @param  time     the new position of the player in the audio source
 * @param  force    whether to force the player to play, even if paused
 */
void AudioPlayer::setCurrentTime(float time, bool force) {
    if (_shadowBuffer != nullptr ||  (!force && _paused)) {
        _startTime = time;
        return;
    }
    
    AVAudioFramePosition framePosition = time * _buffer->getSampleRate();
    AVAudioFrameCount frameLength = (AVAudioFrameCount)(_buffer->getLength() - framePosition);
    
    // Increment the callback counter.
    // This preserves the callback functions, but they will NOT be called by the previous command.
    _nextStamp++;
    
    // Capture the current timestamp by value
    unsigned long stamp = _nextStamp;
    id callback = ^( void ) {
        this->selfDelete(stamp);
    };
    
    // Queue up the sound for as soon as possible
    if (_loop) {
        [_player->player scheduleSegment:_buffer->getData()->file startingFrame:framePosition frameCount:frameLength
                                  atTime:nil completionHandler:nil];
        [_player->player scheduleBuffer:_buffer->getData()->pcmb atTime:nil options:AVAudioPlayerNodeBufferLoops
                      completionHandler:callback];
    } else {
        [_player->player scheduleSegment:_buffer->getData()->file startingFrame:framePosition frameCount:frameLength
                                  atTime:nil completionHandler:callback];
    }
}

/**
 * Sets the volume (0 to 1) of the sound being played.
 *
 * If there is a shadow buffer present, this method will apply to the shadow buffer
 * instead.
 *
 * @param  volume   the volume (0 to 1) to play the sound.
 */
void AudioPlayer::setVolume(float volume) {
    if (_shadowBuffer != nullptr) {
        _shadowVolume = volume;
        return;
    }
    
    _volume = volume;
    if (_currStamp != _nextStamp) {
        _player->player.volume = volume;
    }
}

/**
 * Sets whether the current sound should play in an indefinite loop.
 *
 * If loop is false, then the sound will stop at its natural loop point.
 *
 * If there is a shadow buffer present, this method will apply to the shadow buffer
 * instead.
 *
 * @param  loop whether the current sound should play in an indefinite loop
 */
void AudioPlayer::setLoop(bool loop) {
    if (_shadowBuffer != nullptr) {
        _shadowLoop = loop;
        return;
    } else if (_loop == loop) {
        return;
    }
    
    // Increment the callback counter.
    // This preserves the callback functions, but they will NOT be called by the previous command.
    _nextStamp++;
    
    // Capture the current timestamp by value
    unsigned long stamp = _nextStamp;
    id callback = ^( void ) {
        this->selfDelete(stamp);
    };
    
    // Queue up the sound for as soon as possible
    if (_loop) {
        // Create a blank buffer to break us out of the loop at loop point.
        AVAudioPCMBuffer* blank = [[AVAudioPCMBuffer alloc] initWithPCMFormat:_player->format frameCapacity:0];
        [_player->player scheduleBuffer:blank atTime:nil options:AVAudioPlayerNodeBufferInterruptsAtLoop
                      completionHandler:callback];
    } else {
        // Add a loop
        [_player->player scheduleBuffer:_buffer->getData()->pcmb atTime:nil
                                options:AVAudioPlayerNodeBufferInterruptsAtLoop completionHandler:callback];
    }
    
    _loop = loop;
}

#pragma mark -
#pragma mark Buffer Clean-Up
/**
 * Marks the sound for deletion when it completes
 *
 * This method is called by the callback function associated with the sound
 * when it started playing.  This method takes a time stamp as an argument to
 * ensure that the sound was not previously deleted manually.
 *
 * @param  stamp    the time stamp to verify sound deletion
 */
void AudioPlayer::selfDelete(unsigned long stamp) {
    _currStamp = stamp;
    _autodelete = (_currStamp == _nextStamp);
}

/**
 * Detach the current AudioCache, making this player available again.
 *
 * The original buffer may still be playing, but at a 0-level volume.  The
 * current sound will continue until the end of the buffer, or it is stopped
 * by a subsequent call to play().
 */
void AudioPlayer::detach() {
    CCASSERT(_buffer != nullptr, "Detaching from an empty audio instance");
    
    _buffer->release();
    _buffer = nullptr;
    _paused = false;
    _volume = 0.0;
    _pauseTime = 0.0;
    _startTime = 0.0;
 
    // Remember if we were looping
    bool didLoop = _loop;
    _loop = false;
    
    // Will do nothing if callbacks cleared
    invokePlayCallbacks();
    _nextStamp++;
    
    // Swap in shadow buffer if appropriate
    if (_shadowBuffer != nullptr) {
        swapShadow();
        play();
    } else if (didLoop) {
        // Create a blank buffer to break us out of the loop at loop point.
        AVAudioPCMBuffer* blank = [[AVAudioPCMBuffer alloc] initWithPCMFormat:_player->format frameCapacity:0];
        [_player->player scheduleBuffer:blank atTime:nil options:AVAudioPlayerNodeBufferInterruptsAtLoop
                      completionHandler:nil];
    }
}

/**
 * Invokes all callback functions.
 *
 * These callback functions indicate that the sound has finished playing normally.
 * Callback functions are never called when a sound is stopped manually. The callback
 * functions will be removed once execution is completed.
 */
void AudioPlayer::invokePlayCallbacks() {
    for(auto it = _playCallbacks.begin(); it != _playCallbacks.end(); ++it) {
        (*it)();
    }
    _playCallbacks.clear();
}


#endif
