//
//  CURootLayer.h
//  Cornell Extensions to Cocos2D
//
//  A module for the top layer for a Cocos2d Scene.  This class automates a lot of
//  of the start-up requirements for working in Cocos2d.  To create a Cocos2d
//  game, you just need to subclass the class and implement three methods:
//  start, stop, and update.
//
//  This class is based on the Cocos2d class LayerColor, which is used by their
//  HelloWorld application.  However, it's rendering is much more efficient, as
//  it does not inject a custom drawing command into the pipeline.
//
//  Author: Walker White
//  Version: 12/5/15
//
#ifndef __CU_ROOT_LAYER_H__
#define __CU_ROOT_LAYER_H__

#include <cocos2d.h>
#include <renderer/CCTrianglesCommand.h>

NS_CC_BEGIN

#pragma mark -
#pragma mark Static Constructor Template


namespace GameRoot {
    /**
     * Creates a new Scene object with class T as its root layer.
     *
     * The scene object needs to be attached to a root layer.  However, the
     * designer needs to subclass RootLayer in order to define gameplay
     * features. As the createScene() method associated with Layer is typically
     * static, this means we need to define a new method for each subclass.
     * This function uses templates to emulate a static method that works
     * for any subclass.
     *
     * @return an autoreleased scene with class T as its root layer
     */
    template<class T>
    Scene* createScene() {
        T* layer = new (std::nothrow)T();
        if (layer && layer->init()) {
            layer->autorelease();

            // 'scene' is an autorelease object
            auto scene = Scene::create();
            // add layer as a child to scene
            scene->addChild(layer);
            layer->start();
            // return the scene
            return scene;
        }
        CC_SAFE_DELETE(layer);
        return nullptr;
    }

    /**
     * Creates a new Scene object with class T as its root layer.
     *
     * The scene object needs to be attached to a root layer.  However, the
     * designer needs to subclass RootLayer in order to define gameplay
     * features. As the createScene() method associated with Layer is typically
     * static, this means we need to define a new method for each subclass.
     * This function uses templates to emulate a static method that works
     * for any subclass.
     *
     * @param  size the size of the layer
     *
     * @return an autoreleased scene with class T as its root layer
     */
    template<class T>
    Scene* createScene(const Size& size) {
        T* layer = new (std::nothrow)T();
        if (layer && layer->init(size)) {
            layer->autorelease();
            
            // 'scene' is an autorelease object
            auto scene = Scene::create();
            // add layer as a child to scene
            scene->addChild(layer);
            layer->start();
            // return the scene
            return scene;
        }
        CC_SAFE_DELETE(layer);
        return nullptr;
    }
}

#pragma mark -
#pragma mark Root Layer
/**
 * The top layer for the game scene graph.
 */
class CC_DLL RootLayer: public Layer, public BlendProtocol {
    
#pragma mark Internal Helpers
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CC_DISALLOW_COPY_AND_ASSIGN(RootLayer);
    
protected:
    /** Texture to be applied to the polygon */
    Texture2D*       _texture;
    /** Blending function is required to implement BlendProtocol */
    BlendFunc        _blendFunc;
    
    /** The command for the Cocos2D rendering pipeline */
    TrianglesCommand _command;
    /** Rendering data representing a solid shape */
    TrianglesCommand::Triangles _triangles;
    
    /** Whether or not this layer is currently active (e.g. handling updates) */
    bool _active;
    
    /**
     * Clears the render data, releasing all vertices and indices.
     */
    void clearRenderData();

    /**
     * Updates the color for each vertices to match the node settings.
     */
    void updateColor(void) override;

    /**
     * Update the blend options for this node to use in a render pass
     */
    void updateBlendFunc(void);
    

public:
#pragma mark Attribute Accessors
    /**
     * Sets the untransformed size of the layer.
     *
     * The contentSize remains the same no matter how the layer is scaled or rotated.
     * Typically this size is the same size as the screen.
     *
     * @param  contentSize  The untransformed size of the layer.
     */
    virtual void setContentSize(const Size & size) override;

    /**
     * Sets the blend function to the one specified
     *
     * This is part of BlendProtocol.
     *
     * @param blendFunc the new blend function.
     */
    void setBlendFunc(const BlendFunc &blendFunc) override { _blendFunc = blendFunc; }
    
