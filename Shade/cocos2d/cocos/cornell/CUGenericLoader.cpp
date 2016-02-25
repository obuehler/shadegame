//
//  CUGenericLoader.cpp
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
#include "CUGenericLoader.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark Asset Coordinator

/** The static coordinator singleton */
GenericBaseLoader::Coordinator* GenericBaseLoader::_gCoordinator = nullptr;

/**
 * Creates a new static coordinator
 *
 * The static coordinator is ready to go.  There is no start method.
 */
GenericBaseLoader::Coordinator::Coordinator() : instances(0) {
    _threads = ThreadPool::create(1);
    _threads->retain();
}

/**
 * Destroys the static coordinator, releasing all resources
 *
 * This will immediately orphan all loader instances and should not be called explicitly.
 */
GenericBaseLoader::Coordinator::~Coordinator() {
    for(auto it = _refcnts.begin(); it != _refcnts.end(); ++it) {
        Asset* a = _objects[it->first];
        for(int ii = 0; ii < it->second; ii++) {
            a->release(); // Have to do for each refcount
        }
    }
    _objects.clear();
    _refcnts.clear();
}


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
Asset* GenericBaseLoader::Coordinator::load(Asset* asset) {
    // Check if already allocated to the central hub.
    std::string id = asset->getFile();
    if (isLoaded(id)) {
        _objects[id]->retain();
        _refcnts[id] += 1;
        return _objects[id];
    } else if (isPending(id)) {
        // Unfortunately, busy waiting is the only working cross-platform solution.
        while (isPending(id)) {
            std::this_thread::yield();
        }
        return (isLoaded(id) ? _objects[id] : nullptr);
    }
    
    // FORCE LOAD NOW (and call callbacks)
    asset->retain();
    return allocate(asset);
}

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
void GenericBaseLoader::Coordinator::loadAsync(Asset* asset, std::function<void(Asset* t)> callback) {
    // Check if already allocated to the central hub.
    std::string id = asset->getFile();
    if (isLoaded(id)) {
        _objects[id]->retain();
        _refcnts[id] += 1;
        callback(_objects[id]);
    }
    
    // Otherwise, add the callback to the queue.
    if (_callbacks.find(id) == _callbacks.end()) {
        std::vector<std::function<void(Asset* t)>> cvector;
        cvector.push_back(callback);
        _callbacks.emplace(id,cvector);
    } else {
        _callbacks[id].push_back(callback);
    }
    
    // C++11 closures are great
    asset->retain();
    _threads->addTask([=](void) { this->allocate(asset); });
}

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
Asset* GenericBaseLoader::Coordinator::allocate(Asset* asset) {
    std::string id = asset->getFile();
    bool success = asset->load();
    if (!success) {
        for (auto it = _callbacks[id].begin(); it != _callbacks[id].end(); ++it) {
            (*it)(nullptr);
        }
        asset->release();
        asset = nullptr;
    } else {
        _objects[id] = asset;
        _refcnts[id] = 1;
        for (auto it = _callbacks[id].begin(); it != _callbacks[id].end(); ++it) {
            (*it)(asset);
        }
    }
    _callbacks.erase(id);
    return asset;
}

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
void GenericBaseLoader::Coordinator::release(Asset* asset) {
    CCASSERT(asset, "Attempt to release a nullptr");
    std::string id = asset->getFile();
    _refcnts[id] -= 1;
    
    if (_refcnts[id] == 0) {
        std::unique_lock<std::mutex> lk(_mutex);
        _objects.erase(id);
        _refcnts.erase(id);
        asset->unload();
    }
    asset->release();
    asset = nullptr;
}


#pragma mark -
#pragma mark Asset Loader
/**
 * Creates a new GenericBaseLoader.
 *
 * This constructor does not start the asset loader.  It simply creates an
 * object for the asset loader so that it can be attached to the asset manager.
 * Call start() when you are ready to start using it.
 *
 * @returns an autoreleased assetLoader object
 */
GenericBaseLoader* GenericBaseLoader::create() {
    GenericBaseLoader *loader = new (std::nothrow) GenericBaseLoader();
    if (loader) {
        loader->autorelease();
        return loader;
    }
    CC_SAFE_DELETE(loader);
    return nullptr;
}

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
void GenericBaseLoader::start() {
    if (_active) {
        return;
    }
    if (_gCoordinator == nullptr) {
        _gCoordinator = new (std::nothrow)Coordinator();
    }
    if (_gCoordinator != nullptr) {
        _gCoordinator->instances++;
        _active = true;
    }
}

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
void GenericBaseLoader::stop() {
    if (!_active) {
        return;
    }
    
    CCASSERT(_gCoordinator, "This texture loader was orphaned by the coordinator");
    unloadAll();
    _gCoordinator->instances--;
    if (_gCoordinator->instances == 0) {
        delete _gCoordinator;
        _gCoordinator = nullptr;
    }
    _active = false;
}


#pragma mark -
#pragma mark File Loading
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
Asset* GenericBaseLoader::load(std::string key, Asset* asset) {
    CCASSERT(!contains(key), "Asset key is already in use");
    CCASSERT(_aqueue.find(key) == _aqueue.end(), "Asset key is pending on loader");
    CCASSERT(_gCoordinator, "This asset loader was orphaned by the coordinator");
    
    Asset* result = _gCoordinator->load(asset);
    if (result != nullptr) {
        _assets[key] = result;
    }
    return result;
}

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
void GenericBaseLoader::loadAsync(std::string key, Asset* asset) {
    CCASSERT(!contains(key), "Asset key is already in use");
    CCASSERT(_aqueue.find(key) == _aqueue.end(), "Asset key is pending on loader");
    CCASSERT(_gCoordinator, "This asset loader was orphaned by the coordinator");
    
    _aqueue.insert(key);
    _gCoordinator->loadAsync(asset, [=](Asset* a) { this->allocate(key, a); });
}

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
void GenericBaseLoader::allocate(std::string key, Asset* asset) {
    if (asset != nullptr) {
        _assets[key] = asset;
    }
    _aqueue.erase(key);
}

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
void GenericBaseLoader::unload(std::string key) {
    CCASSERT(contains(key), "Attempt to release resource for unused key");
    CCASSERT(_gCoordinator, "This asset loader was orphaned by the coordinator");
    
    _gCoordinator->release(_assets[key]);
    _aqueue.erase(key);
    _assets.erase(key);
}

/**
 * Unloads all assets present in this loader.
 *
 * This method simply unloads the assets for the scene associated with this
 * loader. The assets will not be deleted or removed from memory until they
 * are removed from all instances of assetLoader.
 *
 * @release all loaded assets
 */
void GenericBaseLoader::unloadAll() {
    CCASSERT(_gCoordinator, "This asset loader was orphaned by the coordinator");
    
    for (auto it = _assets.begin(); it != _assets.end(); ++it ) {
        _gCoordinator->release(it->second);
    }
    _aqueue.clear();
    _assets.clear();
}

NS_CC_END