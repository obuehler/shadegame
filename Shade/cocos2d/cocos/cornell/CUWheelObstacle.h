//
//  CUWheelObstacle.h
//  Cornell Extensions to Cocos2D
//
//  This class implements a circular Physics object. We do not use it in any of our
//  samples,  but it is included for your education.  Note that the shape must be
//  circular, not elliptical.  If you want to make an ellipse, you will need to use
//  the PolygonObstacle class.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 11/24/15
//
#ifndef __CU_WHEEL_OBSTACLE_H__
#define __CU_WHEEL_OBSTACLE_H__

#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include "CUSimpleObstacle.h"


NS_CC_BEGIN

#pragma mark -
#pragma mark Wheel Obstacle

/**
 * Circle-shaped model to support collisions.
 *
 * Note that the shape must be circular, not elliptical. If you want to make an ellipse,
 * you will need to use the PolygonObstacle class.
 *
 * Unless otherwise specified, the center of mass is as the center.
 */
class CC_DLL WheelObstacle : public SimpleObstacle {
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CC_DISALLOW_COPY_AND_ASSIGN(WheelObstacle);
    
protected:
    /** Shape information for this box */
    b2CircleShape _shape;
    /** A cache value for the fixture (for resizing) */
    b2Fixture* _geometry;

    
#pragma mark -
#pragma mark Scene Graph Methods

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
     * Creates a new wheel object at the origin with no radius.
     *
     * @return  An autoreleased physics object
     */
    static WheelObstacle* create();
    
    /**
     * Creates a new wheel object at the given point with no radius.
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
    static WheelObstacle* create(const Vec2& pos);
    
    /**
     * Creates a new wheel object of the given radius.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos      Initial position in world coordinates
     * @param  radius   The wheel radius
     *
     * @return  An autoreleased physics object
     */
    static WheelObstacle* create(const Vec2& pos, float radius);

    
#pragma mark -
#pragma mark Dimensions
    /**
     * Returns the radius of this circle
     *
     * @return the radius of this circle
     */
    float getRadius() const { return _shape.m_radius; }
    
    /**
     * Sets the radius of this circle
     *
     * @param value  the radius of this circle
     */
    void setRadius(float value) { _shape.m_radius = value; markDirty(true); }
    
    
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
     * Creates a new wheel object at the origin.
     */
    WheelObstacle(void) : SimpleObstacle(), _geometry(nullptr) { }
    
    /**
     * Initializes a new wheel object at the origin with no size.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(Vec2::ZERO,0.0); }
    
    /**
     * Initializes a new wheel object at the given point with no size.
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
    virtual bool init(const Vec2& pos) override { return init(pos,0.0); }
    
    /**
     * Initializes a new wheel object of the given dimensions.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos      Initial position in world coordinates
     * @param  radius   The wheel radius
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2& pos, float radius);
};

NS_CC_END
#endif /* defined(__CU_WHEEL_OBSTACLE_H__) */