    /**
     * Returns the current blend function for this node
     *
     * This is part of BlendProtocol.
     *
     * @return the node blend function.
     */
    const BlendFunc& getBlendFunc() const override { return _blendFunc; }
    
    /**
     * Sends drawing commands to the renderer
     *
     * This method is overridden from Node, to draw the background node.  It draws
     * using the same commands as PolygonNode, making it more efficient than the
     * built-in LayerColor.
     *
     * @param renderer   Reference to the render thread
     * @param transform  The accumulated transform from the parent
     * @param flags      Specialized Cocos2d drawing flags
     */
    virtual void draw(Renderer *renderer, const Mat4& transform, uint32_t flags) override;
    
    /**
     * Stops all running actions and schedulers
     */
    virtual void cleanup() override;
    
    
#pragma mark Primary Designer Methods
    /**
     * Returns true if this layer is active
     *
     * An active layer is regularly receiving calls to update.
     *
     * @return true if this layer is active
     */
    bool isActive() const { return _active; }

    /**
     * Starts the layer, allocating initial resources
     *
     * This method is used to bootstrap the game.  It should start up an asset manager
     * and load initial assets.
     */
    virtual void start() {
        _active = true;
        scheduleUpdate();
    }

    /**
     * Stops the layer, releasing all resources
     *
     * This method is used to clean-up any allocation that occurred in either start or
     * update.  While Cocos2d does have rudimentary garbage collection, you still have
     * to release any objects that you have retained.
     */
    virtual void stop() {
        unscheduleUpdate();
        _active = false;
    }

    /**
     * Primary update method called every animation frame
     *
     * This method is called every animation frame.  There is no draw() or render()
     * counterpoint to this method; drawing is done automatically in the scene graph.
     * However, this method is responsible for updating any transforms in the scene graph.
     *
     * @param  dt   the time in seconds since last update
     */
    virtual void update(float dt) override {
        CCASSERT(_active, "Update called on inactive root layer");
        Node::update(dt);
    }
    
    /**
     * Shutsdown the layer, forcing an exit from the application.
     *
     * This is the method to call to cleanly quit the game.
     */
    void shutdown();

    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Initializers
    /**
     * Creates an empty layer.
     *
     * This method does not allocate anything, even the background color.  It must
     * be coupled with a call to init.
     */
    RootLayer(void);
    
    /**
     * Releases all resources allocated with this layer.
     */
    virtual ~RootLayer(void);
    
    /**
     * Intializes an root layer to fit the screen dimensions.
     *
     * The layer will use the XNA background color for nostalgic reasons.
     *
     * @return  true if the layer is initialized properly, false otherwise.
     */
    virtual bool init() override;

    /**
     * Intializes an root layer to fit the given dimensions.
     *
     * The layer will use the XNA background color for nostalgic reasons.
     *
     * @param  size the size for the layer
     *
     * @return  true if the layer is initialized properly, false otherwise.
     */
    virtual bool initWithSize(const Size& size);

    /**
     * Intializes an root layer to fit the given color and dimensions.
     *
     * @param  color    the background color
     * @param  size     the size for the layer
     *
     * @return  true if the layer is initialized properly, false otherwise.
     */
    virtual bool initWithColor(const Color4B& color, const Size& size);

    /**
     * Creates a new Scene object with class T as its root layer.
     *
     * The scene object needs to be attached to a root layer.  However, the
     * designer needs to subclass RootLayer in order to define gameplay
     * features. As the createScene() method associated with Layer is typically
     * static, this means we need to define a new method for each subclass.
     * This function uses templates to emulate a static method that works
     * for any subclass.
     *
     * @return an autoreleased scene with class T as its root layer
     */
    template<class T>
    friend Scene* GameRoot::createScene();
    
    /**
     * Creates a new Scene object with class T as its root layer.
     *
     * The scene object needs to be attached to a root layer.  However, the
     * designer needs to subclass RootLayer in order to define gameplay
     * features. As the createScene() method associated with Layer is typically
     * static, this means we need to define a new method for each subclass.
     * This function uses templates to emulate a static method that works
     * for any subclass.
     *
     * @param  size the size of the layer
     *
     * @return an autoreleased scene with class T as its root layer
     */
    template<class T>
    friend Scene* GameRoot::createScene(const Size& size);
};

NS_CC_END

#endif /* defined(__CU_ROOT_LAYER_H__) */
