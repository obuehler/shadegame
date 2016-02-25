//
//  CUPoly2.h
//  Cornell Extensions to Cocos2D
//
//  This module provides a class that represents a simple polygon.  The purpose of
//  this class is to separate the geometry (and math) of a polygon from the rendering
//  data of a pipeline.  This concept is way too tightly coupled throughout all of
//  Cocos2d, and it makes very simple things extremely complicated. Instead, rendering
//  is pushed to the scene graph nodes (the subclasses of TexturedNode), where it
//  belongs.
//
//  The class supports automatic triangulation when desired.  Triangulation must
//  be called explicitly, because not all applications (e.g. wireframes) want
//  triangulation. This module uses ear-clipping for triangulation, which is faster
//  than poly2tri for our simple applications.
//
//  Math data types are much lighter-weight than other objects, and are intended to
//  be copied.  That is why we do not use reference counting for these objects.
//
//  Author: Walker White
//  Version: 11/15/15
//
#ifndef __CU_POLY2_H__
#define __CU_POLY2_H__

#include <vector>
#include <math/CCGeometry.h>


NS_CC_BEGIN

#pragma mark -
#pragma mark Poly2

/**
 * Class to represent a simple polygon.
 *
 * This class can represent any polygon (including non-convex polygons) that does not
 * have holes or self-interections.  It will not check for holes or self-intersections;
 * those are the responsibility of the programmer.
 *
 * When instantiating this class, the user can provide a set of indices which will be
 * used in rendering.  These indices can either represent a triangulation of the polygon, 
 * or they can represent a traversal (for a wireframe).  The class does not enforce any
 * semantic meaning on these indices.  This class simply stores them.  The semantic
 * meaning is up the appropriate scene graph or graphics class.
 * 
 * This class does provide several methods for automatic index generation.  There is a
 * triangulate() method which will triangulate automatically via an ear-clipping.  
 * algorithm. There is also a traveral method for creating indices for a wireframe.
 */
class CC_DLL Poly2 {
public:
#pragma mark Renderer Enums
    /** Enum for the type of path traversal. */
    enum class Traversal {
        /** Traverse the border, but do not close the ends. */
        OPEN,
        /** Traverse the border, and close the ends. */
        CLOSED,
        /** Traverse the individual triangles in the tesselation. */
        INTERIOR
    };
    /** Enum for the joints of an extruded path. */
    enum class Joint {
        /** No joint; the path will look like a sequence of links */
        NONE,
        /** Mitre joint; ideal for paths with sharp corners */
        MITRE,
        /** Bevel joint; ideal for smoother paths */
        BEVEL,
        /** Round joint; used to smooth out paths with sharp corners */
        ROUND
    };
    /** Enum for the end caps of an extruded path. */
    enum class Cap {
        /** No end cap; the path terminates at the end vertices */
        NONE,
        /** Square cap; like no cap, except the ends are padded by stroke width */
        SQUARE,
        /** Round cap; the ends are half circles whose radius is the stroke width */
        ROUND
    };
    
private:
    /** The vector of vertices in this polygon */
    std::vector<Vec2> _vertices;
    /** The vector of indices in the triangulation */
    std::vector<unsigned short> _indices;
    // CACHED DATA
    /** The bounding box for this polygon */
    Rect _bounds;
    
    /// Helper methods
    /**
     * Compute the bounding box for this polygon.
     *
     * The bounding box is the minimal rectangle that contains all of the vertices in
     * this polygon.  It is recomputed whenever the vertices are set.
     */
    void computeBounds();
    
    
public:
#pragma mark Constructors
    /**
     * Creates an empty polygon.
     *
     * The created polygon has no vertices and no triangulation.  The bounding box is
     * trivial.
     */
    Poly2() { }

    /**
     * Creates a polygon with the given vertices
     *
     * The new polygon has no indices.
     *
     * @param vertices  The vector of vertices (as Vec2) in this polygon
     */
    Poly2(const std::vector<Vec2>& vertices) { set(vertices); }

    /**
     * Creates a polygon with the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * Remember that vertex ii refers to both elements 2*ii and 2*ii+1 in the array of
     * vertices.  This constructor does not verify that the set of indices is valid.
     *
     * @param vertices  The vector of vertices (as Vec2) in this polygon
     * @param indices   The vector of indices for the rendering
     */
    Poly2(const std::vector<Vec2>& vertices, const std::vector<unsigned short>& indices) {
        set(vertices, indices);
    }

