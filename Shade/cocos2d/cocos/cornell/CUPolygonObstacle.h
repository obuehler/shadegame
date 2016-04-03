//
//  CUPolygonObstacle.h
//  Cornell Extensions to Cocos2D
//
//  This class implements a polygonal Physics object.  This is different from PolygonNode,
//  which is used for drawing.  This class is substantially more complex than the other
//  physics objects, but it will allow you to draw arbitrary shapes.  Be careful modifying
//  this file as there are a lot of subtleties here.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 11/24/15
//
#ifndef __CU_POLYGON_OBSTACLE_H__
#define __CU_POLYGON_OBSTACLE_H__

#include "CUSimpleObstacle.h"
#include "CUPoly2.h"

NS_CC_BEGIN

#pragma mark -
#pragma mark Polygon Obstacle

/**
 * Arbitrary polygonal-shaped model to support collisions.
 *
 * The polygon can be any one that is representable by a Poly2 object.  That means that
 * it does not need to be convex, but it cannot have holes or self intersections.
 */
class CC_DLL PolygonObstacle : public SimpleObstacle {
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CC_DISALLOW_COPY_AND_ASSIGN(PolygonObstacle);

protected:
    /** The polygon vertices (for resizing) */
    Poly2 _polygon;
    /** Shape information for this physics object */
    b2PolygonShape* _shapes;
    /** A cache value for the fixtures (for resizing) */
    b2Fixture** _geoms;
    /** Anchor point to synchronize with the scene graph */
    Vec2 _anchor;
    /** In case the number of polygons changes */
    int _fixCount;
    
    
#pragma mark -
#pragma mark Scene Graph Methods
    /**
     * Resets the polygon vertices in the shape to match the dimension.
     *
     * This is an internal method and it does not mark the physics object as dirty.
     *
     * @param  size The new dimension (width and height)
     */
    void resize(const Size& size);
    
    /**
     * Performs any necessary additions to the scene graph node.
     *
     * This method is necessary for custom physics objects that are composed
     * of multiple scene graph nodes.
     */
    virtual void resetSceneNode() override;
    
    /**
     * Redraws the outline of the physics fixtures to the debug node
     *
     * The debug node is use to outline the fixtures attached to this object.
     * This is very useful when the fixtures have a very different shape than
     * the texture (e.g. a circular shape attached to a square texture).
     */
    virtual void resetDebugNode() override;
    
    /**
     * Recreates the shape objects attached to this polygon.
     *
     * This must be called whenever the polygon is resized.
     */
    void resetShapes();
    
    
public:
#pragma mark -
#pragma mark Static Constructors
    /**
     * Creates a (not necessarily convex) polygon
     *
     * The anchor point (the rotational center) of the polygon is at the 
     * center of the polygons bounding box.
     *
     * @param poly   The polygon vertices
     *
     * @return  An autoreleased physics object
     */
    static PolygonObstacle* create(const Poly2& poly);
    
    /**
     * Creates a (not necessarily convex) polygon
     *
     * The anchor point (the rotational center) of the polygon is specified as a
     * ratio of the bounding box.  An anchor point of (0,0) is the bottom left of 
     * the bounding box.  An anchor point of (1,1) is the top right of the bounding 
     * box.  The anchor point does not need to be contained with the bounding box.
     *
     * @param  poly     The polygon vertices
     * @param  anchor   The rotational center of the polygon
     *
     * @return  An autoreleased physics object
     */
    static PolygonObstacle* create(const Poly2& poly, const Vec2& anchor);

	/**
	* Creates a (not necessarily convex) polygon with the given collision filter
	*
	* The anchor point (the rotational center) of the polygon is specified as a
	* ratio of the bounding box.  An anchor point of (0,0) is the bottom left of
	* the bounding box.  An anchor point of (1,1) is the top right of the bounding
	* box.  The anchor point does not need to be contained with the bounding box.
	*
	* @param  poly     The polygon vertices
	* @param  anchor   The rotational center of the polygon
	* @param  filter   The collision filter for this obstacle
	*
	* @return  An autoreleased physics object
	*/
	static PolygonObstacle* create(const Poly2& poly, const Vec2& anchor, const b2Filter* const filter);
    
    
#pragma mark -
#pragma mark Dimensions
    /**
     * Returns the dimensions of the bounding box
     *
     * @return the dimensions of the bounding box
     */
    const Size& getSize() const { return _polygon.getBounds().size; }
    
    /**
     * Sets the dimensions of the bounding box
     *
     * The vertices are rescaled according to their vertex origin.
     * This change cannot happen immediately.  It must wait until the
     * next update is called.  The current anchor point will be preserved.
     *
     * @param value  the dimensions of the bounding box
     */
    void setSize(const Size& value)         { resize(value); markDirty(true); }
    
    /**
     * Sets the dimensions of this box
     *
     * @param width   The width of this box
     * @param height  The height of this box
     */
    void setSize(float width, float height) { setSize(Size(width, height)); }
    
    /**
     * Returns the bounding box width
     *
     * @return the bounding box width
     */
    float getWidth() const { return _polygon.getBounds().size.width; }
    
