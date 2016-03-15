//
//  CUCapsuleObstacle.cpp
//  Cornell Extensions to Cocos2D
//
//  This class implements a capsule physics object. A capsule is a box with semicircular
//  ends along the major axis.  They are a popular physics objects, particularly for
//  character avatars.  The rounded ends means they are less likely to snag, and they
//  naturally fall off platforms when they go too far.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 11/24/15
//
#include "CUCapsuleObstacle.h"

NS_CC_BEGIN

/** How many line segments to use to draw a circle */
#define BODY_DEBUG_SEGS 12

/** Epsilon factor to prevent issues with the fixture seams */
#define DEFAULT_EPSILON 0.01

#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a new capsule object at the origin with no size.
 *
 * @return  An autoreleased physics object
 */
CapsuleObstacle* CapsuleObstacle::create() {
    CapsuleObstacle* capsule = new (std::nothrow) CapsuleObstacle();
    if (capsule && capsule->init()) {
        capsule->autorelease();
        return capsule;
    }
    CC_SAFE_DELETE(capsule);
    return nullptr;
}

/**
 * Creates a new capsule object at the given point with no size.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 *
 * @return  An autoreleased physics object
 */
CapsuleObstacle* CapsuleObstacle::create(const Vec2& pos) {
    CapsuleObstacle* capsule = new (std::nothrow) CapsuleObstacle();
    if (capsule && capsule->init(pos)) {
        capsule->autorelease();
        return capsule;
    }
    CC_SAFE_DELETE(capsule);
    return nullptr;
}

/**
 * Creates a new capsule object of the given dimensions.
 *
 * The orientation of the capsule will be a full capsule along the
 * major axis.  If width == height, it will default to a vertical
 * orientation.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  size     The capsule size (width and height)
 *
 * @return  An autoreleased physics object
 */
CapsuleObstacle* CapsuleObstacle::create(const Vec2& pos, const Size& size) {
    CapsuleObstacle* capsule = new (std::nothrow) CapsuleObstacle();
    if (capsule && capsule->init(pos,size)) {
        capsule->autorelease();
        return capsule;
    }
    CC_SAFE_DELETE(capsule);
    return nullptr;
}

/**
 * Creates a new capsule object of the given dimensions and orientation.
 *
 * The orientation must be consistent with the major axis (or else the
 * two axes must be the same). If the orientation specifies a minor axis,
 * then this constructor will return null.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  size     The capsule size (width and height)
 * @param  orient   The capsule orientation
 *
 * @return  An autoreleased physics object
 */
CapsuleObstacle* CapsuleObstacle::create(const Vec2& pos, const Size& size, CapsuleObstacle::Orientation orient) {
    CapsuleObstacle* capsule = new (std::nothrow) CapsuleObstacle();
    if (capsule && capsule->init(pos,size,orient)) {
        capsule->autorelease();
        return capsule;
    }
    CC_SAFE_DELETE(capsule);
    return nullptr;
}


#pragma mark -
#pragma mark Initialization Methods
/**
 * Initializes a new capsule object of the given dimensions.
 *
 * The orientation of the capsule will be a full capsule along the
 * major axis.  If width == height, it will default to a vertical
 * orientation.
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
bool CapsuleObstacle::init(const Vec2& pos, const Size& size) {
    Orientation orient = (size.width > size.height ? Orientation::HORIZONTAL : Orientation::VERTICAL);
    return init(pos,size,orient);
}


/**
 * Initializes a new capsule object of the given dimensions.
 *
 * The orientation must be consistent with the major axis (or else the
 * two axes must be the same). If the orientation specifies a minor axis,
 * then this initializer will fail.
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
bool CapsuleObstacle::init(const Vec2& pos, const Size& size, CapsuleObstacle::Orientation orient) {
    Obstacle::init(pos);
    _core = nullptr;
    _cap1 = nullptr;
    _cap2 = nullptr;
    _orient = orient;
    _seamEpsilon = DEFAULT_EPSILON;
    resize(size);
    return true;
}


#pragma mark -
#pragma mark Scene Graph Methods

/**
 * Resets the polygon vertices in the shape to match the dimension.
 *
 * @param  size The new dimension (width and height)
 */
