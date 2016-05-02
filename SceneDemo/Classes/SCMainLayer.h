//
//  SCMainLayer.h
//  SceneDemo
//
//  Even with multiple scenes, we need to have a ROOT scene.  This is the scene that we always return
//  to when we leave one of the child scenes.  This does could be a menu screen, or (as in this case)
//  it could be the loading screen.  It is up to you.
//
//  To change scenes, press the play button.  The button will alternate which scene it chooses each
//  time.
//
//  Author: Walker White
//  Version: 4/2/16
//
#ifndef __SC_MAIN_LAYER_H__
#define __SC_MAIN_LAYER_H__

#include <cocos2d.h>
#include <cornell.h>

// Forward declation of supporting classes
class ProgressBar;
class RocketRoot;
class RagdollRoot;

/**
 * Class represents the root node for the primary game scene.
 *
 * Unlike previous demos, you will notice that we put everything into the layer.  We do not 
 * separate out a controller.  This is to show how we could arrange everything as scenes.  
 *
 * Note that this loads all of the assets for all of the scenes at the beginning.  This is 
 * not necessary, but you will want some sort of loading screen in-between scenes if you
 * do loading on demand.  More importantly, you will see that it obtains ownership of all
 * of its child scenes.  This is necessary, as an inactive scene will be deleted if it is
 * not retained.
 */
class MainLayer : public cocos2d::RootLayer {
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CC_DISALLOW_COPY_AND_ASSIGN(MainLayer);
    
    /** Reference to the scene manager (localized asset manager) */
    cocos2d::SceneManager* _assets;
    
    /** Node representing the play button */
    cocos2d::PolygonNode* _playNode;
    /** Node representing the progress bar */
    ProgressBar* _barNode;
    
    /** Simple input handler for this layer */
    cocos2d::TouchListener* _touchListener;
    
    /** Child scene (and associated layer) for the rocket demo */
    cocos2d::Scene* _rocketScene;
    RocketRoot*  _rocketRoot;
    
    /** Child scene (and associated layer) for the ragdoll demo */
    cocos2d::Scene* _ragdollScene;
    RagdollRoot* _ragdollRoot;
    
    /** Whether to chose the rocket demo (instead of the ragdoll demo) */
    bool _gorocket;
    /** Whether to transition to a new scene */
    bool _transition;

    
#pragma mark -
#pragma mark Internal Helpers
    /**
     * Builds the scene graph for the loading screen.
     *
     * The loading screen is what we run while we are waiting for the asynchronous
     * loader for finish with the textures.  Unlike previous demos, this is a 
     * much more full-featured demo.  It has a progress bar and play button.
     */
    void initScreen();
    
    /**
     * Returns a newly created scene containing the given layer
     *
     * This method is different from GameRoot::createScene<T> in that it does not
     * start the layer.  We should only start the layer after transitioning to it.
     *
     * @param layer     The root layer for the scene
     *
     * @return a newly created scene containing the given layer
     */
    cocos2d::Scene* createScene(RootLayer* layer);

    
#pragma mark -
#pragma mark Update Loop
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
    
    
#pragma mark -
#pragma mark Input Handling
    /**
     * Callback for the beginning of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     *
     * @return True if the touch was processed; false otherwise.
     */
    bool touchBeganCB(cocos2d::Touch* t, cocos2d::timestamp_t current);
    
    /**
     * Callback for the end of a touch event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchEndedCB(cocos2d::Touch* t, cocos2d::timestamp_t current);
    
    /**
     * Callback for a touch movement event
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchMovedCB(cocos2d::Touch* t, cocos2d::timestamp_t current);
    
    /**
     * Callback for the cancellation of a touch event
     *
     * Cancellation occurs when an external event—for example,
     * an incoming phone call—disrupts the current app’s event
     * processing.
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void touchCancelCB(cocos2d::Touch* t, cocos2d::timestamp_t current);
    
    
CC_CONSTRUCTOR_ACCESS:
    /** Creates a new, but unactivated main layer */
    MainLayer();
};

#endif // __SC_MAIN_LAYER_H__
