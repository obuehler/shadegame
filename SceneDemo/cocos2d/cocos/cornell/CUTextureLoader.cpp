//
//  CUTextureLoader.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a specific implementation of the Loader class to load
//  textures. Technically, a texture should be identified by both its source
//  file and its texture parameters.  However, Cocos2D does not allow you to
//  to do that -- each texture can be only loaded once. Refactoring this would
//  be a lot more work than we are comfortable with.
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
#include "CUTextureLoader.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark Texture Coordinator

/** The static coordinator singleton */
TextureLoader::Coordinator* TextureLoader::_gCoordinator = nullptr;

/**
 * Creates a new static coordinator
 *
 * The static coordinator is ready to go.  There is no start method.
 */
TextureLoader::Coordinator::Coordinator() : instances(0) {}

/**
 * Destroys the static coordinator, releasing all resources
 *
 * This will immediately orphan all loader instances and should not be called explicitly.
 */
TextureLoader::Coordinator::~Coordinator() {
    for(auto it = _refcnts.begin(); it != _refcnts.end(); ++it) {
        Texture2D* t = _objects[it->first];
        for(int ii = 0; ii < it->second; ii++) {
            t->release(); // Have to do for each refcount
        }
    }
    _sources.clear();
    _objects.clear();
    _refcnts.clear();
}


#pragma mark Allocation Methods
/**
 * Loads the given texture into memory
 *
 * The texture will be loaded synchronously. It will be available immediately. If it was
 * previously loaded asynchronously, this method will block until it is done.
 *
 * @param  source   The pathname to the texture image file
 *
 * @retain the texture asset upon loading
 */
Texture2D* TextureLoader::Coordinator::load(std::string source) {
    // Check if already allocated to the central hub.
    if (isLoaded(source)) {
        _objects[source]->retain();
        _refcnts[source] += 1;
        return _objects[source];
    }
    
    // FORCE LOAD NOW (and call callbacks)
    if (_callbacks.find(source) == _callbacks.end()) {
        std::vector<std::function<void(Texture2D* t)>> cvector;
        _callbacks.emplace(source,cvector);
    }
    TextureCache* cache = Director::getInstance()->getTextureCache();
    Texture2D* texture = cache->addImage(source);
    allocate(texture, source);
    return texture;
}

/**
 * Adds a new texture to the loading queue.
 *
 * The texture will be loaded asynchronously.  When it is finished loading, it
 * will be added to this loader, and accessible to ALL loaders.  If the file
 * is still pending, the callback will be appended to the callback list.
 *
 * @param  source   the pathname to the texture file
 * @param  callback callback to invoke when loading is done.
 *
 * @retain the texture asset upon loading
 */
void TextureLoader::Coordinator::loadAsync(std::string source, std::function<void(Texture2D* t)> callback) {
    // Check if already allocated to the central hub.
    if (isLoaded(source)) {
        _objects[source]->retain();
        _refcnts[source] += 1;
        callback(_objects[source]);
    }
    
    // Otherwise, add the callback to the queue.
    if (_callbacks.find(source) == _callbacks.end()) {
        std::vector<std::function<void(Texture2D* t)>> cvector;
        cvector.push_back(callback);
        _callbacks.emplace(source,cvector);
    } else {
        _callbacks[source].push_back(callback);
    }
    
    // C++11 closures are great
    TextureCache* cache = Director::getInstance()->getTextureCache();
    cache->addImageAsync(source, [=](Texture2D* texture) { this->allocate(texture,source); });
}

/**
 * Creates a texture object and retains a reference to it.
 *
 * This method is called when Cocos2D TextureCache is finished loading.  It takes
 * the results and packages them in a texture file, which is assigned to _sources.
 *
 * @param  source   The texture file information
 * @param  success  Whether the texture was successfully loaded
 *
 * @retain the texture asset
 */
void TextureLoader::Coordinator::allocate(Texture2D* texture, std::string source) {
    if (_callbacks.find(source) == _callbacks.end()) {
        // Force add clobbered callbacks.
        return;
    }
    
    if (texture == nullptr) {
        // Failed to load texture.
        for (auto it = _callbacks[source].begin(); it != _callbacks[source].end(); ++it) {
            (*it)(nullptr);
        }
    } else {
        texture->retain();
        _objects[source] = texture;
        _sources[texture->getName()] = source;
        _refcnts[source] = 1;
        for (auto it = _callbacks[source].begin(); it != _callbacks[source].end(); ++it) {
            (*it)(texture);
        }
    }
    _callbacks.erase(source);
}

/**
 * Safely releases the texture for one loader
 *
 * If there are no more references to the texture, it unloads the texture from memory
 * and deletes this object.
 *
 * @param  texture  the texture to release.
 *
 * @release the texture asset
 */
