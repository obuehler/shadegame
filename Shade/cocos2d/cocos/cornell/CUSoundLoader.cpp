//
//  CUSoundLoader.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a specific implementation of the Loader class to load
//  sounds. It uses the experimental AudioEngine instead of SimpleAudioEngine.
//  This method requires a few modifications to the Cocos2d platform files to
//  work properly.  As AudioEngine is marked as experimental anyway, we felt
//  this was okay.
//
//  As with all of our loaders, this loader is designed to be attached to a scene.
//  This is the natural was to do things, as Cocos2d is scene based.  However,
//  its asset loading is typically done through the director, which is global.
//  This makes is hard to determine when it is safe to unload an asset.  Even
//  though the current scene many not need it, it may be used by another active
//  scene. Unloading the asset would corrupt that scene.
//
//  This loader solves this problem by have a static coordinator behind the
//  scenes.  This coordinate is shared across all loader instances.  It decides
//  When an asset is truly ready to be unloaded.
//
//  Author: Walker White
//  Version: 12/10/15
//
#include "CUSoundLoader.h"
#include "CUSoundEngine.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark Sound Coordinator

/** The static coordinator singleton */
SoundLoader::Coordinator* SoundLoader::_gCoordinator = nullptr;

/**
 * Creates a new static coordinator
 *
 * The static coordinator is ready to go.  There is no start method.
 */SoundLoader::Coordinator::Coordinator() : instances(0) {}

/**
 * Destroys new static coordinator, releasing all resources
 *
 * This will immediately orphan all loader instances and should not be called explicitly.
 */
SoundLoader::Coordinator::~Coordinator() {
    for(auto it = _refcnts.begin(); it != _refcnts.end(); ++it) {
        Sound* s = _sources[it->first];
        for(int ii = 0; ii < it->second; ii++) {
            s->release(); // Have to do for each refcount
        }
    }
    _sources.clear();
    _refcnts.clear();
}


#pragma mark Allocation Methods
/**
 * Loads the given sound into memory
 *
 * The sound will be loaded synchronously. It will be available immediately. If it was
 * previously loaded asynchronously, this method will block until it is done.
 *
 * @param  source   The pathname to the sound file
 *
 * @retain the sound asset upon loading
 */
Sound* SoundLoader::Coordinator::load(std::string source) {
    // Check if already allocated to the central hub.
    if (isLoaded(source)) {
        _sources[source]->retain();
        _refcnts[source] += 1;
        return _sources[source];
    } else if (isPending(source)) {
        // Unfortunately, busy waiting is the only working cross-platform solution.
        while (AENG::isLoaded(source) == 0) {
            std::this_thread::yield();
        }
        return (isLoaded(source) ? _sources[source] : nullptr);
    }

    // Need to queue it up anyway
    std::vector<std::function<void(Sound* s)>> cvector;
    _callbacks.emplace(source,cvector);

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::string path = FileUtils::getInstance()->fullPathForFilename(source);
    allocate(source,path.size() > 0,false);
#else
    AENG::preload(source);
    while (AENG::isLoaded(source) == 0) {
        std::this_thread::yield();
    }
    allocate(source,AENG::isLoaded(source) > 0,true);
#endif

    auto it = _sources.find(source);
    return (it == _sources.end() ? nullptr : it->second);
}

/**
 * Adds a new sound to the loading queue.
 *
 * The sound will be loaded asynchronously.  When it is finished loading, it
 * will be added to this loader, and accessible to ALL loaders.  If the file
 * is still pending, the callback will be appended to the callback list.
 *
 * @param  source   the pathname to the sound file
 * @param  callback callback to invoke when loading is done.
 *
 * @retain the sound asset upon loading
 */
