#include "M_Pedestrian.h"

const map<string, Pedestrian::ActionType> Pedestrian::actionMap = {
	{"walk fast", ActionType::WALK_FAST},
	{"walk slow", ActionType::WALK_SLOW},
	{"look around", ActionType::LOOK_AROUND},
	{"stand", ActionType::STAND}
};