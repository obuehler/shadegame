#include "C_AI.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Collision/b2Collision.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>

bool AIController::init(vector<OurMovingObject<Pedestrian>*> peds, OurMovingObject<Caster>* caster, Shadow* avatar) {
	// Create the world
	_caster = caster;
	_pedMovers = peds;
	_avatar = avatar;
	_active = true;
	return true;
}

AIController::AIController() :
	_caster(nullptr),
	_avatar(nullptr)
{
}


AIController::~AIController() {
	dispose();
}


void AIController::dispose() {
	if (_caster != nullptr) {
		_caster = nullptr;
	}
}


void AIController::update() {
	// Update pedestrians
	if (_active) {
		for (int i = 0; i < _pedMovers.size(); i++) {
			OurMovingObject<Pedestrian>* ped = _pedMovers[i];
			updatePed(ped);
		}
		updateCaster();
	}
}

void AIController::updatePed(OurMovingObject<Pedestrian>* ped) {
	Vec2 movVec = Vec2(_avatar->getHorizontalMovement(), _avatar->getVerticalMovement());
	Vec2 avaPos = _avatar->getPosition();
	Vec2 diff = avaPos - (ped->getPosition());
	int speed = 2;
	CCLOG("%f,%f", diff.x, diff.y);
	if (diff.getLength() < 10) {
		diff.normalize();
		ped->setHorizontalMovement(diff.x*speed);
		ped->setVerticalMovement(diff.y*speed);
		ped->applyForce();
	}
	else {
		ped->_actionQueue->push(Pedestrian::ActionType::STOP, 1);
	}
}

void AIController::updateCaster() {

}


void AIController::reset() {
	_active = false;
	_avatar = nullptr;
	_caster = nullptr;
	_pedMovers.clear();
}


void AIController::stop() {}