//
//  CUSceneManager.cpp
//  Cornell Extensions to Cocos2D
//
//  This class manages a collection of loaders for a single scene.  This allows us to
//  easily load and unload assets that are attached to a single Cocos2D scene.  As
//  Cocos2D is scene oriented, this is a natural way to handle assets.
//
//  Author: Walker White
//  Version: 12/10/15
//
#include "CUSceneManager.h"


NS_CC_BEGIN

#pragma mark -
#pragma mark Activation/Deactivation

/**
 * Creates a new SceneManager.
 *
 * This constructor does not start the scene manager or attach any loaders.  It
 * simply creates an object that is ready to accept loader objects. Call start()
 * when you are ready to start using it.
 *
 * @returns an autoreleased SceneManager object
 */
SceneManager* SceneManager::create() {
    SceneManager *loader = new (std::nothrow) SceneManager();
    if (loader) {
        loader->autorelease();
        return loader;
    }
    CC_SAFE_DELETE(loader);
    return nullptr;
}

/**
 * Starts this asset manager.
 *
 * This method bootstraps the manager with any initial resources that it
 * needs to load assets. Attempts to use any loader attached to this manager,
 * before this method is called, will fail.
 *
 * By separating this call from the constructor, this allows us to
 * construct loaders and attach them to this manager before we are
 * ready to load assets.
 */
void SceneManager::start() {
    CCASSERT(!_active, "Attempt to start an active manager");
    _active = true;
    for(auto it = _handlers.begin(); it != _handlers.end(); ++it) {
        it->second->start();
    }
}

/**
 * Stops this manager,  removing all assets.
 *
 * All of the loaders attached to this resource manager will immediately release
 * their assets.  However, an asset may still be available if it is attached to
 * another loader or scene manager.  This depends on the loaders involved.
 *
 * Once the manager is stopped, any attempts to load a new asset will
 * fail.  You must call start() to begin loading assets again.
 */
void SceneManager::stop() {
    CCASSERT(_active, "Attempt to stop an inactive manager");
    unloadAll();
    for(auto it = _handlers.begin(); it != _handlers.end(); ++it) {
        it->second->stop();
    }
    _active = false;
}


#pragma mark -
#pragma mark Loading/Unloading
/**
 * Returns the number of assets currently loaded.
 *
 * This is a rough way to determine how many assets have been loaded
 * so far. This method counts each asset equally regardless of the memory
 * requirements of each asset.
 *
 * This method is abstract and should be overridden in the specific
 * implementation for each asset.
 *
 * @return the number of assets currently loaded.
 */
size_t SceneManager::loadCount() const {
    size_t result = 0;
    for(auto it = _handlers.begin(); it != _handlers.end(); ++it) {
        result += it->second->loadCount();
    }
    return result;
}

/**
 * Returns the number of assets waiting to load.
 *
 * This is a rough way to determine how many assets are still pending.
 * An asset is pending if it has been loaded asychronously, and the
 * loading process has not yet finished. This method counts each asset
 * equally regardless of the memory requirements of each asset.
 *
 * This method is abstract and should be overridden in the specific
 * implementation for each asset.
 *
 * @return the number of assets waiting to load.
 */
size_t SceneManager::waitCount() const {
    size_t result = 0;
    for(auto it = _handlers.begin(); it != _handlers.end(); ++it) {
        result += it->second->waitCount();
    }
    return result;
}

NS_CC_END