bool CapsuleObstacle::resize(const Size& size) {
    _dimension = size;
    if (size.width < size.height && isHorizontal(_orient)) {
        _orient = Orientation::VERTICAL;    // OVERRIDE
    } else if (size.width > size.height && !isHorizontal(_orient)) {
        _orient = Orientation::HORIZONTAL;  // OVERRIDE
    }
    
    // Get an AABB for the core
    _center.upperBound.x = size.width/2.0f;
    _center.upperBound.y = size.height/2.0f;
    _center.lowerBound.x = -size.width/2.0f;
    _center.lowerBound.y = -size.height/2.0f;
    
    // Now adjust the core
    float r = 0;
    switch (_orient) {
        case Orientation::TOP:
            r = size.width/2.0f;
            _center.upperBound.y -= r;
            _center.lowerBound.x += _seamEpsilon;
            _center.upperBound.x -= _seamEpsilon;
            break;
        case Orientation::VERTICAL:
            r = size.width/2.0f;
            _center.upperBound.y -= r;
            _center.lowerBound.y += r;
            _center.lowerBound.x += _seamEpsilon;
            _center.upperBound.x -= _seamEpsilon;
            break;
        case Orientation::BOTTOM:
            r = size.width/2.0f;
            _center.lowerBound.y += r;
            _center.lowerBound.x += _seamEpsilon;
            _center.upperBound.x -= _seamEpsilon;
            break;
        case Orientation::LEFT:
            r = size.height/2.0f;
            _center.upperBound.x -= r;
            _center.lowerBound.y += _seamEpsilon;
            _center.upperBound.y -= _seamEpsilon;
            break;
        case Orientation::HORIZONTAL:
            r = size.height/2.0f;
            _center.upperBound.x -= r;
            _center.lowerBound.x += r;
            _center.lowerBound.y += _seamEpsilon;
            _center.upperBound.y -= _seamEpsilon;
            break;
        case Orientation::RIGHT:
            r = size.height/2.0f;
            _center.lowerBound.x += r;
            _center.lowerBound.y += _seamEpsilon;
            _center.upperBound.y -= _seamEpsilon;
            break;
    }
    // Handle degenerate polys
    if (_center.lowerBound.x == _center.upperBound.x) {
        _center.lowerBound.x -= _seamEpsilon;
        _center.upperBound.x += _seamEpsilon;
    }
    if (_center.lowerBound.y == _center.upperBound.y) {
        _center.lowerBound.y -= _seamEpsilon;
        _center.upperBound.y += _seamEpsilon;
    }
    
    // Make the box for the core
    b2Vec2 corners[4];
    corners[0].x = _center.lowerBound.x;
    corners[0].y = _center.lowerBound.y;
    corners[1].x = _center.lowerBound.x;
    corners[1].y = _center.upperBound.y;
    corners[2].x = _center.upperBound.x;
    corners[2].y = _center.upperBound.y;
    corners[3].x = _center.upperBound.x;
    corners[3].y = _center.lowerBound.y;
    _shape.Set(corners, 4);
    
    _ends.m_radius = r;
    if (_debug != nullptr) {
        resetDebugNode();
    }
    
    return true;
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
void CapsuleObstacle::resetDebugNode() {
    Rect bounds;
    
    // Create a capsule polygon
    const float coef = (float)M_PI/BODY_DEBUG_SEGS;
    float rx = _ends.m_radius*_drawScale.x;
    float ry = _ends.m_radius*_drawScale.y;
    
    std::vector<Vec2> vertices;
    Vec2 vert;
    
    // Start at top left corner
    vert.x = _center.lowerBound.x*_drawScale.x;
    vert.y = _center.upperBound.y*_drawScale.y;
    vertices.push_back(vert);
    
    // Fan if necessary
    if (_orient == Orientation::TOP || _orient == Orientation::VERTICAL) {
        for(unsigned int ii = 1; ii < BODY_DEBUG_SEGS; ii++) {
            float rads = M_PI-ii*coef;
            vert.x = rx * cosf(rads);
            vert.y = ry * sinf(rads) + _center.upperBound.y*_drawScale.y;
            vertices.push_back(vert);
        }
    }
    
    // Next corner
    vert.x = _center.upperBound.x*_drawScale.x;
    vert.y = _center.upperBound.y*_drawScale.y;
    vertices.push_back(vert);
    
    // Fan if necessary
    if (_orient == Orientation::RIGHT || _orient == Orientation::HORIZONTAL) {
        for(unsigned int ii = 1; ii < BODY_DEBUG_SEGS; ii++) {
            float rads = M_PI/2-ii*coef;
            vert.x = rx * cosf(rads) + _center.upperBound.x*_drawScale.x;
            vert.y = ry * sinf(rads);
            vertices.push_back(vert);
        }
    }
    
    // Next corner
    vert.x = _center.upperBound.x*_drawScale.x;
    vert.y = _center.lowerBound.y*_drawScale.y;
    vertices.push_back(vert);
    
    // Fan if necessary
    if (_orient == Orientation::BOTTOM || _orient == Orientation::VERTICAL) {
        for(unsigned int ii = 1; ii < BODY_DEBUG_SEGS; ii++) {
            float rads = 2*M_PI-ii*coef;
            vert.x = rx * cosf(rads);
            vert.y = ry * sinf(rads) + _center.lowerBound.y*_drawScale.y;
            vertices.push_back(vert);
        }
    }
    
    
    // Next corner
    vert.x = _center.lowerBound.x*_drawScale.x;
    vert.y = _center.lowerBound.y*_drawScale.y;
    vertices.push_back(vert);
    
    // Fan if necessary
    if (_orient == Orientation::LEFT || _orient == Orientation::HORIZONTAL) {
        for(unsigned int ii = 1; ii < BODY_DEBUG_SEGS; ii++) {
            float rads = 3*M_PI/2-ii*coef;
            vert.x = rx * cosf(rads) + _center.lowerBound.x*_drawScale.x;
            vert.y = ry * sinf(rads);
            vertices.push_back(vert);
        }
    }
    
    // Create polygon
    Poly2 poly(vertices);
    poly.traverse(Poly2::Traversal::CLOSED);
    _debug->setPolygon(poly);
}


#pragma mark -
#pragma mark Physics Methods
/**
 * Sets the density of this body
 *
 * The density is typically measured in usually in kg/m^2. The density can be zero or
 * positive. You should generally use similar densities for all your fixtures. This
 * will improve stacking stability.
 *
 * @param value  the density of this body
 */
void CapsuleObstacle::setDensity(float value) {
    _fixture.density = value;
    if (_body != nullptr) {
        _core->SetDensity(value);
        _cap1->SetDensity(value/2.0f);
        _cap2->SetDensity(value/2.0f);
        if (!_masseffect) {
            _body->ResetMassData();
        }
    }
}

/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void CapsuleObstacle::createFixtures() {
    if (_body == nullptr) {
        return;
    }
    
    releaseFixtures();
    
    // Create the fixture
    _fixture.shape = &_shape;
    _core = _body->CreateFixture(&_fixture);
    
    _fixture.density = _fixture.density/2.0f;
    _ends.m_p.Set(0, 0);
    switch (_orient) {
        case Orientation::TOP:
            _ends.m_p.y = _center.upperBound.y;
            _fixture.shape = &_ends;
            _cap1 = _body->CreateFixture(&_fixture);
            _cap2 = nullptr;
            break;
        case Orientation::VERTICAL:
            _ends.m_p.y = _center.upperBound.y;
            _fixture.shape = &_ends;
            _cap1 = _body->CreateFixture(&_fixture);
            _ends.m_p.y = _center.lowerBound.y;
            _fixture.shape = &_ends;
            _cap2 = _body->CreateFixture(&_fixture);
        case Orientation::BOTTOM:
            _ends.m_p.y = _center.lowerBound.y;
            _fixture.shape = &_ends;
            _cap1 = _body->CreateFixture(&_fixture);
            break;
        case Orientation::LEFT:
            _ends.m_p.x = _center.lowerBound.x;
            _fixture.shape = &_ends;
            _cap1 = _body->CreateFixture(&_fixture);
            _cap2 = nullptr;
            break;
        case Orientation::HORIZONTAL:
            _ends.m_p.x = _center.lowerBound.x;
            _fixture.shape = &_ends;
            _cap1 = _body->CreateFixture(&_fixture);
            _ends.m_p.x = _center.upperBound.x;
            _fixture.shape = &_ends;
            _cap2 = _body->CreateFixture(&_fixture);
        case Orientation::RIGHT:
            _ends.m_p.x = _center.upperBound.x;
            _fixture.shape = &_ends;
            _cap1 = _body->CreateFixture(&_fixture);
            break;
    }
    
    markDirty(false);
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects
 */
void CapsuleObstacle::releaseFixtures() {
    if (_core != nullptr) {
        _body->DestroyFixture(_core);
        _core = nullptr;
    }
    if (_cap1 != nullptr) {
        _body->DestroyFixture(_cap1);
        _cap1 = nullptr;
    }
    if (_cap2 != nullptr) {
        _body->DestroyFixture(_cap2);
        _cap2 = nullptr;
    }
}

NS_CC_END