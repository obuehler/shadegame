//
//  CCPolygonNode.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a scene graph node that supports solid 2D polygons.  These
//  polygons may also be textured by a sprite. This class is preferable to Sprite
//  except in the cases where you use Cocos2d's animation events.
//
//  Author: Walker White
//  Version: 2/21/15
//
#include <algorithm>
#include "CUPolygonNode.h"
#include <renderer/CCRenderer.h>
#include <deprecated/CCString.h>


NS_CC_BEGIN

#pragma mark -
#pragma mark Static Constructors

/**
 * Creates an empty polygon with the degenerate texture.
 *
 * You do not need to set the texture. The polygon, however,
 * will also be empty, and must be set via setPolygon.
 *
 * @return An autoreleased sprite object.
 */
PolygonNode* PolygonNode::create() {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->init()) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a solid polygon with the given vertices.
 *
 * The node will use the degenerate texture, which is solid white.
 * Hence the polygon will have a solid color.
 *
 * The polygon will be triangulated using the rules of Poly2.
 *
 * @param   vertices The vertices to texture (expressed in image space)
 * @param   offset   The offset in vertices
 * @param   size     The number of elements in vertices
 *
 * @return  An autoreleased sprite object
 */
PolygonNode* PolygonNode::create(float* vertices, int size, int offset) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->init(vertices,size,offset)) {
        sprite->_polygon.triangulate();
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a solid polygon given polygon shape.
 *
 * The node will use the degenerate texture, which is solid white.
 * Hence the polygon will have a solid color.
 *
 * @param   poly     The polygon to texture
 *
 * @return  An autoreleased sprite object
 */
PolygonNode* PolygonNode::create(const Poly2& poly) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->init(poly)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a solid polygon with the given rect.
 *
 * The rectangle will be converted into a Poly2.  There is little benefit to
 * using a PolygonNode in this way over a normal Sprite. The option is here
 * only for completion.
 *
 * @param   rect     The rectangle to texture
 *
 * @return  An autoreleased sprite object
 */
PolygonNode* PolygonNode::create(const Rect& rect) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->init(rect)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a textured polygon from the image filename.
 *
 * After creation, the polygon will be a rectangle.  The vertices of this
 * polygon will be the corners of the image.
 *
 * @param   filename A path to image file, e.g., "scene1/earthtile.png"
 *
 * @retain  a reference to the newly loaded texture
 * @return  An autoreleased sprite object.
 */
PolygonNode* PolygonNode::createWithFile(const std::string& filename) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->initWithFile(filename)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a textured polygon from the image filename and the given vertices.
 *
 * The polygon will be triangulated using the rules of Poly2.
 *
 * @param   filename A path to image file, e.g., "scene1/earthtile.png"
 * @param   vertices The vertices to texture (expressed in image space)
 * @param   size     The number of elements in vertices
 * @param   offset   The offset in vertices
 *
 * @retain  a reference to the newly loaded texture
 * @return  An autoreleased sprite object
 */
PolygonNode* PolygonNode::createWithFile(const std::string& filename, float* vertices, int size, int offset) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->initWithFile(filename, vertices, size, offset)) {
        sprite->_polygon.triangulate();
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a textured polygon from the image filename and the given polygon.
 *
 * @param   filename A path to image file, e.g., "scene1/earthtile.png"
 * @param   poly     The polygon to texture
 *
 * @retain  a reference to the newly loaded texture
 * @return  An autoreleased sprite object
 */
PolygonNode* PolygonNode::createWithFile(const std::string& filename, const Poly2& poly) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->initWithFile(filename, poly)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a textured polygon from the image filename and the given rect.
 *
 * The rectangle will be converted into a Poly2.  There is little benefit to
 * using a PolygonNode in this way over a normal Sprite. The option is here
 * only for completion.
 *
 * @param   filename A path to image file, e.g., "scene1/earthtile.png"
 * @param   rect     The rectangle to texture
 *
 * @retain  a reference to the newly loaded texture
 * @return  An autoreleased sprite object
 */
PolygonNode* PolygonNode::createWithFile(const std::string& filename, const Rect& rect) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->initWithFile(filename, rect)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a textured polygon from a Texture2D object.
 *
 * After creation, the polygon will be a rectangle. The vertices of this
 * polygon will be the corners of the texture.
 *
 * @param   texture A pointer to a Texture2D object.
 *
 * @retain  a reference to this texture
 * @return  An autoreleased sprite object
 */
PolygonNode* PolygonNode::createWithTexture(Texture2D *texture) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->initWithTexture(texture)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a textured polygon from a Texture2D object and the given vertices.
 *
 * The polygon will be triangulated using the rules of Poly2.
 *
 * @param   texture    A pointer to an existing Texture2D object.
 *                     You can use a Texture2D object for many sprites.
 * @param   vertices   The vertices to texture (expressed in image space)
 * @param   offset     The offset in vertices
 * @param   size       The number of elements in vertices
 *
 * @retain  a reference to this texture
 * @return  An autoreleased sprite object
 */
PolygonNode* PolygonNode::createWithTexture(Texture2D *texture, float* vertices, int size, int offset) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->initWithTexture(texture, vertices, size, offset)) {
        sprite->_polygon.triangulate();
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a textured polygon from a Texture2D object and the given polygon.
 *
 * @param   texture  A pointer to an existing Texture2D object.
 *                   You can use a Texture2D object for many sprites.
 * @param   poly     The polygon to texture
 *
 * @retain  a reference to this texture
 * @return  An autoreleased sprite object
 */
PolygonNode* PolygonNode::createWithTexture(Texture2D *texture, const Poly2& poly) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->initWithTexture(texture, poly)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

/**
 * Creates a textured polygon from a Texture2D object and the given rect.
 *
 * The rectangle will be converted into a Poly2.  There is little benefit to
 * using a PolygonNode in this way over a normal Sprite. The option is here
 * only for completion.
 *
 * @param   texture  A pointer to an existing Texture2D object.
 *                   You can use a Texture2D object for many sprites.
 * @param   rect     The rectangle to texture
 *
 * @retain  a reference to this texture
 * @return  An autoreleased sprite object
 */
PolygonNode* PolygonNode::createWithTexture(Texture2D *texture, const Rect& rect) {
    PolygonNode *sprite = new (std::nothrow) PolygonNode();
    if (sprite && sprite->initWithTexture(texture, rect)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}


#pragma mark -
#pragma mark Allocator

/**
 * Creates an empty polygon with the degenerate texture.
 *
 * You do not need to set the texture. The polygon, however, will also be
 * empty, and must be set via setPolygon.
 *
 * This constructor should never be called directly. Use one of the static
 * constructors instead.
 */
PolygonNode::PolygonNode(void) : TexturedNode() {
    _name = "PolygonNode";
}


#pragma mark -
#pragma mark Attribute Accessors

/**
 * Returns a string description of this object
 *
 * This method is useful for debugging.
 *
 * @return  a string description of this object
 */
std::string PolygonNode::getDescription() const {
    int texture_id = _texture->getName();
    return StringUtils::format("<PolygonNode | Tag = %d, TextureID = %d>", _tag, texture_id );
}


#pragma mark -
#pragma mark Abstract Methods

/**
 * Sends drawing commands to the renderer
 *
 * This method is overridden from Node, to provide the custom commands.
 * PolygonSprites are drawn to the Triangle mesh (which is different
 * from the Quad mesh used for Sprites) to cut down on the number of
 * drawing calls.
 *
 * @param renderer   Reference to the render thread
 * @param transform  The accumulated transform from the parent
 * @param flags      Specialized Cocos2d drawing flags
 */
void PolygonNode::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
    // Don't do calculate the culling if the transform was not updated
    _insideBounds = (flags & FLAGS_TRANSFORM_DIRTY) ? renderer->checkVisibility(transform, _contentSize) : _insideBounds;
    if(_insideBounds) {
        if (_triangles.vertCount == 0) {
            generateRenderData();
        }
        _command.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, _triangles, transform, flags);
        renderer->addCommand(&_command);
    }
}

/**
 * Allocate the render data necessary to render this node.
 *
 * This method allocates the Triangles data used by the TrianglesCommand
 * in the rendering pipeline.
 */
void PolygonNode::generateRenderData() {
    clearRenderData();
    
    allocTriangles(_polygon);
    updateColor();
    updateTextureCoords();
}

NS_CC_END
