//
//  RGRagdollModel.h
//  RagdollDemo
//
//  This module provides the infamous Walker White ragdoll from 3152.  This time it
//  is fully assembled for you.
//
//  Note that this module handles its own scene graph management.  As a ComplexObstacle
//  owns all of its child obstacles, it is natural for it to own the corresponding
//  scene graph.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/15/15
//

#include "RGRagdollModel.h"
#include "RGBubbleGenerator.h"
#include <cornell/CUAssetManager.h>
#include <cornell/CUPolygonNode.h>
#include <cornell/CUWireNode.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>

/** The offset of the snorkel from the doll's head (in Box2d units) */
float BUBB_OFF[] = {0.55f,  1.9f};


#pragma mark -
#pragma mark Static Constructors
/**
 * Creates a new ragdoll with its torso centered at the origin.
 *
 * The ragdoll is scaled so that 1 pixel = 1 Box2d unit
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @return  An autoreleased physics object
 */
RagdollModel* RagdollModel::create() {
    RagdollModel* ragdoll = new (std::nothrow) RagdollModel();
    if (ragdoll && ragdoll->init()) {
        ragdoll->autorelease();
        return ragdoll;
    }
    CC_SAFE_DELETE(ragdoll);
    return nullptr;
}

/**
 * Creates a new ragdoll with its torso at the given position
 *
 * The ragdoll is scaled so that 1 pixel = 1 Box2d unit
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
RagdollModel* RagdollModel::create(const Vec2& pos) {
    RagdollModel* ragdoll = new (std::nothrow) RagdollModel();
    if (ragdoll && ragdoll->init(pos)) {
        ragdoll->autorelease();
        return ragdoll;
    }
    CC_SAFE_DELETE(ragdoll);
    return nullptr;
}

/**
 * Creates a new ragdoll with its torso at the given position
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
 * @return  An autoreleased physics object
 */
