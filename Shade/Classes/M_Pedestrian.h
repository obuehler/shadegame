#ifndef __M_PEDESTRIAN_H__
#define __M_PEDESTRIAN_H__

#include <map>
#include <string>
#include <cocos2d.h>
#include <cornell.h>

#define PEDESTRIAN_SCALE_DOWN 8.0f

#define PEDESTRIAN_DENSITY 1.0f
#define PEDESTRIAN_RESTITUTION 0.0f // TODO increase this when latching is implemented
#define PEDESTRIAN_FRICTION 0.0f

using namespace cocos2d;

struct Pedestrian {
	typedef enum ActionType { WALK_FAST, WALK_SLOW, STAND, LOOK_AROUND } ActionType;

	static const std::string name;

	static const map<std::string, ActionType> actionMap;

	static void act(ActionType action, int actionLength, int actionCounter, BoxObstacle* object, BoxObstacle* shadow); // TODO define this
};

#endif /* __M_PEDESTRIAN_H__ */