void SoundLoader::Coordinator::loadAsync(std::string source, std::function<void(Sound* s)> callback) {
    // Check if already allocated to the central hub.
    if (isLoaded(source)) {
        _sources[source]->retain();
        _refcnts[source] += 1;
        callback(_sources[source]);
    }

    // Otherwise, add the callback to the queue.
    if (_callbacks.find(source) == _callbacks.end()) {
        std::vector<std::function<void(Sound* s)>> cvector;
        cvector.push_back(callback);
        _callbacks.emplace(source,cvector);
    } else {
        _callbacks[source].push_back(callback);
    }

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    std::string path = FileUtils::getInstance()->fullPathForFilename(source);
    allocate(source,path.size() > 0,false);
#else
    AENG::preload(source, [=](bool success) { this->allocate(source, success, true); });
#endif
}

/**
 * Creates a Sound object and retains a reference to it.
 *
 * This method is called when experimental::AudioEngine is done loading.  It takes
 * the results and packages them in a sound file, which is assigned to _sources.
 *
 * @param  source   the sound file information
 * @param  success  whether the sound was successfully loaded
 * @param  preload  whether the sound was preloaded at all
 *
 * @retain the sound asset
 */
void SoundLoader::Coordinator::allocate(std::string source, bool success, bool preload) {
    if (!success) {
        // Failed to load sound.
        for (auto it = _callbacks[source].begin(); it != _callbacks[source].end(); ++it) {
            (*it)(nullptr);
        }
    } else {
        Sound* snd = nullptr;
        if (preload) {
            float duration = AENG::getDuration(source);
            snd = Sound::create(source,duration);
        } else {
            snd = Sound::create(source);
        }
        snd->retain();
        _sources[source] = snd;
        _refcnts[source] = 1;
        for (auto it = _callbacks[source].begin(); it != _callbacks[source].end(); ++it) {
            (*it)(snd);
        }
    }
    _callbacks.erase(source);
}

/**
 * Safely releases the sound for one loader
 *
 * If there are no more references to the sound, it unloads the sound from memory
 * and deletes this object.
 *
 * @param  sound    the sound to release.
 *
 * @release the sound asset
 */
void SoundLoader::Coordinator::release(Sound* sound) {
    std::string source = sound->getSource();
    _refcnts[source] -= 1;
    
    if (_refcnts[source] == 0) {
        _sources.erase(source);
        _refcnts.erase(source);
#if CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
        experimental::AudioEngine::uncache(source);
#endif
    }
    sound->_source = "";
    sound->release();
}


#pragma mark -
#pragma mark Sound Loader
/**
 * Creates a new SoundLoader.
 *
 * This constructor does not start the sound loader.  It simply creates an
 * object for the sound loader so that it can be attached to the asset manager.
 * Call start() when you are ready to start using it.
 *
 * @returns an autoreleased SoundLoader object
 */
SoundLoader* SoundLoader::create() {
    SoundLoader *loader = new (std::nothrow) SoundLoader();
    if (loader) {
        loader->autorelease();
        return loader;
    }
    CC_SAFE_DELETE(loader);
    return nullptr;
}

/**
 * Starts this resource loader.
 *
 * This method bootstraps the loader with any initial resources that it
 * needs to load assets. Attempts to load an asset before this method is
 * called will fail.
 *
 * By separating this call from the constructor, this allows us to
 * construct loaders and attach them to the AssetManager before we are
 * ready to load assets.
 */
