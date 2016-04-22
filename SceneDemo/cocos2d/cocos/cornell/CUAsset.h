//
//  CUAsset.h
//  Cornell Extensions to Cocos2D
//
//  This module provides an abstract class for generic assets.  It is to help with any asset (such
//  (such as a model file or level layout) not explicitly included in the existing asset classes.
//  It has methods for loading and unloading from a file, which any subclass should implement.
//
//  IMPORTANT: In order to work properly, the subclass must have the static constructors create()
//  and create(file) defined.  As with most classes in Cocos2d, you should not allow direct
//  access to the constructors.
//
//  Author: Walker White
//  Version: 1/19/16
//
#ifndef __CU_ASSET_H__
#define __CU_ASSET_H__
#include <base/CCRef.h>
#include <platform/CCFileUtils.h>

NS_CC_BEGIN

#pragma mark -
#pragma mark Generic Asset

/**
 * An abstract class for a generic asset
 *
 * This class is the base class for any generic asset (such as a model file or level layout) 
 * not explicitly included in the existing asset classes. It has abstract methods for loading
 * and unloading from a file, which any subclass should implement.
 *
 * This class SHOULD NOT make any references to AssetManager in the load/unload methods. Assets
 * should be treated as if they load in parallel, not in sequence.  Therefore, it is unsafe to
 * assume that one asset loads before another.  If this asset needs to connect to other assets
 * (sound, images, etc.) this should take place after asset loading, such as during scene graph
 * initialization or the like.
 */
class CC_DLL Asset : public Ref {
protected:
    /** The source file associated with this asset */
    std::string _file;

public:
    /**
     * Returns the source file associate with this asset.
     *
     * @return the source file associate with this asset.
     */
    std::string getFile() const { return _file; }
    
    /**
     * Sets the source file associate with this asset.
     *
     * This method does not reload the asset, but later calls to load will use this
     * file in place of any previous one.
     *
     * @param  file the source file associate with this asset.
     */
    void setFile(std::string file) { _file = file; }

    /**
     * Returns true after successfully loading from a file.
     *
     * This method is abstract.  All subclasses should provide an implementation of 
     * this method.  This method is necessary for an Asset to be used with an instance
     * of GenericLoader.
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded in
     * parallel, not in sequence.  If an asset (like a game level) has references to
     * other assets, then these should be connected later, such as during scene 
     * graph initialization.
     *
     * @return true if successfully loaded the asset from a file
     */
    virtual bool load() = 0;
    
    /**
     * Unloads all resources attached to this file.
     *
     * This method is abstract.  All subclasses should provide an implementation of
     * this method.  This method is necessary for an Asset to be used with an instance
     * of GenericLoader.
     *
     * This load method should NEVER access the AssetManager.  Assets are loaded and
     * unloaded in parallel, not in sequence.  If an asset (like a game level) has 
     * references to other assets, then these should be disconnected earlier.
     */
    virtual void unload() {}

    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates an empty asset with no source file
     */
    Asset() : Ref() { _file = ""; }
    
    /**
     * Deletes this asset, disposing all resources.
     */
    virtual ~Asset() { unload(); }
    
    /**
     * Initializes a new asset with no source file.
     *
     * The source file can be set at any time via the setFile() method. This method
     * does NOT load the asset.  You must call the load() method to do that.
     *
     * @return  true if the asset is initialized properly, false otherwise.
     */
    bool init() { _file = ""; return true; }
    
    /**
     * Initializes a new asset with the given source file.
     *
     * This method does NOT load the asset. You must call the load() method to do that.
     * This method returns false if file does not exist.
     *
     * @return  true if the asset is initialized properly, false otherwise.
     */
    bool init(std::string file) {
        _file = FileUtils::getInstance()->fullPathForFilename(file);
        return _file != "";
    }
};

NS_CC_END

#endif /* __CU_ASSET_H__ */
