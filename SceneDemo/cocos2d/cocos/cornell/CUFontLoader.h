//
//  CUFontLoader.h
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
#ifndef __CU_FONT_LOADER__
#define __CU_FONT_LOADER__
#include <unordered_set>
#include <mutex>
#include "CULoader.h"
#include "CUTTFont.h"
#include "CUThreadPool.h"


NS_CC_BEGIN

#define DEFAULT_SIZE 12.0f

/**
 * Class is a implementation of Loader<TTFont>
 *
 * This asset loader allows us to allocate font objects from the associated
 * source files. Note that a True Type font asset is both the source file AND 
 * the font size.  Because the font is converted to a texture for usage, 
 * different sizes are different fonts. This loader can be given a default
 * font size, so that all fonts loaded have this size.
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
 * Font objects are uniquely identified by their source file and size.  Attempt 
 * to load a font object a second time, even under a new key, will return a
 * reference to the same font object.
 */
class CC_DLL FontLoader : public Loader<TTFont> {
private:
    /** This macro disables the copy constructor (not allowed on assets) */
    CC_DISALLOW_COPY_AND_ASSIGN(FontLoader);
    
protected:
#pragma mark -
#pragma mark Font Coordinator
    /**
     * Class is a static coordinate to garbage collect assets
     *
     * This coordinate tracks asset usage across all active loaders.  When an asset
     * is not used by any loader, it is removed from memory.
     */
    class Coordinator {
    private:
        /** The font objects allocated for each source file */
        std::unordered_map<std::string,TTFont*> _objects;
        /** The number of active references to each texture. */
        std::unordered_map<std::string, int> _refcnts;
        /** The callback functions registered to a texture for asynchronous loading */
        std::unordered_map<std::string,std::vector<std::function<void(TTFont* s)>>> _callbacks;
        
        /** Thread pool for asynchronous loading */
        ThreadPool* _threads;
        /** Mutex for the asynchronous font loading */
        std::mutex _mutex;
        
    public:
        /** The number of active font loader instances */
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
         * It is possible that there are multiple instances of the loader loading the same font.
         * We only want to load it once.
         *
         * @return true if a source is allocated and loaded.
         */
        bool isLoaded(std::string id)  const { return _objects.find(id) != _objects.end(); }
        
        /**
         * Returns true if a source is pending allocation.
         *
         * It is possible that there are multiple instances of the loader loading the same font.
         * We only want to load it once.
         *
         * @return true if a source is pending allocation.
         */
        bool isPending(std::string id) const { return _callbacks.find(id) != _callbacks.end(); }
        
        
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
        TTFont* load(std::string source, float size);
        
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
        void loadAsync(std::string source, float size, std::function<void(TTFont* s)> callback);
        
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
        TTFont* allocateSync(TTFont* texture);

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
        void allocateAsync(TTFont* texture, FontAtlas* atlas);

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
        void release(TTFont* font);
    };
    
    /** The static coordinator singleton */
    static Coordinator* _gCoordinator;
    
#pragma mark -
#pragma mark Font Loader
    /** The default size */
    float _default;
    
    /** The fonts we are expecting that are not yet loaded */
    std::unordered_set<std::string> _fqueue;
    
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
    void allocate(std::string key, TTFont* font);
    
    
public:
#pragma mark Activation/Deactivation
    /**
     * Creates a new FontLoader.
     *
     * This constructor does not start the font loader.  It simply creates an
     * object for the font loader so that it can be attached to the asset manager.
     * Call start() when you are ready to start using it.
     *
     * @returns an autoreleased FontLoader object
     */
    static FontLoader* create();
    
    /**
     * Starts this font loader.
     *
     * Any assets loaded by this loader will be immediately released by
     * the loader.  However, an asset may still be available if it is
     * attached to another loader. The asset manager is backed by a central
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
     * Returns the number of fonts waiting to load.
     *
     * This is a rough way to determine how many fonts are still pending.
     * A font is pending if it has been loaded asychronously, and the
     * loading process has not yet finished. This method counts each font
     * equally regardless of the memory requirements of the format.
     *
     * @return the number of fonts waiting to load.
     */
    size_t waitCount() const override { return _fqueue.size(); }
    
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
    TTFont* load(std::string key, std::string source) override { return load(key,source,_default); }
    
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
     * @retain the loaded font
     * @return the loaded font
     */
    TTFont* load(std::string key, std::string source, float size);
    
    /**
     * Adds a new font to the loading queue.
     *
     * The font will be loaded asynchronously.  When it is finished loading, it
     * will be added to this loader, and accessible under the given key. This
     * method will mark the loading process as not complete, even if it was
     * completed previously.  It is not safe to access the loaded font until
     * it is complete again.
     *
     * The loaded font will use the default size of this loader.
     *
     * @param  key      The key to access the font after loading
     * @param  source   The pathname to the font file
     *
     * @retain the font upon loading
     */
    void loadAsync(std::string key, std::string source) override { loadAsync(key,source,_default); }
    
    /**
     * Adds a new font to the loading queue.
     *
     * The font will be loaded asynchronously.  When it is finished loading, it
     * will be added to this loader, and accessible under the given key. This
     * method will mark the loading process as not complete, even if it was
     * completed previously.  It is not safe to access the loaded font until
     * it is complete again.
     *
     * @param  key      The key to access the font after loading
     * @param  source   The pathname to the font file
     * @param  size     The font size for this asset
     *
     * @retain the font upon loading
     */
    void loadAsync(std::string key, std::string source, float size);
    
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
    void unload(std::string key) override;
    
    /**
     * Unloads all assets present in this loader.
     *
     * This method simply unloads the fonts for the scene associated with this
     * loader. The fonts will not be deleted or removed from memory until they
     * are removed from all instances of FontLoader.
     *
     * @release all loaded fonts
     */
    void unloadAll() override;
    
    
#pragma mark Default Parameters
    /**
     * Returns the default font size
     * 
     * Any font processed by this loader will have this size unless otherwise specified.
     *
     * @return the default font size
     */
    float getDefaultSize() const    { return _default; }
    
    /**
     * Sets the default font size
     *
     * Any font processed by this loader will have this size unless otherwise specified.
     *
     * @param  size     the default font size
     */
    void setDefaultSize(float size) { _default = size; }
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates a new, uninitialized font loader
     */
    FontLoader() : Loader<TTFont>(), _default(DEFAULT_SIZE) {}
    
    /**
     * Disposes of the font loader.
     *
     * This destructor will stop the font loader if not done so already.
     */
    virtual ~FontLoader() { if (_active) { stop(); } }
    
};

NS_CC_END

#endif /* defined(__CU_FONT_LOADER__) */