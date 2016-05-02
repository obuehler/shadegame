//
//  CUAssetManager.h
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
#ifndef __CU_ASSET_MANAGER_H__
#define __CU_ASSET_MANAGER_H__

#include <vector>
#include <cocos2d.h>
#include "CUSceneManager.h"

using namespace std;

NS_CC_BEGIN

#pragma mark -
#pragma mark Asset Manager

/**
 * Singleton class to support asset management.
 *
 * Assets should always be managed by a central loader.  Cocos2D appears to have these
 * things all over the place. This helps centralize them.  This is particularly useful 
 * when implementing scene management.
 *
 * In scene management, each asset is attached to a scene.  This allows you to unload all 
 * of the assets for a scene without unloading all assets.  It is possible for an asset 
 * to be attached to multiple scenes.  In that case, the scenes will attach a reference 
 * count, and the asset will only be unloaded when all associated scenes are unloaded.
 *
 * This class is a singleton, in the same way that director is.  That mean you should not
 * create new instances of this object (and the constructor is protected for that very 
 * reason). Instead, you should use the static method getInstance().
 */
class CC_DLL AssetManager {
protected:
    /** The singleton asset manager */
    static AssetManager* _gManager;
    
    /** The managers for each individual scene */
    std::vector<SceneManager*> _managers;

    /** The current active scene */
    int _scene;
    
public:
#pragma Singleton Access
    /**
     * Initializes the global asset manager.
     *
     * This should be called when the application starts
     */
    static void init();
    
    /**
     * Stops the global asset manager.
     *
     * This releases all of the allocated scene managers.  It should be called when
     * the application quits.
     */
    static void shutdown();
    
    /**
     * Returns a reference tot he global asset manager.
     *
     * @return a reference tot he global asset manager.
     */
    static AssetManager* getInstance() { return _gManager; }

    
#pragma Scene Management
    /**
     * Creates a new scene for managing assets.
     *
     * The new scene will be set as the current scene.
     *
     * @return the index for the new scene manager
     */
    int createScene();

    /**
     * Starts the scene manager for the given index
     *
     * @param  scene    The index for the scene manager
     */
    void startScene(int scene);
    
    /**
     * Starts all of the allocated scene managers.
     */
    void startAll();
    
    /**
     * Stops the scene manager for the given index
     *
     * @param  scene    The index for the scene manager
     */
    void stopScene(int scene);
    
    /**
     * Stops all of the allocated scene managers.
     */
    void stopAll();

    /**
     * Deletes the scene manager for the given index
     *
     * This method will stop the scene manager if it is still active.
     * Future attempts to access a scene manager for this index will
     * raise an exception.
     *
     * @param  scene    The index for the scene manager
     */
    void deleteScene(int scene);

    /**
     * Deletes all of the allocated scene managers.
     *
     * This method will stop the scene managers if they are still active.
     * It will clear the asset manager and future attempts to access the
     * previously allocated scene managers will raise an exception.
     */
    void deleteAll();

    /**
     * Returns True if scene corresponds to an allocated scene.
     *
     * In general, this method will return false if scene was deleted.  However, the
     * scene identifier may be reused by later allocations.
     *
     * @return True if scene corresponds to an allocated scene.
     */
    bool hasScene(int scene) const { return scene < _managers.size() && _managers[scene] != nullptr; }
    
    
#pragma Scene Access

    /**
     * Returns the index for the current scene.
     *
     * If there is no current scene, this method will return -1.
     *
     * @return the index for the current scene.
     */
    int getCurrentIndex() const         { return _scene; }

    /**
     * Sets the index for the current scene.
     *
     * If there is no current scene, this method will return -1.
     *
     * @param  scene    The index for the current scene.
     */
    void setCurrentIndex(int scene)     { _scene = scene; }

    /**
     * Returns the scene manager for the current scene.
     *
     * If there is no active scene, this method will return nullptr.
     *
     * @return the asset manager for the current scene
     */
    SceneManager* getCurrent() const    { return (_scene >= 0 ? _managers.at(_scene) : nullptr); }

    /**
     * Returns the scene manager for the given index.
     *
     * If the scene is invalid, this method will raise an exception.
     *
     * @param  scene    The index for the scene manager
     *
     * @return the asset manager for the given index
     */
    SceneManager* at(int scene) const { return _managers.at(scene); }
    
    /**
     * Returns the scene manager for the given index.
     *
     * If the scene is invalid, this method will raise an exception.
     *
     * @param  scene    The index for the scene manager
     *
     * @return the asset manager for the given index
     */
    SceneManager* operator[](int scene) const { return _managers.at(scene); }

private:
    /** This macro disables the copy constructor (not allowed on assets) */
    CC_DISALLOW_COPY_AND_ASSIGN(AssetManager);

    /**
     * Creates a new, inactive asset manager
     */
    AssetManager() : _scene(-1) {}
    
    /**
     * Deletes the asset manager
     *
     * This method stops all scene managers and releases all resources.
     */
    ~AssetManager() { stopAll(); deleteAll();}
};

NS_CC_END
#endif /* defined(__CU_ASSET_MANAGER_H__) */
