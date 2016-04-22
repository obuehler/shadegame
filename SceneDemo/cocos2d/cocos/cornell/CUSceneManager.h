//
//  CUSceneManager.h
//  Cornell Extensions to Cocos2D
//
//  This class manages a collection of loaders for a single scene.  This allows us to
//  easily load and unload assets that are attached to a single Cocos2D scene.  As
//  Cocos2D is scene oriented, this is a natural way to handle assets.
//
//  Author: Walker White
//  Version: 12/10/15
//
#ifndef __CU_SCENE_MANAGER_H__
#define __CU_SCENE_MANAGER_H__
#include <unordered_map>
#include <cocos2d.h>
#include "CULoader.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark Scene Manager
/**
 * Class is an asset manager for a single scene.
 *
 * This asset manager is uses to manage a collection of loaders.  Loaders must be
 * "attached" to the asset manager.  The asset manager does not come with a collection
 * of asset managers pre-installed.  You will need to do this yourself in the start-up
 * code for each scene.
 *
 * Once a loader is attached to this asset manager, the manager obtains ownership of
 * the loader.  It will be responsible for garbage collection when it is done.
 */
class CC_DLL SceneManager: public Ref {
private:
    /** This macro disables the copy constructor (not allowed on assets) */
    CC_DISALLOW_COPY_AND_ASSIGN(SceneManager);
    
    
protected:
    /** Whether or not this scene manager is active */
    bool _active;
    
    /** The individual loaders for each type */
    std::unordered_map<size_t,BaseLoader*> _handlers;
    
public:
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
    static SceneManager* create();
    
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
    void start();
    
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
    void stop();
    
    /**
     * Returns true if this asset manager is active.
     *
     * An active manager is one that can load assets (e.g. method start() has been
     * called).  If it is not active, none of its attached loaders are active 
     * either.
     *
     * @return true if this asset manager is active.
     */
    bool isActive() const { return _active; }
    

#pragma mark Loader Attachment
    /**
     * Attaches the given loader to an asset manager
     *
     * The type of the asset is specified by the template parameter T.  Once attached, 
     * all assets of type T will use this loader for this scene.  In addition, this asset 
     * manager will obtain ownership of the loader and be responsible for its garbage
     * collection.
     *
     * @param  loader   The loader for asset T
     *
     * @retain a reference to this loader
     * @return false if there is already a loader for this asset
     */
    template<typename T>
    bool attach(Loader<T>* loader) {
        size_t hash = typeid(T).hash_code();
        auto it = _handlers.find(hash);
        if (it != _handlers.end()) {
            return false;
        }
        _handlers[hash] = loader;
        loader->retain();
        if (_active && !loader->isActive()) {
            loader->start();
        } else if (!_active && loader->isActive()) {
            loader->stop();
        }
        return true;
    }

    /**
     * Returns True if there is a loader for the given asset Type
     *
     * The type of the asset is specified by the template parameter T.
     *
     * @return True if there is a loader for the given asset Type
     */
    template<typename T>
    bool isAttached() {
        size_t hash = typeid(T).hash_code();
        return _handlers.find(hash) != _handlers.end();
    }
    
    /**
     * Detaches a loader for an asset type
     *
     * The type of the asset is specified by the template parameter T.  The loader will be 
     * released and deleted if there are no further references to it.
     *
     * @release the reference to the loader
     * @return true if there was a loader of that Type.
     */
    template<typename T>
    bool detach() {
        size_t hash = typeid(T).hash_code();
        auto it = _handlers.find(hash);
        if (it == _handlers.end()) {
            return false;
        }
        it->second->release();
        _handlers.erase(hash);
        return true;
    }
    
    /**
     * Detaches all loaders from this asset manager
     *
     * The loaders will be released and deleted if there are no further 
     * references to them.
     *
     * @release the reference to all loaders
     */
    void detachAll() {
        for(auto it = _handlers.begin(); it != _handlers.end(); ++it) {
            it->second->release();
        }
        _handlers.clear();
    }
    
    /**
     * Returns the loader the given asset Type
     *
     * The type of the asset is specified by the template parameter T.
     *
     * @return the loader the given asset Type
     */
    template<typename T>
    Loader<T>* access() {
        size_t hash = typeid(T).hash_code();
        auto it = _handlers.find(hash);
        if (it == _handlers.end()) {
            CCASSERT(false, "No loader assigned for given type");
            return nullptr;
        }

        return ((Loader<T>*)it->second);
    }

    
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
    size_t loadCount() const;
    
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
    size_t waitCount() const;
    
