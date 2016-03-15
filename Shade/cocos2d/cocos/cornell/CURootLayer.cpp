//
//  CURootLayer.cpp
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
#include "CURootLayer.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark Copied from CCSprite

#define RENDER_IN_SUBPIXEL(__ARGS__) (ceil(__ARGS__))

/**
 * This array is the data of a white image with 2 by 2 dimension.
 * It's used for creating a default texture when the texture is a nullptr.
 */
static unsigned char cc_2x2_white_image[] = {
    // RGBA8888
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};

#define CC_2x2_WHITE_IMAGE_KEY  "/cc_2x2_white_image"


#pragma mark -
#pragma mark Initializers

/**
 * Creates an empty layer.
 *
 * This method does not allocate anything, even the background color.  It must
 * be coupled with a call to init.
 */
RootLayer::RootLayer() :
_texture(nullptr),
_blendFunc(BlendFunc::ALPHA_PREMULTIPLIED) {
    _name = "RootLayer";
    _triangles.vertCount = 0;
    _triangles.verts = nullptr;
    _triangles.indexCount = 0;
    _triangles.indices = nullptr;
}

/**
 * Intializes an root layer to fit the screen dimensions.
 *
 * The layer will use the XNA background color for nostalgic reasons.
 *
 * @return  true if the layer is initialized properly, false otherwise.
 */
bool RootLayer::init() {
    // Throw in XNA coloring for nostalgia
    Size s = Director::getInstance()->getWinSize();
    return initWithColor(Color4B(100, 149, 237, 255), s);
}

/**
 * Intializes an root layer to fit the given dimensions.
 *
 * The layer will use the XNA background color for nostalgic reasons.
 *
 * @param  size the size for the layer
 *
 * @return  true if the layer is initialized properly, false otherwise.
 */
bool RootLayer::initWithSize(const Size& size) {
    // Throw in XNA coloring for nostalgia
    return initWithColor(Color4B(100, 149, 237, 255), size);
}

/**
 * Intializes an root layer to fit the given color and dimensions.
 *
 * @param  color    the background color
 * @param  size     the size for the layer
 *
 * @return  true if the layer is initialized properly, false otherwise.
 */
bool RootLayer::initWithColor(const Color4B& color, const Size& size) {
    if (Layer::init()) {
        // default blend function
        _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
        _displayedColor.r = _realColor.r = color.r;
        _displayedColor.g = _realColor.g = color.g;
        _displayedColor.b = _realColor.b = color.b;
        _displayedOpacity = _realOpacity = color.a;
        
        // Gets the texture by key firstly.
        _texture = Director::getInstance()->getTextureCache()->getTextureForKey(CC_2x2_WHITE_IMAGE_KEY);
            
        // If texture wasn't in cache, create it from RAW data.
        if (_texture == nullptr) {
            Image* image = new (std::nothrow) Image();
            bool isOK = image->initWithRawData(cc_2x2_white_image, sizeof(cc_2x2_white_image), 2, 2, 8);
            CC_UNUSED_PARAM(isOK);
            CCASSERT(isOK, "The 2x2 empty texture was created unsuccessfully.");
            _texture = Director::getInstance()->getTextureCache()->addImage(image, CC_2x2_WHITE_IMAGE_KEY);
            CC_SAFE_RELEASE(image);
        }

        _triangles.vertCount = 4;
        _triangles.verts  = new V3F_C4B_T2F[_triangles.vertCount];

        _triangles.indexCount = 6;
        _triangles.indices = new unsigned short[6];
        _triangles.indices[0] = 0;
        _triangles.indices[1] = 1;
        _triangles.indices[2] = 2;
        _triangles.indices[3] = 2;
        _triangles.indices[4] = 1;
        _triangles.indices[5] = 3;
        
        setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(
                            GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP)
                          );
        updateColor();
        setContentSize(size);
        
        // We need to gain ownership of the texture
        if (_texture) {
            CC_SAFE_RETAIN(_texture);
        }
        return true;
    }
    return false;
}

/**
 * Releases all resources allocated with this layer.
 */
RootLayer::~RootLayer() {
    CC_SAFE_RELEASE(_texture);
    clearRenderData();
}


#pragma mark -
#pragma mark Attribute Accessors
/**
 * Sets the untransformed size of the layer.
 *
 * The contentSize remains the same no matter how the layer is scaled or rotated.
 * Typically this size is the same size as the screen.
 *
 * @param  contentSize  The untransformed size of the layer.
 */
void RootLayer::setContentSize(const Size & size) {
    if (_triangles.verts == nullptr) {
        return;
    }
    
    _triangles.verts[1].vertices.x = size.width;
    _triangles.verts[2].vertices.y = size.height;
    _triangles.verts[3].vertices.x = size.width;
    _triangles.verts[3].vertices.y = size.height;
    Layer::setContentSize(size);
}

#pragma mark -
#pragma mark RGBA Protocol

/**
 * Updates the color for each vertices to match the node settings.
 */
void RootLayer::updateColor(void) {
    Color4B color4(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity );

    for(int ii = 0; ii < _triangles.vertCount; ii++) {
        _triangles.verts[ii].colors = color4;
    }
}

/**
 * Update the blend options for this node to use in a render pass
 */
void RootLayer::updateBlendFunc(void) {
    // it is possible to have an untextured sprite
    if (! _texture || ! _texture->hasPremultipliedAlpha()) {
        _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
        setOpacityModifyRGB(false);
    } else {
        _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
        setOpacityModifyRGB(true);
    }
}

/**
 * Stops all running actions and schedulers
 */
void RootLayer::cleanup() {
    if (_active) {
        stop();
    }
    Node::cleanup();
}

/**
 * Shutsdown the layer, forcing an exit from the application.
 *
 * This is the method to call to cleanly quit the game.
 */
void RootLayer::shutdown() {
    if (_active) {
        stop();
    }
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


#pragma mark -
#pragma mark Rendering methods

/**
 * Clears the render data, releasing all vertices and indices.
 */
void RootLayer::clearRenderData() {
    if (_triangles.verts != nullptr) {
        delete[] _triangles.verts;
    }
    _triangles.vertCount = 0;
    if (_triangles.indices != nullptr) {
        delete[] _triangles.indices;
    }
    _triangles.indexCount = 0;
}

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
void RootLayer::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
    _command.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, _triangles, transform, flags);
    renderer->addCommand(&_command);
}

NS_CC_END