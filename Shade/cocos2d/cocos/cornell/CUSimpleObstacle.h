//
//  CUSimpleObstacle.h
//  Cornell Extensions to Cocos2D
//
//  This class serves to provide a uniform interface for all single-body objects
//  (regardless of shape).  How, it still cannot be instantiated directly, as
//  the correct instantiation depends on the shape.  See BoxObstacle and CircleObstacle
//  for concrete examples
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 11/24/15
//
#ifndef __CC_SIMPLE_OBSTACLE_H__
#define __CC_SIMPLE_OBSTACLE_H__

#include "CUObstacle.h"


NS_CC_BEGIN

#pragma mark -
#pragma mark Simple Obstacle

/**
 * Base model class to support collisions.
 *
 * This is an instance of a Physics object with just one body. It does not have any
 * joints. It is the primary type of physics object. This class does not provide Shape
 * information, and should not be instantiated directly.
 *
 * Many of the method comments in this class are taken from the Box2d manual by
 * Erin Catto (2011).
 */
class CC_DLL SimpleObstacle : public Obstacle {
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CC_DISALLOW_COPY_AND_ASSIGN(SimpleObstacle);

protected:
    /** The physics body for Box2D. */
    b2Body* _body;
    
    /** Number of decimal places to snap position of image to physics body */
    int _posSnap;
    /** Cache of factor to snap position of image to physics body */
    unsigned long _posFact;
    /** Number of decimal places to snap rotation of image to physics body */
    int _angSnap;
    /** Cache of factor to snap rotation of image to physics body */
    unsigned long _angFact;

    
public:
#pragma mark -
#pragma mark BodyDef Methods
    /**
     * Returns the body type for Box2D physics
     *
     * If you want to lock a body in place (e.g. a platform) set this value to STATIC.
     * KINEMATIC allows the object to move (and some limited collisions), but ignores
     * external forces (e.g. gravity). DYNAMIC makes this is a full-blown physics object.
     *
     * @return the body type for Box2D physics
     */
    virtual b2BodyType getBodyType() const override {
        return (_body != nullptr ? _body->GetType() : _bodyinfo.type);
    }
    
    /**
     * Sets the body type for Box2D physics
     *
     * If you want to lock a body in place (e.g. a platform) set this value to STATIC.
     * KINEMATIC allows the object to move (and some limited collisions), but ignores
     * external forces (e.g. gravity). DYNAMIC makes this is a full-blown physics object.
     *
     * @param value the body type for Box2D physics
     */
    virtual void setBodyType(b2BodyType value) override {
        if (_body != nullptr) {
            _body->SetType(value);
        } else {
            _bodyinfo.type = value;
        }
    }
    
    /**
     * Returns the current position for this physics body
     *
     * This method converts from a Box2D vector type to a Cocos2D vector type. This
     * cuts down on the confusion between vector types.  It also means that changes
     * to the returned vector will have no effect on this object.
     *
     * @return the current position for this physics body
     */
    virtual Vec2 getPosition() const override {
        if (_body != nullptr) {
            return Vec2(_body->GetPosition().x,_body->GetPosition().y);
        } else {
            return Vec2(_bodyinfo.position.x,_bodyinfo.position.y);
        }
    }
    
    /**
     * Sets the current position for this physics body
     *
     * This method converts from a Cocos2D vector type to a Box2D vector type. This
     * cuts down on the confusion between vector types.
     *
     * @param value  the current position for this physics body
     */
    virtual void setPosition(const Vec2& value) override { setPosition(value.x,value.y); }
    
    /**
     * Sets the current position for this physics body
     *
     * @param x  the current x-coordinate for this physics body
     * @param y  the current y-coordinate for this physics body
     */
    virtual void setPosition(float x, float y) override {
        if (_body != nullptr) {
            _body->SetTransform(b2Vec2(x,y),_body->GetAngle());
        } else {
            _bodyinfo.position.Set(x,y);
        }
    }
    
