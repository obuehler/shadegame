//
//  CUPolygonObstacle.h
//  Cornell Extensions to Cocos2D
//
//  This class implements a polygonal Physics object.  This is different from PolygonNode,
//  which is used for drawing.  This class is substantially more complex than the other
//  physics objects, but it will allow you to draw arbitrary shapes.  Be careful modifying
//  this file as there are a lot of subtleties here.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 11/24/15
//
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include "CUPolygonObstacle.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a (not necessarily convex) polygon
 *
 * The anchor point (the rotational center) of the polygon is at the
 * center of the polygons bounding box.
 *
 * @param poly   The polygon vertices
 *
 * @return  An autoreleased physics object
 */
PolygonObstacle* PolygonObstacle::create(const Poly2& poly) {
    PolygonObstacle* obstacle = new (std::nothrow) PolygonObstacle();
    if (obstacle && obstacle->init(poly)) {
        obstacle->autorelease();
        return obstacle;
    }
    CC_SAFE_DELETE(obstacle);
    return nullptr;
}

/**
 * Creates a (not necessarily convex) polygon
 *
 * The anchor point (the rotational center) of the polygon is specified as a
 * ratio of the bounding box.  An anchor point of (0,0) is the bottom left of
 * the bounding box.  An anchor point of (1,1) is the top right of the bounding
 * box.  The anchor point does not need to be contained with the bounding box.
 *
 * @param  poly     The polygon vertices
 * @param  anchor   The rotational center of the polygon
 *
 * @return  An autoreleased physics object
 */
PolygonObstacle* PolygonObstacle::create(const Poly2& poly, const Vec2& anchor) {
    PolygonObstacle* obstacle = new (std::nothrow) PolygonObstacle();
    if (obstacle && obstacle->init(poly,anchor)) {
        obstacle->autorelease();
        return obstacle;
    }
    CC_SAFE_DELETE(obstacle);
    return nullptr;
}


#pragma mark -
#pragma mark Initializers
/**
 * Initializes a (not necessarily convex) polygon
 *
 * The anchor point (the rotational center) of the polygon is specified as a
 * ratio of the bounding box.  An anchor point of (0,0) is the bottom left of
 * the bounding box.  An anchor point of (1,1) is the top right of the bounding
 * box.  The anchor point does not need to be contained with the bounding box.
 *
 * @param  poly     The polygon vertices
 * @param  anchor   The rotational center of the polygon
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool PolygonObstacle::init(const Poly2& poly, const Vec2& anchor) {
    Obstacle::init(Vec2::ZERO);
    
    // Compute the position from the anchor point
    Vec2 pos = poly.getBounds().origin;
    pos.x += anchor.x*poly.getBounds().size.width;
    pos.y += anchor.x*poly.getBounds().size.height;
    
    _bodyinfo.position.Set(pos.x,pos.y);
    _anchor = anchor;
    setPolygon(poly);
    return true;
}

/**
 * Deletes this physics object and all of its resources.
 *
 * A non-default destructor is necessary since we must release all
 * the fixture pointers for the polygons.
 */
PolygonObstacle::~PolygonObstacle() {
    CCASSERT(_body == nullptr, "You must deactive physics before deleting an object");
    if (_shapes != nullptr) {
        delete[] _shapes;
        _shapes = nullptr;
    }
}


#pragma mark -
#pragma mark Resizing
/**
 * Resets the polygon vertices in the shape to match the dimension.
 *
 * This is an internal method and it does not mark the physics object as dirty.
 *
 * @param  size The new dimension (width and height)
 */
void PolygonObstacle::resize(const Size& size) {
    // Need to do two things:
    // 1. Adjust the polygon.
    // 2. Update the shape information
    float origwide = _polygon.getBounds().size.width;
    float orighigh = _polygon.getBounds().size.height;
    _polygon *= Vec2(size.width/origwide, size.height/orighigh);
    if (_debug != nullptr) {
        resetDebugNode();
    }
}

