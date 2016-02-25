//
//  PFSpinner.cpp
//  PlatformerDemo
//
//  This class provides a spinning rectangle on a fixed pin.  We did not really need
//  a separate class for this, as it has no update.  However, ComplexObstacles always
//  make joint management easier.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/15/16
//
#include "PFSpinner.h"
#include <cornell/CUBoxObstacle.h>
#include <cornell/CUWheelObstacle.h>
#include <cornell/CUAssetManager.h>
#include <cornell/CUSceneManager.h>
#include <cornell/CUPolygonNode.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/b2World.h>

#pragma mark -
#pragma mark Physics Constants

/** The radius of the central pin */
#define SPIN_PIN_RADIUS 0.1f
/** The density for the spinning barrier */
#define HEAVY_DENSITY  10.0f
/** The density for the central pin */
#define LIGHT_DENSITY   1.0f


#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a new spinner at the origin.
 *
 * The spinner is scaled so that 1 pixel = 1 Box2d unit
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @return  An autoreleased physics object
 */
Spinner* Spinner::create() {
    Spinner* spinner = new (std::nothrow) Spinner();
    if (spinner && spinner->init()) {
        spinner->autorelease();
        return spinner;
    }
    CC_SAFE_DELETE(spinner);
    return nullptr;
}

/**
 * Creates a new spinner at the given position.
 *
 * The spinner is scaled so that 1 pixel = 1 Box2d unit
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
Spinner* Spinner::create(const Vec2& pos) {
    Spinner* spinner = new (std::nothrow) Spinner();
    if (spinner && spinner->init(pos)) {
        spinner->autorelease();
        return spinner;
    }
    CC_SAFE_DELETE(spinner);
    return nullptr;
}

/**
 * Creates a new spinner at the given position.
 *
 * The spinner is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  scale    The drawing scale
 *
 * @return  An autoreleased physics object
 */
Spinner* Spinner::create(const Vec2& pos, const Vec2& scale) {
    Spinner* spinner = new (std::nothrow) Spinner();
    if (spinner && spinner->init(pos,scale)) {
        spinner->autorelease();
        return spinner;
    }
    CC_SAFE_DELETE(spinner);
    return nullptr;
}


#pragma mark -
#pragma mark Initializers
/**
 * Initializes a new spinner at the given position.
 *
 * The spinner is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  scale    The drawing scale
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Spinner::init(const Vec2& pos, const Vec2& scale) {
    Obstacle::init(pos);
    setDrawScale(scale);
    
    string name(SPINNER_NAME);
    setName(name);
    
    // We need to know the content scale for resolution independence
    // If the device is higher resolution than 1024x576, Cocos2d will scale it
    // This was set as the design resolution in AppDelegate
    // To convert from design resolution to real, divide positions by cscale
    float cscale = Director::getInstance()->getContentScaleFactor();
    
    SceneManager* scene = AssetManager::getInstance()->getCurrent();
    Texture2D* image = scene->get<Texture2D>(SPINNER_TEXTURE);
    Size size = image->getContentSize();
    size.width  *= cscale/scale.x;
    size.height *= cscale/scale.y;
    
    // Create the barrier
    Obstacle* body = BoxObstacle::create(pos,size);
    body->setName(BARRIER_NAME);
    body->retain();
    body->setDensity(HEAVY_DENSITY);
    body->setDrawScale(_drawScale);
    _bodies.push_back(body);
    
    // Create the pin
    body = WheelObstacle::create(pos,SPIN_PIN_RADIUS);
    body->setName(SPIN_PIN_NAME);
    body->retain();
    body->setDensity(LIGHT_DENSITY);
    body->setDrawScale(_drawScale);
    body->setBodyType(b2_staticBody);
    _bodies.push_back(body);
    
    return true;
}

/**
 * Creates the joints for this object.
 *
 * This method is executed as part of activePhysics. This is the primary method to
 * override for custom physics objects.
 *
 * @param world Box2D world to store joints
 *
 * @return true if object allocation succeeded
 */
bool Spinner::createJoints(b2World& world) {
    b2RevoluteJointDef jointDef;
    jointDef.bodyA = _bodies[0]->getBody();
    jointDef.bodyB = _bodies[1]->getBody();
    jointDef.localAnchorA.Set(0,0);
    jointDef.localAnchorB.Set(0,0);
    b2Joint* joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);
    
    return true;
}

#pragma mark -
#pragma mark Scene Graph Internals
/**
 * Performs any necessary additions to the scene graph node.
 *
 * This method is necessary for custom physics objects that are composed
 * of multiple scene graph nodes.
 */
void Spinner::resetSceneNode() {
    SceneManager* scene = AssetManager::getInstance()->getCurrent();
    Texture2D* image = scene->get<Texture2D>(SPINNER_TEXTURE);

    float cscale = Director::getInstance()->getContentScaleFactor();
    PolygonNode* sprite = PolygonNode::createWithTexture(image);
    sprite->setScale(cscale);
    _bodies[0]->setSceneNode(sprite);
    _node->addChild(sprite);
    
    // Spinner is invisible
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
void Spinner::resetDebugNode() {
    for(int ii = 0; ii < _bodies.size(); ii++) {
        WireNode* wire = WireNode::create();
        wire->setColor(_debug->getColor());
        _bodies[ii]->setDebugNode(wire);
        _debug->addChild(wire);
    }
}
