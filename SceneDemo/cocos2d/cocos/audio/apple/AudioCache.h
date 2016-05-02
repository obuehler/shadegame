//
//  AudioCache.h
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

#ifndef __AUDIO_CACHE_H_
#define __AUDIO_CACHE_H_

#include "base/CCRef.h"
#include <vector>

// Status values for loading; we want values, not a C++ enum
/** AudioCache failed to read from file */
#define STATUS_FAILED  -2
/** AudioCache has no loaded information */
#define STATUS_EMPTY   -1
/** AudioCache is currently reading data from the file */
#define STATUS_READING  0
/** AudioCache has finished loading, but not notified the engine */
#define STATUS_LOADED   1
/** AudioCache is ready for use */
#define STATUS_READY    2

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

#pragma mark -
#pragma mark Audio Cache
    
/**
 * Buffer storing a loaded audio source.
 *
 * The class represents an audio source loaded into memory.  It provides callback support
 * for asynchronous loading, as well as basic information like duration and sample rate.
 */
class AudioCache : public Ref {
protected:
    /** The (relative) path to the source file */
    std::string     _path;
    /** A reference to the cache sound data for AVAudioEngine */
    AVAudioSource*  _data;
    /** The loading status of this audio cache */
    int _status;
    /** List of callbacks for when loading is complete */
    std::vector< std::function<void(bool)> > _loadCallbacks;

public:
#pragma mark Allocation
    /**
     * Creates a new AudioCache for the given file.
     *
     * This constructor only sets the file name and defaults; it does not load any data
     * from the source file.  Use the method readData() for that.
     *
     * @param   file    The sound source file
     */
    AudioCache(std::string file) : _data(nullptr), _status(STATUS_EMPTY) { _path = file; }
    
    /**
     * Disposes this new AudioCache, releasing all resources.
     */
    ~AudioCache() { dispose(); }
    
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
    void readData();
    
    /**
     * Disposes the audio data for this cache.
     *
     * This method differs from the destructor in that the file path is preserved, so the
     * cache can be reloaded if necessary.
     */
    void dispose();

    
#pragma mark Source Attributes

    /**
     * Returns the (relative) path to the source file
     * 
     * @return the (relative) path to the source file
     */
    std::string getSource() const   { return _path; }
    
    /**
     * Returns the file type for the source file
     *
     * The file type is returned as a string identifying the file suffix (e.g. "mp3", 
     * "caf", "ogg", and so on).
     *
     * @return the file type for the source file
     */
    std::string getFileType() const;
    
    /**
     * Returns a wrapped version of the AVAudioEngine source.
     *
     * See the comments for AVAudioSource for the data format of this struct. If the
     * status is STATUS_EMPTY or STATUS_FAILED, then this method returns nullptr.
     *
     * Returns a wrapped version of the AVAudioEngine source.
     */
    AVAudioSource* getData() const  { return _data; }
    
    /**
     * Returns the length of this audio source in seconds.
     *
     * @return the length of this audio source in seconds.
     */
    double  getDuration() const;

    /**
     * Returns the sample rate of this audio source.
     *
     * @return the sample rate of this audio source.
     */
    double  getSampleRate() const;
    
    /**
     * Returns the frame length of this audio source.
     *
     * The frame length is the duration * the same rate.
     *
     * @return the frame length of this audio source.
     */
    int64_t getLength() const;
    
    /**
     * Returns the number of channels used by this audio source
     *
     * A value of 1 means mono, while 2 means sterio.  Depending on the file format,
     * other channels are possible.
     *
     * @return the number of channels used by this audio source
     */
    int getChannels() const;

    
#pragma mark Asynchronous Loading

    /**
     * Returns the loading status of this audio source.
     *
     * The two primary statuses are STATUS_EMPTY and STATUS_READY.  Anything else is an
     * intermediate status used by the AudioEngine.
     *
     * @return the loading status of this audio source.
     */
    int  getStatus() const      { return _status; }
    
    /**
     * Sets the loading status of this audio source.
     *
     * The two primary statuses are STATUS_EMPTY and STATUS_READY.  Anything else is an
     * intermediate status used by the AudioEngine.
     *
     * @param  status   the loading status of this audio source.
     */
    void setStatus(int status)  { _status = status; }

    /**
     * Adds a callback function for when loading is complete.
     *
     * The callback function takes a single argument indicated whether loading was
     * successfully completed.
     *
     * Callback functions are only called when loading is asynchronous.  They are ignored
     * in synchronous loading.
     *
     * @param  callback the loading callback function
     */
    void addLoadCallback(const std::function<void(bool)>& callback) {
        _loadCallbacks.push_back(callback);
    }
    
    /**
     * Removes all callbacks attached to this audio cache.
     *
     * The callback functions will be removed without execution.
     */
    void clearLoadCallbacks() { _loadCallbacks.clear(); }

    /**
     * Invokes all callbacks with the given success parameter.
     *
     * These callbacks indicate that loading has completed (though perhaps not
     * successfully). The callback functions will be removed once execution is 
     * completed.
     *
     * @param  success  whether loading was successful
     */
    void invokeLoadCallbacks(bool success);
} ;

}
NS_CC_END

#endif // __AUDIO_CACHE_H_
#endif

