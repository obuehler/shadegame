#ifndef __M_CAR_H__
#define __M_CAR_H__

#include <map>
#include <string>

using namespace std;

struct Car {
	typedef enum ActionType { GO, STOP, TURN_LEFT, TURN_RIGHT } ActionType;
	static const map<string, ActionType> actionMap;
	ActionType actionType;
};

#endif /* __M_CAR_H__ */