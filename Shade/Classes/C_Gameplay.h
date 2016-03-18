//
//  PFGameController.h
//  PlatformerDemo
//
//  This is the most important class in this demo.  This class manages the gameplay
//  for this demo.  It also handles collision detection. There is not much to do for
//  collisions; our WorldController class takes care of all of that for us.  This
//  controller mainly transforms input into gameplay.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/15/15
//
#ifndef __C_GAMEPLAY_H__
#define __C_GAMEPLAY_H__

#include "cocos2d.h"
#include <vector>
#include <tuple>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <Box2D/Dynamics/Joints/b2MouseJoint.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include "C_Input.h"
#include "M_Powerup.h"
#include <unordered_set>

// We need a lot of forward references to the classes used by this controller
// These forward declarations are in cocos2d namespace
namespace cocos2d {
class RootLayer;
class WorldController;
class ComplexObstacle;
class ObstacleSelector;
class SceneManager;
}

// These forward declarations are in the project
class InputController;
class Shadow;
class RopeBridge;
class Spinner;


using namespace cocos2d;
using namespace std;

#pragma mark -
#pragma mark GameController
/**
 * Instance is a controls the gameplay for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  The only thing that it does
 * not do is create the scene graph root; that is shared with it by the 
 * game root (which has scaled the scene graph to fix the device with the
 * desired aspect ratio).
 */
class GameController {
private:
	/**
	* Add a horizontal building and shadow to the world.
	* pos is the position of the upper left corner of the building and shadow.
	* The size of the building and shadow will be the size of their source
	* images scaled by scale.
	*/
	void addBuilding(const char* bname,
		const char* sname,
		const Vec2& pos,
		float scale);

protected:

    /** The scene manager for this game demo */
    SceneManager* _assets;
    
    /** Controller for abstracting out input away from layer */
    InputController _input;
    
    /** Reference to the root node of the scene graph */
    RootLayer* _rootnode;
    /** Reference to the physics root of the scene graph */
    Node* _worldnode;
    /** Reference to the debug root of the scene graph */
    Node* _debugnode;
    /** Reference to the win message label */
    Label* _winnode;
    /** Reference to the lose message label */
    Label* _losenode;
	/** Reference to the timer message label */
	Label* _timernode;
	/** Reference to the exposure message label */
	Label* _exposurenode;

    /** The Box2D world */
    WorldController* _world;
    /** The world scale (computed from root node) */
    Vec2 _scale;

    // Physics objects for the game
    /** Reference to the goalDoor (for collision detection) */
    BoxObstacle*    _goalDoor;
    /** Reference to the player avatar */
    Shadow*      _avatar;
    /** Reference to the spinning barrier */
    Spinner*        _spinner;
    /** Reference to the rope bridge */
    RopeBridge*     _ropebridge;
    
    /** Whether or note this game is still active */
    bool _active;
    /** Whether we have completed this "game" */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;
    /** Whether we have failed at this world (and need a reset) */
    bool _failed;
	/** Whether we are in a shadow */
	bool _inShadow;
	/** The current level of exposure */
	float _exposure;
    /** Countdown active for winning or losing */
    int _countdown;
    
    /** Mark set to handle more sophisticated collision callbacks */
    unordered_set<b2Fixture*> _sensorFixtures;
    
    
#pragma mark Internal Object Management
    /**
     * Lays out the game geography.
     */
    void populate();
    
    /**
     * Immediately adds the object to the physics world
     *
     * Objects have a z-order.  This is the order they are drawn in the scene
     * graph node.  Objects with the different textures should have different
     * z-orders whenever possible.  This will cut down on the amount of drawing done
     *
     * param  obj       The object to add
     * param  zOrder    The drawing order
     *
     * @retain a reference to the obstacle
     */
    void addObstacle(Obstacle* obj, int zOrder);
    
    
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
    bool init(RootLayer* root);

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param bounds The game bounds in Box2d coordinates
     * @param scale  The difference between screen and Box2d coordinates
     * @param gravity The gravitational force on this Box2d world
     *
     * @retain a reference to the root layer
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(RootLayer* root, const Rect& rect);
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param bounds The game bounds in Box2d coordinates
     * @param scale  The difference between screen and Box2d coordinates
     * @param gravity The gravitational force on this Box2d world
     *
     * @retain a reference to the root layer
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(RootLayer* root, const Rect& rect, const Vec2& gravity);
    
    
#pragma mark -
#pragma mark State Access
    /**
     * Returns true if the gameplay controller is currently active
     *
     * @return true if the gameplay controller is currently active
     */
    bool isActive( ) const { return _active; }

    /**
     * Returns true if debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @return true if debug mode is active.
     */
    bool isDebug( ) const { return _debug; }
    
    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
    void setDebug(bool value) { _debug = value; _debugnode->setVisible(value); }
    
    /**
     * Returns true if the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @return true if the level is completed.
     */
    bool isComplete( ) const { return _complete; }
    
    /**
     * Sets whether the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @param value whether the level is completed.
     */
    void setComplete(bool value);
    
    /**
     * Returns true if the level is failed.
     *
     * If true, the level will reset after a countdown
     *
     * @return true if the level is failed.
     */
    bool isFailure() const { return _failed; }
    
    /**
     * Sets whether the level is failed.
     *
     * If true, the level will reset after a countdown
     *
     * @param value whether the level is failed.
     */
    void setFailure(bool value);
    
    
#pragma mark -
#pragma mark Allocation
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    GameController();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameController();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Preloads all of the assets necessary for this game world
     */
    void preload();

    
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
#pragma mark Post-Collision Processing
	/**
	 * Determines the type of the power-up collided with and applies the
	 * associated effects to the shadow or the game.
	 *
	 * @param	powerUp		The power-up object encountered
	 */
	void applyPowerup(const Powerup& powerup);

    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * Resets the status of the game so that we can play again.
     *
     * This method disposes of the world and creates a new one.
     */
    void reset() ;

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
     * Add a new bullet to the world and send it in the right direction.
     */
    void createBullet();
    
    /**
     * Remove a new bullet from the world.
     *
     * @param  bullet   the bullet to remove
     */
    void removeBullet(Obstacle* bullet);

	/**
	* Clear all memory when exiting.
	*/
	void stop();

};

#endif /* defined(__C_GAMEPLAY_H__) */
