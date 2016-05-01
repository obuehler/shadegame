#ifndef __C_MAINMENU_H__
#define __C_MAINMENU_H__

#include "cocos2d.h"
#include <vector>
#include <tuple>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <Box2D/Dynamics/Joints/b2MouseJoint.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include "C_Gameplay.h"
#include <cocos2d/cocos/ui/UIButton.h>

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

class MainMenuController;

class MainMenuButton : public ui::Button {

private:
	GameController* _controller;
	MainMenuButton() : ui::Button(), _controller(nullptr) {}
	bool init(GameController* gc);

public:
	static MainMenuButton* create(GameController* gc);
	void dispose();
	GameController* const getController() { return _controller; }
};

#pragma mark -
#pragma mark MainMenuController

class MainMenuController {
private:
	/** Helper to load gameplay controllers in preload */
	void loadGameController(const char * levelkey, const char * levelpath);

protected:
	/** The scene manager for this game demo */
	SceneManager* _assets;
	/** Reference to the root node of the scene graph */
	RootLayer* _rootnode;
	Node* _worldnode;
	// Background node
	PolygonNode* _backgroundnode;
	
	// Vector of gamecontroller trigger buttons
	vector<MainMenuButton*> mainMenuButtons;
	// Active Gamecomtroller
	GameController * _activeController;

	/** Whether or not this menu is still active */
	bool _active;

public:
	bool init(RootLayer* root);

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

	void update(float dt);

#pragma mark -
#pragma mark Gameplay Handling


	/**
	* Resets the status of the game so that we can play again.
	*
	* This method disposes of the world and creates a new one.
	*/
	void reset();

	/**
	* Clear all memory when exiting.
	*/
	void stop();

};

#endif /* defined(__C_MAINMENU_H__) */
