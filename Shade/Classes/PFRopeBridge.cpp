//
//  PFRopeBridge.cpp
//  PlatformerDemo
//
//  This module provides the connected ropebridge from 3152. This time it is fully
//  assembled for you.  Note that this module handles its own scene graph management.
//  As a ComplexObstacle owns all of its child obstacles, it is natural for it to
//  own the corresponding scene graph.
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
#include "PFRopeBridge.h"

#include <cornell/CUBoxObstacle.h>
#include <cornell/CUWheelObstacle.h>
#include <cornell/CUAssetManager.h>
#include <cornell/CUSceneManager.h>
#include <cornell/CUPolygonNode.h>
#include <cornell/CUStrings.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/b2World.h>


#pragma mark -
#pragma mark Physics Constants
/** The radius of each anchor pin */
#define BRIDGE_PIN_RADIUS   0.1f
/** The density of each plank in the bridge */
#define BASIC_DENSITY       1.0f


#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a new horizontal bridge of length 1.
 *
 * The bridge is scaled so that 1 pixel = 1 Box2d unit
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @return  An autoreleased physics object
 */
RopeBridge* RopeBridge::create() {
    RopeBridge* bridge = new (std::nothrow) RopeBridge();
    if (bridge && bridge->init()) {
        bridge->autorelease();
        return bridge;
    }
    CC_SAFE_DELETE(bridge);
    return nullptr;
}

/**
 * Creates a new rope bridge with the given anchors.
 *
 * The bridge is scaled so that 1 pixel = 1 Box2d unit
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  start    Initial anchor in world coordinates
 * @param  end      Final anchor in world coordinates
 *
 * @return  An autoreleased physics object
 */
RopeBridge* RopeBridge::create(const Vec2& start, const Vec2& end) {
    RopeBridge* bridge = new (std::nothrow) RopeBridge();
    if (bridge && bridge->init(start, end)) {
        bridge->autorelease();
        return bridge;
    }
    CC_SAFE_DELETE(bridge);
    return nullptr;
}

/**
 * Creates a new rope bridge with the given anchors.
 *
 * The bridge is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  start    Initial anchor in world coordinates
 * @param  end      Final anchor in world coordinates
 * @param  scale    The drawing scale
 *
 * @return  An autoreleased physics object
 */
RopeBridge* RopeBridge::create(const Vec2& start, const Vec2& end, const Vec2& scale) {
    RopeBridge* bridge = new (std::nothrow) RopeBridge();
    if (bridge && bridge->init(start, end, scale)) {
        bridge->autorelease();
        return bridge;
    }
    CC_SAFE_DELETE(bridge);
    return nullptr;
}


#pragma mark -
#pragma mark Initializers