    /**
     * Returns the x-coordinate for this physics body
     *
     * @return the x-coordinate for this physics body
     */
    virtual float getX() const override {
        return (_body != nullptr ? _body->GetPosition().x : _bodyinfo.position.x);
    }
    
    /**
     * Sets the x-coordinate for this physics body
     *
     * @param value  the x-coordinate for this physics body
     */
    virtual void setX(float value) override {
        if (_body != nullptr) {
            _body->SetTransform(b2Vec2(value,_body->GetPosition().y),_body->GetAngle());
        } else {
            _bodyinfo.position.x = value;
        }
    }
    
    /**
     * Returns the x-coordinate for this physics body
     *
     * @return the x-coordinate for this physics body
     */
    virtual float getY() const override {
        return (_body != nullptr ? _body->GetPosition().y : _bodyinfo.position.y);
    }
    
    /**
     * Sets the y-coordinate for this physics body
     *
     * @param value  the y-coordinate for this physics body
     */
    virtual void setY(float value) override {
        if (_body != nullptr) {
            _body->SetTransform(b2Vec2(_body->GetPosition().y,value),_body->GetAngle());
        } else {
            _bodyinfo.position.y = value;
        }
    }
    
    /**
     * Returns the angle of rotation for this body (about the center).
     *
     * The value returned is in radians
     *
     * @return the angle of rotation for this body
     */
    virtual float getAngle() const override {
        return (_body != nullptr ? _body->GetAngle() : _bodyinfo.angle);
    }
    
    /**
     * Sets the angle of rotation for this body (about the center).
     *
     * @param value  the angle of rotation for this body (in radians)
     */
    virtual void setAngle(float value) override {
        if (_body != nullptr) {
            _body->SetTransform(_body->GetPosition(),value);
        } else {
            _bodyinfo.angle = value;
        }
    }
    
    /**
     * Returns the linear velocity for this physics body
     *
     * This method converts from a Box2D vector type to a Cocos2D vector type. This
     * cuts down on the confusion between vector types.  It also means that changes
     * to the returned vector will have no effect on this object.
     *
     * @return the linear velocity for this physics body
     */
    virtual Vec2 getLinearVelocity() const override {
        if (_body != nullptr) {
            return Vec2(_body->GetLinearVelocity().x,_body->GetLinearVelocity().y);
        } else {
            return Vec2(_bodyinfo.linearVelocity.x,_bodyinfo.linearVelocity.y);
        }
    }
    
    /**
     * Sets the linear velocity for this physics body
     *
     * This method converts from a Cocos2D vector type to a Box2D vector type. This
     * cuts down on the confusion between vector types.
     *
     * @param value  the linear velocity for this physics body
     */
    virtual void setLinearVelocity(const Vec2& value) override { setLinearVelocity(value.x,value.y); }
    
    /**
     * Sets the linear velocity for this physics body
     *
     * @param x  the x-coordinate of the linear velocity
     * @param y  the y-coordinate of the linear velocity
     */
    virtual void setLinearVelocity(float x, float y) override {
        if (_body != nullptr) {
            _body->SetLinearVelocity(b2Vec2(x,y));
        } else {
            _bodyinfo.linearVelocity.Set(x,y);
        }
    }
    
    /**
     * Returns the x-velocity for this physics body
     *
     * @return the x-velocity for this physics body
     */
    virtual float getVX() const override {
        if (_body != nullptr) {
            return _body->GetLinearVelocity().x;
        } else {
            return _bodyinfo.linearVelocity.x;
        }
    }
    
    /**
     * Sets the x-velocity for this physics body
     *
     * @param value  the x-velocity for this physics body
     */
    virtual void setVX(float value) override {
        if (_body != nullptr) {
            _body->SetLinearVelocity(b2Vec2(value,_body->GetLinearVelocity().y));
        } else {
            _bodyinfo.linearVelocity.x = value;
        }
    }
    
    /**
     * Returns the y-velocity for this physics body
     *
     * @return the y-velocity for this physics body
     */
    virtual float getVY() const override {
        if (_body != nullptr) {
            return _body->GetLinearVelocity().y;
        } else {
            return _bodyinfo.linearVelocity.y;
        }
    }
    
