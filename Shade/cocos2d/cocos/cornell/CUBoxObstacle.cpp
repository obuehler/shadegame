//
//  CUBoxObstacle.cpp
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

#include "CUBoxObstacle.h"

NS_CC_BEGIN


#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a new box object at the origin and no size.
 *
 * @return  An autoreleased physics object
 */
BoxObstacle* BoxObstacle::create() {
    BoxObstacle* obstacle = new (std::nothrow) BoxObstacle();
    if (obstacle && obstacle->init()) {
        obstacle->autorelease();
        return obstacle;
    }
    CC_SAFE_DELETE(obstacle);
    return nullptr;
}

/**
 * Creates a new physics object at the given point with no size.
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
BoxObstacle* BoxObstacle::create(const Vec2& pos) {
    BoxObstacle* obstacle = new (std::nothrow) BoxObstacle();
    if (obstacle && obstacle->init(pos)) {
        obstacle->autorelease();
        return obstacle;
    }
    CC_SAFE_DELETE(obstacle);
    return nullptr;
}

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
BoxObstacle* BoxObstacle::create(const Vec2& pos, const Size& size) {
    BoxObstacle* obstacle = new (std::nothrow) BoxObstacle();
    if (obstacle && obstacle->init(pos,size)) {
        obstacle->autorelease();
        return obstacle;
    }
    CC_SAFE_DELETE(obstacle);
    return nullptr;
}


#pragma mark -
#pragma mark Initialization Methods
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
 */
bool BoxObstacle::init(const Vec2& pos, const Size& size) {
    Obstacle::init(pos);
    _geometry = nullptr;
    resize(size);
    return true;
}

/**
 * Resets the polygon vertices in the shape to match the dimension.
 *
 * @param  size The new dimension (width and height)
 */
void BoxObstacle::resize(const Size& size) {
    // Make the box with the center in the center
    _dimension = size;
    b2Vec2 corners[4];
    corners[0].x = -size.width/2.0f;
    corners[0].y = -size.height/2.0f;
    corners[1].x = -size.width/2.0f;
    corners[1].y =  size.height/2.0f;
    corners[2].x =  size.width/2.0f;
    corners[2].y =  size.height/2.0f;
    corners[3].x =  size.width/2.0f;
    corners[3].y = -size.height/2.0f;
    _shape.Set(corners, 4);
    if (_debug != nullptr) {
        resetDebugNode();
    }
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
void BoxObstacle::resetDebugNode() {
    float w = _dimension.width *_drawScale.x;
    float h = _dimension.height*_drawScale.y;
    Poly2 poly(Rect(-w/2.0f,-h/2.0f,w,h));
    poly.traverse(Poly2::Traversal::INTERIOR);
    _debug->setPolygon(poly);
}

#pragma mark -
#pragma mark Physics Methods
/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void BoxObstacle::createFixtures() {
    if (_body == nullptr) {
        return;
    }
    
    releaseFixtures();
    
    // Create the fixture
    _fixture.shape = &_shape;
    _geometry = _body->CreateFixture(&_fixture);
    markDirty(false);
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects
 */
void BoxObstacle::releaseFixtures() {
    if (_geometry != nullptr) {
        _body->DestroyFixture(_geometry);
        _geometry = nullptr;
    }
}

NS_CC_END