/**
 * Initializes a new ragdoll with its torso at the given position
 *
 * The scale is the ratio of drawing coordinates to physics coordinates.
 * This allows us to construct the child objects appropriately.
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
bool RopeBridge::init(const Vec2& start, const Vec2& end, const Vec2& scale) {
    Obstacle::init(start);
    setDrawScale(scale);
    
    string name(BRIDGE_NAME);
    setName(name);
    
    // We need to know the content scale for resolution independence
    // If the device is higher resolution than 1024x576, Cocos2d will scale it
    // This was set as the design resolution in AppDelegate
    // To convert from design resolution to real, divide positions by cscale
    float cscale = Director::getInstance()->getContentScaleFactor();
    SceneManager* scene = AssetManager::getInstance()->getCurrent();
    Texture2D* image = scene->get<Texture2D>(BRIDGE_TEXTURE);
    Size  planksize = image->getContentSize();
    planksize.width  *= cscale/scale.x;
    planksize.height *= cscale/scale.y;
    _linksize = planksize.width;
    float spacing;
    
    // Compute the bridge length
    Vec2 dimen = end-start;
    float length = dimen.length();
    Vec2 norm = dimen;
    norm.normalize();
    
    // If too small, only make one plank.
    int nLinks = (int)(length / _linksize);
    if (nLinks <= 1) {
        nLinks = 1;
        _linksize = length;
        spacing = 0;
    } else {
        spacing = length - nLinks * _linksize;
        spacing /= (nLinks-1);
    }
    
    // Create the first pin
    Obstacle* body = WheelObstacle::create(start,BRIDGE_PIN_RADIUS);
    body->setName(BRIDGE_PIN_NAME+cocos2d::to_string(0));
    body->retain();
    body->setDensity(BASIC_DENSITY);
    body->setBodyType(b2_staticBody);
    body->setDrawScale(_drawScale);
    _bodies.push_back(body);
    
    // Creat the planks
    planksize.width = _linksize;
    for (int ii = 0; ii < nLinks; ii++) {
        float t = ii*(_linksize+spacing) + _linksize/2.0f;
        Vec2 pos = norm*t+start;
        BoxObstacle* plank = BoxObstacle::create(pos, planksize);
        body->setName(PLANK_NAME+cocos2d::to_string(ii));
        plank->retain();
        plank->setDensity(BASIC_DENSITY);
        plank->setDrawScale(_drawScale);
        _bodies.push_back(plank);
    }
    
    // Create the last pin
    body = WheelObstacle::create(end,BRIDGE_PIN_RADIUS);
    body->setName(BRIDGE_PIN_NAME+cocos2d::to_string(1));
    body->retain();
    body->setDensity(BASIC_DENSITY);
    body->setBodyType(b2_staticBody);
    body->setDrawScale(_drawScale);
    _bodies.push_back(body);

    
    return true;
}


#pragma mark -
#pragma mark Physics Methods
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
bool RopeBridge::createJoints(b2World& world) {
    b2Vec2 anchor1(0,0);
    b2Vec2 anchor2(-_linksize/2.0f,0);

    // Definition for a revolute joint
    b2RevoluteJointDef jointDef;
    b2Joint* joint;

    jointDef.bodyA = _bodies[0]->getBody();
    jointDef.bodyB = _bodies[1]->getBody();
    jointDef.localAnchorA = anchor1;
    jointDef.localAnchorB = anchor2;
    jointDef.collideConnected = false;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);

    // Planks together
    anchor1.x = _linksize / 2;
    for (int ii = 1; ii < _bodies.size()-2; ii++) {
        jointDef.bodyA = _bodies[ii]->getBody();
        jointDef.bodyB = _bodies[ii+1]->getBody();
        jointDef.localAnchorA = anchor1;
        jointDef.localAnchorB = anchor2;
        jointDef.collideConnected = false;
        joint = world.CreateJoint(&jointDef);
        _joints.push_back(joint);
    }

    // Final joint
    anchor2.x = 0;
    int ii = (int)_bodies.size();
    jointDef.bodyA = _bodies[ii-2]->getBody();
    jointDef.bodyB = _bodies[ii-1]->getBody();
    jointDef.localAnchorA = anchor1;
    jointDef.localAnchorB = anchor2;
    jointDef.collideConnected = false;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);

    return true;
}


#pragma mark -
#pragma mark Scene Graph Management
/**
 * Performs any necessary additions to the scene graph node.
 *
 * This method is necessary for custom physics objects that are composed
 * of multiple scene graph nodes.
 */
void RopeBridge::resetSceneNode() {
    float cscale = Director::getInstance()->getContentScaleFactor();
    SceneManager* scene = AssetManager::getInstance()->getCurrent();
    Texture2D* image = scene->get<Texture2D>(BRIDGE_TEXTURE);
    for(int ii = 1; ii+1 < _bodies.size(); ii++) {
        PolygonNode* sprite = PolygonNode::createWithTexture(image);
        sprite->setScale(cscale);
        _bodies[ii]->setSceneNode(sprite);
        _node->addChild(sprite);
    }
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
void RopeBridge::resetDebugNode() {
    for(int ii = 0; ii < _bodies.size(); ii++) {
        WireNode* wire = WireNode::create();
        wire->setColor(_debug->getColor());
        _bodies[ii]->setDebugNode(wire);
        _debug->addChild(wire);
    }
}
