#ifndef __C_AI_H__
#define __C_AI_H__

#include <M_Shadow.h>
#include <M_Caster.h>
#include <M_Pedestrian.h>
#include <M_MovingObject.h>
#include "M_LevelInstance.h"


using namespace cocos2d;

class AIController {
	friend class GameController;

	bool _active;

	bool _chasing;

	vector<LevelInstance::MovingObjectMetadata<Pedestrian>> _pedMovers;
	OurMovingObject<Caster>* _caster;
	Shadow* _avatar;
	
#pragma mark -
#pragma mark Allocation
	/**
	* Creates a new game world with the default values.
	*
	* This constructor does not allocate any objects or start the controller.
	* This allows us to use a controller without a heap pointer.
	*/
	AIController();

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*
	* This method is different from dispose() in that it ALSO shuts off any
	* static resources, like the input controller.
	*/
	~AIController();

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*/
	void dispose();

	bool init(LevelInstance*);

	void stop();
	void reset();
	void update();

	void updatePed(OurMovingObject<Pedestrian>*);

	void updateCaster();
	


};

#endif  /* __C_AI_H__ */