    /**
     * Creates a polygon with the given vertices
     *
     * The new polygon has no indices.
     *
     * @param vertices  The vector of vertices (as floats) in this polygon
     */
    Poly2(const std::vector<float>& vertices)    { set(vertices); }
    
    /**
     * Creates a polygon with the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * This constructor does not verify that the set of indices is valid.
     *
     * @param vertices  The vector of vertices (as floats) in this polygon
     * @param indices   The vector of indices for the rendering
     */
    Poly2(const std::vector<float>& vertices, const std::vector<unsigned short>& indices) {
        set(vertices, indices);
    }

    /**
     * Creates a polygon with the given vertices
     *
     * The new polygon has no indices.
     *
     * @param vertices  The array of vertices (as Vec2) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param voffset   The offset in vertices to start the polygon
     */
    Poly2(Vec2* vertices,  int vertsize, int voffset=0) {
        set(vertices, vertsize, voffset);
    }

    /**
     * Creates a polygon with the given vertices
     *
     * The new polygon has no indices.
     *
     * @param vertices  The array of vertices (as floats) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param voffset   The offset in vertices to start the polygon
     */
    Poly2(float* vertices,  int vertsize, int voffset=0) {
        set(vertices, vertsize, voffset);
    }

    /**
     * Creates a polygon with the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * This constructor does not verify that the set of indices is valid.
     *
     * @param vertices  The array of vertices (as Vec2) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param indices   The array of indices for the rendering
     * @param indxsize  The number of elements to use for the indices
     * @param voffset   The offset in vertices to start the polygon
     * @param ioffset   The offset in indices to start from
     */
    Poly2(Vec2* vertices,  int vertsize, unsigned short* indices, int indxsize,
          int voffset=0, int ioffset=0) {
        set(vertices, vertsize, indices, indxsize, voffset, ioffset);
    }

    /**
     * Creates a polygon with the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * This constructor does not verify that the set of indices is valid.
     *
     * @param vertices  The array of vertices (as floats) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param indices   The array of indices for the rendering
     * @param indxsize  The number of elements to use for the indices
     * @param voffset   The offset in vertices to start the polygon
     * @param ioffset   The offset in indices to start from
     */
    Poly2(float* vertices,  int vertsize, unsigned short* indices, int indxsize,
          int voffset=0, int ioffset=0) {
        set(vertices, vertsize, indices, indxsize, voffset, ioffset);
    }

    /**
     * Creates a copy of the given polygon.
     *
     * Both the vertices and the indices are copied.
     *
     * @param poly  The polygon to copy
     */
    Poly2(const Poly2& poly) { set(poly); }
    
    /**
     * Creates a polygon for the given rectangle.
     *
     * The polygon will have four vertices, one for each corner of the rectangle.
     * This optional argument (which is true by default) will initialize the 
     * indices with a triangulation of the rectangle.  This is faster than calling 
     * the triangulate() method directly.
     *
     * @param rect  The rectangle to copy
     * @param index Whether to generate indices for the rect
     */
    Poly2(const Rect& rect, bool index=true) { set(rect,index); }

