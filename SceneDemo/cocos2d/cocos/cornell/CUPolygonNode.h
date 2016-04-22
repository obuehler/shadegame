//
//  CCPolygonNode.h
//  Cornell Extensions to Cocos2D
//
//  This module provides a scene graph node that supports solid 2D polygons.  These
//  polygons may also be textured by a sprite. This class is preferable to Sprite
//  except in the cases where you use Cocos2d's animation events.
//
//  Author: Walker White
//  Version: 2/21/15
//
#ifndef __CC_POLYGON_SPRITE_H__
#define __CC_POLYGON_SPRITE_H__

#include <string>
#include "CUTexturedNode.h"


NS_CC_BEGIN

#pragma mark -
#pragma mark PolygonNode
/**
 * Scene graph node represent a solid 2D polygon textured by a sprite.
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
 * The polygon is specified in image coordinates. Image coordinates are different
 * from texture coordinates. Their origin is at the bottom-left corner of the file,
 * and each pixel is one unit. This makes specifying to polygon more natural for
 * irregular shapes.
 *
 * This means that a polygon with vertices (0,0), (width,0), (width,height),
 * and (0,height) would be identical to a sprite node. However, a polygon with
 * vertices (0,0), (2*width,0), (2*width,2*height), and (0,2*height) would tile
 * the sprite (given the wrap settings) twice both horizontally and vertically.
 *
 * The content size of this node is defined by the size (but not the offset)
 * of the bounding box.  The anchor point is relative to this content size.
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
class CC_DLL PolygonNode : public TexturedNode {

#pragma mark Internal Helpers
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CC_DISALLOW_COPY_AND_ASSIGN(PolygonNode);
    
protected:
    /**
     * Allocate the render data necessary to render this node.
     *
     * This method allocates the Triangles data used by the TrianglesCommand 
     * in the rendering pipeline.
     */
    virtual void generateRenderData() override;

    
public:
#pragma mark Static Constructors
    /**
     * Creates an empty polygon with the degenerate texture.
     *
     * You do not need to set the texture. The polygon, however,
     * will also be empty, and must be set via setPolygon.
     *
     * @return An autoreleased sprite object.
     */
    static PolygonNode* create();
    
    /**
     * Creates a solid polygon with the given vertices.
     *
     * The node will use the degenerate texture, which is solid white.
     * Hence the polygon will have a solid color.
     *
     * The polygon will be triangulated using the rules of Poly2.
     *
     * @param   vertices The vertices to texture (expressed in image space)
     * @param   size     The number of elements in vertices
     * @param   offset   The offset in vertices
     *
     * @return  An autoreleased sprite object
     */
    static PolygonNode* create(float* vertices, int size, int offset=0);
    
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
    static PolygonNode* create(const Poly2& poly);
    
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
    static PolygonNode* create(const Rect& rect);
    
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
    static PolygonNode* createWithFile(const std::string& filename);
    
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
    static PolygonNode* createWithFile(const std::string& filename, float* vertices, int size, int offset=0);
    
    /**
     * Creates a textured polygon from the image filename and the given polygon.
     *
     * @param   filename A path to image file, e.g., "scene1/earthtile.png"
     * @param   poly     The polygon to texture
     *
     * @retain  a reference to the newly loaded texture
     * @return  An autoreleased sprite object
     */
    static PolygonNode* createWithFile(const std::string& filename, const Poly2& poly);
    
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
    static PolygonNode* createWithFile(const std::string& filename, const Rect& rect);
    
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
    static PolygonNode* createWithTexture(Texture2D *texture);
    
    /**
     * Creates a textured polygon from a Texture2D object and the given vertices.
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
     * @return  An autoreleased sprite object
     */
    static PolygonNode* createWithTexture(Texture2D *texture, float* vertices, int size, int offset=0);
    
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
    static PolygonNode* createWithTexture(Texture2D *texture, const Poly2& poly);
    
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
    static PolygonNode* createWithTexture(Texture2D *texture, const Rect& rect);

    
#pragma mark Attribute Accessors
    /**
     * Returns a string description of this object
     *
     * This method is useful for debugging.
     *
     * @return  a string description of this object
     */
    virtual std::string getDescription() const override;
    
    
#pragma mark Rendering Methods
    /**
     * Sends drawing commands to the renderer
     *
     * This method is overridden from Node, to provide the custom commands.
     * PolygonNodes are drawn to the Triangle mesh (which is different
     * from the Quad mesh used for Sprites) to cut down on the number of
     * drawing calls.
     *
     * @param renderer   Reference to the render thread
     * @param transform  The accumulated transform from the parent
     * @param flags      Specialized Cocos2d drawing flags
     */
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Hidden Constructors
    /**
     * Creates an empty polygon with the degenerate texture.
     *
     * You do not need to set the texture. The polygon, however, will also be 
     * empty, and must be set via setPolygon.
     *
     * This constructor should never be called directly. Use one of the static
     * constructors instead.
     */
    PolygonNode(void);
    
    /**
     * Releases all resources allocated with this node.
     *
     * This will release, but not necessarily delete the associated texture.
     * However, the polygon and drawing commands will be deleted and no
     * longer safe to use.
     */
    virtual ~PolygonNode(void)  { }
    
};

NS_CC_END

#endif /* defined(__CC_POLYGON_SPRITE_H__) */