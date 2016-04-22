//
//  AudioPlayer.h
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

#ifndef __AUDIO_PLAYER_H_
#define __AUDIO_PLAYER_H_

#include "CCPlatformMacros.h"

NS_CC_BEGIN
namespace experimental {

/**
 * Forward reference to the AVAudioEngine source.
 *
 * This is a C++ wrapper for AVAudioFile and AVAudioPCMBuffer.  It is necessary for a PIMPL
 * implementation of AudioCache.  It is just POD; there is no associated constructor. The
 * fields of the struct are as follows:
 *
 *     AVAudioPCMBuffer* pcmb;  // The uncompressed source data
 *     AVAudioFile*      file;  // File reference for AVAudioEngine.
 */
struct AVAudioSource;
    
/**
 * Forward reference to the AVAudioEngine player
 *
 * This is a C++ wrapper for AVAudioPlayerNode and AVAudioFormat.  It is necessary for a PIMPL
 * implementation of AudioPlayer.  It is just POD; there is no associated constructor. The
 * fields of the struct are as follows:
 *
 *     AVAudioPlayerNode* player; // The player node for the AVAudioEngine
 *     AVAudioFormat*     format; // The sound format for the mixer graph
 */
struct AVPlayerInstance;
    
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

/** Forward reference to the audio cache */
class AudioCache;

    
#pragma mark -
#pragma mark Audio Player

/** 
 * Player for a single sound instance.
 *
 * An audio player can only play one sound at a time.  The audio engine is hence a collection
 * of multiple players in a mixer graph.  The engine attaches an AudioCache to a player, and the
 * player is responsible for playing that sound until done, or it is stopped manually.  The
 * AudioCache is then detached, and the player is available for another sound.
 *
 * There is a known issue with AVAudioPlayerNode that a hard stop can occasionally produce a 
 * clipping sound.  StackOverflow is littered with this issue and no good solutions.  This class
 * provides a workable, but imperfect solution: sounds are only stop at the end of the buffer,
 * unless completely necessary.  That is, if the sound stops on its own, there is no problem.
 * However, if the sound is stopped manually, then the player simply turns the volume to 0 and
 * allows it to play to completion while marking the buffer for deletion in a subsequent
 * animation frame.
 *
 * This solution has an unfortunate side effect: the engine may have no available players even
 * though all sounds are "stopped".  This is the purpose of the shadow buffer.  If the original
 * buffer is marked for deletion, then the engine can attach a second AudioCache to the shadow
 * buffer.  The original bufffer will be detached the next animation frame and the shadow buffer
 * will immediately start playing.  This allows the engine to fool the user that a player is
 * immediately available, even when it is not.
 *
 * IMPORTANT: For best performance, it is absolutely crucial that all sounds have exactly the 
 * same format. The same file format, the same sampling rate, the same number of channels.  Any 
 * change in format requires a reconfiguration of the mixer graph, and this can cause distortion
 * for rapid-fire sound effects.
 */
class AudioPlayer {
private:
    /** A cyclical reference to the AVAudioEngine (necessary to simplify PIMPL implementation) */
    AVEngineInstance* _engine;
    /** A reference to the player node in AVAudioEngine */
    AVPlayerInstance* _player;
    /** The buffer currently attached to this player for use */
    AudioCache* _buffer;
    
    /** Whether the player is currently playing in a loop */
    bool  _loop;
    /** Whether the player is paused */
    bool  _paused;
    /** The volume of the current sound being played */
    double _volume;
    
    /** The time (in the audio file) at which the sound was paused */
    double _pauseTime;
    /** The time (in the audio file) to resume the sound after a pause */
    double _startTime;
    
    /** Whether this player deleted the buffer itself (because the sound completed) */
    bool _autodelete;
    
    /** 
     * Current callback time stamp.
     * 
     * AVAudioPlayer will invoke a completion callback even if the sound is stopped manually. 
     * And to make matters worse, it does not invoke the callbacks immediately on stop.  We
     * need a way to ignore callbacks if we manually stop a sound.  This index allows us to
     * determine what callback functions we should be paying attention to.
     *
     * A sound is successfully deleted if _currStamp == _nextStamp.
     */
    unsigned long _currStamp;
    
    /**
     * The next callback time stamp to use.
     *
     * AVAudioPlayer will invoke a completion callback even if the sound is stopped manually.
     * And to make matters worse, it does not invoke the callbacks immediately on stop.  We
     * need a way to ignore callbacks if we manually stop a sound.  This index allows us to
     * determine what callback functions we should be paying attention to.
     *
     * A sound is successfully deleted if _currStamp == _nextStamp.
     */
    unsigned long _nextStamp;
    
    /** List of callbacks for when a sound is complete */
    std::vector< std::function<void()> > _playCallbacks;
    
    /** A queued AudioCache to play immediately once the current one is detached */
    AudioCache* _shadowBuffer;
    /** Whether to loop the queued AudioCache */
    bool _shadowLoop;
    /** The volume for the queued AudioCache */
    double _shadowVolume;
    /** The start time for the shadow instance */
    double _shadowTime;
    
    /**
     * Swaps the queued AudioCache with the active buffer, readying it for play
     *
     * The shadowBuffer is set to nullptr, allowing further sounds to be queued.
     */
    void swapShadow();
    
    
public:
#pragma mark Allocation
    /**
     * Creates a new AudioPlayer
     * 
     * This method simply initializes the default values of the attributes.  It does not place
     * the player in the mixer graph.  You must call the method init() to initialize this 
     * player.
     */
    AudioPlayer();
    
