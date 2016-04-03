#ifndef __C_PHYSICS_H__
#define __C_PHYSICS_H__

#include "cocos2d.h"
#include <cornell.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>

#define CHARACTER_FILTER 0x01
#define OBJECT_FILTER 0x02
#define SHADOW_FILTER 0x04
#define CASTER_FILTER 0x08
#define CHARACTER_SENSOR_FILTER 0x10

namespace cocos2d {
	class WorldController;
}

using namespace cocos2d;

class PhysicsController {

	friend class GameController;

	/** Whether we have reached the caster */
	bool _reachedCaster;

	/** The Box2D world */
	WorldController* _world;
	


public:
#pragma mark -
#pragma mark Initialization
	/**
	* Initializes the controller contents, and starts the game
	*
	* The constructor does not allocate any objects or memory.  This allows
	* us to have a non-pointer reference to this controller, reducing our
	* memory allocation.  Instead, allocation happens in this method.
	*
	* The game world is scaled so that the screen coordinates do not agree
	* with the Box2d coordinates.  This initializer uses the default scale.
	*
	* @retain a reference to the root layer
	* @return  true if the controller is initialized properly, false otherwise.
	*/
	bool init(const Rect& rect);

#pragma mark -
#pragma mark Allocation
	/**
	* Creates a new game world with the default values.
	*
	* This constructor does not allocate any objects or start the controller.
	* This allows us to use a controller without a heap pointer.
	*/
	PhysicsController();

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*
	* This method is different from dispose() in that it ALSO shuts off any
	* static resources, like the input controller.
	*/
	~PhysicsController();

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*/
	void dispose();


#pragma mark -
#pragma mark Collision Handling
	/**
	* Processes the start of a collision
	*
	* This method is called when we first get a collision between two objects.  We use
	* this method to test if it is the "right" kind of collision.  In particular, we
	* use it to test if we make it to the win door.  We also us it to eliminate bullets.
	*
	* @param  contact  The two bodies that collided
	*/
	void beginContact(b2Contact* contact);

	/**
	* Processes the end of a collision
	*
	* This method is called when we no longer have a collision between two objects.
	* We use this method allow the character to jump again.
	*
	* @param  contact  The two bodies that collided
	*/
	void endContact(b2Contact* contact);


#pragma mark -
#pragma mark Gameplay Handling
	/**
	* Resets the status of the game so that we can play again.
	*
	* This method disposes of the world and creates a new one.
	*/
	void reset();

	/**
	* Executes the core gameplay loop of this world.
	*
	* This method contains the specific update code for this mini-game. It does
	* not handle collisions, as those are managed by the parent class WorldController.
	* This method is called after input is read, but before collisions are resolved.
	* The very last thing that it should do is apply forces to the appropriate objects.
	*
	* @param  delta    Number of seconds since last animation frame
	*/
	void update(float dt);

	/**
	* Clear all memory when exiting.
	*/
	void stop();

};

#endif  /* __C_PHYSICS_H__ */