RagdollModel* RagdollModel::create(const Vec2& pos, const Vec2& scale) {
    RagdollModel* ragdoll = new (std::nothrow) RagdollModel();
    if (ragdoll && ragdoll->init(pos,scale)) {
        ragdoll->autorelease();
        return ragdoll;
    }
    CC_SAFE_DELETE(ragdoll);
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
bool RagdollModel::init(const Vec2& pos, const Vec2& scale) {
    Obstacle::init(pos);
    setDrawScale(scale);
    
    string name("ragdoll");
    setName(name);
    
    BoxObstacle* part;
    
    // TORSO
    part = makePart(PART_BODY, PART_NONE, pos, scale);
    part->setFixedRotation(true);
    
    // HEAD
    makePart(PART_HEAD, PART_BODY, Vec2(0, TORSO_OFFSET), scale);

    // ARMS
    makePart(PART_LEFT_ARM, PART_BODY, Vec2(-ARM_XOFFSET, ARM_YOFFSET), scale);
    makePart(PART_RIGHT_ARM, PART_BODY, Vec2(ARM_XOFFSET, ARM_YOFFSET), scale);
    
    // FOREARMS
    makePart(PART_LEFT_FOREARM, PART_LEFT_ARM, Vec2(-FOREARM_OFFSET, 0), scale);
    makePart(PART_RIGHT_FOREARM, PART_RIGHT_ARM, Vec2(FOREARM_OFFSET, 0), scale);

    // THIGHS
    makePart(PART_LEFT_THIGH,  PART_BODY, Vec2(-THIGH_XOFFSET, -THIGH_YOFFSET), scale);
    makePart(PART_RIGHT_THIGH, PART_BODY, Vec2(THIGH_XOFFSET,  -THIGH_YOFFSET), scale);
    
    // SHINS
    makePart(PART_LEFT_SHIN,  PART_LEFT_THIGH,  Vec2(0, -SHIN_OFFSET), scale);
    makePart(PART_RIGHT_SHIN, PART_RIGHT_THIGH, Vec2(0, -SHIN_OFFSET), scale);

    // Make the bubbler
    Vec2 offpos = (Vec2)(BUBB_OFF)+pos;
    _bubbler = BubbleGenerator::create(offpos, scale);
    _bodies.push_back(_bubbler);
    _bubbler->retain();
    
    return true;
}

/**
 * Returns a single body part
 *
 * While it looks like this method "connects" the pieces, it does not really.
 * It puts them in position to be connected by joints, but they will fall apart
 * unless you make the joints.
 *
 * @param  part     Part to create
 * @param  connect  Part to connect it to
 * @param  pos      Position RELATIVE to connecting part
 * @param  scale    The drawing scale
 *
 * @return the created body part
 */
BoxObstacle* RagdollModel::makePart(int part, int connect, const Vec2& pos, const Vec2& scale) {
    // We need to know the content scale for resolution independence
    // If the device is higher resolution than 1024x576, Cocos2d will scale it
    // This was set as the design resolution in AppDelegate
    float cscale = Director::getInstance()->getContentScaleFactor();
    
    std::string name = getPartName(part);
    SceneManager* scene = AssetManager::getInstance()->getCurrent();
    Texture2D* image = scene->get<Texture2D>(name);
    Size size = image->getContentSize();
    size.width  /= scale.x;
    size.height /= scale.y;

    Vec2 pos2 = pos;
    if (connect != PART_NONE) {
        pos2 += _bodies[connect]->getPosition();
    }
    
    size = size*cscale;
    BoxObstacle* body = BoxObstacle::create(pos2,size);
    body->setName(getPartName(part));
    body->retain();
    body->setDensity(DEFAULT_DENSITY);
    body->setDrawScale(_drawScale);
    _bodies.push_back(body);
    return body;
}

/**
 * Returns the texture key for the given body part.
 *
 * As some body parts are symmetrical, we reuse textures.
 *
 * @return the texture key for the given body part
 */
std::string RagdollModel::getPartName(int part) {
    switch (part) {
        case PART_HEAD:
            return HEAD_TEXTURE;
        case PART_BODY:
            return BODY_TEXTURE;
        case PART_LEFT_ARM:
        case PART_RIGHT_ARM:
            return ARM_TEXTURE;
        case PART_LEFT_FOREARM:
        case PART_RIGHT_FOREARM:
            return FOREARM_TEXTURE;
        case PART_LEFT_THIGH:
        case PART_RIGHT_THIGH:
            return THIGH_TEXTURE;
        case PART_LEFT_SHIN:
        case PART_RIGHT_SHIN:
            return SHIN_TEXTURE;
        default:
            return "UNKNOWN";
    }
}

/**
 * Destroys this ragdoll, releasing all resources.
 */
RagdollModel::~RagdollModel(void) {
    for(auto it = _bodies.begin(); it != _bodies.end(); ++it) {
        (*it)->release();
    }
    _bodies.clear();
    _bubbler = nullptr;
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
bool RagdollModel::createJoints(b2World& world) {
    
    b2RevoluteJointDef jointDef;
    b2Joint* joint;
    
    // NECK JOINT
    jointDef.bodyA = _bodies[PART_HEAD]->getBody();
    jointDef.bodyB = _bodies[PART_BODY]->getBody();
    jointDef.localAnchorA.Set(0, (-TORSO_OFFSET) / 2);
    jointDef.localAnchorB.Set(0, ( TORSO_OFFSET) / 2);
    jointDef.enableLimit = true;
    jointDef.upperAngle = M_PI/2.0f;
    jointDef.lowerAngle = -M_PI/2.0f;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);
 
    // SHOULDERS
    jointDef.bodyA = _bodies[PART_LEFT_ARM]->getBody();
    jointDef.bodyB = _bodies[PART_BODY]->getBody();
    jointDef.localAnchorA.Set(ARM_XOFFSET / 2, 0);
    jointDef.localAnchorB.Set(-ARM_XOFFSET / 2, ARM_YOFFSET);
    jointDef.enableLimit = true;
    jointDef.upperAngle = M_PI/2.0f;
    jointDef.lowerAngle = -M_PI/2.0f;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);
    
    jointDef.bodyA = _bodies[PART_RIGHT_ARM]->getBody();
    jointDef.bodyB = _bodies[PART_BODY]->getBody();
    jointDef.localAnchorA.Set(-ARM_XOFFSET / 2, 0);
    jointDef.localAnchorB.Set(ARM_XOFFSET / 2, ARM_YOFFSET);
    jointDef.enableLimit = true;
    jointDef.upperAngle = M_PI/2.0f;
    jointDef.lowerAngle = -M_PI/2.0f;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);
    
    // ELBOWS
    jointDef.bodyA = _bodies[PART_LEFT_FOREARM]->getBody();
    jointDef.bodyB = _bodies[PART_LEFT_ARM]->getBody();
    jointDef.localAnchorA.Set(FOREARM_OFFSET / 2, 0);
    jointDef.localAnchorB.Set(-FOREARM_OFFSET / 2, 0);
    jointDef.enableLimit = true;
    jointDef.upperAngle = M_PI/2;
    jointDef.lowerAngle = -M_PI/2;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);
    
    jointDef.bodyA = _bodies[PART_RIGHT_FOREARM]->getBody();
    jointDef.bodyB = _bodies[PART_RIGHT_ARM]->getBody();
    jointDef.localAnchorA.Set(-FOREARM_OFFSET / 2, 0);
    jointDef.localAnchorB.Set(FOREARM_OFFSET / 2, 0);
    jointDef.enableLimit = true;
    jointDef.upperAngle = M_PI/2;
    jointDef.lowerAngle = -M_PI/2;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);
    
     // HIPS
    jointDef.bodyA = _bodies[PART_LEFT_THIGH]->getBody();
    jointDef.bodyB = _bodies[PART_BODY]->getBody();
    jointDef.localAnchorA.Set(0, THIGH_YOFFSET / 2);
    jointDef.localAnchorB.Set(-THIGH_XOFFSET, -THIGH_YOFFSET/2);
    jointDef.enableLimit = true;
    jointDef.upperAngle = M_PI/2;
    jointDef.lowerAngle = -M_PI/2;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);
    
    jointDef.bodyA = _bodies[PART_RIGHT_THIGH]->getBody();
    jointDef.bodyB = _bodies[PART_BODY]->getBody();
    jointDef.localAnchorA.Set(0, THIGH_YOFFSET / 2);
    jointDef.localAnchorB.Set(THIGH_XOFFSET, -THIGH_YOFFSET/2);
    jointDef.enableLimit = true;
    jointDef.upperAngle = M_PI/2;
    jointDef.lowerAngle = -M_PI/2;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);
    
    // KNEES
    jointDef.bodyA = _bodies[PART_LEFT_THIGH]->getBody();
    jointDef.bodyB = _bodies[PART_LEFT_SHIN]->getBody();
    jointDef.localAnchorA.Set(0, -SHIN_OFFSET / 2);
    jointDef.localAnchorB.Set(0, SHIN_OFFSET / 2);
    jointDef.enableLimit = true;
    jointDef.upperAngle = M_PI/2;
    jointDef.lowerAngle = -M_PI/2;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);
    
    jointDef.bodyA = _bodies[PART_RIGHT_THIGH]->getBody();
    jointDef.bodyB = _bodies[PART_RIGHT_SHIN]->getBody();
    jointDef.localAnchorA.Set(0, -SHIN_OFFSET / 2);
    jointDef.localAnchorB.Set(0, SHIN_OFFSET / 2);
    jointDef.enableLimit = true;
    jointDef.upperAngle = M_PI/2;
    jointDef.lowerAngle = -M_PI/2;
    joint = world.CreateJoint(&jointDef);
    _joints.push_back(joint);
    
    // Weld bubbler to the head.
    b2WeldJointDef weldDef;
   weldDef.bodyA = _bodies[PART_HEAD]->getBody();
   weldDef.bodyB = _bubbler->getBody();
   weldDef.localAnchorA.Set(BUBB_OFF[0], BUBB_OFF[1]);
   weldDef.localAnchorB.Set(0, 0);
   joint = world.CreateJoint(&weldDef);
   _joints.push_back(joint);

    // Weld center of mass to torso
    weldDef.bodyA = _bodies[PART_BODY]->getBody();
    weldDef.bodyB = _body;
    weldDef.localAnchorA.Set(0,0);
    weldDef.localAnchorB.Set(0,0);
    joint = world.CreateJoint(&weldDef);
    _joints.push_back(joint);
    
    return true;
}