    /**
     * Returns true if the loader has finished loading all assets.
     *
     * It is not safe to use asynchronously loaded assets until all
     * loading is complete.  This method allows us to determine when
     * asset loading is complete.
     *
     * @return true if the loader has finished loading
     */
    bool isComplete() const { return waitCount() == 0; }
    
    /**
     * Returns the loader progress as a percentage.
     *
     * This method returns a value between 0 and 1.  A value of 0 means
     * no assets have been loaded.  A value of 1 means that all assets
     * have been loaded.
     *
     * Anything in-between indicates that there are assets which have
     * been loaded asynchronously and have not completed loading. It
     * is not safe to use asynchronously loaded assets until all
     * loading is complete.
     *
     * @return the loader progress as a percentage.
     */
    float progress() const  {
        size_t size = loadCount()+waitCount();
        return (size == 0 ? 0.0f : ((float)loadCount())/size);
    }
    
    /**
     * Returns the asset for the given key.
     *
     * The type of the asset is specified by the template parameter T.
     *
     * @param  key  The key to identify the given asset
     *
     * @return the asset for the given key.
     */
    template<typename T>
    T* get(std::string key) const {
        size_t hash = typeid(T).hash_code();
        auto it = _handlers.find(hash);
        if (it == _handlers.end()) {
            CCASSERT(false, "No loader assigned for given type");
            return nullptr;
        }
        
        return ((Loader<T>*)it->second)->get(key);
    }


    /**
     * Loads an asset and assigns it to the given key.
     *
     * The type of the asset is specified by the template parameter T.
     * The asset will be loaded synchronously. It will be available immediately.
     * This method should be limited to those times in which an asset is
     * really necessary immediately, such as for a loading screen.
     *
     * @param  key      The key to access the asset after loading
     * @param  source   The pathname to the asset source
     *
     * @return the loaded asset
     */
    template<typename T>
    T* load(std::string key, std::string source) {
        size_t hash = typeid(T).hash_code();
        auto it = _handlers.find(hash);
        if (it != _handlers.end()) {
            Loader<T>* ref = (Loader<T>*)(it->second);
            return ref->load(key,source);
        }
        
        CCASSERT(false, "No loader assigned for given type");
        return nullptr;
    }
    
    /**
     * Adds a new asset to the loading queue.
     *
     * The type of the asset is specified by the template parameter T.
     * The asset will be loaded asynchronously.  When it is finished loading, 
     * it will be added to this loader, and accessible under the given key. This
     * method will mark the loading process as not complete, even if it was
     * completed previously.  It is not safe to access the loaded asset until
     * it is complete again.
     *
     * @param  key      The key to access the asset after loading
     * @param  source   The pathname to the asset source
     */
    template<typename T>
    void loadAsync(std::string key, std::string source) {
        size_t hash = typeid(T).hash_code();
        auto it = _handlers.find(hash);
        if (it != _handlers.end()) {
            it->second->loadAsync(key,source);
            return;
        }
        
        CCASSERT(false, "No loader assigned for given type");
    }
    
    /**
     * Unloads the asset for the given key.
     *
     * The type of the asset is specified by the template parameter T.
     * This method simply unloads the asset for the scene associated with 
     * this loader. Depending upon the loader, it may not be removed from 
     * memory if there are other loaders referencing the same asset.
     *
     * @param  key  the key referencing the asset
     */
    template<typename T>
    void unload(std::string key) {
        size_t hash = typeid(T).hash_code();
        auto it = _handlers.find(hash);
        if (it != _handlers.end()) {
            it->second->unload(key);
            return;
        }
        
        CCASSERT(false, "No loader assigned for given type");
    }
    
    /**
     * Unloads all assets present in this loader.
     *
     * This method unloads all assets for the scene associated with this
     * loader. Depending upon the loaders, the assets may not be removed 
     * from memory if there are other loaders referencing the same assets.
     */
    void unloadAll() {
        for(auto it = _handlers.begin(); it != _handlers.end(); ++it) {
            it->second->unloadAll();
        }
    }
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates a new, uninitialized scene manager
     */
    SceneManager() : _active(false) {}
    
    /**
     * Disposes of the scene manager.
     *
     * This destructor will stop the scene manager if not done so already.
     */
    ~SceneManager() { if (_active) { stop(); } detachAll(); }
};

NS_CC_END

#endif /* defined(__CU_SCENE_MANAGER_H__) */
