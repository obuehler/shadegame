//
//  CUFontLoader.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a specific implementation of the Loader class to load
//  True Type fonts. Because of how Cocos2D renders fonts, a font is defined by
//  both its source file and its size.  Fonts of different size are always
//  different fonts.
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
#include "CUFontLoader.h"
#include <2d/CCFontAtlasCache.h>  // Not sure why not part of cocos2d.h

NS_CC_BEGIN

#pragma mark -
#pragma mark Font Coordinator

/** The static coordinator singleton */
FontLoader::Coordinator* FontLoader::_gCoordinator = nullptr;

/**
 * Creates a new static coordinator
 *
 * The static coordinator is ready to go.  There is no start method.
 */
FontLoader::Coordinator::Coordinator() : instances(0) {
    _threads = ThreadPool::create(1);
    _threads->retain();
}

/**
 * Destroys the static coordinator, releasing all resources
 *
 * This will immediately orphan all loader instances and should not be called explicitly.
 */
FontLoader::Coordinator::~Coordinator() {
    for(auto it = _refcnts.begin(); it != _refcnts.end(); ++it) {
        TTFont* f = _objects[it->first];
        for(int ii = 0; ii < it->second; ii++) {
            f->release(); // Have to do for each refcount
        }
    }
    _objects.clear();
    _refcnts.clear();
}


#pragma mark Allocation Methods
/**
 * Loads the given font into memory
 *
 * The font will be loaded synchronously. It will be available immediately. If it was
 * previously loaded asynchronously, this method will block until it is done.
 *
 * @param  source   The pathname to the font file
 * @param  size     The font size
 *
 * @retain the font asset upon loading
 */
TTFont* FontLoader::Coordinator::load(std::string source, float size) {
    // Check if already allocated to the central hub.
    std::string id = TTFont::buildIdentifier(source,size);
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
    TTFont* font = TTFont::create(source, size);
    return allocateSync(font);
}

/**
 * Adds a new font to the loading queue.
 *
 * The font will be loaded asynchronously.  When it is finished loading, it
 * will be added to this loader, and accessible to ALL loaders.  If the file
 * is still pending, the callback will be appended to the callback list.
 *
 * @param  source   the pathname to the font file
 * @param  callback callback to invoke when loading is done.
 *
 * @retain the font asset upon loading
 */
void FontLoader::Coordinator::loadAsync(std::string source, float size, std::function<void(TTFont* t)> callback) {
    // Check if already allocated to the central hub.
    std::string id = TTFont::buildIdentifier(source,size);
    if (isLoaded(id)) {
        _objects[id]->retain();
        _refcnts[id] += 1;
        callback(_objects[id]);
    }
    
    // Otherwise, add the callback to the queue.
    if (_callbacks.find(id) == _callbacks.end()) {
        std::vector<std::function<void(TTFont* t)>> cvector;
        cvector.push_back(callback);
        _callbacks.emplace(id,cvector);
    } else {
        _callbacks[id].push_back(callback);
    }
    
    // C++11 closures are great
    TTFont* font = TTFont::create(source, size);
    font->retain();
    
    _threads->addTask([=](void) {
        std::function<void(FontAtlas* atlas)> cb = [=](FontAtlas* atlas){ this->allocateAsync(font, atlas); };
        FontAtlasCache::getFontAtlasTTFAsyncSafe(&(font->getTTF()), cb);
    });
}

/**
 * Creates a font object and retains a reference to it.
 *
 * Parameter font is a partially created font object.  It has TTConfig information
 * but no associated atlas.  This allocates the atlas synchronously blocking until
 * it is done.  It returns a reference to the same font object, now with atlas
 * information included.
 *
 * @param  font     the partially created font object
 *
 * @retain the font asset
 * @return the same font object, complete with atlas
 */
TTFont* FontLoader::Coordinator::allocateSync(TTFont* font) {
    font->_atlas = FontAtlasCache::getFontAtlasTTF(&(font->_config));
    if (font->_atlas == nullptr) {
        for (auto it = _callbacks[font->getName()].begin(); it != _callbacks[font->getName()].end(); ++it) {
            (*it)(nullptr);
        }
        font = nullptr;
    } else {
        font->retain();
        _objects[font->getName()] = font;
        _refcnts[font->getName()] = 1;
        for (auto it = _callbacks[font->getName()].begin(); it != _callbacks[font->getName()].end(); ++it) {
            (*it)(font);
        }
    }
    _callbacks.erase(font->getName());
    return font;
}

/**
 * Creates a font object and retains a reference to it.
 *
 * Parameter font is a partially created font object.  It has TTConfig information
 * but no associated atlas.  This allocates the font object with the given atlas, 
 * which was created asynchronously. This method should be used as a callback for
 * the asynchronous loader.
 *
 * @param  font     the partially created font object
 * @param  atlas    the atlas to associate with the font object
 *
 * @retain the font asset
 */
void FontLoader::Coordinator::allocateAsync(TTFont* font, FontAtlas* atlas) {
    if (atlas == nullptr) {
        // Failed to load font.
        for (auto it = _callbacks[font->getName()].begin(); it != _callbacks[font->getName()].end(); ++it) {
            (*it)(nullptr);
        }
        font->release();
    } else {
        font->_atlas = atlas;
        _objects[font->getName()] = font;
        _refcnts[font->getName()] = 1;
        for (auto it = _callbacks[font->getName()].begin(); it != _callbacks[font->getName()].end(); ++it) {
            (*it)(font);
        }
    }
    _callbacks.erase(font->getName());
}

