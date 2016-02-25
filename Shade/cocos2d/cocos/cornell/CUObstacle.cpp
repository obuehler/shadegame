//
//  CUObstacle.cpp
//  Cornell Extensions to Cocos2D
//
//  Cocos2d fits well with Box2d physics.  However, it tightly couples the physics
//  into the scene graph, which is the cardinal sin. With that said, there are some
//  times in which coupling is okay for convenience reasons (particularly when we
//  have the option to uncouple).  This module is such an example; it couples the
//  bodies and fixtures from Box2d into a single class, making the physics easier
//  to use (in most cases).
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 11/24/15
//
#include "CUObstacle.h"


NS_CC_BEGIN
#pragma mark -
#pragma mark Initializers
/*
 * Creates a new physics object at the origin.
 */
Obstacle::Obstacle(void) {
    _debug = nullptr;
    _node  = nullptr;
}

/**
 * Deletes this physics object and all of its resources.
 *
 * We have to make the destructor public so that we can polymorphically
 * delete physics objects.
 *
 * A non-default destructor is necessary since we must release all
 * claims on scene graph nodes.
 */
Obstacle::~Obstacle() {
    if (_node != nullptr) {
        _node->release();
        _node = nullptr;
    }
    if (_debug != nullptr) {
        _debug->release();
        _debug = nullptr;
    }
}

/**
 * Initializes a new physics object at the given point
 *
 * @param  vec  Initial position in world coordinates
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Obstacle::init(const Vec2& vec) {
    _drawScale.set(1.0f,1.0f);
    
    // Object has yet to be deactivated
    _remove = false;
    
    // Allocate the body information
    _bodyinfo.awake  = true;
    _bodyinfo.allowSleep = true;
    _bodyinfo.gravityScale = 1.0f;
    _bodyinfo.position.Set(vec.x,vec.y);
    // Objects are physics objects unless otherwise noted
    _bodyinfo.type = b2_dynamicBody;
    
    // Turn off the mass information
    _masseffect = false;
    
    return true;
}

/**
 * Copies the state from the given body to the body def.
 *
 * This is important if you want to save the state of the body before removing
 * it from the world.
 */
void Obstacle::setBodyState(const b2Body& body) {
    _bodyinfo.type   = body.GetType();
    _bodyinfo.angle  = body.GetAngle();
    _bodyinfo.active = body.IsActive();
    _bodyinfo.awake  = body.IsAwake();
    _bodyinfo.bullet = body.IsBullet();
    _bodyinfo.position.Set(body.GetPosition().x,body.GetPosition().y);
    _bodyinfo.linearVelocity.Set(body.GetLinearVelocity().x,body.GetLinearVelocity().y);
    _bodyinfo.allowSleep = body.IsSleepingAllowed();
    _bodyinfo.fixedRotation = body.IsFixedRotation();
    _bodyinfo.gravityScale  = body.GetGravityScale();
    _bodyinfo.angularDamping = body.GetAngularDamping();
    _bodyinfo.linearDamping  = body.GetLinearDamping();
}


#pragma mark -
#pragma mark MassData Methods

/**
 * Sets the center of mass for this physics body
 *
 * @param x  the x-coordinate of the center of mass for this physics body
 * @param y  the y-coordinate of the center of mass for this physics body
 */
void Obstacle::setCentroid(float x, float y) {
    if (!_masseffect) {
        _masseffect = true;
        _massdata.I = getInertia();
        _massdata.mass = getMass();
    }
    _massdata.center.Set(x,y);
}

/**
 * Sets the rotational inertia of this body
 *
 * For static bodies, the mass and rotational inertia are set to zero. When
 * a body has fixed rotation, its rotational inertia is zero.
 *
 * @param value  the rotational inertia of this body
 */
void Obstacle::setInertia(float value) {
    if (!_masseffect) {
        _masseffect = true;
        _massdata.center.Set(getCentroid().x,getCentroid().y);
        _massdata.mass = getMass();
    }
    _massdata.I = value;
}

/**
 * Sets the mass of this body
 *
 * The value is usually in kilograms.
 *
 * @param value  the mass of this body
 */
void Obstacle::setMass(float value) {
    if (!_masseffect) {
        _masseffect = true;
        _massdata.center.Set(getCentroid().x,getCentroid().y);
        _massdata.I = getInertia();
    }
    _massdata.mass = value;
}


#pragma mark -
#pragma mark Scene Graph Methods

