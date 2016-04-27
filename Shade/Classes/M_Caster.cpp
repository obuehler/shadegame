#include <math.h>
#include <cocos2d.h>
#include "M_Caster.h"

using namespace cocos2d;

void Caster::act(Caster::ActionType action, BoxObstacle * object, BoxObstacle * shadow) {
	b2Vec2 moveVector;
	b2Body* obody;
	Vec2 pos = object->getPosition();

	float angle = object->getAngle();
	float speed = 2.0f; // Speed should be pulled out and made a parameter

	//CCLOG("%f", angle);
	switch (action) {
		case GO: // Go forward in the current direction
			//CCLOG("%s", "GO");
			moveVector = b2Vec2(speed*cos(angle), speed*sin(angle));
			obody = object->getBody();
			obody->SetLinearVelocity(moveVector);
			break;
		case STOP: // Stop moving
			//CCLOG("%s", "STOP");
			moveVector = b2Vec2(0.0f, 0.0f);
			obody = object->getBody();
			obody->SetLinearVelocity(moveVector);
			break;
		case TURN_LEFT:
			//obody->setTransform(pos, angle + (M_PI_2));
			object->setAngle(angle - (M_PI_2));
			break;
		case TURN_RIGHT:
			object->setAngle(angle + (M_PI_2));
			break;
		default:
			CCAssert(false, "Invalid ActionType for act");
			break;
	}
}

map<string, Caster::ActionType> Caster::actionMap = {
	{ "stop", ActionType::STOP },
	{ "go", ActionType::GO },
	{ "left", ActionType::TURN_LEFT },
	{ "right", ActionType::TURN_RIGHT },
};

