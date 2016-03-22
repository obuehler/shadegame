//
//  CUTexturedNode.cpp
//  Cornell Extensions to Cocos2D
//
//  This module provides an abstract class for textured scene graph nodes.  Currently,
//  the only textured nodes are Sprite and its variations.  These are very limited, as
//  they only texture quads.  Sometimes we want a textured polygon.  Sometimes we want
//  a textured line.  Currently, the only way to do this (particularly the latter)
//  is to define custom drawing commands, like DrawNode does.  This is incredibly
//  inefficient, and will drop framerate from all the GPU stalls.  This class provides
//  native support for Cocos2d's specialized rendering pipeline.
//
//  You should never instantiate an object of this class.  Instead, you should use
//  one of the concrete subclasses: WireNode, PolygonNode, or PathNode.
//
//  Author: Walker White
//  Version: 11/15/15
//
// TODO: Note difference in Sprite Coordinates
#include <algorithm>
#include "CUTexturedNode.h"
#include <renderer/CCTextureCache.h>
#include <renderer/CCTexture2D.h>
#include <renderer/CCRenderer.h>
#include <base/CCDirector.h>


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
* Creates an empty polygon with the degenerate texture.
*
* You do not need to set the texture. The polygon, however, will also be
* empty, and must be set via setPolygon.
*
* This constructor should never be called directly, as this is an
* abstract class.
*/
TexturedNode::TexturedNode() :
	_texture(nullptr),
	_insideBounds(false),
	_blendFunc(BlendFunc::DISABLE),
	_opacityModifyRGB(true),
	_flipHorizontal(false),
	_flipVertical(false) {
	_name = "TexturedNode";
	_triangles.vertCount = 0;
	_triangles.verts = nullptr;
	_triangles.indexCount = 0;
	_triangles.indices = nullptr;
}

