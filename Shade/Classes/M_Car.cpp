#include <cocos2d.h>
#include "M_Car.h"
#include <cornell.h>

using namespace cocos2d;

Vec2 Car::act(Car::ActionType action) {
	cout << "car act called";
	return Vec2(0.1f, 0.0f);
}

map<string, Car::ActionType> Car::actionMap = {
	{ "stop", ActionType::STOP },
	{ "go", ActionType::GO },
	{ "left", ActionType::TURN_LEFT },
	{ "right", ActionType::TURN_RIGHT },
};