    /**
     * Deletes the given polygon.
     */
    ~Poly2() { }

     
#pragma mark Initializers
    /**
     * Sets the polygon to have the given vertices
     *
     * The new polygon has no indices.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The vector of vertices (as Vec2) in this polygon
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const std::vector<Vec2>& vertices);

    /**
     * Sets the polygon to have the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * Remember that vertex ii refers to both elements 2*ii and 2*ii+1 in the array of
     * vertices.  This constructor does not verify that the set of indices is valid.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The vector of vertices (as Vec2) in this polygon
     * @param indices   The vector of indices for the rendering
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const std::vector<Vec2>& vertices, const std::vector<unsigned short>& indices);
    
    /**
     * Sets the polygon to have the given vertices
     *
     * The new polygon has no indices.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The vector of vertices (as floats) in this polygon
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const std::vector<float>& vertices);
    
    /**
     * Sets a polygon to have the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * This constructor does not verify that the set of indices is valid.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The vector of vertices (as floats) in this polygon
     * @param indices   The vector of indices for the rendering
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const std::vector<float>& vertices, const std::vector<unsigned short>& indices);
    
    /**
     * Sets the polygon to have the given vertices.
     *
     * The new polygon has no indices.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The array of vertices (as Vec2) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param voffset   The offset in vertices to start the polygon
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(Vec2* vertices, int vertsize, int voffset=0);

    /**
     * Sets the polygon to have the given vertices.
     *
     * The new polygon has no indices.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The array of vertices (as floats) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param voffset   The offset in vertices to start the polygon
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(float* vertices,  int vertsize, int voffset=0) {
        return set((Vec2*)vertices, vertsize/2, voffset/2);
    }
    
    /**
     * Sets the polygon to have the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * This constructor does not verify that the set of indices is valid.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The array of vertices (as Vec2) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param indices   The array of indices for the rendering
     * @param indxsize  The number of elements to use for the indices
     * @param voffset   The offset in vertices to start the polygon
     * @param ioffset   The offset in indices to start from
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(Vec2* vertices, int vertsize, unsigned short* indices, int indxsize,
               int voffset=0, int ioffset=0);
    
    /**
     * Sets the polygon to have the given vertices and indices.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * Remember that, for float representation, vertex ii refers to both elements
     * 2*ii and 2*ii+1 in the array of vertices. This constructor does not verify
     * that the set of indices is valid.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param vertices  The array of vertices (as floats) in this polygon
     * @param vertsize  The number of elements to use from vertices
     * @param indices   The array of indices for the rendering
     * @param indxsize  The number of elements to use for the indices
     * @param voffset   The offset in vertices to start the polygon
     * @param ioffset   The offset in indices to start from
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(float* vertices, int vertsize, unsigned short* indices, int indxsize,
               int voffset=0, int ioffset=0) {
        return set((Vec2*)vertices, vertsize/2, indices, indxsize, voffset/2, ioffset);
    }

    /**
     * Sets this polygon to be a copy of the given one.
     *
     * Both the vertices and the indices are copied. All of the content arrays are
     * copied, so that this polygon does not hold any references to elements of poly.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param poly  The polygon to copy
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const Poly2& poly);
    
    /**
     * Sets the polygon to represent the given rectangle.
     *
     * The polygon will have four vertices, one for each corner of the rectangle.
     * This optional argument (which is true by default) will initialize the
     * indices with a triangulation of the rectangle.  This is faster than calling
     * the triangulate() method directly.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param rect  The rectangle to copy
     * @param index Whether to generate indices for the rect
     *
     * @return This polygon, returned for chaining
     */
    Poly2& set(const Rect& rect, bool index=true);

    /**
     * Sets the polygon to represent a line from origin to dest.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param   origin  The line origin.
     * @param   dest    The line destination.
     *
     * @return This polygon, returned for chaining
     */
    Poly2& setLine(const Vec2& origin, const Vec2& dest);

    /**
     * Sets the polygon to represent an ellipse of the given dimensions.
     *
     * The triangulation will show the boundary, not the circle tesselation.
     *
     * @param   center      The ellipse center point.
     * @param   size        The size of the ellipse.
     * @param   segments    The number of segments to use.
     *
     * @return This polygon, returned for chaining
     */
    Poly2& setEllipse(const Vec2& center, const Size& size, unsigned int segments);

    
#pragma mark Index Generation
    /**
     * Sets the indices for this polygon to the ones given.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * Remember that vertex ii refers to both elements 2*ii and 2*ii+1 in the array
     * vertices.  This constructor does not verify that the set of indices is valid.
     *
     * @param indices   The vector of indices for the rendering
     *
     * @return This polygon, returned for chaining
     */
    Poly2& setIndices(const std::vector<unsigned short>& indices);

    /**
     * Sets the indices for this polygon to the ones given.
     *
     * A valid list of indices must only refer to vertices in the vertex array.
     * Remember that vertex ii refers to both elements 2*ii and 2*ii+1 in the array
     * vertices.  This constructor does not verify that the set of indices is valid.
     *
     * The provided array is copied.  The polygon does not retain a reference.
     * This method returns a reference to this polygon for chaining.
     *
     * @param indices   The array of indices for the rendering
     * @param indxsize  The number of elements to use for the indices
     * @param ioffset   The offset in indices to start from
     *
     * @return This polygon, returned for chaining
     */
    Poly2& setIndices(unsigned short* indices, int indxsize, int ioffset=0);
    
