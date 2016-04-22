//
//  SCMainLayer.cpp
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
#include "SCMainLayer.h"
#include "SCProgressBar.h"
#include "RDRocketRoot.h"
#include "RGRagdollRoot.h"
#include <cocos2d.h>
#include <cornell.h>

USING_NS_CC;

#pragma mark -
#pragma mark Loading Screen

#define LOADING_IMAGE       "background"
#define PROGRESS_IMAGE      "progress"
#define PLAY_IMAGE          "play"
#define PLAY_OFFSET         -200
#define PLAY_SCALE          0.75f

#pragma mark -
#pragma mark Initialization

/** Creates a new, but unactivated main layer */
MainLayer::MainLayer() :
_playNode(nullptr),
_barNode(nullptr),
_touchListener(nullptr),
_rocketScene(nullptr),
_ragdollScene(nullptr),
_rocketRoot(nullptr),
_ragdollRoot(nullptr),
_gorocket(true),
_transition(false)
{
}

/**
 * Builds the scene graph for the loading screen.
 *
 * The loading screen is what we run while we are waiting for the asynchronous
 * loader for finish with the textures.  Unlike previous demos, this is a
 * much more full-featured demo.  It has a progress bar and play button.
 */
void MainLayer::initScreen() {
    // Load the background image
    TextureLoader* tloader = (TextureLoader*)_assets->access<Texture2D>();
    
    Texture2D::TexParams params;
    params.wrapS = GL_CLAMP_TO_EDGE;
    params.wrapT = GL_CLAMP_TO_EDGE;
    params.magFilter = GL_LINEAR;
    params.minFilter = GL_LINEAR;
    
    tloader->load(LOADING_IMAGE, "textures/background.png", params);
    Texture2D* bkgd = tloader->get(LOADING_IMAGE);
    
    tloader->load(PLAY_IMAGE, "textures/play.png", params);
    Texture2D* play = tloader->get(PLAY_IMAGE);
    
    tloader->load(PROGRESS_IMAGE, "textures/progressbar.png", params);
    Texture2D* bar = tloader->get(PROGRESS_IMAGE);
    
    Size size = getContentSize();
    Vec2 center(size.width/2.0f,size.height/2.0f);
    
    // Create the background image
    auto bkgdnode = PolygonNode::createWithTexture(bkgd);
    bkgdnode->setAnchorPoint(Vec2(0.5f,0.5f));
    bkgdnode->setPosition(center);
    
    // Create the progress bar
    _barNode = ProgressBar::create(bar);
    _barNode->setAnchorPoint(Vec2(0.5f,0.5f));
    _barNode->setPosition(center+Vec2(0,-200));
    
    // Create the play button
    _playNode = PolygonNode::createWithTexture(play);
    _playNode->setAnchorPoint(Vec2(0.5f,0.5f));
    _playNode->setPosition(center+Vec2(0,-200));
    _playNode->setScale(0.75f);
    _playNode->setVisible(false);
    
    // Add everything to this layer
    addChild(bkgdnode, 1);
    addChild(_barNode, 2);
    addChild(_playNode, 3);
    
    // Create the touch listener. This is an autorelease object.
    _touchListener = TouchListener::create();
    if (_touchListener != nullptr) {
        _touchListener->retain();
        _touchListener->onTouchBegan = [this](Touch* t, timestamp_t time) {
            return touchBeganCB(t,time);
        };
        _touchListener->onTouchMoved = [=](Touch* t, timestamp_t time) {
            return this->touchMovedCB(t,time);
        };
        _touchListener->onTouchEnded = [=](Touch* t, timestamp_t time) {
            return this->touchEndedCB(t,time);
        };
        _touchListener->onTouchCancelled = [=](Touch* t, timestamp_t time) {
            return this->touchCancelCB(t,time);
        };
    }
}

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
Scene* MainLayer::createScene(RootLayer* layer) {
    auto scene = Scene::create();
    scene->retain();
    scene->addChild(layer);
    return scene;
}



#pragma mark -
#pragma mark Update Loop

