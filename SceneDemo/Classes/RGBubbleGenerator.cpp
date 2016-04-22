//
//  RGBubbleGenerator.cpp
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
#include "RGBubbleGenerator.h"
#include "cornell/CUPolygonNode.h"
#include "cornell/CUAssetManager.h"

/** Creates a new Particle with no lifespan */
BubbleGenerator::Particle::Particle() : life(-1) {
    SceneManager* scene = AssetManager::getInstance()->getCurrent();
    Texture2D* image = scene->get<Texture2D>(BUBBLE_TEXTURE);
    node = PolygonNode::createWithTexture(image);
    // We need to know the content scale for resolution independence
    // If the device is higher resolution than 1024x576, Cocos2d will scale it
    // This was set as the design resolution in AppDelegate
    float cscale = Director::getInstance()->getContentScaleFactor();
    node->setScale(cscale);
    node->retain();
}

/** Disposes of this particle, permanently deleting all resources */
BubbleGenerator::Particle::~Particle() {
    node->release();
    node = nullptr;
}

/**
 * Updates the particle based on drawing scale
 *
 * @param  scale    the current drawing scale
 */
void BubbleGenerator::Particle::update(const Vec2& scale) {
    Vec2 pos(position.x*scale.x,position.y*scale.y);
    node->setPosition(pos);
}


#pragma mark -
#pragma mark Static Constructors
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
BubbleGenerator* BubbleGenerator::create() {
    BubbleGenerator* bubbles = new (std::nothrow) BubbleGenerator();
    if (bubbles && bubbles->init()) {
        bubbles->autorelease();
        return bubbles;
    }
    CC_SAFE_DELETE(bubbles);
    return nullptr;
}

/**
 * Creates a new bubble generator at the given position.
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
 * @return  An autoreleased physics object
 */
BubbleGenerator* BubbleGenerator::create(const Vec2& pos) {
    BubbleGenerator* bubbles = new (std::nothrow) BubbleGenerator();
    if (bubbles && bubbles->init(pos)) {
        bubbles->autorelease();
        return bubbles;
    }
    CC_SAFE_DELETE(bubbles);
    return nullptr;
}

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
BubbleGenerator* BubbleGenerator::create(const Vec2& pos, const Vec2& scale) {
    BubbleGenerator* bubbles = new (std::nothrow) BubbleGenerator();
    if (bubbles && bubbles->init(pos,scale)) {
        bubbles->autorelease();
        return bubbles;
    }
    CC_SAFE_DELETE(bubbles);
    return nullptr;
}


#pragma mark -
#pragma mark Initializers

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
bool BubbleGenerator::init(const Vec2& pos, const Vec2& scale) {
    // We need to know the content scale for resolution independence
    // If the device is higher resolution than 1024x576, Cocos2d will scale it
    // This was set as the design resolution in AppDelegate
    float cscale = Director::getInstance()->getContentScaleFactor();
    WheelObstacle::init(pos,BUBBLE_RADIUS*cscale);
    setDrawScale(scale);
    setDensity(1);
    _lifespan = DEFAULT_LIFE;
    _cooldown = 0;

    _memory = new GreedyFreeList<Particle>(MAX_PARTICLES);
    return true;
}

/**
 * Destroys this bubble generator, releasing all resources.
 */
BubbleGenerator::~BubbleGenerator() {
    delete _memory;
    _memory = nullptr;
}


#pragma mark -
#pragma mark Bubble Management

/**
 * Generates a new bubble object and puts it on the screen.
 */
void BubbleGenerator::bubble() {
    Particle* p = _memory->alloc();
    if (p != nullptr) {
        p->position.set(getPosition());
        p->life = _lifespan;
        p->update(_drawScale);
        _node->addChild(p->node);
    }
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method for cooldowns and bubble movement.  We also override
 * the graphics support in WheelObstacle, as our shape behaves differently.
 *
 * @param  delta    number of seconds since last animation frame
 */
void BubbleGenerator::update(float dt) {
    for(int ii = 0; ii < MAX_PARTICLES; ii++) {
        Particle* p = &(_memory->getPreallocated()[ii]);
        if (p->life > 0) {
            p->position.y += 1/_drawScale.x;
            p->life -= 1;
            if (p->life == 0) {
                _memory->free(p);
                _node->removeChild(p->node);
            } else {
                p->update(_drawScale);
            }
        }
    }
    
    if (_cooldown == 0) {
        _bubbled = true;
        bubble();
        _cooldown = BUBBLE_TIME;
    } else {
        _bubbled = false;
        _cooldown--;
    }
    updateDebug(dt);
}

/**
 * Updates the debug Node, displaying the generator position.
 *
 * This part of the code behaves like WheelObstacle.  We had to factor it
 * out of update() to reinstate it.
 *
 * @param  delta    number of seconds since last animation frame
 */
void BubbleGenerator::updateDebug(float dt) {
    Vec2 pos = getPosition();
    pos.scale(_drawScale);
    float angle = -getAngle()*180.0f/M_PI;
    
    // Positional snap
    if (_posSnap >= 0) {
        pos.x = floor((pos.x*_posFact+0.5f)/_posFact);
        pos.y = floor((pos.y*_posFact+0.5f)/_posFact);
    }
    // Rotational snap
    if (_angSnap >= 0) {
        angle = floor((angle*_angFact+0.5f)/_angFact);
    }
    
    if (_debug != nullptr) {
        _debug->setPosition(pos);
        _debug->setRotation(angle);
    }
}