void TextureLoader::Coordinator::release(Texture2D* texture) {
    CCASSERT(texture, "Attempt to release a nullptr");
    std::string source = _sources[texture->getName()];
    _refcnts[source] -= 1;
    
    if (_refcnts[source] == 0) {
        _sources.erase(texture->getName());
        _objects.erase(source);
        _refcnts.erase(source);
        TextureCache* cache = Director::getInstance()->getTextureCache();
        cache->removeTexture(texture);
    }
    texture->release();
}


#pragma mark -
#pragma mark Texture Loader
/**
 * Creates a new TextureLoader.
 *
 * This constructor does not start the texture loader.  It simply creates an
 * object for the texture loader so that it can be attached to the asset manager.
 * Call start() when you are ready to start using it.
 *
 * @returns an autoreleased TextureLoader object
 */
TextureLoader* TextureLoader::create() {
    TextureLoader *loader = new (std::nothrow) TextureLoader();
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
void TextureLoader::start() {
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
    _default.minFilter = GL_NEAREST;
    _default.magFilter = GL_LINEAR;
    _default.wrapS = GL_CLAMP_TO_EDGE;
    _default.wrapT = GL_CLAMP_TO_EDGE;
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
void TextureLoader::stop() {
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

/**
 * Loads a texture and assigns it to the given key.
 *
 * The texture will be loaded synchronously. It will be available immediately.
 * This method should be limited to those times in which a texture is really
 * necessary immediately, such as for a loading screen.
 *
 * @param  key      The key to access the texture after loading
 * @param  source   The pathname to the texture image file
 * @param  params   The texture parameters for initialization
 *
 * @retain the loaded texture
 * @return the loaded texture
 */
Texture2D* TextureLoader::load(std::string key, std::string source, const Texture2D::TexParams& params) {
    CCASSERT(!contains(key), "Asset key is already in use");
    CCASSERT(_tqueue.find(key) == _tqueue.end(), "Asset key is pending on loader");
    CCASSERT(_gCoordinator, "This texture loader was orphaned by the coordinator");
    
    Texture2D* texture = _gCoordinator->load(source);
    if (texture != nullptr) {
        texture->setTexParameters(params);
        _assets[key] = texture;
    }
    return texture;
}

/**
 * Adds a new texture to the loading queue.
 *
 * The texture will be loaded asynchronously.  When it is finished loading, it
 * will be added to this loader, and accessible under the given key. This
 * method will mark the loading process as not complete, even if it was
 * completed previously.  It is not safe to access the loaded texture until
 * it is complete again.
 *
 * @param  key      The key to access the texture after loading
 * @param  source   The pathname to the texture image file
 * @param  params   The texture parameters for initialization
 *
 * @retain the texture upon loading
 */
void TextureLoader::loadAsync(std::string key, std::string source, const Texture2D::TexParams& params) {
    CCASSERT(!contains(key), "Asset key is already in use");
    CCASSERT(_tqueue.find(key) == _tqueue.end(), "Asset key is pending on loader");
    CCASSERT(_gCoordinator, "This texture loader was orphaned by the coordinator");
    
    _tqueue.insert(key);
    _gCoordinator->loadAsync(source, [=](Texture2D* texture) { this->allocate(key, texture, params); });
}

/**
 * A function to create a new texture from a filename.
 *
 * This method should be part of a C++11 closure so that it can be used
 * by the thread pool for asynchronous loading. When done, it will safely
 * update the data structures of this loader.
 *
 * @param  key      The key for the texture information
 * @param  texture  The texture to associate with the key
 * @param  params   The texture parameters to initialize the texture
 */
void TextureLoader::allocate(std::string key, Texture2D* texture, const Texture2D::TexParams& params) {
    if (texture != nullptr) {
        texture->setTexParameters(params);
        _assets[key] = texture;
    }
    _tqueue.erase(key);
}

/**
 * Unloads the texture for the given key.
 *
 * This method simply unloads the texture for the scene associated with this
 * loader. The texture will not be deleted or removed from memory until it
 * is removed from all instances of TextureLoader.
 *
 * @param  key  the key referencing the texture
 *
 * @release the texture for the given key
 */
void TextureLoader::unload(std::string key) {
    CCASSERT(contains(key), "Attempt to release resource for unused key");
    CCASSERT(_gCoordinator, "This texture loader was orphaned by the coordinator");
    
    _gCoordinator->release(_assets[key]);
    _tqueue.erase(key);
    _assets.erase(key);
}

/**
 * Unloads all assets present in this loader.
 *
 * This method simply unloads the textures for the scene associated with this
 * loader. The textures will not be deleted or removed from memory until they
 * are removed from all instances of TextureLoader.
 *
 * @release all loaded textures
 */
void TextureLoader::unloadAll() {
    CCASSERT(_gCoordinator, "This texture loader was orphaned by the coordinator");
    
    for (auto it = _assets.begin(); it != _assets.end(); ++it ) {
        _gCoordinator->release(it->second);
    }
    _tqueue.clear();
    _assets.clear();
}

NS_CC_END