/**
* Intializes an empty polygon with the degenerate texture.
*
* You do not need to set the texture. The polygon, however, will also be
* empty, and must be set via setPolygon.
*
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::init() {
	return initWithTexture(nullptr, Rect::ZERO);
}

/**
* Intializes a solid polygon with the given vertices.
*
* The node will use the degenerate texture, which is solid white.
* Hence the polygon will have a solid color.
*
* The polygon will be triangulated using the rules of Poly2.
*
* @param   vertices   The vertices to texture (expressed in image space)
* @param   size       The number of elements in vertices
* @param   offset     The offset in vertices
*
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::init(float* vertices, int size, int offset) {
	return initWithTexture(nullptr, vertices, size, 0);
}

/**
* Intializes a solid polygon given polygon shape.
*
* The node will use the degenerate texture, which is solid white.
* Hence the polygon will have a solid color.
*
* @param   poly     The polygon to texture
*
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::init(const Poly2& poly) {
	return initWithTexture(nullptr, poly);
}

/**
* Intializes a solid polygon with the given rect.
*
* The rectangle will be converted into a Poly2.  There is little benefit to
* using a PolygonNode in this way over a normal Sprite. The option is here
* only for completion.
*
* @param   rect     The rectangle to texture
*
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::init(const Rect& rect) {
	return initWithTexture(nullptr, rect);
}

/**
* Intializes a textured polygon from the image filename.
*
* After creation, the polygon will be a rectangle.  The vertices of this
* polygon will be the corners of the image.
*
* @param filename   A path to image file, e.g., "scene1/earthtile.png"
*
* @retain  a reference to the newly loaded texture
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::initWithFile(const std::string& filename) {
	CCASSERT(filename.size()>0, "Invalid filename for sprite");

	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(filename);
	if (texture) {
		Rect bounds = Rect::ZERO;
		bounds.size = texture->getContentSize();
		return initWithTexture(texture, bounds);
	}

	return false;
}

/**
* Initializes a textured polygon from the image filename and the given vertices.
*
* The polygon will be triangulated using the rules of Poly2.
*
* @param   filename   A path to image file, e.g., "scene1/earthtile.png"
* @param   vertices   The vertices to texture (expressed in image space)
* @param   offset     The offset in vertices
* @param   size       The number of elements in vertices
*
* @retain  a reference to the newly loaded texture
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::initWithFile(const std::string &filename, float* vertices, int size, int offset) {
	CCASSERT(filename.size()>0, "Invalid filename for sprite");

	_polygon.set(vertices, size, offset);
	return initWithFile(filename, _polygon);
}

/**
* Initializes a textured polygon from the image filename and the given polygon.
*
* @param filename  A path to image file, e.g., "scene1/earthtile.png"
* @param poly      The polygon to texture
*
* @retain  a reference to the newly loaded texture
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::initWithFile(const std::string &filename, const Poly2& poly) {
	CCASSERT(filename.size()>0, "Invalid filename");

	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(filename);
	if (texture) {
		return initWithTexture(texture, poly);
	}

	return false;
}

/**
* Initializes a textured polygon from the image filename and the given rect.
*
* The rectangle will be converted into a Poly2.  There is little benefit to
* using a TexturedNode in this way over a normal Sprite. The option is here
* only for completion.
*
* @param filename  A path to image file, e.g., "scene1/earthtile.png"
* @param rect      The rectangle to texture
*
* @retain  a reference to the newly loaded texture
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::initWithFile(const std::string &filename, const Rect& rect) {
	CCASSERT(filename.size()>0, "Invalid filename");

	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(filename);
	if (texture) {
		return initWithTexture(texture, rect);
	}

	return false;
}

/**
* Initializes a textured polygon from a Texture2D object.
*
* After creation, the polygon will be a rectangle. The vertices of this
* polygon will be the corners of the texture.
*
* @param texture   A pointer to a Texture2D object.
*
* @retain  a reference to this texture
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::initWithTexture(Texture2D *texture) {
	CCASSERT(texture != nullptr, "Invalid texture for sprite");

	Rect bounds = Rect::ZERO;
	bounds.size = texture->getContentSize();
	return initWithTexture(texture, bounds);
}

/**
* Initializes a textured polygon from a Texture2D object and the given vertices.
*
* The polygon will be triangulated using the rules of Poly2.
*
* @param   texture    A pointer to an existing Texture2D object.
*                     You can use a Texture2D object for many sprites.
* @param   vertices   The vertices to texture (expressed in image space)
* @param   size       The number of elements in vertices
* @param   offset     The offset in vertices
*
* @retain  a reference to this texture
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::initWithTexture(Texture2D *texture, float* vertices, int size, int offset) {
	_polygon.set(vertices, size, offset);
	return initWithTexture(texture, _polygon);
}

/**
* Initializes a textured polygon from a Texture2D object and the given polygon.
*
* @param   texture  A pointer to an existing Texture2D object.
*                   You can use a Texture2D object for many sprites.
* @param   poly     The polygon to texture
*
* @retain  a reference to this texture
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::initWithTexture(Texture2D *texture, const Poly2& poly) {
	bool result = false;
	if (Node::init()) {
		_opacityModifyRGB = true;
		_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

		// default transform anchor: center
		setAnchorPoint(Vec2(0.5f, 0.5f));

		// shader state
		setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(
			GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP)
			);

		// Update texture (calls updateBlendFunc)
		setTexture(texture);
		setPolygon(poly);

		result = true;
	}

	return result;
}

/**
* Initializes a textured polygon from a Texture2D object and the given rect.
*
* The rectangle will be converted into a Poly2.  There is little benefit to
* using a TexturedNode in this way over a normal Sprite. The option is here
* only for completion.
*
* @param   texture  A pointer to an existing Texture2D object.
*                   You can use a Texture2D object for many sprites.
* @param   rect     The rectangle to texture
*
* @retain  a reference to this texture
* @return  true if the sprite is initialized properly, false otherwise.
*/
bool TexturedNode::initWithTexture(Texture2D *texture, const Rect& rect) {
	bool result = false;
	if (Node::init()) {
		_opacityModifyRGB = true;
		_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

		// default transform anchor: center
		setAnchorPoint(Vec2(0.5f, 0.5f));

		// shader state
		setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(
			GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP)
			);

		// update texture (calls updateBlendFunc)
		setTexture(texture);
		setPolygon(rect);

		result = true;
	}
	return result;
}

/**
* Releases all resources allocated with this sprite.
*
* This will release, but not necessarily delete the associated texture.
* However, the polygon and drawing commands will be deleted and no
* longer safe to use.
*/
TexturedNode::~TexturedNode() {
	CC_SAFE_RELEASE(_texture);
	clearRenderData();
}


#pragma mark -
#pragma mark Attribute Accessors