    /**
     * Disposes of this player, removing it from the mixer graph
     */
    ~AudioPlayer() { dispose(); }
    
    
    /**
     * Attaches this player to the AVAudioEngine mixer graph.
     *
     * @param  engine   wrapped instance of the AVAudioEngine
     */
    void init(AVEngineInstance* engine);

    /**
     * Removes this player from the mixer graph.
     * 
     * This method differs from the destructor in that the player can be reattached with a
     * subsequent call to init().
     */
    void dispose();


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
    void attach(AudioCache* buffer, float volume=1.0, bool loop=false);
    
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
    void attachShadow(AudioCache* buffer, float volume=1.0, bool loop=false);
    
    /**
     * Returns true if there is an AudioCache attached to this player
     *
     * A player with an occupied buffer is not available for another sound (though 
     * the shadow buffer my be available.
     *
     * @return true if there is an AudioCache attached to this player
     */
    bool isAttached() const { return _buffer != nullptr; }

    /**
     * Returns true if there is an AudioCache queued for later.
     *
     * A player can only have one shadow buffer at a time.
     *
     * @return true if there is an AudioCache queued for later.
     */
    bool isShadowed() const { return _shadowBuffer != nullptr; }

    /**
     * Returns a reference to the current active buffer
     *
     * @return a reference to the current active buffer
     */
    AudioCache* getBuffer() { return _buffer; }
    
    
#pragma mark Playback Control
    /**
     * Plays the current AudioCache buffer immediately.
     */
    void play();
    
    /**
     * Pauses the current AudioCache.
     *
     * The AudioCache is not marked for deletion and will pick up from where it stopped
     * when the sound is resumed.  If the sound is already paused, this method will fail.
     *
     * @return true if the sound is successfully paused
     */
    bool pause();

    /**
     * Resumes the current AudioCache.
     *
     * If the sound was previously paused, this pick up from where it stopped.  If the
     * sound is not paused, this method will fail.
     *
     * @return true if the sound is successfully resumed
     */
    bool resume();
    
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
    void stop();
    
    
#pragma mark Playback Attributes
    /**
     * Returns true if this player is currently paused
     *
     * @return true if this player is currently paused
     */
    bool  isPaused() const  { return _paused; }
    
    /**
     * Returns the length of this sound being played, in seconds.
     *
     * If there is a shadow buffer present, this method will apply to the shadow buffer
     * instead.
     *
     * @return the length of this sound being played, in seconds.
     */
    float getDuration() const;
    
    /**
     * Returns the current position of the sound being played, in seconds.
     *
     * If there is a shadow buffer present, this method will apply to the shadow buffer
     * instead.
     *
     * @return the current position of the sound being played, in seconds.
     */
    float getCurrentTime() const;
    
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
    void setCurrentTime(float time, bool force=false);
    
    /**
     * Returns the volume (0 to 1) of the sound being played.
     *
     * If there is a shadow buffer present, this method will apply to the shadow buffer
     * instead.
     *
     * @return the volume (0 to 1) of the sound being played.
     */
    float getVolume() const { return (_shadowBuffer == nullptr ? _volume : _shadowVolume); }
    
    /**
     * Sets the volume (0 to 1) of the sound being played.
     *
     * If there is a shadow buffer present, this method will apply to the shadow buffer
     * instead.
     *
     * @param  volume   the volume (0 to 1) to play the sound.
     */
    void setVolume(float volume);
    
    /**
     * Returns true if the current sound is in an indefinite loop.
     *
     * If there is a shadow buffer present, this method will apply to the shadow buffer
     * instead.
     *
     * @return true if the current sound is in an indefinite loop.
     */
    bool  getLoop() const   { return (_shadowBuffer == nullptr ? _loop : _shadowLoop); }
    
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
    void setLoop(bool loop);

    
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
    void selfDelete(unsigned long stamp);
    
    /**
     * Resets the deletion status after a self-deletion.
     *
     * This method is called by AudioEngine in the update() method for clean-up.
     */
    void resetDelete()          { _autodelete = false; }
    
    /**
     * Returns true if this player deleted the sound itself.
     * 
     * This method is called by the AudioEngine in the update() method to check
     * whether clean-up is necessary.
     */
    bool isSelfDeleted() const  { return _autodelete;  }
    
    /**
     * Returns true if the sound is marked by deletion.
     * 
     * A sound marked for deletion has not yet detached its buffer.  This will
     * be handled by the update() method in AudioEngine in the next animation
     * frame.
     */
    bool isDeleted() const      { return _currStamp == _nextStamp; }
    
    /**
     * Detach the current AudioCache, making this player available again.
     *
     * The original buffer may still be playing, but at a 0-level volume.  The
     * current sound will continue until the end of the buffer, or it is stopped
     * by a subsequent call to play().
     */
    void detach();

    /**
     * Invokes all callback functions.
     *
     * These callback functions indicate that the sound has finished playing normally.
     * Callback functions are never called when a sound is stopped manually. The callback 
     * functions will be removed once execution is completed.
     */
    void invokePlayCallbacks();
    
    /**
     * Removes all callbacks attached to this player.
     *
     * The callback functions will be removed without execution.
     */
    void clearPlayCallbacks() { _playCallbacks.clear(); }
    
    /**
     * Adds a callback function for when the sound is complete.
     *
     * Callback functions are only called when the sound completes normally.  They are always
     * ignored when the sound is stopped manually.
     *
     * @param  callback the playback callback function
     */
    void addPlayCallback(const std::function<void()>& callback) {
        _playCallbacks.push_back(callback);
    }
};

}
NS_CC_END
#endif // __AUDIO_PLAYER_H_
#endif