/**
 * Create new fixtures for this body, defining the shape
 *
 * This method is typically undefined for complex objects.  However, it
 * is necessary if we want to weld the body to track the center of mass.
 * Joints without fixtures are undefined.
 */
void RagdollModel::createFixtures() {
    if (_body == nullptr) {
        return;
    }
    
    releaseFixtures();
    
    // Create the fixture for the center of mass
    b2CircleShape shape;
    shape.m_radius = CENTROID_RADIUS;
    _fixture.shape = &shape;
    _fixture.density = CENTROID_DENSITY;
    _centroid = _body->CreateFixture(&_fixture);

    markDirty(false);
}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This method is typically undefined for complex objects.  However, it
 * is necessary if we want to weld the body to track the center of mass.
 * Joints without fixtures are undefined.
 */
void RagdollModel::releaseFixtures() {
    if (_centroid != nullptr) {
        _body->DestroyFixture(_centroid);
        _centroid = nullptr;
    }
}


#pragma mark -
#pragma mark Scene Graph Management
/**
 * Performs any necessary additions to the scene graph node.
 *
 * This method is necessary for custom physics objects that are composed
 * of multiple scene graph nodes.
 */
void RagdollModel::resetSceneNode() {
    // We need to know the content scale for resolution independence
    // If the device is higher resolution than 1024x576, Cocos2d will scale it
    // This was set as the design resolution in AppDelegate
    float cscale = Director::getInstance()->getContentScaleFactor();

    SceneManager* scene = AssetManager::getInstance()->getCurrent();

    for(int ii = 0; ii <= PART_RIGHT_SHIN; ii++) {
        std::string name = getPartName(ii);
        Texture2D* image = scene->get<Texture2D>(name);
        PolygonNode* sprite = PolygonNode::createWithTexture(image);
        sprite->setScale(cscale);
        if (ii == PART_RIGHT_ARM || ii == PART_RIGHT_FOREARM) {
            sprite->flipHorizontal(true); // More reliable than rotating 90 degrees.
        }
        _bodies[ii]->setSceneNode(sprite);
        _node->addChild(sprite);
    }
        
    // Bubbler takes a standard node
    Node* bubbs = Node::create();
    bubbs->setPosition(_node->getPosition());
    _bubbler->setSceneNode(bubbs);
    _node->addChild(bubbs);
}

/**
 * Redraws the outline of the physics fixtures to the debug node
 *
 * The debug node is use to outline the fixtures attached to this object.
 * This is very useful when the fixtures have a very different shape than
 * the texture (e.g. a circular shape attached to a square texture).
 */
void RagdollModel::resetDebugNode() {
    for(int ii = 0; ii <= PART_RIGHT_SHIN; ii++) {
        WireNode* wire = WireNode::create();
        wire->setColor(Color3B::YELLOW);
        wire->setOpacity(192);
        _bodies[ii]->setDebugNode(wire);
        _debug->addChild(wire);
    }
    
	// Add a debug for the bubbler
    WireNode* wire = WireNode::create();
    wire->setColor(Color3B::RED);
    wire->setOpacity(192);
    _bubbler->setDebugNode(wire);
    _debug->addChild(wire);
}
