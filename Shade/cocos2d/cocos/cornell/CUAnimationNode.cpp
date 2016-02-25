//
//  CUAnimationNode.cpp
//  Cornell Extensions to Cocos2D
//
//  Animation in Cocos2d is extremely heavy-weight.  You have to create a lot of
//  classes just to cycle through one simple filmstrip.  While there are many reasons
//  for this engineering decision (many of which I disagree with), it makes it very
//  difficult to get started with animation in Cocos2d.
//
//  This class helps makes things simple again.  It provides a straight-forward
//  filmstrip API, similar to what you saw in the intro class.  However, note that
//  this class extends PolygonNode and not Sprite.  Therefore, you could conceivably
//  animate the filmstrip over polygons.  However, this can have undesirable effects
//  if the polygon coordinates extend beyond a single animation frame; the Cocos2d
//  renderer does not allow us to wrap a single frame of a texture atlas.
//
//  Author: Walker White
//  Version: 12/10/15
//

#include "CUAnimationNode.h"


NS_CC_BEGIN

#pragma mark -
#pragma mark Static Constructors

/**
 * Creates a new filmstrip node from the given texture.
 *
 * The size of the node is equal to the size of a single frame in the filmstrip.
 * To resize the node, scale it up or down.  Do NOT change the polygon, as that
 * will interfere with the animation.
 *
 * @param texture   The texture image to use
 * @param rows      The number of rows in the filmstrip
 * @param cols      The number of columns in the filmstrip
 *
 * @retain  a reference to this texture
 * @return The allocated filmstrip as an autorelease object
 */
AnimationNode* AnimationNode::create(Texture2D* texture, int rows, int cols) {
    AnimationNode *filmStrip = new (std::nothrow) AnimationNode();
    if (filmStrip && filmStrip->initWithFilmstrip(texture, rows, cols, rows*cols)) {
        filmStrip->autorelease();
        return filmStrip;
    }
    CC_SAFE_DELETE(filmStrip);
    return nullptr;
}

/**
 * Creates a new filmstrip from the given texture.
 *
 * The parameter size is to indicate that there are unused frames in the
 * filmstrip.  The value size must be less than or equal to rows*cols, or
 * this constructor will raise an error.
 *
 * The size of the node is equal to the size of a single frame in the filmstrip.
 * To resize the node, scale it up or down.  Do NOT change the polygon, as that
 * will interfere with the animation.
 *
 * @param texture   The texture image to use
 * @param rows      The number of rows in the filmstrip
 * @param cols      The number of columns in the filmstrip
 * @param size      The number of frames in the filmstrip
 *
 * @retain  a reference to this texture
 * @return The allocated filmstrip as an autorelease object
 */
AnimationNode* AnimationNode::create(Texture2D* texture, int rows, int cols, int size) {
    AnimationNode *filmStrip = new (std::nothrow) AnimationNode();
    if (filmStrip && filmStrip->initWithFilmstrip(texture, rows, cols, size)) {
        filmStrip->autorelease();
        return filmStrip;
    }
    CC_SAFE_DELETE(filmStrip);
    return nullptr;
}


#pragma mark -
#pragma mark Internal Constructors

/**
 * Constructs a new empty filmstrip
 *
 * You should never call this constructor.  Call create() instead.
 */
AnimationNode::AnimationNode() :
_cols(0),
_size(0),
_frame(0),
_bounds(Rect::ZERO) {
    _name = "AnimationNode";
}

/**
 * Initializes the film strip with the given texture.
 *
 * The parameter size is to indicate that there are unused frames in the
 * filmstrip.  The value size must be less than or equal to rows*cols, or
 * this constructor will raise an error.
 *
 * The size of the node is equal to the size of a single frame in the filmstrip.
 * To resize the node, scale it up or down.  Do NOT change the polygon, as that
 * will interfere with the animation.
 *
 * @param texture   The texture image to use
 * @param rows      The number of rows in the filmstrip
 * @param cols      The number of columns in the filmstrip
 * @param size      The number of frames in the filmstrip
 *
 * @retain  a reference to this texture
 * @return True if initialization was successful; false otherwise.
 */
bool AnimationNode::initWithFilmstrip(Texture2D* texture, int rows, int cols, int size) {
    CCASSERT(size <= rows*cols, "ERROR: Invalid strip size");
    
    this->_cols = cols;
    this->_size = size;
    _bounds.size = texture->getContentSize();
    _bounds.size.width /= cols;
    _bounds.size.height /= rows;
    return this->initWithTexture(texture, _bounds);
}


#pragma mark -
#pragma mark Attribute Accessors

/**
 * Sets the active frame as the given index.
 *
 * If the frame index is invalid, an error is raised.
 *
 * @param frame the index to make the active frame
 */
void AnimationNode::setFrame(int frame) {
    CCASSERT(frame >= 0 && frame < _size, "ERROR: Invalid animation frame");

    _frame = frame;
    float x = (frame % _cols)*_bounds.size.width;
    float y = _texture->getContentSize().height - (1+frame/_cols)*_bounds.size.height;
    shiftPolygon(x-_bounds.origin.x, y-_bounds.origin.y);
    _bounds.origin.set(x,y);
}

NS_CC_END