/**
 * Sets the drawing scale for this physics object
 *
 * The drawing scale is the number of pixels to draw before Box2D unit. Because
 * mass is a function of area in Box2D, we typically want the physics objects
 * to be small.  So we decouple that scale from the physics object.  However,
 * we must track the scale difference to communicate with the scene graph.
 *
 * We allow for the scaling factor to be non-uniform.
 *
 * @param x  the x-axis scale for this physics object
 * @param y  the y-axis scale for this physics object
 */
void Obstacle::setDrawScale(float x, float y) {
    _drawScale.set(x,y);
    if (_node != nullptr) {
        resetSceneNode();
        positionSceneNode();
    }
    if (_debug != nullptr) {
        resetDebugNode();
        positionDebugNode();
    }
}

/**
 * Sets the scene graph node for drawing purposes.
 *
 * The scene graph is completely decoupled from the physics system.  The node
 * does not have to be the same size as the physics body. We only guarantee
 * that the node is positioned correctly according to the drawing scale.
 *
 * @param value  the scene graph node for drawing purposes.
 *
 * @retain  a reference to this scene graph node
 * @release the previous debug graph node used by this object
 */
void Obstacle::setSceneNode(Node* node) {
    // Release the node if we have one previously
    if (_node != nullptr) {
        _node->release();
    }
    _node = node;
    // Remember to retain the node
    if (_node != nullptr) {
        _node->retain();
        this->positionSceneNode();
        this->resetSceneNode();
    }
}

/**
 * Sets the scene graph node for debugging purposes.
 *
 * The scene graph is completely decoupled from the physics system.  The node
 * does not have to be the same size as the physics body. We only guarantee
 * that the node is positioned correctly according to the drawing scale.
 *
 * @param value  the scene graph node for drawing purposes.
 *
 * @retain  a reference to this debug scene graph node
 * @release the previous debug scene graph node used by this object
 */
void Obstacle::setDebugNode(WireNode* node) {
    // Release the node if we have one previously
    if (_debug != nullptr) {
        _debug->release();
    }
    _debug = node;
    // Remember to retain the node
    if (_debug != nullptr) {
        _debug->retain();
        this->positionDebugNode();
        this->resetDebugNode();
    }
}

/**
 * Repositions the scene node so that it agrees with the physics object.
 *
 * By default, the position of a node should be the body position times
 * the draw scale.  However, for some obstacles (particularly complex
 * obstacles), it may be desirable to turn the default functionality
 * off.  Hence we have made this virtual.
 */
void Obstacle::positionSceneNode() {
    CCASSERT(_node, "Attempt to reposition a null scene node");
    Vec2 pos = getPosition();
    pos.scale(_drawScale);
    float angle = -getAngle()*180.0f/M_PI;
    _node->setPosition(pos);
    _node->setRotation(angle);
}

/**
 * Repositions the debug node so that it agrees with the physics object.
 *
 * By default, the position of a node should be the body position times
 * the draw scale.  However, for some obstacles (particularly complex
 * obstacles), it may be desirable to turn the default functionality
 * off.  Hence we have made this virtual.
 */
void Obstacle::positionDebugNode() {
    CCASSERT(_debug, "Attempt to reposition a null debug node");
    Vec2 pos = getPosition();
    pos.scale(_drawScale);
    float angle = -getAngle()*180.0f/M_PI;
    _debug->setPosition(pos);
    _debug->setRotation(angle);
}



#pragma mark -
#pragma Debugging Methods

/**
 * Returns a string representation of this physics object.
 *
 * This method converts the physics object into a string for debugging.  By
 * default it shows the tag and position.  Other physics objects may want to
 * override this method for more detailed information.
 *
 * @return a string representation of this physics object
 */
std::string Obstacle::toString() const {
    std::stringstream ss;
    Vec2 p = getPosition();
    ss << "[Obstacle " << _tag << ": (" << p.x << "," << p.y << "), ";
    ss << (isActive() ? "active ]" : "inactive ]");
    return ss.str();
}

/**
 * Outputs this physics object to the given output stream.
 *
 * This function uses the toString() method to convert the physics object
 * into a string
 *
 * @param  os   the output stream
 * @param  obj  the physics object to ouput
 *
 * @return the output stream
 */
std::ostream& operator<<(std::ostream& os, const Obstacle& obj) {
    os << obj.toString();
    return os;
}

NS_CC_END
