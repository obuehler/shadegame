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
	CCLOG("%s", "UPDATECALLED");
	// Update pedestrians
	for (int i = 0; i < _pedMovers.size(); i++) {
		OurMovingObject<Pedestrian>* ped = _pedMovers[i];
		updatePed(ped);
	}

	updateCaster(_caster);
}

void AIController::updatePed(OurMovingObject<Pedestrian>*) {

}

void AIController::updateCaster(OurMovingObject<Caster>*) {

}


void AIController::reset() {

}


void AIController::stop() {}