//
//  SCProgressBar.h
//  SceneDemo
//
//  This module provides a simple progress bar that is defined by a single
//  texture atlas.
//
//  Author: Walker White
//  Version: 4/2/16
//
#include "SCProgressBar.h"

USING_NS_CC;

/** So our interior shape is not degenerate */
#define MIN_EXPANSE 0.1f

/** Creates a new uninitialized Progress Bar */
ProgressBar::ProgressBar() :
_progress(0),
_background(nullptr),
_start(nullptr),
_interior(nullptr),
_finish(nullptr) {
}

/**
 * Creates a new progress bar for the given atlas.
 *
 * The atlas must consist of two equal size rounded rectangles of
 * equal with.  They should be stacked on top of each other and
 * separated by a pixel to prevent bleeding.
 *
 * @param atlas the texture atlas
 *
 * @return  An autoreleased physics object
 */
ProgressBar* ProgressBar::create(Texture2D* atlas) {
    ProgressBar *node = new (std::nothrow) ProgressBar();
    if (node && node->init(atlas)) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

/**
 * Initializes a new progress bar for the given atlas.
 *
 * The atlas must consist of two equal size rounded rectangles of
 * equal with.  They should be stacked on top of each other and
 * separated by a pixel to prevent bleeding.
 *
 * @param atlas the texture atlas
 *
 * @return  true if the progress bar initialized properly, false otherwise.
 */
bool ProgressBar::init(Texture2D* atlas) {
    if (Node::init()) {
        _fullsize = atlas->getContentSizeInPixels();
        _fullsize.height = (_fullsize.height-1)/2;
        setContentSize(_fullsize);
        
        _background = PolygonNode::createWithTexture(atlas, Rect(Vec2(0,_fullsize.height+1), _fullsize));
        _background->setAnchorPoint(Vec2::ZERO);
        addChild(_background);

        _capsize = _fullsize;
        _capsize.width = _fullsize.height/2;
        
        _start = PolygonNode::createWithTexture(atlas, Rect(Vec2::ZERO, _capsize));
        _start->setAnchorPoint(Vec2::ZERO);
        _start->setPosition(Vec2::ZERO);
        addChild(_start);
        
        _expanse.origin = Vec2(_capsize.width,0);
        _expanse.size   = Size(MIN_EXPANSE,_capsize.height);
        
        _interior = PolygonNode::createWithTexture(atlas, _expanse);
        _interior->setAnchorPoint(Vec2::ZERO);
        _interior->setPosition(_expanse.origin);
        addChild(_interior);
        
        _finish = PolygonNode::createWithTexture(atlas, Rect(Vec2(_fullsize.width-_capsize.width,0), _capsize));
        _finish->setAnchorPoint(Vec2::ZERO);
        _finish->setPosition(Vec2(_capsize.width+_expanse.size.width,0));
        addChild(_finish);
        
        return true;
    }
    return false;
}

/**
 * Sets the current progress of this bar
 *
 * The progress is always a value between 0 (no progress) and 1
 * (completed progress).  If the value is outside that range, this
 * method will clamp it.
 *
 * Calling this method will update the child nodes to correctly
 * display the progress.
 *
 * @value the current progress of this bar
 */
void ProgressBar::setProgress(float value) {
    // Clamp value if necessary.
    if (value < 0) value = 0.0f;
    if (value > 1) value = 1.0f;
    
    _expanse.size.width = _fullsize.width-2*_capsize.width-MIN_EXPANSE;
    _expanse.size.width *= value;
    _expanse.size.width += MIN_EXPANSE;
    
    _interior->setPolygon(_expanse);
    _finish->setPosition(Vec2(_capsize.width+_expanse.size.width,0));
    
    _progress = value;
}
