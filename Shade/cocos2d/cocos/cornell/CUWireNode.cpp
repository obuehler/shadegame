//
//  CUWireNode.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a scene graph node for wireframes.  Wireframes are lines, not
//  solid shapes.  They have no thickness.  They are primarily useful for debugging.
//
//  This class infinitely better than the horrible DrawNode, which is outside the Cocos2d
//  render pipeline, and hence incredibly slow.  You should avoid that class whenever
//  possible.
//
//  Author: Walker White
//  Version: 11/15/15
//
#include <algorithm>
#include "CUWireNode.h"
#include <renderer/CCTextureCache.h>
#include <renderer/CCRenderer.h>
#include <deprecated/CCString.h>


NS_CC_BEGIN

#pragma mark -
#pragma mark Static Constructors

/**
 * Creates an empty wireframe node.
 *
 * The underlying polygon is empty, and must be set via setPolygon.
 *
 * @return An autoreleased wireframe node
 */
WireNode* WireNode::create() {
    WireNode *node = new (std::nothrow) WireNode();
    if (node && node->init()) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

/**
 * Creates a wireframe with the given vertices.
 *
 * The traversal path will be open.  To create a different traversal, use the
 * createWithPoly() method.
 *
 * @param   vertices  The vertices forming the wireframe path
 * @param   size      The number of elements in vertices
 * @param   offset    The offset in vertices
 *
 * @return  An autoreleased wireframe node
 */
WireNode* WireNode::createWithVertices(float* vertices, int size, int offset) {
    WireNode *node = new (std::nothrow) WireNode();
    if (node && node->init(vertices,size,offset)) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

/**
 * Creates a wireframe with the given polygon.
 *
 * The wireframe is determined by the polygon traversal.  There are three options.
 *
 * The wireframe is a sequence of lines that is determined by the polygon indices.
 * To create a specific traversal, call the traverse() method in Poly2 before
 * assigning it to this Wireframe.
 *
 * @param   poly      The polygon for the wireframe path
 *
 * @return  An autoreleased wireframe node
 */
WireNode* WireNode::createWithPoly(const Poly2& poly) {
    WireNode *node = new (std::nothrow) WireNode();
    if (node && node->init(poly)) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

/**
 * Creates a wireframe with the given rect.
 *
 * The rectangle will be converted into a Poly2, and the traversal is closed.
 *
 * @param   rect    The rectangle for the wireframe path
 *
 * @return  An autoreleased wireframe node
 */
WireNode* WireNode::createWithRect(const Rect& rect) {
    WireNode *node = new (std::nothrow) WireNode();
    if (node && node->init(rect)) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

/**
 * Creates a wireframe that is a line from origin to destination.
 *
 * @param   origin  The line origin.
 * @param   dest    The line destination.
 *
 * @return  An autoreleased wireframe node
 */
WireNode* WireNode::createWithLine(const Vec2 &origin, const Vec2 &dest) {
    WireNode *node = new (std::nothrow) WireNode();
    if (!node) {
        CC_SAFE_DELETE(node);
        return nullptr;
    }
    
    Poly2 poly;
    poly.setLine(origin, dest);
    if (node && node->init(poly)) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

/**
 * Creates a wireframe that is an ellipse with given the center and dimensions.
 *
 * The wireframe will show the boundary, not the circle tesselation.
 *
 * @param   center      The ellipse center point.
 * @param   size        The size of the ellipse.
 * @param   segments    The number of segments to use.
 *
 * @return  An autoreleased wireframe node
 */
WireNode* WireNode::createWithEllipse(const Vec2& center, const Size& size, unsigned int segments) {
    WireNode *node = new (std::nothrow) WireNode();
    if (!node) {
        CC_SAFE_DELETE(node);
        return nullptr;
    }
    
    Poly2 poly;
    poly.setEllipse(center,size,segments);
    if (node && node->init(poly)) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


#pragma mark -
#pragma mark Allocator

/**
 * Creates an empty wireframe without a polygon.
 *
 * The underlying polygon is empty, and must be set via setPolygon.
 */
WireNode::WireNode() : TexturedNode() {
    _command.setType(RenderCommand::Type::WIREFRAME_COMMAND);
    _name = "WireNode";
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
std::string WireNode::getDescription() const {
    return StringUtils::format("<WireNode | Tag = %d>", _tag );
}




#pragma mark -
#pragma mark Abstract Methods

/**
 * Sends drawing commands to the renderer
 *
 * This method is overridden from Node, to provide the custom commands.
 * Wireframes are drawn to the Wireframe mesh (which is different
 * from the Triangle mesh used for solid shapes) to cut down on the number of
 * drawing calls.
 *
 * @param renderer   Reference to the render thread
 * @param transform  The accumulated transform from the parent
 * @param flags      Specialized Cocos2d drawing flags
 */
void WireNode::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
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
 * The render data for this node is a wireframe for the given
 * traversal.
 */
void WireNode::generateRenderData() {
    clearRenderData();

    allocTriangles(_polygon);
    updateColor();
    updateTextureCoords();
}

NS_CC_END