/**
* Sets the node texture to a new sprite allocated from a filename.
*
* This method will have no effect on the polygon vertices.  Unlike Sprite,
* TexturedNode decouples the geometry from the texture.  That is because
* we expect the vertices to not match the texture perfectly.
*
* @param   filename A path to image file, e.g., "scene1/earthtile.png"
*
* @retain  a reference to the newly loaded texture
* @release the previous scene texture used by this object
*/
void TexturedNode::setTexture(const std::string &filename) {
	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(filename);
	setTexture(texture);
}

/**
* Sets the node texture to the one specified.
*
* This method will have no effect on the polygon vertices.  Unlike Sprite,
* TexturedNode decouples the geometry from the texture.  That is because
* we expect the vertices to not match the texture perfectly.
*
* @param   texture  A pointer to an existing Texture2D object.
*                   You can use a Texture2D object for many sprites.
*
* @retain  a reference to this texture
* @release the previous scene texture used by this object
*/
void TexturedNode::setTexture(Texture2D *texture) {
	// accept texture==nil as argument
	CCASSERT(!texture || dynamic_cast<Texture2D*>(texture), "setTexture expects a Texture2D. Invalid argument");

	if (texture == nullptr) {
		// Gets the texture by key firstly.
		texture = Director::getInstance()->getTextureCache()->getTextureForKey(CC_2x2_WHITE_IMAGE_KEY);

		// If texture wasn't in cache, create it from RAW data.
		if (texture == nullptr) {
			Image* image = new (std::nothrow) Image();
			bool isOK = image->initWithRawData(cc_2x2_white_image, sizeof(cc_2x2_white_image), 2, 2, 8);
			CC_UNUSED_PARAM(isOK);
			CCASSERT(isOK, "The 2x2 empty texture was created unsuccessfully.");

			texture = Director::getInstance()->getTextureCache()->addImage(image, CC_2x2_WHITE_IMAGE_KEY);
			CC_SAFE_RELEASE(image);
		}
	}

	if (_texture != texture) {
		CC_SAFE_RETAIN(texture);
		CC_SAFE_RELEASE(_texture);
		_texture = texture;
		clearRenderData();
		updateBlendFunc();
		updateTextureCoords();
	}
}

/**
* Sets the texture polgon to the vertices expressed in image space.
*
* The polygon will be triangulated according to the rules of Poly2.
*
* @param   vertices The vertices to texture
* @param   offset   The offset in vertices
* @param   size     The number of elements in vertices
*/
void TexturedNode::setPolygon(float* vertices, int size, int offset) {
	_polygon.set(vertices, size, offset);
	setPolygon(_polygon);
}

