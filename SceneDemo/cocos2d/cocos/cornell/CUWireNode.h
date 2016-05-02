//
//  CUWireNode.h
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
#ifndef __CU_WIRENODE_H__
#define __CU_WIRENODE_H__


#include <string>
#include "CUPoly2.h"
#include "CUTexturedNode.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark WireNode

/**
 * Scene graph node to represent a wireframe
 *
 * This class uses the Cocos2D rendering pipeline to batch all of the lines for
 * a wireframe into a single mesh with one drawing call, improving performance.
 *
 * The wireframes are lines, but they can still be textured.  However, generally
 * you will only want to create a wireframe with the degenerate texture (to draw
 * a solid, colored line).  Hence, none of the static constructors take a texture.
 * You are free to update the texture after creation, if you wish.
 *
 * The node shape is stored as polygon. The wireframe shape is determined by the 
 * polygon traversal.  There are three options, defined in Poly2.
 *
 *     OPEN:     The traversal is in order, but does not close the ends.
 *
 *     CLOSED:   The traversal is in order, and closes the ends.
 *
 *     INTERIOR: The traverse will outline the default triangulation.
 *
 * The default is CLOSED.
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
class CC_DLL WireNode : public TexturedNode {

#pragma mark Internal Helpers
private:
    /** This macro disables the copy constructor (not allowed on scene graphs) */
    CC_DISALLOW_COPY_AND_ASSIGN(WireNode);
    
protected:
    /**
     * Allocate the render data necessary to render this node.
     *
     * The render data for this node is a wireframe for the given
     * traversal.
     */
    void generateRenderData() override;

    
public:
#pragma mark Static Constructors
    /**
     * Creates an empty wireframe node.
     *
     * The underlying polygon is empty, and must be set via setPolygon.
     *
     * @return An autoreleased wireframe node
     */
    static WireNode* create();
    
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
    static WireNode* createWithVertices(float* vertices, int size, int offset=0);
    
    /**
     * Creates a wireframe with the given polygon.
     *
     * The wireframe is a sequence of lines that is determined by the polygon indices.
     * To create a specific traversal, call the traverse() method in Poly2 before
     * assigning it to this Wireframe.
     *
     * @param   poly      The polygon for the wireframe path
     *
     * @return  An autoreleased wireframe node
     */
    static WireNode* createWithPoly(const Poly2& poly);
    
    /**
     * Creates a wireframe with the given rect.
     *
     * The rectangle will be converted into a Poly2, and the traversal is closed.
     *
     * @param   rect    The rectangle for the wireframe path
     *
     * @return  An autoreleased wireframe node
     */
    static WireNode* createWithRect(const Rect& rect);
    
    /** 
     * Creates a wireframe that is a line from origin to destination.
     *
     * @param   origin  The line origin.
     * @param   dest    The line destination.
     *
     * @return  An autoreleased wireframe node
     */
    static WireNode* createWithLine(const Vec2 &origin, const Vec2 &dest);

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
    static WireNode* createWithEllipse(const Vec2& center, const Size& size, unsigned int segments);

    
    
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
     * Creates an empty wireframe without a polygon.
     *
     * The underlying polygon is empty, and must be set via setPolygon.
     */
    WireNode();
    
    /**
     * Releases all resources allocated with this sprite.
     *
     * After this is called, the polygon and drawing commands will be deleted 
     * and no longer safe to use.
     */
    virtual ~WireNode(void) { }
};

NS_CC_END

#endif /* defined(__CU_WIRENODE_H__) */
