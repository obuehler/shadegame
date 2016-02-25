//
//  CUSoundLoader.h
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
#ifndef __CU_SOUND_LOADER__
#define __CU_SOUND_LOADER__
#include <unordered_set>
#include "CUSound.h"
#include "CULoader.h"

NS_CC_BEGIN

/**
 * Class is a implementation of Loader<Sound>
 *
 * This asset loader allows us to allocate sound objects from the associated
 * sound files.  Even though Android does not allow preloading of sounds, 
 * this loader gives a platform-indepedent way of managing sound assets.
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
 * Sound objects are uniquely identified by their source file.  Attempt to 
 * load a sound file a second time, even under a new key, will return a 
 * reference to the same sound object.
 */
class CC_DLL SoundLoader : public Loader<Sound> {
private:
    /** This macro disables the copy constructor (not allowed on assets) */
    CC_DISALLOW_COPY_AND_ASSIGN(SoundLoader);
    
protected:
#pragma mark -
#pragma mark Sound Coordinator
    /**
     * Class is a static coordinate to garbage collect assets
     *
     * This coordinate tracks asset usage across all active loaders.  When an asset
     * is not used by any loader, it is removed from memory.
     */
    class Coordinator {
    private:
        /** The sound objects allocated for each source file */
        std::unordered_map<std::string,Sound*> _sources;
        /** The number of active references to each sound file. */
        std::unordered_map<std::string,int>    _refcnts;
        /** The callback functions registered to a file for asynchronous loading */
        std::unordered_map<std::string,std::vector<std::function<void(Sound* s)>>> _callbacks;
    public:
        /** The number of active sound loader instances */
        size_t instances;
        
        /**
         * Creates a new static coordinator 
         * 
         * The static coordinator is ready to go.  There is no start method.
         */
        Coordinator();
        
        /** 
         * Destroys new static coordinator, releasing all resources 
         *
         * This will immediately orphan all loader instances and should not be called explicitly.
         */
        ~Coordinator();

        /**
         * Returns true if a source is allocated and loaded.
         *
         * It is possible that there are multiple instances of the loader loading the same sound.
         * We only want to load it once.
         *
         * @return true if a source is allocated and loaded.
         */
        bool isLoaded(std::string source)  const { return _sources.find(source) != _sources.end(); }

        /**
         * Returns true if a source is pending allocation.
         *
         * It is possible that there are multiple instances of the loader loading the same sound.
         * We only want to load it once.
         *
         * @return true if a source is pending allocation.
         */
        bool isPending(std::string source)  const { return _callbacks.find(source) != _callbacks.end(); }

        
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
        Sound* load(std::string source);
        
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
        void loadAsync(std::string source, std::function<void(Sound* s)> callback);

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
        void allocate(std::string source, bool success, bool preload = true);
        
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
        void release(Sound* sound);
    };
    
    /** The static coordinator singleton */
    static Coordinator* _gCoordinator;
    
#pragma mark -
#pragma mark Sound Loader
    /** The sounds we are expecting that are not yet loaded */
    std::unordered_set<std::string> _squeue;

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
    void allocate(std::string key, Sound* sound);

    
public:
#pragma mark Activation/Deactivation
    /**
     * Creates a new SoundLoader.
     * 
     * This constructor does not start the sound loader.  It simply creates an
     * object for the sound loader so that it can be attached to the asset manager.
     * Call start() when you are ready to start using it.
     *
     * @returns an autoreleased SoundLoader object
     */
    static SoundLoader* create();

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
     * attached to another loader. The asset manager is backed by a central
     * coordinator that allows the sharing of assets.
     *
     * Once the loader is stopped, any attempts to load a new asset will
     * fail.  You must call start() to begin loading assets again.
     */
    void stop() override;
    
    
#pragma mark Loading/Unloading
    /**
     * Returns the number of sounds waiting to load.
     *
     * This is a rough way to determine how many sounds are still pending.
     * A sound is pending if it has been loaded asychronously, and the
     * loading process has not yet finished. This method counts each sound
     * equally regardless of the memory requirements of the format.
     *
     * @return the number of sounds waiting to load.
     */
    size_t waitCount() const override { return _squeue.size(); }

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
    Sound* load(std::string key, std::string source) override;
    
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
    void loadAsync(std::string key, std::string source) override;
    
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
    void unload(std::string key) override;
    
    /**
     * Unloads all assets present in this loader.
     *
     * This method simply unloads the sounds for the scene associated with this
     * loader. The sounds will not be deleted or removed from memory until they
     * are removed from all instances of SoundLoader.
     *
     * @release all loaded sounds
     */
    void unloadAll() override;

    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates a new, uninitialized sound loader
     */
    SoundLoader() : Loader<Sound>() {}

    /**
     * Disposes of the sound loader.
     *
     * This destructor will stop the sound loader if not done so already.
     */
    virtual ~SoundLoader() { if (_active) { stop(); } }

};

NS_CC_END

#endif /* defined(__CU_SOUND_LOADER__) */
