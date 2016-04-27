#ifndef __M_PEDESTRIAN_H__
#define __M_PEDESTRIAN_H__

#include <map>
#include <string>

#define PEDESTRIAN_SCALE_DOWN 8.0f

using namespace std;

struct Pedestrian {
	typedef enum ActionType { WALK_FAST, WALK_SLOW, STAND, LOOK_AROUND } ActionType;

	static const map<string, ActionType> actionMap;
};

#endif /* __M_PEDESTRIAN_H__ */
