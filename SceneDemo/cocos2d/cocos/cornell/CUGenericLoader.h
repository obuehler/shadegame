//
//  CUGenericLoader.h
//  Cornell Extensions to Cocos2D
//
//  This module provides a templated implementation of the Loader class to load
//  any subclass of asset. Unlike the base loader, this class has a static
//  coordinator behind the scenes to manage resources access scene.  This is
//  similar to the solutions for our other loaders.  Hence we have a consistent
//  loading framework with maximum flexibility.
//
//  Author: Walker White
//  Version: 1/20/16
//
#ifndef __CU_ASSET_LOADER__
#define __CU_ASSET_LOADER__
#include <unordered_set>
#include <mutex>
#include "CULoader.h"
#include "CUAsset.h"
#include "CUThreadPool.h"


NS_CC_BEGIN

#pragma mark -
#pragma mark  Abstract Base Class
/**
 * Class is a implementation of Loader<Asset>
 *
 * This asset loader allows us to allocate generic from the associated source
 * files. As with all of our loaders, this loader is designed to be attached 
 * to a scene. This is the natural was to do things, as Cocos2d is scene based.  
 * However, its asset loading is typically done through the director, which is
 * global. This makes is hard to determine when it is safe to unload an asset. 
 * Even though the current scene many not need it, it may be used by another 
 * active scene. Unloading the asset would corrupt that scene.
 *
 * This loader solves this problem by have a static coordinator behind the
 * scenes.  This coordinate is shared across all loader instances.  It decides
 * When an asset is truly ready to be unloaded.  We assume that all instances
 * are run only in the Director thread.
 *
 * This class should not be created directly.  Instead, it should be wrapped
 * by an instance of GenericLoader<T>.
 */
class CC_DLL GenericBaseLoader : public Loader<Asset> {
private:
    /** This macro disables the copy constructor (not allowed on assets) */
    CC_DISALLOW_COPY_AND_ASSIGN(GenericBaseLoader);
    
protected:
#pragma mark -
#pragma mark Asset Coordinator
    /**
     * Class is a static coordinate to garbage collect assets
     *
     * This coordinate tracks asset usage across all active loaders.  When an asset
     * is not used by any loader, it is removed from memory.
     */
    class Coordinator {
    private:
        /** The asset objects allocated for each source file */
        std::unordered_map<std::string,Asset*> _objects;
        /** The number of active references to each texture. */
        std::unordered_map<std::string, int> _refcnts;
        /** The callback functions registered to a texture for asynchronous loading */
        std::unordered_map<std::string,std::vector<std::function<void(Asset* s)>>> _callbacks;
        
        /** Thread pool for asynchronous loading */
        ThreadPool* _threads;
        /** Mutex for the asynchronous asset loading */
        std::mutex _mutex;
        
    public:
        /** The number of active asset loader instances */
        size_t instances;
        
        /**
         * Creates a new static coordinator
         *
         * The static coordinator is ready to go.  There is no start method.
         */
        Coordinator();
        
        /**
         * Destroys the static coordinator, releasing all resources
         *
         * This will immediately orphan all loader instances and should not be called explicitly.
         */
        ~Coordinator();
        
        /**
         * Returns true if a source is allocated and loaded.
         *
         * It is possible that there are multiple instances of the loader loading the same asset.
         * We only want to load it once.
         *
         * @return true if a source is allocated and loaded.
         */
        bool isLoaded(std::string id)  const { return _objects.find(id) != _objects.end(); }
        
        /**
         * Returns true if a source is pending allocation.
         *
         * It is possible that there are multiple instances of the loader loading the same asset.
         * We only want to load it once.
         *
         * @return true if a source is pending allocation.
         */
        bool isPending(std::string id) const { return _callbacks.find(id) != _callbacks.end(); }
        
        
#pragma mark Allocation Methods
        /**
         * Loads the given asset into memory
         *
         * Parameter asset is a partially created asset object.  It has the file set, but
         * has not loaded any resources.  This method calls the load() method, (in either
         * the main or a separate thread). It returns a reference to the same asset object,
         * now with resources loaded.
         *
         * The asset will be loaded synchronously. It will be available immediately. If it was
         * previously loaded asynchronously, this method will block until it is done.
         *
         * @param  source   The pathname to the asset file
         * @param  size     The  size
         *
         * @retain the asset upon loading
         */
        Asset* load(Asset* asset);
        
