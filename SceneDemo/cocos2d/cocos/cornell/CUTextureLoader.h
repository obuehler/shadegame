//
//  CUTextureLoader.h
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
#ifndef __CU_TEXTURE_LOADER__
#define __CU_TEXTURE_LOADER__
#include <unordered_set>
#include <renderer/CCTexture2D.h>
#include "CULoader.h"

NS_CC_BEGIN

/**
 * Class is a implementation of Loader<Texture2D>
 *
 * This asset loader allows us to allocate texture objects from the associated
 * image files. Technically, a texture should be identified by both its source
 * file and its texture parameters.  However, Cocos2D does not allow you to
 * to do that -- each texture can be only loaded once. Refactoring this would
 * be a lot more work than we are comfortable with.  Hence changeing the texture
 * parameters for any texture object will change it across all references.
 *
 * As with all of our loaders, this loader is designed to be attached to a scene.
 * This is the natural was to do things, as Cocos2d is scene based.  However,
 * its asset loading is typically done through the director, which is global.
 * This makes is hard to determine when it is safe to unload an asset.  Even
 * though the current scene many not need it, it may be used by another active
 * scene. Unloading the asset would corrupt that scene.
 *
 * This loader solves this problem by have a static coordinator behind the
 * scenes.  This coordinate is shared across all loader instances.  It decides
 * When an asset is truly ready to be unloaded.  We assume that all instances
 * are run only in the Director thread.
 *
 * Texture objects are uniquely identified by their image file.  Attempt to
 * load a image file a second time, even under a new key, will return a
 * reference to the same texture object, even if different parameters are used.
 */
class CC_DLL TextureLoader : public Loader<Texture2D> {
private:
    /** This macro disables the copy constructor (not allowed on assets) */
    CC_DISALLOW_COPY_AND_ASSIGN(TextureLoader);
    
protected:
#pragma mark -
#pragma mark Texture Coordinator
    /**
     * Class is a static coordinate to garbage collect assets
     *
     * This coordinate tracks asset usage across all active loaders.  When an asset
     * is not used by any loader, it is removed from memory.
     */
    class Coordinator {
    private:
        /** The image file for each texture ID */
        std::unordered_map<GLuint,std::string> _sources;
        /** The texture objects allocated for each source file */
        std::unordered_map<std::string,Texture2D*> _objects;
        /** The number of active references to each texture. */
        std::unordered_map<std::string, int>   _refcnts;
        /** The callback functions registered to a texture for asynchronous loading */
        std::unordered_map<std::string,std::vector<std::function<void(Texture2D* s)>>> _callbacks;
        
    public:
        /** The number of active texture loader instances */
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
         * It is possible that there are multiple instances of the loader loading the same texture.
         * We only want to load it once.
         *
         * @return true if a source is allocated and loaded.
         */
        bool isLoaded(std::string source)  const { return _objects.find(source) != _objects.end(); }
        
        /**
         * Returns true if a source is pending allocation.
         *
         * It is possible that there are multiple instances of the loader loading the same texture.
         * We only want to load it once.
         *
         * @return true if a source is pending allocation.
         */
        bool isPending(std::string source) const { return _callbacks.find(source) != _callbacks.end(); }
        
        
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
        Texture2D* load(std::string source);
        
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
        void loadAsync(std::string source, std::function<void(Texture2D* s)> callback);
        
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
        void allocate(Texture2D* texture, std::string source);
        
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
        void release(Texture2D* texture);
    };
    
    /** The static coordinator singleton */
    static Coordinator* _gCoordinator;
    
    
#pragma mark -
#pragma mark Texture Loader
    /** The default texture parameters */
    Texture2D::TexParams _default;
    
    /** The textures we are expecting that are not yet loaded */
    std::unordered_set<std::string> _tqueue;
    
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
    void allocate(std::string key, Texture2D* texture, const Texture2D::TexParams& params);
    
    
public:
#pragma mark Activation/Deactivation
    /**
     * Creates a new TextureLoader.
     *
     * This constructor does not start the texture loader.  It simply creates an
     * object for the texture loader so that it can be attached to the asset manager.
     * Call start() when you are ready to start using it.
     *
     * @returns an autoreleased TextureLoader object
     */
    static TextureLoader* create();
    
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
     * Returns the number of textures waiting to load.
     *
     * This is a rough way to determine how many texture are still pending.
     * A texture is pending if it has been loaded asychronously, and the
     * loading process has not yet finished. This method counts each texture
     * equally regardless of the memory requirements of the format.
     *
     * @return the number of textures waiting to load.
     */
    size_t waitCount() const override { return _tqueue.size(); }
    
    /**
     * Loads a texture and assigns it to the given key.
     *
     * The texture will be loaded synchronously. It will be available immediately.
     * This method should be limited to those times in which a texture is really
     * necessary immediately, such as for a loading screen.
     *
     * The texture will be initialized with the default texture parameters.
     *
     * @param  key      The key to access the texture after loading
     * @param  source   The pathname to the texture image file
     *
     * @retain the loaded texture
     * @return the loaded texture
     */
    Texture2D* load(std::string key, std::string source) override { return load(key,source,_default); }
    
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
    Texture2D* load(std::string key, std::string source, const Texture2D::TexParams& params);
    
    /**
     * Adds a new texture to the loading queue.
     *
     * The texture will be loaded asynchronously.  When it is finished loading, it
     * will be added to this loader, and accessible under the given key. This
     * method will mark the loading process as not complete, even if it was
     * completed previously.  It is not safe to access the loaded texture until
     * it is complete again.
     *
     * The texture will be initialized with the default texture parameters.
     *
     * @param  key      The key to access the texture after loading
     * @param  source   The pathname to the texture image file
     *
     * @retain the texture upon loading
     */
    void loadAsync(std::string key, std::string source) override { loadAsync(key,source,_default); }

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
    void loadAsync(std::string key, std::string source, const Texture2D::TexParams& params);

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
    void unload(std::string key) override;
    
    /**
     * Unloads all assets present in this loader.
     *
     * This method simply unloads the textures for the scene associated with this
     * loader. The textures will not be deleted or removed from memory until they
     * are removed from all instances of TextureLoader.
     *
     * @release all loaded textures
     */
    void unloadAll() override;
    
    
#pragma mark Defaults
    /**
     * Returns the default texture parameters
     *
     * Any font processed by this loader will use these settings unless otherwise specified.
     *
     * @return the default texture parameters
     */
    const Texture2D::TexParams& getDefaultParameters() const        { return _default; }
    
    /**
     * Sets the default texture parameters
     *
     * Any font processed by this loader will use these settings unless otherwise specified.
     *
     * @param  params   the default texture parameters
     */
    void setDefaultParameters(const Texture2D::TexParams& params)   { _default = params; }
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates a new, uninitialized texture loader
     */
    TextureLoader() : Loader<Texture2D>() {}
    
    /**
     * Disposes of the texture loader.
     *
     * This destructor will stop the texture loader if not done so already.
     */
    virtual ~TextureLoader() { if (_active) { stop(); } }
};

NS_CC_END

#endif /* defined(__CU_TEXTURE_LOADER__) */