    /**
     * Generates indices from a default triangulation of this polygon.
     *
     * This method uses ear-clipping for triangulation, which is faster
     * than poly2tri for our simple applications.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @return This polygon, returned for chaining
     */
    Poly2& triangulate();
    
    /**
     * Generates indices from a traversal of the polygon vertices.
     *
     * A traversal is useful for creating a wireframe from a polygon.  The type of
     * wireframe generated depends on the traversal value:
     *
     *     OPEN:     The traversal is in order, but does not close the ends.
     *
     *     CLOSED:   The traversal is in order, and closes the ends.
     *
     *     INTERIOR: The traverse will outline the default triangulation.
     *
     * The default is CLOSED_TRAVERSAL.
     *
     * This method returns a reference to this polygon for chaining.
     *
     * @param   traversal The path to traverse for index generation.
     *
     * @return This polygon, returned for chaining
     */
    Poly2& traverse(Traversal traversal=Traversal::CLOSED);
    
    /**
     * Generates a new polygon that is an extrusion of this one.
     *
     * An extrusion of a polygon is a second polygon that follows the path of
     * the first one, but gives it width.  Hence it takes a path and turns it
     * into a solid shape. This is more complicated than simply triangulating
     * the original polygon.  The new polygon has more vertices, depending on 
     * the choice of joint (shape at the corners) and cap (shape at the end).
     *
     * Unlike the traverse option, this method cannot be used to extrude an
     * internal polygon tesselation. It assumes that the path is continuous.
     *
     * CREDITS: This code is ported from the Kivy implementation of Line in package
     * kivy.vertex_instructions.  My belief is that this port is acceptable within
     * the scope of the Kivy license.  There are no specific credits in that file,
     * so there is no one specific to credit.  However, thanks to the Kivy team for
     * doing the heavy lifting on this method.
     *
     * Because they did all the hard work, I will plug their picture of how joints
     * and end caps work:
     *
     *      http://kivy.org/docs/_images/line-instruction.png
     *
     * This method does not allocate a new polygon.  You provide it with a polygon
     * to store the values.  This method returns a reference to that polygon (not
     * the original one) for chaining.
     *
     * @param   poly    The polygon to store the new vertices and indices
     * @param   stroke  The stroke width of the extrusion
     * @param   closed  Whether or not the polygon path is closed
     * @param   joint   The joint shape (default NONE) for corners
     * @param   cap     The cap shape (default NONE) for the two ends
     *
     * @return Reference to the newly initialized polygon
     */
    Poly2* extrude(Poly2& poly, float stroke, bool closed=true, Joint joint=Joint::NONE, Cap cap=Cap::NONE);


#pragma mark Accessors
    /**
     * Returns the list of vertices
     *
     * This accessor will not permit any changes to the vertex array.  To change
     * the array, you must change the polygon via a set() method.
     *
     * @return a reference to the vertex array
     */
    const std::vector<Vec2>& getVertices() const { return _vertices; }
    
    /**
     * Returns a reference to list of indices index.
     *
     * This accessor will not permit any changes to the index array.  To change
     * the array, you must change the polygon via a set() method.
     *
     * @return a reference to the vertex array
     */
    const std::vector<unsigned short>& getIndices() const  { return _indices; }
    
    /**
     * Returns the bounding box for the polygon
     *
     * The bounding box is the minimal rectangle that contains all of the vertices in
     * this polygon.  It is recomputed whenever the vertices are set.
     *
     * @return the bounding box for the polygon
     */
    const Rect& getBounds() const { return _bounds; }
    

#pragma mark Operators
    /**
     * Sets this polygon to be a copy of the given one.
     *
     * All of the contents are copied, so that this polygon does not hold any
     * references to elements of the other polygon. This method returns
     * a reference to this polygon for chaining.
     *
     * @param poly  The polygon to copy
     *
     * @return This polygon, returned for chaining
     */
    Poly2& operator= (const Poly2& other) { return set(other); }
    
    /**
     * Uniformly scales all of the vertices of this polygon.
     *
     * The vertices are scaled from the origin of the coordinate space.  This means
     * that, if the origin is not in the interior of this polygon, the polygon
     * will be effectively translated by the scaling.
     *
     * @param scale The uniform scaling factor
     *
     * @return This polygon, scaled uniformly.
     */
    Poly2& operator*=(float scale);
    