    /**
     * Sets the bounding box width
     *
     * The vertices are rescaled according to their vertex origin.
     * This change cannot happen immediately.  It must wait until the
     * next update is called.  The current anchor point will be preserved.
     *
     * @param value  the bounding box width
     */
    void setWidth(float value) { setSize(value,getHeight()); }
    
    /**
     * Returns the bounding box height
     *
     * @return the bounding box height
     */
    float getHeight() const { return _polygon.getBounds().size.height; }
    
    /**
     * Sets the bounding box height
     *
     * The vertices are rescaled according to their vertex origin.
     * This change cannot happen immediately.  It must wait until the
     * next update is called.  The current anchor point will be preserved.
     *
     * @param value  the bounding box height
     */
    void setHeight(float value) { setSize(getWidth(),value); }
    
    /**
     * Returns the rotational center of this polygon
     *
     * The anchor point of the polygon is specified as ratio of the bounding
     * box.  An anchor point of (0,0) is the bottom left of the bounding box.
     * An anchor point of (1,1) is the top right of the bounding box.  The
     * anchorpoint does not need to be contained with the bounding box.
     *
     * @return the rotational center of this polygon
     */
    const Vec2& getAnchor() const { return _anchor; }
    
    /**
     * Sets the rotational center of this polygon
     *
     * The anchor point of the polygon is specified as ratio of the bounding
     * box.  An anchor point of (0,0) is the bottom left of the bounding box.
     * An anchor point of (1,1) is the top right of the bounding box.  The
     * anchorpoint does not need to be contained with the bounding box.
     *
     * @param value  the rotational center of this polygon
     */
    void setAnchor(const Vec2& value) { setAnchor(value.x, value.y); }
    
    /**
     * Sets the rotational center of this polygon
     *
     * The anchor point of the polygon is specified as ratio of the bounding
     * box.  An anchor point of (0,0) is the bottom left of the bounding box.
     * An anchor point of (1,1) is the top right of the bounding box.  The
     * anchorpoint does not need to be contained with the bounding box.
     *
     * @param x  the x-coordinate of the rotational center
     * @param y  the y-coordinate of the rotational center
     */
    void setAnchor(float x, float y);
    
    /**
     * Returns the polygon defining this object
     *
     * @return the polygon defining this object
     */
    const Poly2& getPolygon() const { return _polygon; }
    
    /**
     * Sets the polygon defining this object
     *
     * This change cannot happen immediately.  It must wait until the
     * next update is called.  The current anchor point will be preserved.
     *
     * @param value   the polygon defining this object
     */
    void setPolygon(const Poly2&);
    
    
#pragma mark -
#pragma mark Physics Methods
    
    /**
     * Create new fixtures for this body, defining the shape
     *
     * This is the primary method to override for custom physics objects
     */
    virtual void createFixtures() override;
    
    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects
     */
    virtual void releaseFixtures() override;
    
    
#pragma mark -
#pragma mark Initializers
CC_CONSTRUCTOR_ACCESS:
    /**
     * Creates an empty polygon at the origin.
     *
     * This obstacle has a position, but no shape
     */
    PolygonObstacle(void) : SimpleObstacle(), _shapes(nullptr), _geoms(nullptr) { }
    
    /**
     * Deletes this physics object and all of its resources.
     *
     * A non-default destructor is necessary since we must release all
     * the fixture pointers for the polygons.
     */
    virtual ~PolygonObstacle();

    // Turn off init warnings
    using SimpleObstacle::init;

    /**
     * Initializes a (not necessarily convex) polygon
     *
     * The anchor point (the rotational center) of the polygon is at the
     * center of the polygons bounding box.
     *
     * @param poly   The polygon vertices
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Poly2& poly) { return init(poly,Vec2(0.5,0.5), nullptr); }
    
    /**
     * Initializes a (not necessarily convex) polygon
     *
     * The anchor point (the rotational center) of the polygon is specified as a
     * ratio of the bounding box.  An anchor point of (0,0) is the bottom left of
     * the bounding box.  An anchor point of (1,1) is the top right of the bounding
     * box.  The anchor point does not need to be contained with the bounding box.
     *
     * @param  poly     The polygon vertices
     * @param  anchor   The rotational center of the polygon
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
	virtual bool init(const Poly2& poly, const Vec2& anchor) { return init(poly, anchor, nullptr); }

	/**
	* Initializes a (not necessarily convex) polygon with the given collision filter.
	*
	* The anchor point (the rotational center) of the polygon is specified as a
	* ratio of the bounding box.  An anchor point of (0,0) is the bottom left of
	* the bounding box.  An anchor point of (1,1) is the top right of the bounding
	* box.  The anchor point does not need to be contained with the bounding box.
	*
	* @param  poly     The polygon vertices
	* @param  anchor   The rotational center of the polygon
	* @param  filter   The collision filter for this obstacle
	*
	* @return  true if the obstacle is initialized properly, false otherwise.
	*/
	virtual bool init(const Poly2& poly, const Vec2& anchor, const b2Filter* const filter);
    
};

NS_CC_END
#endif /* defined(__CU_POLYGON_OBSTACLE_H__) */