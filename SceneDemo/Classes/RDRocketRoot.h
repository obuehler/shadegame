//
//  RDRocketRoot.h
//  RocketDemo
//
//  This class is a reimagining of the RocketDemo.  Instead of splitting the layer and the
//  the controller, we combine them into a single class.  This is because we are now using
//  scenes instead of controllers to alter game functionality.
//
//  The most important thing to understand this time is the difference between init() and
//  start().  The method init() is called as soon as we create the layer and its associated
//  scene, even if it is not immediately active  The method start() is called it is activated
//  (and stop() is called when it is deactivated).
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 4/2/16
//
#ifndef __RD_ROCKET_ROOT_H__
#define __RD_ROCKET_ROOT_H__

#include <vector>
#include <cocos2d.h>
#include <cornell.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include "RDRocketInput.h"
#include "RDRocketModel.h"

using namespace cocos2d;

/**
 * Class represents the root node for the rocket demo.
 *
 * Note that this is constructed VERY differently than previous root nodes.  That is
 * because, while it is the root of the ragdoll demo, it is not the root of the
 * the entire application.  Therefore, it should be prepared to stop() and start()
 * multiple times while the applciation is running.
 *
 * Otherwise, this is identical to the rocket demo
 */
class RocketRoot : public RootLayer {
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CC_DISALLOW_COPY_AND_ASSIGN(RocketRoot);
    
protected:
    /** Reference to the scene manager (localized asset manager) */
    SceneManager* _assets;
    /** The scene manager's index in the global asset manager */
    int _sceneidx;
    
    /** Controller for abstracting out input away from layer */
    RocketInput _input;
    
    /** Reference to the root node of the scene graph */
    Node* _rootnode;
    /** Reference to the physics root of the scene graph */
    Node* _worldnode;
    /** Reference to the debug root of the scene graph */
    Node* _debugnode;
    /** Reference to the win message label */
    Label* _winnode;
    
    /** The Box2D world */
    WorldController* _world;
    /** The world scale (computed from root node) */
    Vec2 _scale;
    
    // Physics objects for the game
    /** Reference to the goalDoor (for collision detection) */
    BoxObstacle* _goalDoor;
    /** Reference to the rocket/player avatar */
    RocketModel* _rocket;
    
    /** Whether or note this game is still active */
    bool _active;
    /** Whether we have completed this "game" */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;
    
    
#pragma mark Internal Helpers
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
     */
    void addObstacle(Obstacle* obj, int zOrder);
    
    /**
     * Updates that animation for a single burner
     *
     * This method is here instead of the the rocket model because of our philosophy
     * that models should always be lightweight.  Animation includes sounds and other
     * assets that we do not want to process in the model
     *
     * @param  burner   The rocket burner to animate
     * @param  on       Whether to turn the animation on or off
     */
    void updateBurner(RocketModel::Burner burner, bool on);
    
    
#pragma mark -
#pragma mark Update Loop
public:
    /**
     * Created the demo contents. but does NOT start the game
     *
     * This constructor creates any objects that are to be reused across multiple
     * instances of the demo, such as the Box2D world, or any child nodes.  However,
     * it does not assume that any assets have finised loading.  Initialization
     * that requires assets is delayed to start().
     *
     * @return  An autoreleased root layer
     */
    static RocketRoot* create();

    /**
     * Starts the layer, allocating initial resources
     *
     * This method is used to bootstrap the game.  It should access the asset manager
     * and perform any intialization that requires the assets.
     *
     * IMPORTANT: This method does not start up any global singletons.  That is because
     * it is not attached to the root scene of the application.
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
     * Stops the layer, releasing all non-reusable resources
     *
     * This method is used to clean-up any allocation that occurred in either start or
     * update.  While Cocos2d does have rudimentary garbage collection, you still have
     * to release any objects that you have retained.
     *
     * IMPORTANT: This method does not stop any global singletons.  That is because
     * it is not attached to the root scene of the application.
     */
    void stop() override;
    
    
#pragma mark -
#pragma mark State Management
    /**
     * Resets the status of the game so that we can play again.
     *
     * This method disposes of the world and creates a new one.
     */
    void reset();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Preloads all of the assets necessary for this game world
     */
    void preload();

    /**
     * Returns the current progress of the asset loading
     *
     * The scene should not be started until this value is 1.0f
     *
     * @return the current progress of the asset loading
     */
    float progress() const { return _assets == nullptr ? 0.0f : _assets->progress(); }

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
    void setComplete(bool value) { _complete = value; _winnode->setVisible(value); }
    
    
#pragma mark -
#pragma mark Collision Handling
    /**
     * Processes the start of a collision
     *
     * This method is called when we first get a collision between two objects.  We use
     * this method to test if it is the "right" kind of collision.  In particular, we
     * use it to test if we make it to the win door.
     *
     * @param  contact  The two bodies that collided
     */
    void beginContact(b2Contact* contact);
    
    /**
     * Handles any modifications necessary before collision resolution
     *
     * This method is called just before Box2D resolves a collision.  We use this method
     * to implement sound on contact, using the algorithms outlined in Ian Parberry's
     * "Introduction to Game Physics with Box2D".
     *
     * @param  contact  The two bodies that collided
     * @param  contact  The collision manifold before contact
     */
    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);

    
#pragma mark -
#pragma mark Initialization
CC_CONSTRUCTOR_ACCESS:
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     */
   	RocketRoot();
    
    /** Disposes of the ragdoll demo, releasing all resources */
    virtual ~RocketRoot()  { dispose(); }
    
    /**
     * Initializes the demo contents. but does NOT start the game
     *
     * This initializer creates any objects that are to be reused across multiple
     * instances of the demo, such as the Box2D world, or any child nodes.  However,
     * it does not assume that any assets have finised loading.  Initialization
     * that requires assets is delayed to start().
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init() override;
};

#endif /* defined(__RD_ROCKET_ROOT_H__) */