//
//  CUPathNode.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides a scene graph node for paths and polygon boundaries. It is
//  similar to a wireframe, except that the lines have width. Because of how the
//  lines are extruded, a path cannot make an interior traversal (like a wireframe).
//  The only path options are open or closed.
//
//  This class infinitely better than the horrible DrawNode, which is outside the Cocos2d
//  render pipeline, and hence incredibly slow.  You should avoid that class whenever
//  possible.
//
//  Author: Walker White
//  Version: 11/15/15
//
#include <algorithm>
#include "CUPathNode.h"
#include <renderer/CCTextureCache.h>
#include <renderer/CCRenderer.h>
#include <deprecated/CCString.h>


NS_CC_BEGIN

#pragma mark -
#pragma mark Static Constructors

/**
* Creates an empty path node.
*
* The underlying polygon is empty, and must be set via setPolygon.
*
* @return An autoreleased path node
*/
PathNode* PathNode::create() {
	PathNode *node = new (std::nothrow) PathNode(1.0f);
	if (node && node->init()) {
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
}

/**
* Creates a path with the given vertices and stroke width.
*
* The path traverses the vertices in order, regardless of any
* tesselation. The path is closed by default.
*
* @param   vertices  The vertices forming the wireframe path
* @param   size      The number of elements in vertices
* @param   stroke    The width of the path
* @param   closed    Whether or not the polygon path is closed.
*
* @return  An autoreleased path node
*/
PathNode* PathNode::createWithVertices(float* vertices, int size, float stroke, bool closed) {
	PathNode *node = new (std::nothrow) PathNode(stroke, closed);
	if (node && node->init(vertices, size, 0)) {
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
}

/**
* Creates a path with the given polygon and stroke width.
*
* The path traverses the vertices of the polygon in order, regardless of any
* tesselation (e.g. the indices are ignored). The path is closed by default.
*
* @param   poly      The polygon for the wireframe path
* @param   stroke    The width of the path
* @param   closed    Whether or not the polygon path is closed.
*
* @return  An autoreleased path node
*/
PathNode* PathNode::createWithPoly(const Poly2& poly, float stroke, bool closed) {
	PathNode *node = new (std::nothrow) PathNode(stroke, closed);
	if (node && node->init(poly)) {
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;

}

/**
* Creates a path with the given rect and stroke width.
*
* The path will traverse the corners of the rectangle, and the traversal is closed.
*
* @param   rect      The rectangle for the wireframe path
* @param   stroke    The width of the path
*
* @return  An autoreleased path node
*/
PathNode* PathNode::createWithRect(const Rect& rect, float stroke) {
	PathNode *node = new (std::nothrow) PathNode(stroke);
	if (node && node->init(rect)) {
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
}

/**
* Creates a path that is a line from origin to destination.
*
* @param   origin  The line origin
* @param   dest    The line destination
* @param   stroke  The width of the path
*
* @return  An autoreleased path node
*/
PathNode* PathNode::createWithLine(const Vec2 &origin, const Vec2 &dest, float stroke) {
<<<<<<< HEAD
	PathNode *node = new (std::nothrow) PathNode(stroke, false);
	if (!node) {
		CC_SAFE_DELETE(node);
		return nullptr;
	}

	GLfloat verts[4];
	verts[0] = origin.x; verts[1] = origin.y;
	verts[2] = dest.x;   verts[3] = dest.y;
	node->_polygon.set(verts, 4, 0);

	if (node->init(node->_polygon)) {
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
=======
    PathNode *node = new (std::nothrow) PathNode(stroke,false);
    if (!node) {
        CC_SAFE_DELETE(node);
        return nullptr;
    }
    
    GLfloat verts[4];
    verts[0] = origin.x; verts[1] = origin.y;
    verts[2] = dest.x;   verts[3] = dest.y;
    node->_polygon.set(verts,4,0);
    
    if (node->init(node->_polygon)) {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
>>>>>>> 136cf0348962ea43f20cb5acb087c19c859a4a57
}

/**
* Creates a wireframe that is an ellipse with given the center and dimensions.
*
* The wireframe will show the boundary, not the circle tesselation.
*
* @param   center      The ellipse center point
* @param   size        The size of the ellipse
* @param   stroke      The width of the path
* @param   segments    The number of segments to use
*
* @return  An autoreleased path node
*/
PathNode* PathNode::createWithCircle(const Vec2& center, const Size& size, float stroke, unsigned int segments) {
	PathNode *node = new (std::nothrow) PathNode(stroke);
	if (!node) {
		CC_SAFE_DELETE(node);
		return nullptr;
	}

	const float coef = 2.0f * (float)M_PI / segments;
	const unsigned int counts = 2 * segments + 2;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT
	GLfloat* verts = new GLfloat[counts];
	for (unsigned int ii = 0; ii <= segments; ii++) {
		float rads = ii*coef;
		verts[2 * ii] = 0.5f * size.width  * cosf(rads) + center.x;
		verts[2 * ii + 1] = 0.5f * size.height * sinf(rads) + center.y;
	}
	node->_polygon.set(verts, 0, counts);
	delete[] verts;
#else
<<<<<<< HEAD
	GLfloat verts[counts];
	for (unsigned int ii = 0; ii <= segments; ii++) {
		float rads = ii*coef;
		verts[2 * ii] = 0.5f * size.width  * cosf(rads) + center.x;
		verts[2 * ii + 1] = 0.5f * size.height * sinf(rads) + center.y;
	}
	node->_polygon.set(verts, counts, 0);
=======
    GLfloat verts[counts];
    for (unsigned int ii = 0; ii <= segments; ii++) {
        float rads = ii*coef;
        verts[2 * ii] = 0.5f * size.width  * cosf(rads) + center.x;
        verts[2 * ii + 1] = 0.5f * size.height * sinf(rads) + center.y;
    }
    node->_polygon.set(verts, counts, 0);
>>>>>>> 136cf0348962ea43f20cb5acb087c19c859a4a57
#endif

	if (node->init(node->_polygon)) {
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
}


#pragma mark -
#pragma mark Allocator

/**
* Creates an empty path node.
*
* The underlying polygon is empty, and must be set via setPolygon.
*
* Any polygon added to this node is closed by default.
*
* @param stroke The stroke width of the path
* @param closed Whether or not the polygon path is closed
*/
PathNode::PathNode(float stroke, bool closed) : TexturedNode() {
	_name = "PathNode";
	_stroke = stroke;
	_closed = closed;
	_joint = Poly2::Joint::NONE;
	_endcap = Poly2::Cap::NONE;
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
std::string PathNode::getDescription() const {
	return StringUtils::format("<PathNode | Tag = %d>", _tag);
}

/**
* Sets the stroke width of the path.
*
* @param   stroke  The stroke width of the path
*/
void PathNode::setStroke(float stroke) {
	bool changed = (stroke != _stroke);
	_stroke = stroke;

	if (changed && _stroke > 0) {
		clearRenderData();
		_polygon.extrude(_extrusion, _stroke, _closed, _joint, _endcap);
		setContentSize(_extrusion.getBounds().size);
	}
	else if (changed) {
		clearRenderData();
		_polygon.traverse(_closed ? Poly2::Traversal::CLOSED : Poly2::Traversal::OPEN);
		setContentSize(_polygon.getBounds().size);
	}
}

/**
* Sets whether the path is closed.
*
* @param   closed  Whether the path is closed.
*/
void PathNode::setClosed(bool closed) {
	bool changed = (closed != _closed);
	_closed = closed;

	if (changed) {
		clearRenderData();
	}
}

/**
* Sets the joint type between path segments.
*
* @param   joint  Joint type between path segments
*/
void PathNode::setJoint(Poly2::Joint joint) {
	bool changed = (joint != _joint);
	_joint = joint;

	if (changed && _stroke > 0) {
		clearRenderData();
		_polygon.extrude(_extrusion, _stroke, _closed, _joint, _endcap);
		setContentSize(_extrusion.getBounds().size);
	}
}

/**
* Sets the cap shape at the ends of the path.
*
* @param  cap  Cap shape at the ends of the path.
*/
void PathNode::setCap(Poly2::Cap cap) {
	bool changed = (cap != _endcap);
	_endcap = cap;

	if (changed && _stroke > 0) {
		clearRenderData();
		_polygon.extrude(_extrusion, _stroke, _closed, _joint, _endcap);
		setContentSize(_extrusion.getBounds().size);
	}
}


/**
* Sets the texture polygon to the given one in image space.
*
* @param poly  The polygon to texture
*/
void PathNode::setPolygon(const Poly2& poly) {
	TexturedNode::setPolygon(poly);
	if (_stroke > 0) {
		_polygon.extrude(_extrusion, _stroke, _closed, _joint, _endcap);
		setContentSize(_extrusion.getBounds().size);
	}
	else {
		_polygon.traverse(_closed ? Poly2::Traversal::CLOSED : Poly2::Traversal::OPEN);
	}
}

/**
* Sets the texture polygon to one equivalent to the given rect.
*
* The rectangle will be converted into a Poly2.  Unless you are
* constructing wireframes, there is little benefit to using a
* TexturedNode in this way over a normal Sprite. The option is
* here only for completion.
*
* @param rect  The rectangle to texture
*/
void PathNode::setPolygon(const Rect& rect) {
	TexturedNode::setPolygon(rect);
	if (_stroke > 0) {
		_polygon.extrude(_extrusion, _stroke, _closed, _joint, _endcap);
		setContentSize(_extrusion.getBounds().size);
	}
	else {
		_polygon.traverse(_closed ? Poly2::Traversal::CLOSED : Poly2::Traversal::OPEN);
	}
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
void PathNode::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
	// Don't do calculate the culling if the transform was not updated
	_insideBounds = (flags & FLAGS_TRANSFORM_DIRTY) ? renderer->checkVisibility(transform, _contentSize) : _insideBounds;

	if (_insideBounds) {
		if (_triangles.vertCount == 0) {
			generateRenderData();
		}
		_command.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, _triangles, transform, flags);
		renderer->addCommand(&_command);
	}
}

/**
* Allocate the wireframe data necessary to render this node.
*
* This method allocates the Wireframe data used by the
* WireframeCommand in the rendering pipeline.
*/
void PathNode::generateRenderData() {
	clearRenderData();
	if (_stroke > 0) {
		allocTriangles(_extrusion);
		_command.setType(RenderCommand::Type::TRIANGLES_COMMAND);
	}
	else {
		allocTriangles(_polygon);
		_command.setType(RenderCommand::Type::WIREFRAME_COMMAND);
	}
	updateColor();
	updateTextureCoords();
}

NS_CC_END