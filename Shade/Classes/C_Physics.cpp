#include "C_Physics.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>

bool PhysicsController::init(const Rect& rect) {
	// Create the world
	_world = WorldController::create(rect, 0);
	_world->retain();
	_world->activateCollisionCallbacks(true);
	_world->onBeginContact = [this](b2Contact* contact) {
		beginContact(contact);
	};
	_world->onEndContact = [this](b2Contact* contact) {
		endContact(contact);
	};
}


PhysicsController::PhysicsController() {
	_world(nullptr);
}


PhysicsController::~PhysicsController() {
	dispose();
}


PhysicsController::dispose() {
	if (_world != nullptr) {
		_world->clear();
		_world->release();
	}
}