        /**
         * Adds a new asset to the loading queue.
         *
         * Parameter asset is a partially created asset object.  It has the file set, but
         * has not loaded any resources.  This method calls the load() method, (in either
         * the main or a separate thread). It returns a reference to the same asset object,
         * now with resources loaded.
         *
         * The asset will be loaded asynchronously.  When it is finished loading, it
         * will be added to this loader, and accessible to ALL loaders.  If the file
         * is still pending, the callback will be appended to the callback list.
         *
         * @param  source   the pathname to the asset file
         * @param  callback callback to invoke when loading is done.
         *
         * @retain the asset upon loading
         */
        void loadAsync(Asset* asset, std::function<void(Asset* s)> callback);
        
        /**
         * Creates a asset object and retains a reference to it.
         *
         * Parameter asset is a partially created asset object.  It has the file set, but
         * has not loaded any resources.  This method calls the load() method, (in either
         * the main or a separate thread). It returns a reference to the same asset object, 
         * now with resources loaded.
         *
         * @param  asset     the partially created asset object
         *
         * @retain the asset
         * @return the same asset object, loaded from file
         */
        Asset* allocate(Asset* asset);
        
        /**
         * Safely releases the asset for one loader
         *
         * If there are no more references to the asset, it unloads the asset from memory
         * and deletes this object.
         *
         * @param  asset     the asset to release.
         *
         * @release the asset
         */
        void release(Asset* asset);
    };
    
    /** The static coordinator singleton */
    static Coordinator* _gCoordinator;

    
#pragma mark -
#pragma mark Asset Loader
    /** The assets we are expecting that are not yet loaded */
    std::unordered_set<std::string> _aqueue;
    
    /**
     * A function to create a new asset from a filename.
     *
     * This method should be part of a C++11 closure so that it can be used
     * by the thread pool for asynchronous loading. When done, it will safely
     * update the data structures of this loader.
     *
     * @param  key      The key for the asset information
     * @param  asset     The asset to associate with the key
     */
    void allocate(std::string key, Asset* asset);
    
    
public:
#pragma mark Activation/Deactivation
    /**
     * Creates a new GenericBaseLoader.
     *
     * This constructor does not start the asset loader.  It simply creates an
     * object for the asset loader so that it can be attached to the asset manager.
     * Call start() when you are ready to start using it.
     *
     * @returns an autoreleased assetLoader object
     */
    static GenericBaseLoader* create();
    
    /**
     * Starts this asset loader.
     *
     * Any assets loaded by this loader will be immediately released by
     * the loader.  However, an asset may still be available if it is
     * attached to another loader. The asset loader is backed by a central
     * coordinator that allows the sharing of assets.
     *
     * Once the loader is stopped, any attempts to load a new asset will
     * fail.  You must call start() to begin loading assets again.
     */
    void start() override;
    
    /**
     * Stops this resource loader removing all assets.
     *
     * Any assets loaded by this loader will be immediately released by
     * the loader.  However, an asset may still be available if it is
     * attached to another loader.
     *
     * Once the loader is stopped, any attempts to load a new asset will
     * fail.  You must call start() to begin loading assets again.
     */
    void stop() override;
    
    
#pragma mark Loading/Unloading
    /**
     * Returns the number of assets waiting to load.
     *
     * This is a rough way to determine how many assets are still pending.
     * A asset is pending if it has been loaded asychronously, and the
     * loading process has not yet finished. This method counts each asset
     * equally regardless of the memory requirements of the format.
     *
     * @return the number of assets waiting to load.
     */
    size_t waitCount() const override { return _aqueue.size(); }
    
    /**
     * This method is disabled, as the GenericLoader<T> will wrap it with the
     * correct version of the method.
     */
    Asset* load(std::string key, std::string source) override { return nullptr; }

