//
//  CUSimpleObstacle.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a physics object that supports mutliple Bodies.
//  This is the base class for objects that are tied together with joints.
//
//  This class does not provide Shape information, and cannot be instantiated
//  directly.  There are no default complex objects.  You will need to create
//  your own subclasses to use this class.  It is very similar to Lab 4
//  from CS 3152.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 11/24/15
//

#include "CUComplexObstacle.h"
#include <Box2D/Dynamics/b2World.h>

NS_CC_BEGIN


#pragma mark -
#pragma mark Fixture Methods
/**
 * Sets the density of this body
 *
 * The density is typically measured in usually in kg/m^2. The density can be zero or
 * positive. You should generally use similar densities for all your fixtures. This
 * will improve stacking stability.
 *
 * This method affects the root body of this composite structure only.  If you want
 * to set the value for any of the child obstacles, iterate over the children.
 *
 * @param value  the density of this body
 */
void ComplexObstacle::setDensity(float value) {
    _fixture.density = value;
    if (_body != nullptr) {
        for (b2Fixture* f = _body->GetFixtureList(); f; f = f->GetNext()) {
            f->SetDensity(value);
        }
        if (!_masseffect) {
            _body->ResetMassData();
        }
    }
}

/**
 * Sets the friction coefficient of this body
 *
 * The friction parameter is usually set between 0 and 1, but can be any non-negative
 * value. A friction value of 0 turns off friction and a value of 1 makes the friction
 * strong. When the friction force is computed between two shapes, Box2D must combine
 * the friction parameters of the two parent fixtures. This is done with the geometric
 * mean.
 *
 * This method affects the root body of this composite structure only.  If you want
 * to set the value for any of the child obstacles, iterate over the children.
 *
 * @param value  the friction coefficient of this body
 */
void ComplexObstacle::setFriction(float value) {
    _fixture.friction = value;
    if (_body != nullptr) {
        for (b2Fixture* f = _body->GetFixtureList(); f; f = f->GetNext()) {
            f->SetFriction(value);
        }
    }
}

/**
 * Sets the restitution of this body
 *
 * Restitution is used to make objects bounce. The restitution value is usually set
 * to be between 0 and 1. Consider dropping a ball on a table. A value of zero means
 * the ball won't bounce. This is called an inelastic collision. A value of one means
 * the ball's velocity will be exactly reflected. This is called a perfectly elastic
 * collision.
 *
 * This method affects the root body of this composite structure only.  If you want
 * to set the value for any of the child obstacles, iterate over the children.
 *
 * @param value  the restitution of this body
 */
void ComplexObstacle::setRestitution(float value) {
    _fixture.restitution = value;
    if (_body != nullptr) {
        for (b2Fixture* f = _body->GetFixtureList(); f; f = f->GetNext()) {
            f->SetRestitution(value);
        }
    }
}

/**
 * Sets whether this object is a sensor.
 *
 * Sometimes game logic needs to know when two entities overlap yet there should be
 * no collision response. This is done by using sensors. A sensor is an entity that
 * detects collision but does not produce a response.
 *
 * This method affects the root body of this composite structure only.  If you want
 * to set the value for any of the child obstacles, iterate over the children.
 *
 * @param value  whether this object is a sensor.
 */
void ComplexObstacle::setSensor(bool value) {
    _fixture.isSensor = value;
    if (_body != nullptr) {
        for (b2Fixture* f = _body->GetFixtureList(); f; f = f->GetNext()) {
            f->SetSensor(value);
        }
    }
}

/**
 * Sets the filter data for this object
 *
 * Collision filtering allows you to prevent collision between fixtures. For example,
 * say you make a character that rides a bicycle. You want the bicycle to collide
 * with the terrain and the character to collide with the terrain, but you don't want
 * the character to collide with the bicycle (because they must overlap). Box2D
 * supports such collision filtering using categories and groups.
 *
 * A value of null removes all collision filters. This method affects ALL of the
 * bodies in this composite structure.  For fine-grain control, you will need to
 * loop over all elements in the composite structure.
 *
 * @param value  the filter data for this object
 */
void ComplexObstacle::setFilterData(b2Filter value) {
    _fixture.filter = value;
    if (_body != nullptr) {
        for (b2Fixture* f = _body->GetFixtureList(); f; f = f->GetNext()) {
            f->SetFilterData(value);
        }
    }
}