    /**
     * Nonuniformly scales all of the vertices of this polygon.
     *
     * The vertices are scaled from the origin of the coordinate space.  This means
     * that, if the origin is not in the interior of this polygon, the polygon
     * will be effectively translated by the scaling.
     *
     * @param scale The non-uniform scaling factor
     *
     * @return This polygon, scaled non-uniformly.
     */
    Poly2& operator*=(const Vec2& scale);
    
    /**
     * Transforms all of the vertices of this polygon.
     *
     * The vertices are transformed as points; they are treated if they are homongenous
     * vectors with z = 0.
     *
     * @param Mat4 The transform matrix
     *
     * @return This polygon with the vertices transformed
     */
    Poly2& operator*=(const Mat4& transform);
    
    /**
     * Uniformly scales all of the vertices of this polygon.
     *
     * The vertices are scaled from the origin of the coordinate space.  This means
     * that, if the origin is not in the interior of this polygon, the polygon
     * will be effectively translated by the scaling.
     *
     * @param scale The inverse of the uniform scaling factor
     *
     * @return This polygon, scaled uniformly.
     */
    Poly2& operator/=(float scale);
    
    /**
     * Nonuniformly scales all of the vertices of this polygon.
     *
     * The vertices are scaled from the origin of the coordinate space.  This means
     * that, if the origin is not in the interior of this polygon, the polygon
     * will be effectively translated by the scaling.
     *
     * @param scale The inverse of the non-uniform scaling factor
     *
     * @return This polygon, scaled non-uniformly.
     */
    Poly2& operator/=(const Vec2& scale);
    
    /**
     * Uniformly translates all of the vertices of this polygon.
     *
     * The vertices are all shifted by the given amount. Note that this operation
     * has no effect on getTriangles(), as the drawing vertices are always normalized
     * to have an origin relative to the polygon bounding box.
     *
     * @param offset The uniform translation amount
     *
     * @return This polygon, translated uniformly.
     */
    Poly2& operator+=(float offset);
    
    /**
     * Non-uniformly translates all of the vertices of this polygon.
     *
     * The vertices are all shifted by the given amount. Note that this operation
     * has no effect on getTriangles(), as the drawing vertices are always normalized
     * to have an origin relative to the polygon bounding box.
     *
     * @param offset The non-uniform translation amount
     *
     * @return This polygon, translated non-uniformly.
     */
    Poly2& operator+=(const Vec2& offset);
    
    /**
     * Uniformly translates all of the vertices of this polygon.
     *
     * The vertices are all shifted by the given amount. Note that this operation
     * has no effect on getTriangles(), as the drawing vertices are always normalized
     * to have an origin relative to the polygon bounding box.
     *
     * @param offset The inverse of the uniform translation amount
     *
     * @return This polygon, translated uniformly.
     */
    Poly2& operator-=(float offset);
    
    /**
     * Non-uniformly translates all of the vertices of this polygon.
     *
     * The vertices are all shifted by the given amount. Note that this operation
     * has no effect on getTriangles(), as the drawing vertices are always normalized
     * to have an origin relative to the polygon bounding box.
     *
     * @param offset The inverse of the non-uniform translation amount
     *
     * @return This polygon, translated non-uniformly.
     */
    Poly2& operator-=(const Vec2& offset);
    
    /**
     * Returns a new polygon by scaling the vertices uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This means
     * that, if the origin is not in the interior of this polygon, the polygon
     * will be effectively translated by the scaling.
     *
     * @param scale The uniform scaling factor
     *
     * @return The scaled polygon
     */
    Poly2 operator*(float scale) const { return Poly2(*this) *= scale; }
    
    /**
     * Returns a new polygon by scaling the vertices non-uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This means
     * that, if the origin is not in the interior of this polygon, the polygon
     * will be effectively translated by the scaling.
     *
     * @param scale The non-uniform scaling factor
     *
     * @return The scaled polygon
     */
    Poly2 operator*(const Vec2& scale) const { return Poly2(*this) *= scale; }
    
    /**
     * Returns a new polygon by transforming all of the vertices of this polygon.
     *
     * The vertices are transformed as points; they are treated if they are homongenous
     * vectors with z = 0.
     *
     * @param Mat4 The transform matrix
     *
     * @return The transformed polygon
     */
    Poly2 operator*(const Mat4& transform) const { return Poly2(*this) *= transform; }
    