/**
 * Starts the layer, allocating initial resources
 *
 * This method is used to bootstrap the game.  It should start up an asset manager
 * and load initial assets.
 */
void MainLayer::start() {
    auto scene = AssetManager::getInstance()->createScene();
    _assets = AssetManager::getInstance()->at(scene);
    _assets->attach<Texture2D>(TextureLoader::create());
    _assets->start();
    
    _rocketScene = nullptr;
    _ragdollScene = nullptr;
    
    // Create a "loading" screen
    initScreen();
    RootLayer::start(); // YOU MUST END with call to parent
}

/**
 * Updates the game for a single animation frame
 *
 * This method is called every animation frame.  There is no draw() or render()
 * counterpoint to this method; drawing is done automatically in the scene graph.
 * However, this method is responsible for updating any transforms in the scene graph.
 *
 * @param  dt   the time in seconds since last update
 */
void MainLayer::update(float dt) {
    // Do nothing if not initialized
    if (_barNode == nullptr) {
        return;
    }
    
    // Check if we resumed control from a specific level
    if (_playNode->isVisible() && !_touchListener->isActive()) {
        _touchListener->start(1);
    }
    
    // Load the two child scenes if we have not yet done so.
    if (_rocketScene == nullptr) {
        _rocketRoot  = RocketRoot::create();
        _rocketScene = createScene(_rocketRoot);
        _rocketRoot->preload();
    }
    
    if (_ragdollScene == nullptr) {
        _ragdollRoot  = RagdollRoot::create();
        _ragdollScene = createScene(_ragdollRoot);
        _ragdollRoot->preload();
    }
    
    // Move the progress bar as we load
    if (_barNode->getProgress() < 1.0f) {
        float value = (_rocketRoot->progress()+_ragdollRoot->progress())/2.0f;
        _barNode->setProgress(value);
    } else if (_barNode->isVisible()) {
        _barNode->setVisible(false);
        _playNode->setVisible(true);
    }
}


/**
 * Stops the layer, releasing all resources
 *
 * This method is used to clean-up any allocation that occurred in either start or
 * update.  While Cocos2d does have rudimentary garbage collection, you still have
 * to release any objects that you have retained.
 */
void MainLayer::stop() {
    RootLayer::stop();  // YOU MUST BEGIN with call to parent
    
    // YOU MUST STOP THE SOUNDS FOR THIS SCENE BEFORE UNLOADING THEM
    SoundEngine::getInstance()->stopAll();
    _assets->stop();
}


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
bool MainLayer::touchBeganCB(Touch* t, timestamp_t current) {
    // Get offset from node center.
    Vec2 pos = _playNode->getPosition()-t->getLocation();
    
    // Get node radius
    Rect rect = _playNode->getBoundingBox();
    float radius2 = (rect.size.width/2.0f)*(rect.size.width/2.0f);
    
    if (pos.lengthSquared() < radius2) {
        _transition = true;
        return true;
    }
    return false;
}

/**
 * Callback for the end of a touch event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void MainLayer::touchEndedCB(Touch* t, timestamp_t current) {
    if (_transition) {
        _touchListener->stop();
        _transition = false;
        
        if (_gorocket) {
        	// Have a cross-fade transition
            Director::getInstance()->pushScene(TransitionCrossFade::create(0.5,_rocketScene));
            // Have no transition
            // Director::getInstance()->pushScene(_rocketScene);
            _rocketRoot->start();
            _gorocket = false;
        } else {
        	// Have a cross-fade transition
            Director::getInstance()->pushScene(TransitionCrossFade::create(0.5,_ragdollScene));
            // Have no transition
            // Director::getInstance()->pushScene(_ragdollScene);
            _ragdollRoot->start();
            _gorocket = true;
        }
    }
}


/**
 * Callback for a touch movement event
 *
 * @param t     The touch information
 * @param event The associated event
 */
void MainLayer::touchMovedCB(Touch* t, timestamp_t current) {
    // This example only has gesture support.  Nothing to do here.
}

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
void MainLayer::touchCancelCB(Touch* t, timestamp_t current) {
}