    /**
     * Loads a asset and assigns it to the given key.
     *
     * Parameter asset is a partially created asset object.  It has the file set, but
     * has not loaded any resources.  This method calls the load() method, (in either
     * the main or a separate thread). It returns a reference to the same asset object,
     * now with resources loaded.
     *
     * The asset will be loaded synchronously. It will be available immediately.
     * This method should be limited to those times in which an asset is really
     * necessary immediately, such as for a loading screen.
     *
     * @param  key      The key to access the asset after loading
     * @param  source   The pathname to the asset file
     *
     * @retain the loaded asset
     * @return the loaded asset
     */
    Asset* load(std::string key, Asset* asset);
    
    /**
     * This method is disabled, as the GenericLoader<T> will wrap it with the
     * correct version of the method.
     */
    void loadAsync(std::string key, std::string source) override {}

    /**
     * Adds a new asset to the loading queue.
     *
     * Parameter asset is a partially created asset object.  It has the file set, but
     * has not loaded any resources.  This method calls the load() method, (in either
     * the main or a separate thread). It returns a reference to the same asset object,
     * now with resources loaded.
     *
     * The asset will be loaded asynchronously.  When it is finished loading, it will
     * be added to this loader, and accessible under the given key. This method will
     * mark the loading process as not complete, even if it was completed previously.
     * It is not safe to access the loaded asset until it is complete again.
     *
     * @param  key      The key to access the asset after loading
     * @param  source   The pathname to the asset file
     *
     * @retain the asset upon loading
     */
    void loadAsync(std::string key, Asset* asset);
    
    /**
     * Unloads the asset for the given key.
     *
     * This method simply unloads the asset for the scene associated with this
     * loader. The asset will not be deleted or removed from memory until it
     * is removed from all instances of assetLoader.
     *
     * @param  key  the key referencing the asset
     *
     * @release the asset for the given key
     */
    void unload(std::string key) override;
    
    /**
     * Unloads all assets present in this loader.
     *
     * This method simply unloads the assets for the scene associated with this
     * loader. The assets will not be deleted or removed from memory until they
     * are removed from all instances of assetLoader.
     *
     * @release all loaded assets
     */
    void unloadAll() override;
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates a new, uninitialized asset loader
     */
    GenericBaseLoader() : Loader<Asset>() {}
    
    /**
     * Disposes of the asset loader.
     *
     * This destructor will stop the asset loader if not done so already.
     */
    virtual ~GenericBaseLoader() { if (_active) { stop(); } }
};


#pragma mark -
#pragma mark Asset Loader Template
/**
 * Class is a implementation of Loader<T> where T is a subclass of Asset.
 *
 * This asset loader allows us to allocate generic from the associated source
 * files. Instances of this loader wrap GenericBaseLoader, giving us access to
 * the static coordinator for assets. It decides When an asset is truly ready 
 * to be unloaded.  We assume that all instances are run only in the Director 
 * thread.
 */
template <class T>
class GenericLoader : public Loader<T> {
protected:
    /** Wrapped reference to the base loader */
    GenericBaseLoader* _internal;
    
public:
    /**
     * Creates a new GenericLoader.
     *
     * This constructor does not start the generic loader.  It simply creates an
     * object for the generic loader so that it can be attached to the asset manager.
     * Call start() when you are ready to start using it.
     *
     * @returns an autoreleased GenericLoader object
     */
    static GenericLoader<T>* create() {
        GenericLoader<T> *loader = new (std::nothrow) GenericLoader<T>();
        if (loader) {
            loader->autorelease();
            return loader;
        }
        CC_SAFE_DELETE(loader);
        return nullptr;
    }
    
    /**
     * Starts this GenericLoader.
     *
     * Any assets loaded by this loader will be immediately released by the loader.
     * However, an asset may still be available if it is attached to another loader. 
     * The asset manager is backed by a central coordinator that allows the sharing 
     * of assets.
     *
     * Once the loader is stopped, any attempts to load a new asset will fail.  
     * You must call start() to begin loading assets again.
     */
    void start() override {
        if (_internal->isActive()) {
            return;
        }
        BaseLoader::start();
        _internal->start();
    }
    