    /**
     * Returns a new polygon by scaling the vertices uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This means
     * that, if the origin is not in the interior of this polygon, the polygon
     * will be effectively translated by the scaling.
     *
     * @param scale The inverse of the uniform scaling factor
     *
     * @return The scaled polygon
     */
    Poly2 operator/(float scale) const { return Poly2(*this) /= scale; }
    
    /**
     * Returns a new polygon by scaling the vertices non-uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This means
     * that, if the origin is not in the interior of this polygon, the polygon
     * will be effectively translated by the scaling.
     *
     * @param scale The inverse of the non-uniform scaling factor
     *
     * @return The scaled polygon
     */
    Poly2 operator/(const Vec2& scale) const { return Poly2(*this) /= scale; }
    
    /**
     * Returns a new polygon by translating the vertices uniformly.
     *
     * The vertices are all shifted by the given amount. Note that this operation
     * has no effect on getTriangles(), as the drawing vertices are always normalized
     * to have an origin relative to the polygon bounding box.
     *
     * @param offset The uniform translation amount
     *
     * @return The translated polygon
     */
    Poly2 operator+(float offset) const { return Poly2(*this) += offset; }
    
    /**
     * Returns a new polygon by translating the vertices non-uniformly.
     *
     * The vertices are all shifted by the given amount. Note that this operation
     * has no effect on getTriangles(), as the drawing vertices are always normalized
     * to have an origin relative to the polygon bounding box.
     *
     * @param offset The non-uniform translation amount
     *
     * @return The translated polygon
     */
    Poly2 operator+(const Vec2& offset) const { return Poly2(*this) += offset; }
    
    /**
     * Returns a new polygon by translating the vertices uniformly.
     *
     * The vertices are all shifted by the given amount. Note that this operation
     * has no effect on getTriangles(), as the drawing vertices are always normalized
     * to have an origin relative to the polygon bounding box.
     *
     * @param offset The inverse of the uniform translation amount
     *
     * @return The translated polygon
     */
    Poly2 operator-(float offset) { return Poly2(*this) -= offset; }
    
    /**
     * Returns a new polygon by translating the vertices non-uniformly.
     *
     * The vertices are all shifted by the given amount. Note that this operation
     * has no effect on getTriangles(), as the drawing vertices are always normalized
     * to have an origin relative to the polygon bounding box.
     *
     * @param offset The inverse of the non-uniform translation amount
     *
     * @return The translated polygon
     */
    Poly2 operator-(const Vec2& offset) { return Poly2(*this) -= offset; }
    
    /**
     * Returns a new polygon by scaling the vertices uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This means
     * that, if the origin is not in the interior of this polygon, the polygon
     * will be effectively translated by the scaling.
     *
     * @param scale The uniform scaling factor
     *
     * @return The scaled polygon
     */
    friend Poly2 operator*(float scale, const Poly2& poly) { return poly*scale; }
    
    /**
     * Returns a new polygon by scaling the vertices non-uniformly.
     *
     * The vertices are scaled from the origin of the coordinate space.  This means
     * that, if the origin is not in the interior of this polygon, the polygon
     * will be effectively translated by the scaling.
     *
     * @param scale The non-uniform scaling factor
     *
     * @return The scaled polygon
     */
    friend Poly2 operator*(const Vec2& scale, const Poly2& poly) { return poly*scale; }
    
    /**
     * Returns a new polygon by translating the vertices uniformly.
     *
     * The vertices are all shifted by the given amount. Note that this operation
     * has no effect on getTriangles(), as the drawing vertices are always normalized
     * to have an origin relative to the polygon bounding box.
     *
     * @param offset The uniform translation amount
     *
     * @return The translated polygon
     */
    friend Poly2 operator+(float offset, const Poly2& poly) { return poly+offset; }
    
    /**
     * Returns a new polygon by translating the vertices non-uniformly.
     *
     * The vertices are all shifted by the given amount. Note that this operation
     * has no effect on getTriangles(), as the drawing vertices are always normalized
     * to have an origin relative to the polygon bounding box.
     *
     * @param offset The non-uniform translation amount
     *
     * @return The translated polygon
     */
    friend Poly2 operator+(const Vec2& offset, const Poly2& poly) { return poly+offset; }
    
};

NS_CC_END
#endif /* defined(__CU_POLY2_H__) */