#pragma mark -
#pragma mark Physics Methods

/**
 * Creates the physics Body(s) for this object, adding them to the world.
 *
 * Implementations of this method should NOT retain a reference to World.
 * That is a tight coupling that we should avoid.
 *
 * @param world Box2D world to store body
 *
 * @return true if object allocation succeeded
 */
bool ComplexObstacle::activatePhysics(b2World& world) {
    // Make a body, if possible
    _bodyinfo.active = true;
    _body = world.CreateBody(&_bodyinfo);
    _body->SetUserData((void*)this);
    
    // Only initialize if a body was created.
    bool success = true;
    if (_body == nullptr) {
        _bodyinfo.active = false;
        return false;
    }
    createFixtures();
    
    // Active all other bodies.
    for(auto it = _bodies.begin(); it != _bodies.end(); ++it) {
        success = success && (*it)->activatePhysics(world);
    }
    success = success && createJoints(world);
    
    // Clean up if we failed
    if (!success) {
        deactivatePhysics(world);
    }
    return success;
}

/**
 * Destroys the physics Body(s) of this object if applicable,
 * removing them from the world.
 *
 * @param world Box2D world that stores body
 */
void ComplexObstacle::deactivatePhysics(b2World& world) {
    // Should be good for most (simple) applications.
    if (_body != nullptr) {
        for(auto it = _joints.begin(); it!= _joints.end(); ++it) {
            world.DestroyJoint(*it);
        }
        _joints.clear();
        for(auto it = _bodies.begin(); it!= _bodies.end(); ++it) {
            (*it)->deactivatePhysics(world);
        }
        
        releaseFixtures();
        
        // Snapshot the values
        setBodyState(*_body);
        world.DestroyBody(_body);
        _body = nullptr;
        _bodyinfo.active = false;
    }
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * This method is called AFTER the collision resolution state. Therefore, it
 * should not be used to process actions or any other gameplay information.  Its
 * primary purpose is to adjust changes to the fixture, which have to take place
 * after collision.
 *
 * In other words, this is the method that updates the scene graph.  If
 * you forget to call it, it will not draw your changes.
 *
 * @param dt Timing values from parent loop
 */
void ComplexObstacle::update(float delta) {
    // Recreate the fixture object if dimensions changed.
    if (isDirty()) {
        createFixtures();
    }
    
    // Update the scene graph if appropriate
    if (_tracking) {
        if (_node != nullptr) {
            Vec2 pos = getPosition();
            pos.scale(_drawScale);
            _node->setPosition(pos);
            _node->setRotation(-getAngle()*180.0f/M_PI);
        }
        if (_debug != nullptr) {
            Vec2 pos = getPosition();
            pos.scale(_drawScale);
            _debug->setPosition(pos);
            _debug->setRotation(-getAngle()*180.0f/M_PI);
        }
    }
    
    // Update the children
    for(auto it = _bodies.begin(); it!= _bodies.end(); ++it) {
        (*it)->update(delta);
    }
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
void ComplexObstacle::setDrawScale(float x, float y) {
    Obstacle::setDrawScale(x,y);
    for(auto it = _bodies.begin(); it!= _bodies.end(); ++it) {
        (*it)->setDrawScale(x,y);
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
void ComplexObstacle::positionSceneNode() {
    if (_tracking) {
        Obstacle::positionSceneNode();
    }
}

/**
 * Repositions the debug node so that it agrees with the physics object.
 *
 * By default, the position of a node should be the body position times
 * the draw scale.  However, for some obstacles (particularly complex
 * obstacles), it may be desirable to turn the default functionality
 * off.  Hence we have made this virtual.
 */
void ComplexObstacle::positionDebugNode() {
    if (_tracking) {
        Obstacle::positionDebugNode();
    }
}


#pragma mark -
#pragma mark Memory Management

/**
 * Deletes this physics object and all of its resources.
 *
 * We have to make the destructor public so that we can polymorphically
 * delete physics objects.
 *
 * The purpose of this destructor is to warn us if we delete an object
 * pre-maturely.
 */
ComplexObstacle::~ComplexObstacle() {
    CCASSERT(_body == nullptr, "You must deactive physics before deleting an object");
    for(auto it = _bodies.begin(); it != _bodies.end(); ++it) {
        (*it)->release();
    }
    _bodies.clear();
}

NS_CC_END