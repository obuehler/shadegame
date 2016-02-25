//
//  CUSound.cpp
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
#include "CUSound.h"

NS_CC_BEGIN

/**
 * Creates a new sound object for the given source file.
 *
 * The sound file has no duration, indicating that it was not preloaded.
 *
 * @param  source   the source file for the sound
 *
 * @return an autoreleased sound object
 */
Sound* Sound::create(std::string source) {
    Sound *sound = new (std::nothrow) Sound();
    if (sound && sound->init(source)) {
        sound->autorelease();
        return sound;
    }
    CC_SAFE_DELETE(sound);
    return nullptr;
}

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
Sound* Sound::create(std::string source, float duration) {
    Sound *sound = new (std::nothrow) Sound();
    if (sound && sound->init(source,duration)) {
        sound->autorelease();
        return sound;
    }
    CC_SAFE_DELETE(sound);
    return nullptr;
}

NS_CC_END