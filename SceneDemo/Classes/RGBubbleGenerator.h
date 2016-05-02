//
//  RGBubbleGenerator.h
//  RagdollDemo
//
//  This module provides a physics object that can generate bubbles from its current
//  location.  It is a physics object so that we can weld it to another object to
//  control its current position.  However, its main purpose is as a particular generator
//  and therefore its graphics are slightly different than other physics obstacles.
//  See the comments on setSceneNode below for more information.
//
//  Notices that this class makes use of GreedyFreelist.  That is a free list that
//  aggressively recycles objects, making it ideal for a particle system.
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
#ifndef __RG_BUBBLE_GENERATOR_H__
#define __RG_BUBBLE_GENERATOR_H__

#include "cornell/CUWheelObstacle.h"
#include "cornell/CUGreedyFreeList.h"
#include <Box2D/Collision/Shapes/b2CircleShape.h>

using namespace cocos2d;


#pragma mark -
#pragma mark Bubble Defaults
/** The name of the bubble texture */
#define BUBBLE_TEXTURE  "bubble"
/** Maximum number of bubbles at a time */
#define MAX_PARTICLES   6
/** How long we have left to live */
#define DEFAULT_LIFE    250
/** How often we make bubbles */
#define BUBBLE_TIME     200
/** How big the bubbles are */
#define BUBBLE_RADIUS   0.2


#pragma mark -
#pragma mark Bubble Generator
/**
 * Physics object that generates non-physics bubble shapes.
 *
 * It is a physics object so that we can weld it to another object to control its
 * current position. However, its main purpose is as a particular generator, where
 * each particle is generated relative to the current location of this generator.
 *
 * The graphics for this class are slightly different than they are for other 
 * physics obstacles.  The debug Node works as normal; it controls a positional 
 * wireframe that shows the current location of the generator.  However, the scene
 * node should be a blank node centered at the origin.  That way, the the particles
 * can be added as individual children to this node.
 *
 *  Notices that this class makes use of GreedyFreelist.  That is a free list that
 *  aggressively recycles objects, making it ideal for a particle system.
 */
class BubbleGenerator : public WheelObstacle {
protected:
#pragma mark Particle System
    /**
     * Representation of the bubbles for drawing purposes. 
     *
     * This is a graphics object, NOT a physics object.
     */
    class Particle {
    public:
        /** Position of the bubble in Box2d space */
        Vec2 position;
        /** The number of animation frames left to live */
        int life;
        /** The image for this particle */
        Node* node;
        
        /** Creates a new Particle with no lifespan */
        Particle();
        
        /** Disposes of this particle, permanently deleting all resources */
        ~Particle();
        
        /** Resets the particle so it can be reclaimed by the free list */
        void reset() {
            position.set(0,0);
            node->setPosition(Vec2::ZERO);
            life = -1;
        }
        
        /** 
         * Updates the particle based on drawing scale 
         *
         * @param  scale    the current drawing scale
         */
        void update(const Vec2& scale);
    };
    
    /** Free list to manage the system of particles. */
    GreedyFreeList<Particle>* _memory;
    
    /** How long bubbles live after creation */
    int _lifespan;
    /** How long until we can make another bubble */
    int _cooldown;
    /** Whether or not we bubbled this animation frame */
    bool _bubbled;
    
    
#pragma mark Static Constructors
public:
    /**
     * Creates a new bubble generator at the origin.
     *
     * The generator is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  An autoreleased physics object
     */
    static BubbleGenerator* create();

    /**
     * Creates a new bubble generator at the given position.
     *
     * The generator is scaled so that 1 pixel = 1 Box2d unit
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
    static BubbleGenerator* create(const Vec2& pos);
    
    /**
     * Creates a new bubble generator at the given position.
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
    static BubbleGenerator* create(const Vec2& pos, const Vec2& scale);
    
    
#pragma mark Bubble Management
    /**
     * Returns the lifespan of a generated bubble.
     *
     * @return the lifespan of a generated bubble.
     */
    int getLifeSpan() const { return _lifespan; }
    
    /**
     * Sets the lifespan of a generated bubble.
     *
     * Changing this does not effect bubbles already generated.
     *
     * @param value the lifespan of a generated bubble.
     */
    void setLifeSpan(int value) { _lifespan = value; }

    /** 
     * Generates a new bubble object and puts it on the screen.
     */
    void bubble();

    /**
     * Returns true if we generated a bubble this animation frame.
     *
     * @return true if we generated a bubble this animation frame.
     */
    bool didBubble() const { return _bubbled; }
    
    
#pragma mark Animation Support
    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method for cooldowns and bubble movement.  We also override
     * the graphics support in WheelObstacle, as our shape behaves differently.
     *
     * @param  delta    number of seconds since last animation frame
     */
    virtual void update(float dt) override;
    
    /**
     * Updates the debug Node, displaying the generator position.
     *
     * This part of the code behaves like WheelObstacle.  We had to factor it
     * out of update() to reinstate it.
     *
     * @param  delta    number of seconds since last animation frame
     */
    void updateDebug(float dt);

    /**
     * Repositions the scene node so that it agrees with the physics object.
     *
     * We disable positioning as the node be fixed, regardless of movement.
     */
    virtual void positionSceneNode() override {}
    
    
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /*
     * Creates a new bubble generator at the origin.
     */
    BubbleGenerator(void) : WheelObstacle() { _bubbled = false; }
    
    /**
     * Destroys this bubble generator, releasing all resources.
     */
    virtual ~BubbleGenerator(void);

    /**
     * Initializes a new bubble generator at the origin.
     *
     * The generator is scaled so that 1 pixel = 1 Box2d unit
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
     * Initializes a new bubble generator at the given position.
     *
     * The generator is scaled so that 1 pixel = 1 Box2d unit
     *
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos      Initial position in world coordinates
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2& pos) override { return init(pos,Vec2::ONE); }

    /**
     * Initializes a new bubble generator at the given position.
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
};

#endif /* __RG_BUBBLE_GENERATOR_H__ */
