//
//  CUSound.h
//  Cornell Extensions to Cocos2D
//
//  This method provides a platform-independent way of refering to a (potentially)
//  loaded sound asset. Technically, sounds are just referred to be file name, so
//  this class seems unnecessary.  However there is a lot of hidden functionality
//  in the experimental AudioEngine for querying information like play duration or
//  file format.  We have made changes to the AudioEngine to expose this information.
//  As AudioEngine is marked as experimental anyway, we felt this was okay.
//
//  Author: Walker White
//  Version: 12/10/15
//
#ifndef __CU_SOUND_H__
#define __CU_SOUND_H__

#include <cocos2d.h>
#include <audio/include/AudioEngine.h>

NS_CC_BEGIN

#define AENG experimental::AudioEngine

#pragma mark -
#pragma mark Sound Class

class SoundLoader;

/**
 * Class provides a reference to a (potentially) loaded asset.
 *
 * We say potentially because Android has substantially inferior sound capabilities
 * and cannot preload sounds. This means that the asset handling for Android is very
 * different from other platforms.  To avoid this, we introduce this class as a way
 * of handling the sound across all platforms.  It can refer to a sound that is
 * preloaded, or one that should be loaded when used.
 *
 * This class uses Cocos2d reference counting for memory management.  The constructors
 * are protected because only the SoundLoader should construct sounds.
 */
class CC_DLL Sound : public Ref {
private:
    /** This macro disables the copy constructor (not allowed on assets) */
    CC_DISALLOW_COPY_AND_ASSIGN(Sound);
    
protected:
    /** The source file for the sound */
    std::string _source;
    /** The duration of the sound asset */
    float _duration;
    
public:
#pragma mark Attributes
    /**
     * Returns the source file for this sound.
     *
     * @return the source file for this sound.
     */
    const std::string& getSource() const { return _source; }
    
    /**
     * Returns the file suffix for this sound.
     *
     * Until we expose more functionality about the encoding, this is a poor
     * man's way of determining the file format.
     *
     * @return the file suffix for this sound.
     */
    std::string getSuffix() const {
        size_t pos = _source.rfind(".");
        return (pos == std::string::npos ? "" : _source.substr(pos));
    }
    
    /**
     * Returns the duration of this sound file.
     *
     * If the file has not been preloaded, we cannot determine the duration.
     * In that case, the duration is experimental::AudioEngine::TIME_UNKNOWN.
     *
     * @return the duration of this sound file.
     */
    float getDuration() const { return _duration; }
    
    /**
     * Returns true if the sound file was preloaded.
     *
     * This should return true on any platform that is not Android or Linux.
     *
     * @return true if the sound file was preloaded.
     */
    bool  isPreloaded() const { return _duration != AENG::TIME_UNKNOWN; }
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates a new, uninitialized sound object.
     */
    Sound() : Ref(), _source(""), _duration(0.0f) {}
    
    /**
     * Deletes this sound object
     *
     * The sound should be unloaded before deletion.
     */
    ~Sound() { CCASSERT(_source.empty(), "Sound asset was not unloaded"); }
    
    /**
     * Creates a new sound object for the given source file.
     *
     * The sound file has no duration, indicating that it was not preloaded.
     *
     * @param  source   the source file for the sound
     *
     * @return an autoreleased sound object
     */
    static Sound* create(std::string source);
    
    /**
     * Creates a new sound object for the given source file.
     *
     * The sound file has a duration, indicating that it was preloaded.
     *
     * @param  source   the source file for the sound
     * @param  duration the duration of the sound file
     *
     * @return an autoreleased sound object
     */
    static Sound* create(std::string source, float duration);
    
    /**
     * Initializes a new sound object for the given source file.
     *
     * The sound file has no duration, indicating that it was not preloaded.
     *
     * @param  source   the source file for the sound
     *
     * @return true if the object was initialized successfully
     */
    bool init(std::string source) {
        _source = source; _duration = experimental::AudioEngine::TIME_UNKNOWN;
        return true;
    }
    
    /**
     * Initializes a new sound object for the given source file.
     *
     * The sound file has a duration, indicating that it was preloaded.
     *
     * @param  source   the source file for the sound
     * @param  duration the duration of the sound file
     *
     * @return true if the object was initialized successfully
     */
    bool init(std::string source, float duration) {
        _source = source; _duration = duration;
        return true;
    }
    
    /** Allow the sound loader access to the constructors */
    friend class SoundLoader;
};

NS_CC_END

#endif /* defined(__CU_SOUND_H__) */