    /**
     * Sets the y-velocity for this physics body
     *
     * @param value  the y-velocity for this physics body
     */
    virtual void setVY(float value) override {
        if (_body != nullptr) {
            _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x,value));
        } else {
            _bodyinfo.linearVelocity.y = value;
        }
    }
    
    /**
     * Returns the angular velocity for this physics body
     *
     * The rate of change is measured in radians per step
     *
     * @return the angular velocity for this physics body
     */
    virtual float getAngularVelocity() const override {
        return (_body != nullptr ? _body->GetAngularVelocity() : _bodyinfo.angularVelocity);
    }
    
    /**
     * Sets the angular velocity for this physics body
     *
     * @param value the angular velocity for this physics body (in radians)
     */
    virtual void setAngularVelocity(float value) override {
        if (_body != nullptr) {
            _body->SetAngularVelocity(value);
        } else {
            _bodyinfo.angularVelocity = value;
        }
    }
    
    /**
     * Returns true if the body is active
     *
     * An inactive body not participate in collision or dynamics. This state is similar
     * to sleeping except the body will not be woken by other bodies and the body's
     * fixtures will not be placed in the broad-phase. This means the body will not
     * participate in collisions, ray casts, etc.
     *
     * @return true if the body is active
     */
    virtual bool isActive() const override {
        return (_body != nullptr ? _body->IsActive() : _bodyinfo.active);
    }
    
    /**
     * Sets whether the body is active
     *
     * An inactive body not participate in collision or dynamics. This state is similar
     * to sleeping except the body will not be woken by other bodies and the body's
     * fixtures will not be placed in the broad-phase. This means the body will not
     * participate in collisions, ray casts, etc.
     *
     * @param value  whether the body is active
     */
    virtual void setActive(bool value) override {
        if (_body != nullptr) {
            _body->SetActive(value);
        } else {
            _bodyinfo.active = value;
        }
    }
    
    /**
     * Returns true if the body is awake
     *
     * An sleeping body is one that has come to rest and the physics engine has decided
     * to stop simulating it to save CPU cycles. If a body is awake and collides with a
     * sleeping body, then the sleeping body wakes up. Bodies will also wake up if a
     * joint or contact attached to them is destroyed.  You can also wake a body manually.
     *
     * @return true if the body is awake
     */
    virtual bool isAwake() const override {
        return (_body != nullptr ? _body->IsAwake() : _bodyinfo.awake);
    }
    
    /**
     * Sets whether the body is awake
     *
     * An sleeping body is one that has come to rest and the physics engine has decided
     * to stop simulating it to save CPU cycles. If a body is awake and collides with a
     * sleeping body, then the sleeping body wakes up. Bodies will also wake up if a
     * joint or contact attached to them is destroyed.  You can also wake a body manually.
     *
     * @param value  whether the body is awake
     */
    virtual void setAwake(bool value) override {
        if (_body != nullptr) {
            _body->SetAwake(value);
        } else {
            _bodyinfo.awake = value;
        }
    }
    
    /**
     * Returns false if this body should never fall asleep
     *
     * An sleeping body is one that has come to rest and the physics engine has decided
     * to stop simulating it to save CPU cycles. If a body is awake and collides with a
     * sleeping body, then the sleeping body wakes up. Bodies will also wake up if a
     * joint or contact attached to them is destroyed.  You can also wake a body manually.
     *
     * @return false if this body should never fall asleep
     */
    virtual bool isSleepingAllowed() const override {
        return (_body != nullptr ? _body->IsSleepingAllowed() : _bodyinfo.allowSleep);
    }
    
    /**
     * Sets whether the body should ever fall asleep
     *
     * An sleeping body is one that has come to rest and the physics engine has decided
     * to stop simulating it to save CPU cycles. If a body is awake and collides with a
     * sleeping body, then the sleeping body wakes up. Bodies will also wake up if a
     * joint or contact attached to them is destroyed.  You can also wake a body manually.
     *
     * @param value  whether the body should ever fall asleep
     */
    virtual void setSleepingAllowed(bool value) override {
        if (_body != nullptr) {
            _body->SetSleepingAllowed(value);
        } else {
            _bodyinfo.allowSleep = value;
        }
    }
    
    /**
     * Returns true if this body is a bullet
     *
     * By default, Box2D uses continuous collision detection (CCD) to prevent dynamic
     * bodies from tunneling through static bodies. Normally CCD is not used between
     * dynamic bodies. This is done to keep performance reasonable. In some game
     * scenarios you need dynamic bodies to use CCD. For example, you may want to shoot
     * a high speed bullet at a stack of dynamic bricks. Without CCD, the bullet might
     * tunnel through the bricks.
     *
     * Fast moving objects in Box2D can be labeled as bullets. Bullets will perform CCD
     * with both static and dynamic bodies. You should decide what bodies should be
     * bullets based on your game design.
     *
     * @return true if this body is a bullet
     */
    virtual bool isBullet() const override {
        return (_body != nullptr ? _body->IsBullet() : _bodyinfo.bullet);
    }
    
    /**
     * Sets whether this body is a bullet
     *
     * By default, Box2D uses continuous collision detection (CCD) to prevent dynamic
     * bodies from tunneling through static bodies. Normally CCD is not used between
     * dynamic bodies. This is done to keep performance reasonable. In some game
     * scenarios you need dynamic bodies to use CCD. For example, you may want to shoot
     * a high speed bullet at a stack of dynamic bricks. Without CCD, the bullet might
     * tunnel through the bricks.
     *
     * Fast moving objects in Box2D can be labeled as bullets. Bullets will perform CCD
     * with both static and dynamic bodies. You should decide what bodies should be
     * bullets based on your game design.
     *
     * @param value  whether this body is a bullet
     */
    virtual void setBullet(bool value) override {
        if (_body != nullptr) {
            _body->SetBullet(value);
        } else {
            _bodyinfo.bullet = value;
        }
    }
    
    /**
     * Returns true if this body be prevented from rotating
     *
     * This is very useful for characters that should remain upright.
     *
     * @return true if this body be prevented from rotating
     */
    virtual bool isFixedRotation() const override {
        return (_body != nullptr ? _body->IsFixedRotation() : _bodyinfo.fixedRotation);
    }
    
    /**
     * Sets whether this body be prevented from rotating
     *
     * This is very useful for characters that should remain upright.
     *
     * @param value  whether this body be prevented from rotating
     */
    virtual void setFixedRotation(bool value) override {
        if (_body != nullptr) {
            _body->SetFixedRotation(value);
        } else {
            _bodyinfo.fixedRotation = value;
        }
    }
    
    /**
     * Returns the gravity scale to apply to this body
     *
     * This allows isolated objects to float.  Be careful with this, since increased
     * gravity can decrease stability.
     *
     * @return the gravity scale to apply to this body
     */
    virtual float getGravityScale() const override {
        return (_body != nullptr ? _body->GetGravityScale() : _bodyinfo.gravityScale);
    }
    
    /**
     * Sets the gravity scale to apply to this body
     *
     * This allows isolated objects to float.  Be careful with this, since increased
     * gravity can decrease stability.
     *
     * @param value  the gravity scale to apply to this body
     */
    virtual void setGravityScale(float value) override {
        if (_body != nullptr) {
            _body->SetGravityScale(value);
        } else {
            _bodyinfo.gravityScale = value;
        }
    }
    
    /**
     * Returns the linear damping for this body.
     *
     * Linear damping is use to reduce the linear velocity. Damping is different than
     * friction because friction only occurs with contact. Damping is not a replacement
     * for friction and the two effects should be used together.
     *
     * Damping parameters should be between 0 and infinity, with 0 meaning no damping,
     * and infinity meaning full damping. Normally you will use a damping value between
     * 0 and 0.1. Most people avoid linear damping because it makes bodies look floaty.
     *
     * @return the linear damping for this body.
     */
    virtual float getLinearDamping() const override {
        return (_body != nullptr ? _body->GetLinearDamping() : _bodyinfo.linearDamping);
    }
    
    /**
     * Sets the linear damping for this body.
     *
     * Linear damping is use to reduce the linear velocity. Damping is different than
     * friction because friction only occurs with contact. Damping is not a replacement
     * for friction and the two effects should be used together.
     *
     * Damping parameters should be between 0 and infinity, with 0 meaning no damping,
     * and infinity meaning full damping. Normally you will use a damping value between
     * 0 and 0.1. Most people avoid linear damping because it makes bodies look floaty.
     *
     * @param value  the linear damping for this body.
     */
    virtual void setLinearDamping(float value) override {
        if (_body != nullptr) {
            _body->SetLinearDamping(value);
        } else {
            _bodyinfo.linearDamping = value;
        }
    }
    
    /**
     * Returns the angular damping for this body.
     *
     * Angular damping is use to reduce the angular velocity. Damping is different than
     * friction because friction only occurs with contact. Damping is not a replacement
     * for friction and the two effects should be used together.
     *
     * Damping parameters should be between 0 and infinity, with 0 meaning no damping,
     * and infinity meaning full damping. Normally you will use a damping value between
     * 0 and 0.1.
     *
     * @return the angular damping for this body.
     */
    virtual float getAngularDamping() const override {
        return (_body != nullptr ? _body->GetAngularDamping() : _bodyinfo.angularDamping);
    }
    
    /**
     * Sets the angular damping for this body.
     *
     * Angular damping is use to reduce the angular velocity. Damping is different than
     * friction because friction only occurs with contact. Damping is not a replacement
     * for friction and the two effects should be used together.
     *
     * Damping parameters should be between 0 and infinity, with 0 meaning no damping,
     * and infinity meaning full damping. Normally you will use a damping value between
     * 0 and 0.1.
     *
     * @param value  the angular damping for this body.
     */
    virtual void setAngularDamping(float value) override {
        if (_body != nullptr) {
            _body->SetAngularDamping(value);
        } else {
            _bodyinfo.angularDamping = value;
        }
    }
    
    
