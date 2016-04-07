#include "M_Car.h"
#include <cocos2d.h>

const map<string, Car::ActionType> Car::actionMap = {
	{ "go", ActionType::GO },
	{ "stop", ActionType::STOP },
	{ "turn left", ActionType::TURN_LEFT },
	{ "turn right", ActionType::TURN_RIGHT }
};