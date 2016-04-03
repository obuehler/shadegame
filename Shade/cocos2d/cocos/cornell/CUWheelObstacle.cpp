//
//  CUWheelObstacle.cpp
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

#include "CUWheelObstacle.h"


NS_CC_BEGIN

/** How many line segments to use to draw a circle */
#define BODY_DEBUG_SEGS 12

#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a new wheel object at the origin with no radius.
 *
 * @return  An autoreleased physics object
 */
WheelObstacle* WheelObstacle::create() {
    WheelObstacle* obstacle = new (std::nothrow) WheelObstacle();
    if (obstacle && obstacle->init()) {
        obstacle->autorelease();
        return obstacle;
    }
    CC_SAFE_DELETE(obstacle);
    return nullptr;
}

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
WheelObstacle* WheelObstacle::create(const Vec2& pos) {
    WheelObstacle* obstacle = new (std::nothrow) WheelObstacle();
    if (obstacle && obstacle->init(pos)) {
        obstacle->autorelease();
        return obstacle;
    }
    CC_SAFE_DELETE(obstacle);
    return nullptr;
}

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
WheelObstacle* WheelObstacle::create(const Vec2& pos, float radius) {
    WheelObstacle* obstacle = new (std::nothrow) WheelObstacle();
    if (obstacle && obstacle->init(pos,radius)) {
        obstacle->autorelease();
        return obstacle;
    }
    CC_SAFE_DELETE(obstacle);
    return nullptr;
}


#pragma mark -
#pragma mark Initializers

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
bool WheelObstacle::init(const Vec2& pos, float radius, const b2Filter* const filter) {
    Obstacle::init(pos);
	_filterPtr = filter;
    _geometry = nullptr;
    _shape.m_radius = radius;
    return true;
}


#pragma mark -
#pragma mark Scene Graph Methods

/**
 * Redraws the outline of the physics fixtures to the debug node
 *
 * The debug node is use to outline the fixtures attached to this object.
 * This is very useful when the fixtures have a very different shape than
 * the texture (e.g. a circular shape attached to a square texture).
 *
 * Unfortunately, the current implementation is very inefficient.  Cocos2d
 * does not batch drawnode commands like it does Sprites or PolygonSprites.
 * Therefore, every distinct DrawNode is a distinct OpenGL call.  This can
 * really hurt framerate when debugging mode is on.  Ideally, we would refactor
 * this so that we only draw to a single, master draw node.  However, this
 * means that we would have to handle our own vertex transformations, instead
 * of relying on the transforms in the scene graph.
 */
void WheelObstacle::resetDebugNode() {
    Poly2 poly;
    Size size(2*getRadius()*_drawScale.x,2*getRadius()*_drawScale.y);
    poly.setEllipse(Vec2::ZERO,size,BODY_DEBUG_SEGS);
    _debug->setPolygon(poly);
}

#pragma mark -
#pragma mark Physics Methods
/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void WheelObstacle::createFixtures() {
    if (_body == nullptr) {
        return;
    }
    
    releaseFixtures();
    
    // Create the fixture
    _fixture.shape = &_shape;
	if (_filterPtr != nullptr) _fixture.filter = *_filterPtr;
    _geometry = _body->CreateFixture(&_fixture);
    markDirty(false);
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects
 */
void WheelObstacle::releaseFixtures() {
    if (_geometry != nullptr) {
        _body->DestroyFixture(_geometry);
        _geometry = nullptr;
    }
}

NS_CC_END