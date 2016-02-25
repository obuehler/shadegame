//
//  CUAssetManager.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a singleton class to support asset management. Assets
//  should always be managed by a central loader.  Cocos2D appears to have these
//  things all over the place. This is a way to centralize everything.
//
//  More importantly, this asset loader allows for scene management, which is not
//  something available in the various asset loaders in Cocos2D.  Scene management
//  allows you to attach assets to a scene, and load and unload them for that scene.
//
//  Author: Walker White
//  Version: 12/3/15
//
#include "CUAssetManager.h"

NS_CC_BEGIN

#pragma mark -
#pragma Singleton Access

/** Initialization of static reference */
AssetManager* AssetManager::_gManager = nullptr;

/**
 * Initializes the global asset manager.
 *
 * This should be called when the application starts
 */
void AssetManager::init() {
    if (_gManager == nullptr) {
        _gManager = new AssetManager();
    }
}

/**
 * Stops the global asset manager.
 *
 * This releases all of the allocated scene managers.  It should be called when
 * the application quits.
 */
void AssetManager::shutdown() {
    if (_gManager != nullptr) {
        delete _gManager;
        _gManager = nullptr;
    }
}


#pragma mark -
#pragma Scene Management
/**
 * Creates a new scene for managing assets.
 *
 * The new scene will be set as the current scene.
 *
 * @return the index for the new scene manager
 */
int AssetManager::createScene() {
    _scene = (int)_managers.size();
    SceneManager* scene = SceneManager::create();
    scene->retain();
    _managers.push_back(scene);
    
    return _scene;
}

/**
 * Starts the scene manager for the given index
 *
 * @param  scene    The index for the scene manager
 */
void AssetManager::startScene(int scene) {
    _managers[scene]->start();
}

/**
 * Starts all of the allocated scene managers.
 */
void AssetManager::startAll() {
    for(auto it = _managers.begin(); it != _managers.end(); ++it) {
        if (!(*it)->isActive()) {
            (*it)->start();
        }
    }
}

/**
 * Stops the scene manager for the given index
 *
 * @param  scene    The index for the scene manager
 */
void AssetManager::stopScene(int scene) {
    _managers[scene]->stop();
}

/**
 * Stops all of the allocated scene managers.
 */
void AssetManager::stopAll() {
    for(auto it = _managers.begin(); it != _managers.end(); ++it) {
        if ((*it)->isActive()) {
            (*it)->stop();
        }
    }
}

/**
 * Deletes the scene manager for the given index
 *
 * This method will stop the scene manager if it is still active.
 * Future attempts to access a scene manager for this index will
 * raise an exception.
 *
 * @param  scene    The index for the scene manager
 */
void AssetManager::deleteScene(int scene) {
    CCASSERT(hasScene(scene), "Attempting to delete nonexistent scene");
    _managers[scene]->release();
    _managers[scene] = nullptr;
}

/**
 * Deletes all of the allocated scene managers.
 *
 * This method will stop the scene managers if they are still active.
 * It will clear the asset manager and future attempts to access the
 * previously allocated scene managers will raise an exception.
 */
void AssetManager::deleteAll() {
    for(auto it = _managers.begin(); it != _managers.end(); ++it) {
        (*it)->release();
    }
    _managers.clear();
}

NS_CC_END
