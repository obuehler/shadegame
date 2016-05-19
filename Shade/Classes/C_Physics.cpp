#include "C_Physics.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Collision/b2Collision.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include "M_LevelInstance.h"
#include <ShadowCount.h>

bool PhysicsController::init(const Size& size) {
	_reachedCaster = false;
	
	// Create the world
	_world = WorldController::create(Rect(Vec2(0,0), size), Vec2(0.0f, 0.0f));
	if (_world != nullptr) {
		_world->retain();
		_world->activateCollisionCallbacks(true);
		_world->onBeginContact = [this](b2Contact* contact) {
			beginContact(contact);
		};
		_world->onEndContact = [this](b2Contact* contact) {
			endContact(contact);
		};
		return true;
	}
	return false;
}


PhysicsController::PhysicsController() :
	_world(nullptr)
{
}


PhysicsController::~PhysicsController() {
	dispose();
}


void PhysicsController::dispose() {
	if (_world != nullptr) {
		_world->clear();
		_world->release();
		_world = nullptr;
	}
}


void PhysicsController::update(float dt) {
	// Turn the physics engine crank.
	_world->update(dt);

	// Since items may be deleted, garbage collect
	_world->garbageCollect();
}


#pragma mark -
#pragma mark Collision Handling
/**
* Processes the start of a collision
*
* This method is called when we first get a collision between two objects.  We use
* this method to test if it is the "right" kind of collision.  In particular, we
* use it to test if we make it to the win door.
*
* @param  contact  The two bodies that collided
*/
void PhysicsController::beginContact(b2Contact* contact) {
	b2Fixture* fix1 = contact->GetFixtureA();
	b2Fixture* fix2 = contact->GetFixtureB();

	// If one of the fixtures is from a shadow, the other fixture is
	// definitely our character. Mark as in shadow if so.
	if (fix1->GetFilterData().categoryBits == SHADOW_BIT)
		((ShadowCount*)(fix2->GetUserData()))->inc();
	if (fix2->GetFilterData().categoryBits == SHADOW_BIT)
		((ShadowCount*)(fix1->GetUserData()))->inc();

	// If we hit the caster, we are done
	if ((fix1->GetFilterData().categoryBits == CASTER_BIT && fix2->GetFilterData().categoryBits == CHARACTER_SENSOR_BIT) ||
		(fix2->GetFilterData().categoryBits == CASTER_BIT && fix1->GetFilterData().categoryBits == CHARACTER_SENSOR_BIT)) {
		_reachedCaster = true;
	}
}

/**
* Callback method for the start of a collision
*
* This method is called when two objects cease to touch.  The main use of this method
* is to determine when the characer is NOT on the ground.  This is how we prevent
* double jumping.
*/
void PhysicsController::endContact(b2Contact* contact) {
	b2Fixture* fix1 = contact->GetFixtureA();
	b2Fixture* fix2 = contact->GetFixtureB();
	ShadowCount* sc = nullptr;
	if (fix1->GetFilterData().categoryBits == SHADOW_BIT) {
		sc = (ShadowCount*)(fix2->GetUserData()); 
	}
	if (fix2->GetFilterData().categoryBits == SHADOW_BIT) {
		sc = (ShadowCount*)(fix1->GetUserData());
	}
	if (sc != nullptr) sc->dec();
	
}

void PhysicsController::reset() {
	_reachedCaster = false;
	_world->clear();
}