/**
 * Safely releases the font for one loader
 *
 * If there are no more references to the font, it unloads the font from memory
 * and deletes this object.
 *
 * @param  font     the font to release.
 *
 * @release the font asset
 */
void FontLoader::Coordinator::release(TTFont* font) {
    CCASSERT(font, "Attempt to release a nullptr");
    _refcnts[font->getName()] -= 1;
    
    if (_refcnts[font->getName()] == 0) {
        std::unique_lock<std::mutex> lk(_mutex);
        _objects.erase(font->getName());
        _refcnts.erase(font->getName());
        FontAtlasCache::releaseFontAtlas(font->_atlas);
    }
    font->_atlas = nullptr;
    font->release();
}


#pragma mark -
#pragma mark Font Loader
/**
 * Creates a new FontLoader.
 *
 * This constructor does not start the font loader.  It simply creates an
 * object for the font loader so that it can be attached to the asset manager.
 * Call start() when you are ready to start using it.
 *
 * @returns an autoreleased FontLoader object
 */
FontLoader* FontLoader::create() {
    FontLoader *loader = new (std::nothrow) FontLoader();
    if (loader) {
        loader->autorelease();
        return loader;
    }
    CC_SAFE_DELETE(loader);
    return nullptr;
}

/**
 * Starts this font loader.
 *
 * This method bootstraps the loader with any initial resources that it
 * needs to load assets. Attempts to load an asset before this method is
 * called will fail.
 *
 * By separating this call from the constructor, this allows us to
 * construct loaders and attach them to the AssetManager before we are
 * ready to load assets.
 */
void FontLoader::start() {
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
 * attached to another loader. The asset manager is backed by a central
 * coordinator that allows the sharing of assets.
 *
 * Once the loader is stopped, any attempts to load a new asset will
 * fail.  You must call start() to begin loading assets again.
 */
void FontLoader::stop() {
    if (!_active) {
        return;
    }
    
    CCASSERT(_gCoordinator, "This font loader was orphaned by the coordinator");
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
 * Loads a font and assigns it to the given key.
 *
 * The font will be loaded synchronously. It will be available immediately.
 * This method should be limited to those times in which a font is
 * really necessary immediately, such as for a loading screen.
 *
 * The loaded font will use the default size of this loader.
 *
 * @param  key      The key to access the font after loading
 * @param  source   The pathname to the font file
 *
 * @retain the loaded font
 * @return the loaded font
 */
TTFont* FontLoader::load(std::string key, std::string source, float size) {
    CCASSERT(!contains(key), "Asset key is already in use");
    CCASSERT(_fqueue.find(key) == _fqueue.end(), "Asset key is pending on loader");
    CCASSERT(_gCoordinator, "This font loader was orphaned by the coordinator");
    
    TTFont* font = _gCoordinator->load(source,size);
    if (font != nullptr) {
        _assets[key] = font;
    }
    return font;
}

/**
 * Loads a font and assigns it to the given key.
 *
 * The font will be loaded synchronously. It will be available immediately.
 * This method should be limited to those times in which a font is
 * really necessary immediately, such as for a loading screen.
 *
 * @param  key      The key to access the font after loading
 * @param  source   The pathname to the font file
 * @param  size     The font size for this asset
 *
 * @retain the font asset upon loading
 */
void FontLoader::loadAsync(std::string key, std::string source, float size) {
    CCASSERT(!contains(key), "Asset key is already in use");
    CCASSERT(_fqueue.find(key) == _fqueue.end(), "Asset key is pending on loader");
    CCASSERT(_gCoordinator, "This font loader was orphaned by the coordinator");
    
    _fqueue.insert(key);
    _gCoordinator->loadAsync(source, size, [=](TTFont* font) { this->allocate(key, font); });
}

/**
 * A function to create a new font from a filename.
 *
 * This method should be part of a C++11 closure so that it can be used
 * by the thread pool for asynchronous loading. When done, it will safely
 * update the data structures of this loader.
 *
 * @param  key      The key for the font information
 * @param  font     The font to associate with the key
 */
void FontLoader::allocate(std::string key, TTFont* font) {
    if (font != nullptr) {
        _assets[key] = font;
    }
    _fqueue.erase(key);
}

/**
 * Unloads the font for the given key.
 *
 * This method simply unloads the font for the scene associated with this
 * loader. The font will not be deleted or removed from memory until it
 * is removed from all instances of FontLoader.
 *
 * @param  key  the key referencing the font
 *
 * @release the font for the given key
 */
void FontLoader::unload(std::string key) {
    CCASSERT(contains(key), "Attempt to release resource for unused key");
    CCASSERT(_gCoordinator, "This font loader was orphaned by the coordinator");
    
    _gCoordinator->release(_assets[key]);
    _fqueue.erase(key);
    _assets.erase(key);
}

/**
 * Unloads all assets present in this loader.
 *
 * This method simply unloads the fonts for the scene associated with this
 * loader. The founds will not be deleted or removed from memory until they
 * are removed from all instances of FontLoader.
 *
 * @release all loaded fonts
 */
void FontLoader::unloadAll() {
    CCASSERT(_gCoordinator, "This font loader was orphaned by the coordinator");
    
    for (auto it = _assets.begin(); it != _assets.end(); ++it ) {
        _gCoordinator->release(it->second);
    }
    _fqueue.clear();
    _assets.clear();
}

NS_CC_END