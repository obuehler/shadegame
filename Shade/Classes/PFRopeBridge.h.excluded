//
//  PFRopeBridge.h
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
#ifndef __PF_ROPE_BRIDGE_H__
#define __PF_ROPE_BRIDGE_H__
#include <cornell/CUComplexObstacle.h>


using namespace cocos2d;

#pragma mark -
#pragma mark Bridge Constants
/** The key for the texture for the bridge planks */
#define BRIDGE_TEXTURE "bridge"
/** The debug name for the entire obstacle */
#define BRIDGE_NAME     "bridge"
/** The debug name for each plank */
#define PLANK_NAME      "barrier"
/** The debug name for each anchor pin */
#define BRIDGE_PIN_NAME "pin"


#pragma mark -
#pragma mark Rope Bridge
/**
 * A bridge with planks connected by revolute joints.
 *
 * Note that this module handles its own scene graph management.  As a ComplexObstacle
 * owns all of its child obstacles, it is natural for it to own the corresponding
 * scene graph. In order to work correctly, the AssetManager must be pointing to the
 * correct scene when the ragdoll is created.
 */
class RopeBridge : public ComplexObstacle {
protected:
    /** The width of a single plank in the bridge */
    float _linksize;

    
#pragma mark -
#pragma mark Scene Graph Internals
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
     *
     * Unfortunately, the current implementation is very inefficient.  Cocos2d
     * does not batch drawnode commands like it does Sprites or PolygonSprites.
     * Therefore, every distinct DrawNode is a distinct OpenGL call.  This can
     * really hurt framerate when debugging mode is on.  Ideally, we would refactor
     * this so that we only draw to a single, master draw node.  However, this
     * means that we would have to handle our own vertex transformations, instead
     * of relying on the transforms in the scene graph.
     */
    virtual void resetDebugNode() override;

    
public:
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
    static RopeBridge* create();
    
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
    static RopeBridge* create(const Vec2& start, const Vec2& end);
    
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
    static RopeBridge* create(const Vec2& start, const Vec2& end, const Vec2& scale);
    
    
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
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Hidden Constructors
    /**
     * Creates a degenerate rope bridge.
     *
     * This constructor does not initialize any of the spinner values beyond
     * the defaults.  To use a rope bridge, you must call init().
     */
    RopeBridge() : ComplexObstacle() { }
    
    /**
     * Initializes a new horizontal bridge of length 1.
     *
     * The bridge is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(Vec2::ZERO, Vec2(1,0), Vec2::ONE); }

    /**
     * Initializes a new rope bridge with the given anchors.
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
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2& start, const Vec2& end) { return init(start, end, Vec2::ONE); }

    /**
     * Initializes a new rope bridge with the given anchors.
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
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2& start, const Vec2& end, const Vec2& scale);

};

#endif /* __PF_ROPE_BRIDGE_H__ */
