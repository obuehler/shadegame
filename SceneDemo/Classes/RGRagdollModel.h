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
//  Version: 1/15/16
//

#ifndef __RD_RAGDOLL_MODEL_H__
#define __RD_RAGDOLL_MODEL_H__

#include <cornell/CUComplexObstacle.h>
#include <cornell/CUBoxObstacle.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>

/** Forward reference to the Bubble Generator */
class BubbleGenerator;

using namespace cocos2d;

#pragma mark -
#pragma mark Body Part Indices
/** Indices for the body parts in the bodies array */
#define PART_NONE           -1
#define PART_BODY           0
#define PART_HEAD           1
#define PART_LEFT_ARM       2
#define PART_RIGHT_ARM      3
#define PART_LEFT_FOREARM   4
#define PART_RIGHT_FOREARM  5
#define PART_LEFT_THIGH     6
#define PART_RIGHT_THIGH    7
#define PART_LEFT_SHIN      8
#define PART_RIGHT_SHIN     9


#pragma mark -
#pragma mark Body Part Textures
/** Textures for the body parts in the asset manager */
#define HEAD_TEXTURE        "head"
#define BODY_TEXTURE        "body"
#define ARM_TEXTURE         "arm"
#define FOREARM_TEXTURE     "forearm"
#define THIGH_TEXTURE       "thigh"
#define SHIN_TEXTURE        "shin"

#pragma mark -
#pragma mark Body Part Offsets
// Layout of ragdoll
//
// o = joint
//                   ___
//                  |   |
//                  |_ _|
//   ______ ______ ___o___ ______ ______
//  |______o______o       o______o______|
//                |       |
//                |       |
//                |_______|
//                | o | o |
//                |   |   |
//                |___|___|
//                | o | o |
//                |   |   |
//                |   |   |
//                |___|___|
//
/** Distance between torso center and face center */
#define TORSO_OFFSET    3.8f
/** Y-distance between torso center and arm center */
#define ARM_YOFFSET     1.75f
/** X-distance between torso center and arm center */
#define ARM_XOFFSET     3.15f
/** Distance between center of arm and center of forearm */
#define FOREARM_OFFSET  2.75f
/** X-distance from center of torso to center of leg */
#define THIGH_XOFFSET   0.75f
/** Y-distance from center of torso to center of thigh */
#define THIGH_YOFFSET   3.5f
/** Distance between center of thigh and center of shin */
#define SHIN_OFFSET     2.25f


#pragma mark -
#pragma mark Physics Constants
/** The density for each body part */
#define DEFAULT_DENSITY  1.0f
/** The density for the center of mass */
#define CENTROID_DENSITY 0.1f
/** The radius for the center of mass */
#define CENTROID_RADIUS  0.1f


#pragma mark -
#pragma mark Ragdoll

/**
 * A ragdoll whose body parts are boxes connected by joints
 *
 * Note that this module handles its own scene graph management.  As a ComplexObstacle
 * owns all of its child obstacles, it is natural for it to own the corresponding
 * scene graph. In order to work correctly, the AssetManager must be pointing to the
 * correct scene when the ragdoll is created.
 *
 * See the ragdoll diagram above, with the position offsets.
 */
class RagdollModel : public ComplexObstacle {
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CC_DISALLOW_COPY_AND_ASSIGN(RagdollModel);

protected:
    /** Shape to treat the root body as a center of mass */
    b2Fixture* _centroid;
    /** Bubble generator to glue to snorkler. */
    BubbleGenerator* _bubbler;
    
    /**
     * Returns the texture key for the given body part.
     *
     * As some body parts are symmetrical, we reuse textures.
     *
     * @return the texture key for the given body part
     */
    static std::string getPartName(int part);

    
public:
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
    static RagdollModel* create();
    
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
    static RagdollModel* create(const Vec2& pos);
    
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
    static RagdollModel* create(const Vec2& pos, const Vec2& scale);
    
    
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
    bool createJoints(b2World& world) override;
 
    /**
     * Create new fixtures for this body, defining the shape
     *
     * This method is typically undefined for complex objects.  While they
     * need a root body, they rarely need a root shape.  However, we provide
     * this method for maximum flexibility.
     */
    virtual void createFixtures() override;
    
    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This method is typically undefined for complex objects.  While they
     * need a root body, they rarely need a root shape.  However, we provide
     * this method for maximum flexibility.
     */
    virtual void releaseFixtures() override;


#pragma mark Attribute Accessors
    /**
     * Returns the bubble generator for this ragdoll
     *
     * The bubble generator will be offset at the snorkel on the head.
     *
     * @return the bubble generator for this ragdoll
     */
    const BubbleGenerator* getBubbleGenerator() const { return _bubbler; }

    
#pragma mark Scene Graph Management
    /**
     * Performs any necessary additions to the scene graph node.
     *
     * This method is necessary for custom physics objects that are composed
     * of multiple scene graph nodes.
     */
    virtual void resetSceneNode() override;
    
    /**
     * Redraws the outline of the physics fixtures to the debug node
     *
     * The debug node is use to outline the fixtures attached to this object.
     * This is very useful when the fixtures have a very different shape than
     * the texture (e.g. a circular shape attached to a square texture).
     */
    virtual void resetDebugNode() override;
    
    
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /*
     * Creates a new ragdoll at the origin.
     */
    RagdollModel(void) : ComplexObstacle() {
        _centroid = nullptr;
        _bubbler = nullptr;
    }
    
    /**
     * Destroys this ragdoll, releasing all resources.
     */
    virtual ~RagdollModel(void);
    
    /**
     * Initializes a new ragdoll with its torso centered at the origin.
     *
     * The ragdoll is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(Vec2::ZERO,Vec2::ONE); }
    
    /**
     * Initializes a new a new ragdoll with its torso at the given position
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
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2& pos) override { return init(pos,Vec2::ONE); }
    
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
    virtual bool init(const Vec2& pos, const Vec2& scale);

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
    BoxObstacle* makePart(int part, int connect, const Vec2& pos, const Vec2& scale);
};

#endif /* defined(__RD_RAGDOLL_MODEL_H__) */
