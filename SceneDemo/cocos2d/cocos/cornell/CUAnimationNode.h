//
//  CUAnimationNode.h
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
#ifndef __CU_FILM_STRIP_H__
#define __CU_FILM_STRIP_H__

#include "CUPolygonNode.h"
#include <2d/CCSprite.h>


NS_CC_BEGIN

#pragma mark -
#pragma mark AnimationNode

/**
 * Class to support simple film strip animation
 *
 * A filmstrip is just a sprite sheet.  The creation API is very similar to 
 * the class Sprite.  However, you must also specify the rows and columns in 
 * the sprite sheet so that it can break up the images for you.
 *
 * The constructors always set this object equal to a rectangle the same size as
 * a single frame in the sprite sheet.  However, this class extends PolygonNode and 
 * not Sprite.  Therefore you could conceivably animate the filmstrip over polygons.  
 * This can have undesirable effects if the polygon coordinate extend beyond a single 
 * animation frame; the Cocos2d renderer does not allow us to wrap a single frame of 
 * a texture atlas.
 *
 * For example, suppose you have a filmstrip where each frame has a given width and 
 * height.  Then setting the polygon to a triangle with vertices (0,0), (width/2, height), 
 * and (width,height) is okay.  However, the vertices (0,0), (width, 2*height), and
 * (2*width, height) is not.
 */
class CC_DLL AnimationNode : public PolygonNode {
    
private:
    /** The number of columns in this filmstrip */
    int _cols;
    /** The number of frames in this filmstrip */
    int _size;
    /** The active animation frame */
    int _frame;
    /** The size of a single animation frame (different from active polygon) */
    Rect _bounds;
    
public:
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
    static AnimationNode* create(Texture2D* texture, int rows, int cols);
    
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
    static AnimationNode* create(Texture2D* texture, int rows, int cols, int size);

    
#pragma mark Attribute Accessors
    /**
     * Returns the number of frames in this filmstrip.
     *
     * @return the number of frames in this filmstrip.
     */
    int getSize()   { return _size; }
    
    /**
     * Returns the current active frame.
     *
     * @return the current active frame.
     */
    unsigned int getFrame()  { return _frame; }
    
    /**
     * Sets the active frame as the given index.
     *
     * If the frame index is invalid, an error is raised.
     *
     * @param frame the index to make the active frame
     */
    void setFrame(int frame);

    
#pragma mark Internal Constructors
CC_CONSTRUCTOR_ACCESS:
    // WE FOLLOW THE TEMPLATE FOR SPRITE HERE
    /**
     * Constructs a new empty filmstrip
     *
     * You should never call this constructor.  Call create() instead.
     */
    AnimationNode();
    
    /**
     * Disposes of a filmstrip and its contents
     */
    ~AnimationNode() { }

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
    bool initWithFilmstrip(Texture2D* texture, int rows, int cols, int size);

};

NS_CC_END

#endif /* defined(__CU_FILM_STRIP_H__) */