/**
 * Recreates the shape objects attached to this polygon.
 *
 * This must be called whenever the polygon is resized.
 */
void PolygonObstacle::resetShapes() {
    int ntris =  (int)_polygon.getIndices().size() / 3;
    if (_shapes != nullptr) {
        delete[] _shapes;
    }
    
    Vec2 pos = getPosition();
    _shapes = new b2PolygonShape[ntris];
    b2Vec2 triangle[3];
    for(int ii = 0; ii < ntris; ii++) {
        for(int jj = 0; jj < 3; jj++) {
            unsigned short ind = _polygon.getIndices()[3*ii+jj];
            Vec2 temp = _polygon.getVertices()[ind]-pos;
            triangle[jj].x = temp.x;
            triangle[jj].y = temp.y;
        }
        _shapes[ii].Set(triangle,3);
    }
    
    if (_geoms == nullptr) {
        _geoms = new b2Fixture*[ntris];
        for(int ii = 0; ii < ntris; ii++) { _geoms[ii] = nullptr; }
        _fixCount = ntris;
    } else {
        markDirty(true);
    }
}


#pragma mark -
#pragma mark Dimensions

/**
 * Sets the rotational center of this polygon
 *
 * The anchor point of the polygon is specified as ratio of the bounding
 * box.  An anchor point of (0,0) is the bottom left of the bounding box.
 * An anchor point of (1,1) is the top right of the bounding box.  The
 * anchorpoint does not need to be contained with the bounding box.
 *
 * @param x  the x-coordinate of the rotational center
 * @param y  the y-coordinate of the rotational center
 */
void PolygonObstacle::setAnchor(float x, float y) {
    _anchor.set(x,y);
    
    // Compute the position from the anchor point
    Vec2 pos = _polygon.getBounds().origin;
    pos.x += x*_polygon.getBounds().size.width;
    pos.y += x*_polygon.getBounds().size.height;
    setPosition(pos.x,pos.y);
    resetShapes();
}

/**
 * Sets the polygon defining this object
 *
 * This change cannot happen immediately.  It must wait until the
 * next update is called.
 *
 * @param value   the polygon defining this object
 */
void PolygonObstacle::setPolygon(const Poly2& poly) {
    _polygon.set(poly);
    resetShapes();
}


#pragma mark -
#pragma mark Scene Graph Methods
/**
 * Performs any necessary additions to the scene graph node.
 *
 * This method is necessary for custom physics objects that are composed
 * of multiple scene graph nodes.
 */
void PolygonObstacle::resetSceneNode() {
    _node->setAnchorPoint(_anchor);
}

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
void PolygonObstacle::resetDebugNode() {
    Poly2 copy(_polygon);
    copy *= _drawScale;
    copy.traverse(Poly2::Traversal::INTERIOR);
    _debug->setPolygon(copy);
    _debug->setAnchorPoint(_anchor);
}


#pragma mark -
#pragma mark Physics Methods

/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void PolygonObstacle::createFixtures() {
    if (_body == nullptr) {
        return;
    }
    
    // Create the fixtures
    releaseFixtures();
    for(int ii = 0; ii < _fixCount; ii++) {
        _fixture.shape = &(_shapes[ii]);
        _geoms[ii] = _body->CreateFixture(&_fixture);
    }
    markDirty(false);
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects
 */
void PolygonObstacle::releaseFixtures() {
    if (_geoms != nullptr && _geoms[0] != nullptr) {
        for(int ii = 0; ii < _fixCount; ii++) {
            _body->DestroyFixture(_geoms[ii]);
            _geoms[ii] = nullptr;
        }
    }
    if (_geoms != nullptr && _fixCount != (int)_polygon.getIndices().size()/3) {
        delete[] _geoms;
        _fixCount = (int)_polygon.getIndices().size()/3;
        _geoms = new b2Fixture*[_fixCount];
    }
}

NS_CC_END
