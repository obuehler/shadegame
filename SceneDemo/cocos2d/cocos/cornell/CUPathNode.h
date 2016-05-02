//
//  CUPathNode.h
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
#ifndef __CU_PATHNODE_H__
#define __CU_PATHNODE_H__


#include <string>
#include "CUPoly2.h"
#include "CUTexturedNode.h"
#include <renderer/CCTrianglesCommand.h>

NS_CC_BEGIN

#pragma mark -
#pragma mark PathNode

/**
 * Scene graph node to represent a path with width.
 *
 * This class uses the Cocos2D rendering pipeline to batch graphics information
 * into a single mesh (with one drawing call), whenever possible. Changes to the
 * textures, or drawing a line instead of a solid shape will require a new batch.
 * You should play with your scene graph ordering to best improve performance.
 *
 * Because paths have width, it is natural to texture them.  However, generally
 * you will only want to create a path with the degenerate texture (to draw
 * a solid, colored path).  Hence, none of the static constructors take a texture.
 * You are free to update the texture after creation, if you wish.
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
class CC_DLL PathNode : public TexturedNode {
    
#pragma mark Internal Helpers
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CC_DISALLOW_COPY_AND_ASSIGN(PathNode);
    
protected:
    /** The extrusion polygon, when stroke > 0 */
    Poly2 _extrusion;
    /** Whether or not the polygon path is closed. */
    bool _closed;
    /** The stroke width of this path. */
    float _stroke;
    /** The joint between segments of the path. */
    Poly2::Joint _joint;
    /** The shape of the two end caps of the path. */
    Poly2::Cap _endcap;

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
     * Creates an empty path node.
     *
     * The underlying polygon is empty, and must be set via setPolygon.
     *
     * @return An autoreleased path node
     */
    static PathNode* create();
    
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
    static PathNode* createWithVertices(float* vertices, int size, float stroke, bool closed=true);
    
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
    static PathNode* createWithPoly(const Poly2& poly, float stroke, bool closed=true);
    
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
    static PathNode* createWithRect(const Rect& rect, float stroke);
    
    /**
     * Creates a path that is a line from origin to destination.
     *
     * @param   origin  The line origin
     * @param   dest    The line destination
     * @param   stroke  The width of the path
     *
     * @return  An autoreleased path node
     */
    static PathNode* createWithLine(const Vec2 &origin, const Vec2 &dest, float stroke);
    
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
    static PathNode* createWithCircle( const Vec2& center, const Size& size, float stroke, unsigned int segments);
    
    
#pragma mark Attribute Accessors
    
    /**
     * Sets the stroke width of the path.
     *
     * @param   stroke  The stroke width of the path
     */
    void setStroke(float stroke);
    
    /**
     * Returns the stroke width of the path.
     *
     * @return the stroke width of the path.
     */
    float getStroke() const { return _stroke; }
    
    /**
     * Sets whether the path is closed.
     *
     * @param   closed  Whether the path is closed.
     */
    void setClosed(bool closed);
    
    /**
     * Returns whether the path is closed.
     *
     * @return whether the path is closed.
     */
    bool getClosed() const { return _closed; }

    /**
     * Sets the joint type between path segments.
     *
     * @param   joint  Joint type between path segments
     */
    void setJoint(Poly2::Joint joint);
    
    /**
     * Returns the joint type between path segments.
     *
     * @return the joint type between path segments.
     */
    Poly2::Joint getJoint() const { return _joint; }
    
    /**
     * Sets the cap shape at the ends of the path.
     *
     * @param  cap  Cap shape at the ends of the path.
     */
    void setCap(Poly2::Cap cap);
    
    /**
     * Returns the cap shape at the ends of the path.
     *
     * @return the cap shape at the ends of the path.
     */
    Poly2::Cap getCap() const { return _endcap; }

    /**
     * Returns a string description of this object
     *
     * This method is useful for debugging.
     *
     * @return  a string description of this object
     */
    virtual std::string getDescription() const override;
    
    /**
     * Sets the polygon to the given one in texture space.
     *
     * @param poly  The polygon to texture
     */
    virtual void setPolygon(const Poly2& poly) override;
    
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
    virtual void setPolygon(const Rect& rect) override;

    
#pragma mark Rendering Methods
    
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
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
    
    
CC_CONSTRUCTOR_ACCESS:
#pragma mark Hidden Constructors
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
    PathNode(float stroke, bool closed=true);
    
    /**
     * Releases all resources allocated with this sprite.
     *
     * After this is called, the polygon and drawing commands will be deleted
     * and no longer safe to use.
     */
    virtual ~PathNode(void) { }
};

NS_CC_END

#endif /* defined(__CU_PATHNODE_H__) */