void SoundLoader::start() {
    if (_active) {
        return;
    }
    CCASSERT(SoundEngine::getInstance(), "SoundEngine must be started before loading assets");
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
 * attached to another loader. The asset manager is backed by a central
 * coordinator that allows the sharing of assets.
 *
 * Once the loader is stopped, any attempts to load a new asset will
 * fail.  You must call start() to begin loading assets again.
 */
void SoundLoader::stop() {
    if (!_active) {
        return;
    }
    CCASSERT(SoundEngine::getInstance(), "Assets must be unloaded before SoundEngine is stopped");
    CCASSERT(_gCoordinator, "This sound loader was orphaned by the coordinator");
    unloadAll();
    _gCoordinator->instances--;
    if (_gCoordinator->instances == 0) {
        delete _gCoordinator;
        _gCoordinator = nullptr;
    }
    _active = false;
}


#pragma mark -
#pragma mark Asset Management

/**
 * Loads a sound and assigns it to the given key.
 *
 * The sound will be loaded synchronously. It will be available immediately.
 * This method should be limited to those times in which a sound is
 * really necessary immediately, such as for a loading screen.
 *
 * @param  key      The key to access the sound after loading
 * @param  source   The pathname to the sound file
 *
 * @retain the sound upon loading
 * @return the loaded sound
 */
Sound* SoundLoader::load(std::string key, std::string source) {
    CCASSERT(SoundEngine::getInstance(), "SoundEngine must be started before loading assets");
    CCASSERT(!contains(key), "Asset key is already in use");
    CCASSERT(_squeue.find(key) == _squeue.end(), "Asset key is pending on loader");
    CCASSERT(_gCoordinator, "This sound loader was orphaned by the coordinator");
    
    Sound* sound = _gCoordinator->load(source);
    if (sound != nullptr) {
        _assets[key] = sound;
    }
    return sound;
}

/**
 * Adds a new sound to the loading queue.
 *
 * The sound will be loaded asynchronously.  When it is finished loading, it
 * will be added to this loader, and accessible under the given key. This
 * method will mark the loading process as not complete, even if it was
 * completed previously.  It is not safe to access the loaded sound until
 * it is complete again.
 *
 * @param  key      The key to access the sound after loading
 * @param  source   The pathname to the sound file
 *
 * @retain the sound upon loading
 */
void SoundLoader::loadAsync(std::string key, std::string source) {
    CCASSERT(SoundEngine::getInstance(), "SoundEngine must be started before loading assets");
    CCASSERT(!contains(key), "Asset key is already in use");
    CCASSERT(_squeue.find(key) == _squeue.end(), "Asset key is pending on loader");
    CCASSERT(_gCoordinator, "This sound loader was orphaned by the coordinator");

    _squeue.emplace(key);
    _gCoordinator->loadAsync(source, [=](Sound* sound) { this->allocate(key, sound); });
}

/**
 * A function to create a new sound from a filename.
 *
 * This method should be part of a C++11 closure so that it can be used
 * by the thread pool for asynchronous loading. When done, it will safely
 * update the data structures of this loader.
 *
 * @param  key      the key for the sound information
 * @param  sound    the sound to associate with the key
 */
void SoundLoader::allocate(std::string key, Sound* sound) {
    if (sound != nullptr) {
        _assets[key] = sound;
    }
    _squeue.erase(key);
}

/**
 * Unloads the sound for the given key.
 *
 * This method simply unloads the sound for the scene associated with this
 * loader. The sound will not be deleted or removed from memory until it
 * is removed from all instances of SoundLoader.
 *
 * @param  key  the key referencing the sound
 *
 * @release the sound for the given key
 */
void SoundLoader::unload(std::string key) {
    CCASSERT(contains(key), "Attempt to release resource for unused key");
    CCASSERT(_gCoordinator, "This sound loader was orphaned by the coordinator");

    _gCoordinator->release(_assets[key]);
    _assets.erase(key);
}

/**
 * Unloads all assets present in this loader.
 *
 * This method simply unloads the sounds for the scene associated with this
 * loader. The sounds will not be deleted or removed from memory until they
 * are removed from all instances of SoundLoader.
 *
 * @release all loaded sounds
 */
void SoundLoader::unloadAll() {
    CCASSERT(_gCoordinator, "This sound loader was orphaned by the coordinator");

    for (auto it = _assets.begin(); it != _assets.end(); ++it ) {
        _gCoordinator->release(it->second);
    }
    _assets.clear();
}

NS_CC_END