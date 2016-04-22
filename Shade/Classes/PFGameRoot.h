//
//  PFGameRoot.h
//  PlatformerDemo
//
//  This is the root class for a single game scene.  It shows how to subclass RootLayer to
//  make a new game.  This is a standard design pattern that we have used in all our samples.
//  You may find that you want more than one scene in your game, but that is not always
//  necessary.  You may find it easier to work with a single scene.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/15/15
//
#ifndef __PF_GAME_ROOT_H__
#define __PF_GAME_ROOT_H__

#include <cocos2d.h>
#include <cornell.h>
#include "C_Gameplay.h"


using namespace cocos2d;

/**
* Class represents the root node for the game scene.
*
* As we have mentioned in class, Cocos2D has decided to violate MVC separation
* in horrible, horrible ways.  To get a game on a 2D device, you NEED to have
* a layer.  The layer is what (1) displays the graphics, (2) receives the touch
* inputs, and (3) has an update method that is called regularly.
*
* Instead of fighting it, we simply make the Layer class the root scene of our
* game engine.  However, from this point onward, we obey MVC separation as much
* as possible.
*/
class PlatformRoot : public RootLayer {
protected:
	// CONTROLLERS
	/** The primary controller for the game world */
	//MainMenuController _gameplay;
	GameController _gameplay;

	/** Whether or not we have finished preloading all assets */
	bool _preloaded;

	/**
	* Builds the scene graph for the loading screen.
	*
	* The loading screen is what we run while we are waiting for the asynchronous
	* loader for finish with the textures.  Right now, the loading screen is just
	* the word "Loading...".  We do this because the loading screen can only use
	* assets that have been loaded already, and the font is the only thing that
	* is guaranteed to be loaded at start.
	*/
	void displayLoader();


public:
	/**
	* Starts the layer, allocating initial resources
	*
	* This method is used to bootstrap the game.  It should start up an asset manager
	* and load initial assets.
	*/
	void start() override;

	/**
	* Updates the game for a single animation frame
	*
	* This method is called every animation frame.  There is no draw() or render()
	* counterpoint to this method; drawing is done automatically in the scene graph.
	* However, this method is responsible for updating any transforms in the scene graph.
	*
	* @param  dt   the time in seconds since last update
	*/
	void update(float dt) override;

	/**
	* Stops the layer, releasing all resources
	*
	* This method is used to clean-up any allocation that occurred in either start or
	* update.  While Cocos2d does have rudimentary garbage collection, you still have
	* to release any objects that you have retained.
	*/
	void stop() override;
};

#endif /* defined(__PF_GAME_ROOT_H__) */