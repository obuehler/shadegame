#include <math.h>
#include <cocos2d.h>
#include "M_Car.h"
#include <cocos2d.h>

#define CAR_SPEED 2.0f

using namespace cocos2d;

void Car::act(Car::ActionType action, int actionLength, int actionCounter, BoxObstacle * object, BoxObstacle * shadow) {
	b2Vec2 moveVector;
	b2Body* obody;
	b2Body* sbody;
	Vec2 pos = object->getPosition();

	float angle = object->getAngle();

	//CCLOG("%f", angle);
	switch (action) {
		case GO: // Go forward in the current direction
			//CCLOG("%s", "GO");
			moveVector = b2Vec2(CAR_SPEED*cos(angle), CAR_SPEED*sin(angle));
			obody = object->getBody();
			obody->SetLinearVelocity(moveVector);
			sbody = shadow->getBody();
			sbody->SetLinearVelocity(moveVector);
			break;
		case STOP: // Stop moving
			//CCLOG("%s", "STOP");
			moveVector = b2Vec2(0.0f, 0.0f);
			obody = object->getBody();
			obody->SetLinearVelocity(moveVector);
			sbody = shadow->getBody();
			sbody->SetLinearVelocity(moveVector);
			break;
		case TURN_LEFT:
			//obody->setTransform(pos, angle + (M_PI_2));
			object->setAngle(angle - (M_PI_2));
			shadow->setAngle(angle - (M_PI_2));
			break;
		case TURN_RIGHT:
			object->setAngle(angle + (M_PI_2));
			shadow->setAngle(angle + (M_PI_2));
			break;
		default:
			CCLOG("%s", "BAD");
			break;
	}
}

const map<string, Car::ActionType> Car::actionMap = {
	{ "stop", ActionType::STOP },
	{ "go", ActionType::GO },
	{ "left", ActionType::TURN_LEFT },
	{ "right", ActionType::TURN_RIGHT },
};