#pragma mark -
#pragma mark FixtureDef Methods
    /**
     * Sets the density of this body
     *
     * The density is typically measured in usually in kg/m^2. The density can be zero or
     * positive. You should generally use similar densities for all your fixtures. This
     * will improve stacking stability.
     *
     * @param value  the density of this body
     */
    virtual void setDensity(float value) override;
    
    /**
     * Sets the friction coefficient of this body
     *
     * The friction parameter is usually set between 0 and 1, but can be any non-negative
     * value. A friction value of 0 turns off friction and a value of 1 makes the friction
     * strong. When the friction force is computed between two shapes, Box2D must combine
     * the friction parameters of the two parent fixtures. This is done with the geometric
     * mean.
     *
     * @param value  the friction coefficient of this body
     */
    virtual void setFriction(float value) override;
    
    /**
     * Sets the restitution of this body
     *
     * Restitution is used to make objects bounce. The restitution value is usually set
     * to be between 0 and 1. Consider dropping a ball on a table. A value of zero means
     * the ball won't bounce. This is called an inelastic collision. A value of one means
     * the ball's velocity will be exactly reflected. This is called a perfectly elastic
     * collision.
     *
     * @param value  the restitution of this body
     */
    virtual void setRestitution(float value) override;
    
    /**
     * Sets whether this object is a sensor.
     *
     * Sometimes game logic needs to know when two entities overlap yet there should be
     * no collision response. This is done by using sensors. A sensor is an entity that
     * detects collision but does not produce a response.
     *
     * @param value  whether this object is a sensor.
     */
    virtual void setSensor(bool value) override;
    
    /**
     * Sets the filter data for this object
     *
     * Collision filtering allows you to prevent collision between fixtures. For example,
     * say you make a character that rides a bicycle. You want the bicycle to collide
     * with the terrain and the character to collide with the terrain, but you don't want
     * the character to collide with the bicycle (because they must overlap). Box2D
     * supports such collision filtering using categories and groups.
     *
     * A value of null removes all collision filters.
     *
     * @param value  the filter data for this object
     */
    virtual void setFilterData(b2Filter value) override;
    
    