    /**
     * Stops this GenericLoader, removing all assets.
     *
     * Any assets loaded by this loader will be immediately released by the loader.
     * However, an asset may still be available if it is attached to another loader.
     * Once the loader is stopped, any attempts to load a new asset will fail.
     * You must call start() to begin loading assets again.
     */
    void stop() override {
        if (!_internal->isActive()) {
            return;
        }
        BaseLoader::stop();
        _internal->stop();
    }


#pragma mark Loading/Unloading
    /**
     * Returns the number of assets waiting to load.
     *
     * This is a rough way to determine how many assets are still pending.
     * An asset is pending if it has been loaded asychronously, and the
     * loading process has not yet finished. This method counts each asset
     * equally regardless of the memory requirements of the format.
     *
     * @return the number of assets waiting to load.
     */
    size_t waitCount() const override { return _internal->waitCount(); }
    
    /**
     * Returns the number of assets currently loaded.
     *
     * This is a rough way to determine how many assets have been loaded
     * so far. This method counts each asset equally regardless of the memory
     * requirements of each asset.
     *
     * @return the number of assets currently loaded.
     */
    size_t loadCount() const override { return _internal->loadCount(); }
    
    /**
     * Returns true if the key maps to a loaded asset.
     *
     * This method is useful in case the asset fails to load.
     *
     * @param  key  the key associated with the asset
     *
     * @return True if the key maps to a loaded asset.
     */
    bool contains(std::string key) const override { return _internal->contains(key); }

    /**
     * Returns the asset for the given key.
     *
     * If the key is valid, the asset is guaranteed not to be null.  Otherwise,
     * this method returns nullptr
     *
     * @param  key  the key associated with the asset
     *
     * @return the asset pointer for the given key
     */
    T* get(std::string key) const override {
        Asset* asset = _internal->get(key);
        return dynamic_cast<T*>(asset);
    }
    
    /**
     * Loads a asset and assigns it to the given key.
     *
     * The asset will be loaded synchronously. It will be available immediately.
     * This method should be limited to those times in which an asset is really
     * necessary immediately, such as for a loading screen.
     *
     * @param  key      The key to access the asset after loading
     * @param  source   The pathname to the asset file
     *
     * @retain the loaded asset
     * @return the loaded asset
     */
    T* load(std::string key, std::string source) override {
        T* asset = T::create(source);
        if (asset != nullptr) {
            Asset* result = _internal->load(key,asset);
            return dynamic_cast<T*>(result);
        }
        return nullptr;
    }
    
    /**
     * Adds a new asset to the loading queue.
     *
     * The asset will be loaded asynchronously.  When it is finished loading, it
     * will be added to this loader, and accessible under the given key. This
     * method will mark the loading process as not complete, even if it was
     * completed previously.  It is not safe to access the loaded asset until
     * it is complete again.
     *
     * @param  key      The key to access the asset after loading
     * @param  source   The pathname to the asset file
     *
     * @retain the asset upon loading
     */
    void loadAsync(std::string key, std::string source) override {
        T* asset = T::create(source);
        if (asset != nullptr) {
            _internal->loadAsync(key,asset);
        }
    }
    
    /**
     * Unloads the asset for the given key.
     *
     * This method simply unloads the asset for the scene associated with this
     * loader. The asset will not be deleted or removed from memory until it
     * is removed from all instances of GenericLoader.
     *
     * @param  key  the key referencing the asset
     *
     * @release the asset for the given key
     */
    void unload(std::string key) override { _internal->unload(key); }
    
    /**
     * Unloads all assets present in this loader.
     *
     * This method simply unloads the assets for the scene associated with this
     * loader. The assets will not be deleted or removed from memory until they
     * are removed from all instances of GenericLoader.
     *
     * @release all loaded assets
     */
    void unloadAll() override { _internal->unloadAll(); }

    
//CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates a new, uninitialized asset loader
     */
    GenericLoader() : Loader<T>() {
        _internal = GenericBaseLoader::create();
        _internal->retain();
    }
    
    /**
     * Disposes of the asset loader.
     *
     * This destructor will stop the asset loader if not done so already.
     */
    virtual ~GenericLoader() {
        if (_internal->isActive()) { _internal->stop(); }
        _internal->release();
        _internal = nullptr;
    }
};


NS_CC_END

#endif /* defined(__CU_ASSET_LOADER__) */