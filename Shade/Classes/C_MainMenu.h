#ifndef __C_MAINMENU_H__
#define __C_MAINMENU_H__

#include "cocos2d.h"
#include <vector>
#include <tuple>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <Box2D/Dynamics/Joints/b2MouseJoint.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>

// We need a lot of forward references to the classes used by this controller
// These forward declarations are in cocos2d namespace
namespace cocos2d {
	class RootLayer;
	class ComplexObstacle;
	class ObstacleSelector;
	class SceneManager;
}

using namespace cocos2d;
using namespace std;

#pragma mark -
#pragma mark MainMenuController

class MainMenuController {
private:

protected:
	/** Reference to the root node of the scene graph */
	RootLayer* _rootnode;
	Node* _worldnode;

	/** Whether or not this menu is still active */
	bool _active;

public:
	bool init(RootLayer* root);
	bool init(RootLayer* root, const Rect& rect);
#pragma mark -
#pragma mark Allocation
	/**
	* Creates a new game world with the default values.
	*
	* This constructor does not allocate any objects or start the controller.
	* This allows us to use a controller without a heap pointer.
	*/
	MainMenuController();

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*
	* This method is different from dispose() in that it ALSO shuts off any
	* static resources, like the input controller.
	*/
	~MainMenuController();

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*/
	void dispose();

	/**
	* Preloads all of the assets necessary for this game world
	*/
	void preload();

#pragma mark -
#pragma mark State Access
	/**
	* Returns true if the gameplay controller is currently active
	*
	* @return true if the gameplay controller is currently active
	*/
	bool isActive() const { return _active; }

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

#endif /* defined(__C_MAINMENU_H__) */