/**
* Sets the texture polygon to the given one in image space.
*
* @param poly  The polygon to texture
*/
void TexturedNode::setPolygon(const Poly2& poly) {
	if (&_polygon != &poly) {
		_polygon.set(poly);
	}

	clearRenderData();
	setContentSize(_polygon.getBounds().size);
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
void TexturedNode::setPolygon(const Rect& rect) {
	_polygon.set(rect);
	clearRenderData();
	setContentSize(_polygon.getBounds().size);
}

/**
* Translates the polygon by the given amount.
*
* Remember that translating the polygon has no effect on the
* shape or position.  Because the polygon is expressed in
* texture coordinates, all it does is shift the texture coords
* of the polygon.  Hence this method can be used for animation
* and filmstrips.
*
* Calling this method is faster than changing the polygon and
* resetting it.
*
* @param   dx  The amount to shift horizontally.
* @param   dx  The amount to shift horizontally.
*/
void TexturedNode::shiftPolygon(float dx, float dy) {
	_polygon += Vec2(dx, dy);
	float w = _texture->getContentSize().width;
	float h = _texture->getContentSize().height;
	for (int ii = 0; ii < _triangles.vertCount; ii++) {
		_triangles.verts[ii].texCoords.u += dx / w;
		_triangles.verts[ii].texCoords.v -= dy / h;
	}
}


#pragma mark -
#pragma mark RGBA Protocol

/**
* Sets whether the opacity should be premultiplied into the color.
*
* This method is necessary because of how scene graphs work
*
* @param modify whether the opacity should be premultiplied into the color
*/
void TexturedNode::setOpacityModifyRGB(bool modify) {
	if (_opacityModifyRGB != modify) {
		_opacityModifyRGB = modify;
		updateColor();
	}
}

/**
* Returns true if the opacity should be premultiplied into the color.
*
* This method is necessary because of how scene graphs work
*
* @return true if the opacity should be premultiplied into the color
*/
bool TexturedNode::isOpacityModifyRGB(void) const {
	return _opacityModifyRGB;
}

/**
* Updates the color for each vertices to match the node settings.
*/
void TexturedNode::updateColor(void) {
	if (_triangles.vertCount == 0) {
		return;
	}

	Color4B color4(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);

	// special opacity for premultiplied textures
	if (_opacityModifyRGB) {
		color4.r *= _displayedOpacity / 255.0f;
		color4.g *= _displayedOpacity / 255.0f;
		color4.b *= _displayedOpacity / 255.0f;
	}

	for (int ii = 0; ii < _triangles.vertCount; ii++) {
		_triangles.verts[ii].colors = color4;
	}
}


#pragma mark -
#pragma mark Texture protocol

/**
* Update the blend options for this node to use in a render pass
*/
void TexturedNode::updateBlendFunc(void) {
	// it is possible to have an untextured sprite
	if (!_texture || !_texture->hasPremultipliedAlpha()) {
		_blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
		setOpacityModifyRGB(false);
	}
	else {
		_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
		setOpacityModifyRGB(true);
	}
}

/**
* Updates the texture coordinates for this polygon
*
* The texture coordinates are computed assuming that the polygon is
* defined in image space, with the origin in the bottom left corner
* of the texture.
*/
void TexturedNode::updateTextureCoords() {
	if (_triangles.vertCount == 0) {
		return;
	}

	float w = _texture->getContentSize().width;
	float h = _texture->getContentSize().height;
	Vec2 origin = _polygon.getBounds().origin;
	for (int ii = 0; ii < _triangles.vertCount; ii++) {
		_triangles.verts[ii].texCoords.u = (_triangles.verts[ii].vertices.x + origin.x) / w;
		if (_flipHorizontal) {
			_triangles.verts[ii].texCoords.u = 1 - _triangles.verts[ii].texCoords.u;
		}
		_triangles.verts[ii].texCoords.v = (_triangles.verts[ii].vertices.y + origin.y) / h;
		if (!_flipVertical) {
			_triangles.verts[ii].texCoords.v = 1 - _triangles.verts[ii].texCoords.v;
		}
	}
}


#pragma mark -
#pragma mark Rendering methods

/**
* Generates the triangles data to render a shape from the polygon.
*
* The shape may either be solid or a wireframe. This is determined by the specific
* draw method for this class.  This method simply generates the triangles and
* and traversal information from the Poly2 class.
*
* @param   poly        The polygon to traverse
*
* @return Reference to the filled triangle data
*/
TrianglesCommand::Triangles& TexturedNode::allocTriangles(const Poly2& poly) {
	clearRenderData();
	int vertsize = (int)poly.getVertices().size();
	V3F_C4B_T2F* verts = nullptr;
	if (vertsize > 0) {
		verts = new V3F_C4B_T2F[vertsize];
		for (int ii = 0; ii < vertsize; ii++) {
			float x = poly.getVertices()[ii].x - poly.getBounds().origin.x;
			float y = poly.getVertices()[ii].y - poly.getBounds().origin.y;
			verts[ii].vertices = Vec3(x, y, 0.0f);
			// These values should be set later; initialize for safety.
			verts[ii].texCoords = Tex2F(0, 0);
			verts[ii].colors = Color4B::WHITE;
		}
	}

	unsigned short* indxs = new unsigned short[poly.getIndices().size()];
	std::copy(poly.getIndices().begin(), poly.getIndices().end(), indxs);

	// Copy to triangles
	_triangles.verts = verts;
	_triangles.vertCount = vertsize;
	_triangles.indices = indxs;
	_triangles.indexCount = (int)poly.getIndices().size();

	return _triangles;
}

/**
* Clears the render data, releasing all vertices and indices.
*/
void TexturedNode::clearRenderData() {
	if (_triangles.verts != nullptr) {
		delete[] _triangles.verts;
		_triangles.verts = nullptr;
	}
	_triangles.vertCount = 0;
	if (_triangles.indices != nullptr) {
		delete[] _triangles.indices;
		_triangles.indices = nullptr;
	}
	_triangles.indexCount = 0;
}


NS_CC_END