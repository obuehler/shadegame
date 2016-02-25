//
//  CUTexturedNode.h
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
#ifndef __CU_TEXTURED_NODE_H__
#define __CU_TEXTURED_NODE_H__

#include <string>
#include "CUPoly2.h"
#include <2d/CCNode.h>
#include <base/CCProtocols.h>
#include <renderer/CCTrianglesCommand.h>

NS_CC_BEGIN

#pragma mark -
#pragma mark TexturedNode
/**
 * Abstract scene graph node represent a textured shape.
 *
 * This class uses the Cocos2D rendering pipeline to batch graphics information
 * into a single mesh (with one drawing call), whenever possible. Changes to the
 * textures, or drawing a line instead of a solid shape will require a new batch.
 * You should play with your scene graph ordering to best improve performance.
 *
 * All graphics data sent to the rendering pipeline must have a texture. If no 
 * texture is specified, the node will use the degenerate texture "/cc_2x2_white_image".
 * This is an all white texture that produces solid images.
 *
 * The node shape is stored as polygon.  This is true regardless of whether the
 * node is displaying solid shapes or a wireframe.  The polygon is itself specified 
 * in image coordinates. Image coordinates are different from texture coordinates.
 * Their origin is at the bottom-left corner of the file, and each pixel is one
 * unit. This makes specifying to polygon more natural for irregular shapes.
 *
 * This means that a polygon with vertices (0,0), (width,0), (width,height),
 * and (0,height) would be identical to a sprite node. However, a polygon with 
 * vertices (0,0), (2*width,0), (2*width,2*height), and (0,2*height) would tile 
 * the sprite (given the wrap settings) twice both horizontally and vertically.
 *
 * The content size of this node is defined by the size (but not the offset)
 * of the bounding box. The anchor point is relative to this content size.
 * The default anchor point in TexturedNode is (0.5, 0.5).  This means that a
 * uniform translation of the polygon (in contrast to the node itself) will not 
 * move the shape on the the screen.  Instead, it will just change the part of
 * the texture it uses.
 *
 * For example, suppose the texture has given width and height.  We have one
 * polygon with vertices (0,0), (width/2,0), (width/2,height/2), and (0,height/2). 
 * We have another polygon with vertices (width/2,height/2), (width,height/2), 
 * (width,height), and (width/2,height).  Both polygons would create a rectangle
 * of size (width/2,height/2). centered at the node position.  However, the
 * first would use the bottom left part of the texture, while the second would
 * use the top right.
 */
class CC_DLL TexturedNode : public Node, public TextureProtocol {
    
#pragma mark Internal Helpers
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CC_DISALLOW_COPY_AND_ASSIGN(TexturedNode);
    
protected:
    /** Texture to be applied to the polygon */
    Texture2D*       _texture;
    /** The polygon defining this node, with vertices in texture space */
    Poly2            _polygon;
    /** Blending function is required to implement TextureProtocol */
    BlendFunc        _blendFunc;
    /** Whether or not the sprite was inside bounds the previous frame */
    bool _insideBounds;
    /** Whether to support opacity and RGB protocol */
    bool _opacityModifyRGB;

    /** The command for the Cocos2D rendering pipeline */
    TrianglesCommand _command;
    /** Rendering data representing a solid shape */
    TrianglesCommand::Triangles _triangles;
    
    /** Whether or not to flip the texture horizontally */
    bool _flipHorizontal;
    /** Whether or not to flip the texture vertically */
    bool _flipVertical;
    
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
    TrianglesCommand::Triangles& allocTriangles(const Poly2& poly);
    
    /**
     * Clears the render data, releasing all vertices and indices.
     */
    void clearRenderData();

    /**
     * Updates the color for each vertices to match the node settings.
     */
    void updateColor() override;
    
    /**
     * Updates the blend options for this node to use in a render pass
     */
    void updateBlendFunc();

    /**
     * Updates the texture coordinates for this polygon
     *
     * The texture coordinates are computed assuming that the polygon is
     * defined in image space, with the origin in the bottom left corner
     * of the texture.
     */
    void updateTextureCoords();

    
#pragma mark Abstract Methods

    /**
     * Allocate the render data necessary to render this node.
     *
     * This method is abstract, since it needs access to the implementation
     * specific render data.
     */
    virtual void generateRenderData()   {   }
    
    
public:
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
    virtual void setTexture(const std::string &filename);
    
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
    virtual void setTexture(Texture2D *texture) override;
    
    /**
     * Returns the Texture2D object used by this node.
     *
     * @return the Texture2D object used by this node
     */
    virtual Texture2D* getTexture() const override { return _texture; }
    
    /**
     * Sets the polgon to the vertices expressed in texture space.
     *
     * The polygon will be triangulated according to the rules of Poly2.
     *
     * @param   vertices The vertices to texture
     * @param   size     The number of elements in vertices
     * @param   offset   The offset in vertices
     */
    virtual void setPolygon(float* vertices, int size, int offset=0);
    
    /**
     * Sets the polygon to the given one in texture space.
     *
     * @param poly  The polygon to texture
     */
    virtual void setPolygon(const Poly2& poly);
    
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
    virtual void setPolygon(const Rect& rect);
    