#pragma mark -
#pragma mark MassData Methods
    /**
     * Returns the center of mass of this body
     *
     * This method converts from a Box2D vector type to a Cocos2D vector type. This
     * cuts down on the confusion between vector types.  It also means that changes
     * to the returned vector will have no effect on this object.
     *
     * @return the center of mass for this physics body
     */
    virtual Vec2 getCentroid() const override {
        if (_body != nullptr) {
            return Vec2(_body->GetLocalCenter().x,_body->GetLocalCenter().y);
        } else {
            return Vec2(_massdata.center.x,_massdata.center.y);
        }
    }
    
    /**
     * Sets the center of mass for this physics body
     *
     * This method converts from a Cocos2D vector type to a Box2D vector type. This
     * cuts down on the confusion between vector types.
     *
     * @param value  the center of mass for this physics body
     */
    virtual void setCentroid(const Vec2& value) override { setCentroid(value.x,value.y); }
    
    /**
     * Sets the center of mass for this physics body
     *
     * @param x  the x-coordinate of the center of mass for this physics body
     * @param y  the y-coordinate of the center of mass for this physics body
     */
    virtual void setCentroid(float x, float y) override {
        Obstacle::setCentroid(x, y);
        if (_body != nullptr) {
            _body->SetMassData(&_massdata); // Protected accessor?
        }
    }
    
    /**
     * Returns the rotational inertia of this body
     *
     * For static bodies, the mass and rotational inertia are set to zero. When
     * a body has fixed rotation, its rotational inertia is zero.
     *
     * @return the rotational inertia of this body
     */
    virtual float getInertia() const override {
        return  (_body != nullptr ? _body->GetInertia() : _massdata.I);
    }
    
    /**
     * Sets the rotational inertia of this body
     *
     * For static bodies, the mass and rotational inertia are set to zero. When
     * a body has fixed rotation, its rotational inertia is zero.
     *
     * @param value  the rotational inertia of this body
     */
    virtual void setInertia(float value) override {
        Obstacle::setInertia(value);
        if (_body != nullptr) {
            _body->SetMassData(&_massdata); // Protected accessor?
        }
    }
    
    /**
     * Returns the mass of this body
     *
     * The value is usually in kilograms.
     *
     * @return the mass of this body
     */
    virtual float getMass() const override {
        return  (_body != nullptr ? _body->GetMass() : _massdata.mass);
    }
    
    /**
     * Sets the mass of this body
     *
     * The value is usually in kilograms.
     *
     * @param value  the mass of this body
     */
    virtual void setMass(float value) override {
        Obstacle::setMass(value);
        if (_body != nullptr) {
            _body->SetMassData(&_massdata); // Protected accessor?
        }
    }
    
    /**
     * Resets this body to use the mass computed from the its shape and density
     */
    virtual void resetMass() override {
        Obstacle::resetMass();
        if (_body != nullptr) {
            _body->ResetMassData();
        }
    }
    
    
