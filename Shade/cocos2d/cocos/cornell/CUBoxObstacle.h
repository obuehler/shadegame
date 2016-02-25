//
//  CUBoxObstacle.h
//  Cornell Extensions to Cocos2D
//
//  This class implements a rectangular physics object, and is the primary type of
//  physics object to use.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 11/24/15
//
#ifndef __CU_BOX_OBSTACLE_H__
#define __CU_BOX_OBSTACLE_H__

#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include "CUSimpleObstacle.h"


NS_CC_BEGIN
#pragma mark -
#pragma mark Box Obstacle
/**
 * Box-shaped model to support collisions.
 *
 * Given the name Box2D, this is your primary model class.  Most of the time,
 * unless it is a player controlled avatar, you do not even need to subclass
 * BoxObject.  Look through some of our samples and see how many times we use 
 * this class.
 *
 * Unless otherwise specified, the center of mass is as the center.
 */
class CC_DLL BoxObstacle : public SimpleObstacle {
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CC_DISALLOW_COPY_AND_ASSIGN(BoxObstacle);

protected:
    /** Shape information for this box */
    b2PolygonShape _shape;
    /** A cache value for the fixture (for resizing) */
    b2Fixture* _geometry;
    /** The width and height of the box */
    Size _dimension;
    
    
#pragma mark -
#pragma mark Scene Graph Methods

    /**
     * Resets the polygon vertices in the shape to match the dimension.
     *
     * This is an internal method and it does not mark the physics object as dirty.
     *
     * @param  size The new dimension (width and height)
     */
    void resize(const Size& size);
    
    /**
     * Redraws the outline of the physics fixtures to the debug node
     *
     * The debug node is use to outline the fixtures attached to this object.
     * This is very useful when the fixtures have a very different shape than
     * the texture (e.g. a circular shape attached to a square texture).
     */
    virtual void resetDebugNode() override;

    
public:
#pragma mark -
#pragma mark Static Constructors
    /**
     * Creates a new box object at the origin with no size.
     *
     * @return  An autoreleased physics object
     */
    static BoxObstacle* create();
    
    /**
     * Creates a new box object at the given point with no size.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos  Initial position in world coordinates
     *
     * @return  An autoreleased physics object
     */
    static BoxObstacle* create(const Vec2& pos);
    
    /**
     * Creates a new box object of the given dimensions.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos  Initial position in world coordinates
     * @param  size The box size (width and height)
     *
     * @return  An autoreleased physics object
     */
    static BoxObstacle* create(const Vec2& pos, const Size& size);
    
    
#pragma mark -
#pragma mark Dimensions
    /**
     * Returns the dimensions of this box
     *
     * @return the dimensions of this box
     */
    const Size& getDimension() const { return _dimension; }
    
    /**
     * Sets the dimensions of this box
     *
     * @param value  the dimensions of this box
     */
    void setDimension(const Size& value)         { resize(value); markDirty(true); }
    
    /**
     * Sets the dimensions of this box
     *
     * @param width   The width of this box
     * @param height  The height of this box
     */
    void setDimension(float width, float height) { setDimension(Size(width,height)); }

    /**
     * Returns the box width
     *
     * @return the box width
     */
    float getWidth() const { return _dimension.width; }
    
    /**
     * Sets the box width
     *
     * @param value  the box width
     */
    void setWidth(float value) { setDimension(value,_dimension.height); }
    
    /**
     * Returns the box height
     *
     * @return the box height
     */
    float getHeight() const { return _dimension.height; }
    
    /**
     * Sets the box height
     *
     * @param value  the box height
     */
    void setHeight(float value) { setDimension(_dimension.width,value); }
    
    
#pragma mark -
#pragma mark Physics Methods
    
    /**
     * Create new fixtures for this body, defining the shape
     *
     * This is the primary method to override for custom physics objects
     */
    virtual void createFixtures() override;
    
    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects
     */
    virtual void releaseFixtures() override;
    

#pragma mark -
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /*
     * Creates a new box object at the origin.
     */
    BoxObstacle(void) : SimpleObstacle(), _geometry(nullptr) { }
    
    /**
     * Initializes a new box object at the origin with no size.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(Vec2::ZERO,Size::ZERO); }
    
    /**
     * Initializes a new box object at the given point with no size.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos  Initial position in world coordinates
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2& pos) override { return init(pos,Size::ZERO); }
    
    /**
     * Initializes a new box object of the given dimensions.
     *
     * The scene graph is completely decoupled from the physics system.  
     * The node does not have to be the same size as the physics body. We 
     * only guarantee that the scene graph node is positioned correctly 
     * according to the drawing scale.
     *
     * @param  pos  Initial position in world coordinates
     * @param  size The box size (width and height)
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2& pos, const Size& size);
    
};

NS_CC_END
#endif /* defined(__CU_BOX_OBSTACLE_H__) */