    /**
     * Returns the texture polygon for this scene graph node
     *
     * @returns the texture polygon for this scene graph node
     */
    const Poly2& getPolygon() const { return _polygon; }

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
    virtual void shiftPolygon(float dx, float dy);
    
    /**
     * Returns the rect of the Polygon in points
     *
     * The bounding rect is the smallest rectangle containing all
     * of the points in the polygon.
     *
     * This value also defines the content size of the node.  The
     * polygon will be shifted so that its bounding rect is centered
     * at the node center.
     */
    const Rect& getBoundingRect() const { return _polygon.getBounds(); }
    
    /**
     * Sets the blend function to the one specified
     *
     * This is part of TextureProtocol.
     *
     * @param blendFunc the new blend function.
     *
     */
    void setBlendFunc(const BlendFunc &blendFunc) override { _blendFunc = blendFunc; }
    
    /**
     * Returns the current blend function for this node
     *
     * This is part of TextureProtocol.
     *
     * @return the node blend function.
     *
     */
    const BlendFunc& getBlendFunc() const override { return _blendFunc; }
    
    /**
     * Sets whether the opacity should be premultiplied into the color.
     *
     * This method is necessary because of how scene graphs work
     *
     * @param modify whether the opacity should be premultiplied into the color
     */
    virtual void setOpacityModifyRGB(bool modify) override;
    
    /**
     * Returns true if the opacity should be premultiplied into the color.
     *
     * This method is necessary because of how scene graphs work
     *
     * @return true if the opacity should be premultiplied into the color
     */
    virtual bool isOpacityModifyRGB(void) const override;
    
    /**
     * Flips the texture coordinates horizontally if flag is true.
     *
     * Flipping the texture coordinates replaces each u coordinate with 
     * 1-u.  Hence this operation is defined even if the texture coordinates 
     * are outside the range 0..1.
     *
     * @param  flag whether to flip the coordinates horizontally
     */
    void flipHorizontal(bool flag) { _flipHorizontal = flag; updateTextureCoords(); }

    /** 
     * Returns true if the texture coordinates are flipped horizontally.
     *
     * @return true if the texture coordinates are flipped horizontally.
     */
    bool isFlipHorizontal() const { return _flipHorizontal; }

    /**
     * Flips the texture coordinates vertically if flag is true.
     *
     * Flipping the texture coordinates replaces each v coordinate with
     * 1-v.  Hence this operation is defined even if the texture coordinates
     * are outside the range 0..1.
     *
     * @param  flag whether to flip the coordinates vertically
     */
    void flipVertical(bool flag) { _flipVertical = flag; updateTextureCoords(); }
    
    /**
     * Returns true if the texture coordinates are flipped vertically.
     *
     * @return true if the texture coordinates are flipped vertically.
     */
    bool isFlipVertical() const { return _flipVertical; }
    
    
CC_CONSTRUCTOR_ACCESS:
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
    TexturedNode(void);
    
    /**
     * Releases all resources allocated with this node.
     *
     * This will release, but not necessarily delete the associated texture.
     * However, the polygon and drawing commands will be deleted and no
     * longer safe to use.
     */
    virtual ~TexturedNode(void);
    
    /**
     * Intializes an empty polygon with the degenerate texture.
     *
     * You do not need to set the texture. The polygon, however, will also be
     * empty, and must be set via setPolygon.
     *
     * @return  true if the sprite is initialized properly, false otherwise.
     */
    virtual bool init() override;
  
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
    virtual bool init(float* vertices, int size, int offset=0);
    
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
    virtual bool  init(const Poly2& poly);
    
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
    virtual bool  init(const Rect& rect);
    
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
    virtual bool initWithFile(const std::string& filename);
    
    /**
     * Initializes a textured polygon from the image filename and the given vertices.
     *
     * The polygon will be triangulated using the rules of Poly2.
     *
     * @param   filename   A path to image file, e.g., "scene1/earthtile.png"
     * @param   vertices   The vertices to texture (expressed in image space)
     * @param   size       The number of elements in vertices
     * @param   offset     The offset in vertices
     *
     * @retain  a reference to the newly loaded texture
     * @return  true if the sprite is initialized properly, false otherwise.
     */
    virtual bool initWithFile(const std::string& filename, float* vertices, int size, int offset=0);
    
    /**
     * Initializes a textured polygon from the image filename and the given polygon.
     *
     * @param filename  A path to image file, e.g., "scene1/earthtile.png"
     * @param poly      The polygon to texture
     *
     * @retain  a reference to the newly loaded texture
     * @return  true if the sprite is initialized properly, false otherwise.
     */
    virtual bool initWithFile(const std::string& filename, const Poly2& poly);
    
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
    virtual bool initWithFile(const std::string& filename, const Rect& rect);
    
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
    virtual bool initWithTexture(Texture2D *texture);
    
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
    virtual bool initWithTexture(Texture2D *texture, float* vertices, int size, int offset=0);
    
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
    virtual bool initWithTexture(Texture2D *texture, const Poly2& poly);
    
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
    virtual bool initWithTexture(Texture2D *texture, const Rect& rect);

};

NS_CC_END

#endif /* defined(__CU_TEXTURED_NODE_H__) */