#pragma mark -
#pragma mark Physics Methods
    /**
     * Returns the Box2D body for this object.
     *
     * You use this body to add joints and apply forces.
     *
     * @return the Box2D body for this object.
     */
    virtual b2Body* getBody() override { return _body; }
    
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
    virtual bool activatePhysics(b2World& world) override;
    
    /**
     * Destroys the physics Body(s) of this object if applicable,
     * removing them from the world.
     *
     * @param world Box2D world that stores body
     */
    virtual void deactivatePhysics(b2World& world) override;
    
    /**
     * Create new fixtures for this body, defining the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    virtual void createFixtures() {}
    
    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    virtual void releaseFixtures() {}
    
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
    virtual void update(float delta) override;

    
#pragma mark -
#pragma mark Render Snap
    /**
     * Returns the number of decimal places to snap position of image to physics body
     *
     * Physics bodies will have very precise positions, but these fractional positions may
     * not be ideal for drawing, and may produce artifacts.  When the value of snap is
     * nonnegative, Cocos2d will round the position of the image to snap decimal places.
     *
     * For example, if the snap is 0, it will always round position to the nearest integer.
     * If it is 1, it will round it to the nearest 10th of a point.  If it is -1 (or any
     * negative value) it will not snap at all.
     *
     * @return the number of decimal places to snap position of image to physics body
     */
    int getPositionSnap()   { return _posSnap; }
    
    /**
     * Sets the number of decimal places to snap position of image to physics body
     *
     * Physics bodies will have very precise positions, but these fractional positions may
     * not be ideal for drawing, and may produce artifacts.  When the value of snap is 
     * nonnegative, Cocos2d will round the position of the image to snap decimal places.
     *
     * For example, if the snap is 0, it will always round position to the nearest integer.
     * If it is 1, it will round it to the nearest 10th of a point.  If it is -1 (or any
     * negative value) it will not snap at all.
     *
     * @param  snap the number of decimal places to snap position of image to physics body
     */
    void setPositionSnap(unsigned int snap) {
        _posSnap = snap;
        _posFact = (unsigned long)(_posSnap >= 0 ? std::pow(10, snap) : 0);
    }

    /**
     * Returns the number of decimal places to snap rotation of image to physics body
     *
     * Physics bodies will have very precise angles, but these fractional angles may
     * not be ideal for drawing, and may produce artifacts.  When the value of snap is
     * nonnegative, Cocos2d will round the rotation (measured in degrees, as that is 
     * the value used by images) of the image to snap decimal places.
     *
     * For example, if the snap is 0, it will always round the angle to the nearest degree.
     * If it is 1, it will round it to the nearest 10th of a degree.  If it is -1 (or any
     * negative value) it will not snap at all.
     *
     * @return the number of decimal places to snap rotation of image to physics body
     */
    int getAngleSnap()      { return _angSnap; }
    
    /**
     * Sets the number of decimal places to snap position of image to physics body
     *
     * Physics bodies will have very precise angles, but these fractional angles may
     * not be ideal for drawing, and may produce artifacts.  When the value of snap is
     * nonnegative, Cocos2d will round the rotation (measured in degrees, as that is
     * the value used by images) of the image to snap decimal places.
     *
     * For example, if the snap is 0, it will always round the angle to the nearest degree.
     * If it is 1, it will round it to the nearest 10th of a degree.  If it is -1 (or any
     * negative value) it will not snap at all.
     *
     * @param  snap the number of decimal places to snap position of image to physics body
     */
    void setAngleSnap(unsigned int snap)    {
        _angSnap = snap;
        _angFact = (unsigned long)(_angSnap >= 0 ? std::pow(10, snap) : 0);
    }
    
    /**
     * Repositions the scene node so that it agrees with the physics object.
     *
     * By default, the position of a node should be the body position times
     * the draw scale.  However, for some obstacles (particularly complex
     * obstacles), it may be desirable to turn the default functionality
     * off.  Hence we have made this virtual.
     */
    virtual void positionSceneNode() override;
    
    /**
     * Repositions the debug node so that it agrees with the physics object.
     *
     * By default, the position of a node should be the body position times
     * the draw scale.  However, for some obstacles (particularly complex
     * obstacles), it may be desirable to turn the default functionality
     * off.  Hence we have made this virtual.
     */
    virtual void positionDebugNode() override;
    
    
#pragma mark -
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /**
     * Creates a new simple physics object at the origin.
     */
    SimpleObstacle() : Obstacle(), _body(nullptr) {
        _posSnap = _angSnap = -1;
    }

    /**
     * Deletes this physics object and all of its resources.
     *
     * We have to make the destructor public so that we can polymorphically
     * delete physics objects.
     *
     * The purpose of this destructor is to warn us if we delete an object
     * pre-maturely.
     */
    virtual ~SimpleObstacle() {
        CCASSERT(_body == nullptr, "You must deactive physics before deleting an object");
    }

};

NS_CC_END
#endif /* defined(__CC_SIMPLE_OBSTACLE_H__) */