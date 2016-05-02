//
//  CULoader.h
//  Cornell Extensions to Cocos2D
//
//  This module provides the base templates for loader classes.  Our goal with
//  this module is to create a modular loader system that is as close to
//  Nathan Sweet's flexible and modular AssetManager for libGDX.  To do this in
//  C++ requires three layers:
//
//  The first layer is a polymorphic base that is used the AssetManager for
//  adding and removing loaders. It is the C++ equivalent of a Java interface.
//
//  The second layer is a template that provides type correctness when accessing
//  components in the AssetManager.  It has generic functionality that is
//  necessary for all loaders, but is missing the information needed to load
//  the specific asset.
//
//  The top layer is a specific class for each asset that implements the specific
//  loading functionality.
//
//  This module implements the first two layers.  As they are both a template and
//  a pure polymorphic class, only a header file is necessary.  There is no
//  associated CPP file with this header.
//
//  Author: Walker White
//  Version: 12/10/15
//
#ifndef __CU_LOADER_H__
#define __CU_LOADER_H__

#include <cocos2d.h>
#include <unordered_map>

NS_CC_BEGIN

#pragma mark -
#pragma mark Polymorphic Base
/**
 * Class provides a polymorphic base to the loader system.
 *
 * This is effectively a Java-style interface.  It identifies the methods that 
 * all loaders must have, and provides a type for the AssetManager to use in
 * its underlying storage container.
 */
class CC_DLL BaseLoader : public Ref {
protected:
    /** Whether or not this resource loader is active */
    bool _active;

public:
#pragma mark Activation/Deactivation
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
     *
     * This method is abstract and should be overridden in the specific
     * implementation for each asset.
     */
    virtual void start() { _active = true; }
    
    /**
     * Stops this resource loader removing all assets.
     *
     * Any assets loaded by this loader will be immediately released by
     * the loader.  However, an asset may still be available if it is 
     * attached to another loader.  See the description of the specific
     * implementation for how assets are released.
     *
     * Once the loader is stopped, any attempts to load a new asset will
     * fail.  You must call start() to begin loading assets again.
     *
     * This method is abstract and should be overridden in the specific
     * implementation for each asset.
     */
    virtual void stop() {  _active = false; }
    
    /**
     * Returns true if this resource loader is active.
     *
     * An active resource loader is one that can load assets (e.g. method start()
     * has been called).
     *
     * @return true if this resource loader is active.
     */
    bool isActive() const { return _active; }

    
#pragma mark Loading/Unloading
    /**
     * Adds a new asset to the loading queue.
     *
     * The asset will be loaded asynchronously.  When it is finished loading, it
     * will be added to this loader, and accessible under the given key. This
     * method will mark the loading process as not complete, even if it was
     * completed previously.  It is not safe to access the loaded asset until
     * it is complete again.
     *
     * This method is abstract and should be overridden in the specific
     * implementation for each asset.
     *
     * @param  key      The key to access the asset after loading
     * @param  source   The pathname to the asset
     */
    virtual void loadAsync(std::string key, std::string source) {}
    
    /**
     * Unloads the asset for the given key.
     *
     * An asset may still be available if it is attached to another loader.
     * See the description of the specific implementation for how assets
     * are released.
     *
     * This method is abstract and should be overridden in the specific
     * implementation for each asset.
     *
     * @param  key  the key referencing the asset
     */
    virtual void unload(std::string key) {}
    
    /**
     * Unloads all assets present in this loader.
     *
     * An asset may still be available if it is attached to another loader.
     * See the description of the specific implementation for how assets
     * are released.  This method is similar to stop(), except that new
     * assets can be loaded.
     *
     * This method is abstract and should be overridden in the specific
     * implementation for each asset.
     */
    virtual void unloadAll() {}
    

#pragma mark Progress Monitoring
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
    virtual size_t loadCount() const { return 0; }

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
    virtual size_t waitCount() const { return 0; }

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
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates the base loader
     *
     * This constructor is protected, as loaders will use Cocos2d's reference
     * counting system for garbage collection.
     */
    BaseLoader() : Ref(), _active(false) {}

    /**
     * Diposes the base loader, releasing all resources
     *
     * This destructor is protected, as loaders will use Cocos2d's reference
     * counting system for garbage collection.
     */
    virtual ~BaseLoader()   { stop(); }
};


#pragma mark -
#pragma mark Templated Middle Layer

/**
 * Type specific template for each loader.
 * 
 * This template works like a generic abstract class in Java.  I provides some
 * type correctness.  It also provides some base functionality that is common
 * for all loaders.  Methods marked as abstract must be overriden in specific
 * loader implementations.
 *
 * All assets are assigned a key and retrieved via that key.  This for a quick
 * way to reference assets.
 */
template <class T>
class Loader : public BaseLoader {
protected:
    /** Hash map storing the loaded assets */
    std::unordered_map<std::string, T*> _assets;

public:
#pragma mark Asset Access
    /**
     * Returns true if the key maps to a loaded asset.
     *
     * This method is useful in case the asset fails to load.
     *
     * @param  key  the key associated with the asset
     *
     * @return True if the key maps to a loaded asset.
     */
    virtual bool contains(std::string key) const { return _assets.find(key) != _assets.end(); }

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
    virtual T* get(std::string key) const {
        auto it = _assets.find(key);
        return (it == _assets.end() ? nullptr : it->second);
    }

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
    T* operator[](std::string key) const { return get(key); }
    

#pragma mark Asset Loading
    /**
     * Returns the number of assets currently loaded.
     *
     * This is a rough way to determine how many assets have been loaded
     * so far. This method counts each asset equally regardless of the memory
     * requirements of each asset.
     *
     * @return the number of assets currently loaded.
     */
    virtual size_t loadCount() const override { return _assets.size(); }

    /**
     * Loads an asset and assigns it to the given key.
     *
     * The asset will be loaded synchronously. It will be available immediately.
     * This method should be limited to those times in which an asset is 
     * really necessary immediately, such as for a loading screen.
     *
     * This method is abstract and should be overridden in the specific
     * implementation for each asset.
     *
     * @param  key      The key to access the asset after loading
     * @param  source   The pathname to the asset
     *
     * @return the loaded asset
     */
    virtual T* load(std::string key, std::string source) { return nullptr; }
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates the loader template
     *
     * This constructor is protected, as loaders will use Cocos2d's reference
     * counting system for garbage collection.
     */
    Loader() : BaseLoader() {}
};

NS_CC_END

#endif /* defined(__CU_LOADER_H__) */
