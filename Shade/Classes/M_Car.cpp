#include <cocos2d.h>
#include "M_Car.h"

using namespace cocos2d;

void Car::act(Car::ActionType action, BoxObstacle * object, BoxObstacle * shadow) {
	b2Vec2 moveVector;
	b2Body* obody;
	b2Body* sbody;
	switch (action) {
		case GO: // Go forward in the current direction
			CCLOG("%s", "GO");
			moveVector = b2Vec2(1.0f, 0.0f);
			obody = object->getBody();
			obody->SetLinearVelocity(moveVector);
			sbody = shadow->getBody();
			sbody->SetLinearVelocity(moveVector);
			break;
		case STOP: // Stop moving
			CCLOG("%s", "STOP");
			moveVector = b2Vec2(0.0f, 0.0f);
			obody = object->getBody();
			obody->SetLinearVelocity(moveVector);
			sbody = shadow->getBody();
			sbody->SetLinearVelocity(moveVector);
			break;
		default:
			CCLOG("%s", "BAD");
			break;
	}
}

map<string, Car::ActionType> Car::actionMap = {
	{ "stop", ActionType::STOP },
	{ "go", ActionType::GO },
	{ "left", ActionType::TURN_LEFT },
	{ "right", ActionType::TURN_RIGHT },
};

