#ifndef __M_LEVEL_H__
#define __M_LEVEL_H__

#include <tuple>
#include <MovingObjectMetadata.h>

using namespace std;

struct Level {
	int levelIndex;
	tuple<float, float, float> _shadowPos;
	tuple<float, float, float> _casterPos;
	MovingObjectMetadata* _movingObjects;
	tuple<float, float, float> _coins;
	tuple
	// TODO add powerups
};



#endif /* __M_LEVEL_H__ */