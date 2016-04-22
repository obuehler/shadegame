//
//  CUTTFont.cpp
//  Cornell Extensions to Cocos2D
//
//  This method provides a platform-independent way of refering to a loaded True Type
//  font asset. Depending on the need, Cocos2D asks you to refer to a True Type font
//  by either its TTConfig information or its FontAtlas.  This class abstracts both of
//  these into a single package for uniform usage.
//
//  Author: Walker White
//  Version: 12/10/15
//
#include "CUTTFont.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <sstream>
#endif

NS_CC_BEGIN

/**
 * Creates a string that may be used as a unique identifier for a font, given a source
 * name and size.
 *
 * @param  source   the source file for the font
 * @param  size     the font size
 *
 * @return a string unique to the given parameters
 */
std::string TTFont::buildIdentifier(std::string source, float size) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::stringstream ss;
    ss << source << "::" << size;
    return ss.str();
#else
    return source + "::" + std::to_string(size);
#endif
}

/**
 * Creates a new font object for the given source file and size.
 *
 * The font object must have a size in addition to its source file
 *
 * @param  source   the source file for the font
 * @param  size     the font size
 *
 * @return an autoreleased font object
 */
TTFont* TTFont::create(std::string source, float size) {
    TTFont *font = new (std::nothrow) TTFont();
    
    if (font && font->init(source,size)) {
        font->autorelease();
        return font;
    }
    CC_SAFE_DELETE(font);
    return nullptr;
}

/**
 * Initializes a new sound object for the given source file.
 *
 * The font object must have a size in addition to its source file
 *
 * @param  source   the source file for the font
 * @param  size     the font size
 *
 * @return true if the font was initialized successfully
 */
bool TTFont::init(std::string source, float size) {
    _source = source;
    _ident  = buildIdentifier(source,size);
    _config = TTFConfig(source.c_str(),size,GlyphCollection::DYNAMIC);
    return true;
}

NS_CC_END