//
//  CUCubicSpline.h
//  Cornell Extensions to Cocos2D
//
//  This module provides a class that represents a spline of cubic beziers. A
//  bezier spline is just a sequence of beziers joined together, so that the end
//  of one is the beginning of the other. Cubic beziers have four control points,
//  two for the vertex anchors and two for their tangents.
//
//  Cocos2d does have a lot of spline support.  In addition to cubic beziers, it
//  also has quadratic beziers, cardinal splines, and Catmull-Rom splines.  However,
//  in true Cocos2d fashion, these are all tightly coupled with drawing code.  We do
//  not want splines embedded with drawing code.  We want a mathematics object that
//  we can adjust and compute with. Hence the purpose of this class.  We chose
//  cubic splines because they are the most natural for editting (as seen in
//  Adobe Illustrator).
//
//  If you want to draw a CubicSpline, use the allocPath() method to allocate a Poly2
//  value for the spline.  We have to turn shapes into polygons to draw them anyway,
//  and this allows us to do all of the cool things we can already do with paths,
//  like extrude them or create wireframes.
//
//  Math data types are much lighter-weight than other objects, and are intended to
//  be copied.  That is why we do not use reference counting for these objects.
//
//  Author: Walker White
//  Version: 11/24/15
//
#ifndef __CU_CUBIC_SPLINE_H__
#define __CU_CUBIC_SPLINE_H__

#include "CUPoly2.h"

NS_CC_BEGIN

class Polynomial;

/** The default tolerance for the polygon approximation functions */
#define DEFAULT_TOLERANCE   0.25

#pragma mark -
#pragma mark CubicSpline

/**
 * Class to represent a spline of quadratic beziers.
 *
 * A bezier spline is a sequence of beziers, where the start of one is the begining of 
 * the other.  A bezier spline may be open or closed. In a closed spline, the end of
 * the last bezier is the beginning of the first (or in the case of a degenerate 
 * bezier, a bezier with the same beginning and end).
 *
 * A single cubic bezier is represented by the four points.  There are the two anchor 
 * points P1 and P2.  These represent the start and end of the curve.  In addition, each 
 * of these points has a tangent: T1 and T2.  The curve is defined so that P1 has a right
 * tangent of T1, and P2 has a left tangent of T2.  The tangents themselves are given
 * as points, not vectors (so the tangent vector is Tn-Pn). These four points are known 
 * as the control points. When we represent a bezier, we typically represent it as a list 
 * of four points in this order: P1, T1, T2, and P2.
 *
 * In a bezier spline, the first anchor point of the next curve is the same as the last 
 * anchor point of the previous one.  There is no need to duplicate this information.
 * However, the tanget is not a duplicate, since anchor points on the interior of the
 * spline have both a left and right tangent.  Therefore, the control point list always
 * has two tangents between any two anchors. Thus bezier spline of n beziers will
 * contain 3n+1 control points.
 *
 * This class does not contain any drawing functionality at all.  If you wish to
 * draw a bezier, create a Poly2 approximation with the allocPath() method.  This
 * method creates a line-segment approximation of the bezier, in much the same way
 * that we approximate circles or ellipses when drawing them.  You can then use the
 * many features for drawing a Poly2 object, such as extrusion or wireframes.
 *
 * This class has a lot of advanced methods to detect the nearest anchor, tanget,
 * or curve location to a point. These are designed so that you can edit a bezier
 * in a level editor for you game. These methods determine the part of the bezier
 * closest to you mouse location, so that you can select and edit them.
 */
class CC_DLL CubicSpline {
protected:
    
    /** The number of segments in this spline */
    int _size;
    
    /**
     * The defining control points of this spline (both anchor points and tangents).
     *
     * The number of elements in this array is 6*size+2. Each point is an adjacent pair in
     * the array, and each segment has four points (anchor, tangent, tangent, anchor).  The
     * first and last anchor of each segment is shared and not repeated.
     */
    std::vector<Vec2> _points;
    
    /** For each anchor point in the spline, whether it is a smooth point or a hinge point. */
    std::vector<bool>  _smooth;
    
    /** Whether or not the spline is closed.  This effects editting and polygon approximation */
    bool _closed;

    
public:
    /** 
     * Termination criteria for de Castlejau's recursive subdivision
     *
     * This is used by the polygon approximation functions.  To convert a bezier
     * into a polygon, we recursively subdivide the bezier until we reach the
     * terminal condition.  We then use the anchor points of the subdivided
     * bezier to define our polygon.
     */
    enum Criterion {
        /**
         * The FLAT termination criterion.
         *
         * It guarantees a limit on the flatness (which in this context means the
         * distance from the curve to the polygon divided by point spacing).
         */
        FLAT,
        
        /**
         * The DISTANCE termination criterion.  
         *
         * It guarantees that the curve lies within a certain distance from the 
         * polygon defined by the points.
         */
        DISTANCE,
        
        /**
         * The SPACING termination criterion .  
         * 
         * It guarantees that the points will be less than a certain distance apart.
         */
        SPACING
    };

    
#pragma mark Constructors
    /**
     * Creates an empty spline.
     *
     * The minimum spline possible has 4 points: two anchors and two tangents.
     * This sets them all to (0,0).  As a degenerate spline, it is closed.
     */
    CubicSpline() : CubicSpline(Vec2::ZERO,Vec2::ZERO)    {   }
    
    /**
     * Creates a degenerate spline of one point
     *
     * The minimum spline possible has 4 points: two anchors and two tangents.
     * This sets them all to (x,y). As a degenerate spline, it is closed.
     *
     * @param  point    The bezier anchor point
     * @param  y    The y-coordinate of the point
     */
    CubicSpline(const Vec2& point) : CubicSpline(point,point)  {    }
    
    /**
     * Creates a spline of two points
     *
     * The minimum spline possible has 4 points: two anchors and two tangents.
     * This sets the start to be the first anchor point, and end to be the
     * second.  The tangents, are the same as the anchor points, which means
     * that the tangents are degenerate.  This has the effect of making the
     * bezier a straight line from start to end. The spline is open, unless
     * start and end are the same.
     *
     * @param  start    The first bezier anchor point
     * @param  end      The second bezier anchor point
     */
    CubicSpline(const Vec2& start, const Vec2& end);

    /**
     * Creates a spline from the given control points.
     *
     * The control points must be specified in the form
     *
     *      anchor, tangent, tangent, anchor, tangent ... anchor
     *
     * That is, starts and ends with anchors, and every two anchors have two
     * tangents (right of the first, left of the second) in between. As each
     * point is two floats, the value size must be equal to 2 mod 6.
     *
     * The created spline is open.
     *
     * @param  points   The array of control points as floats
     * @param  offset   The starting offset in the control point array
     * @param  size     The number of floats to use in the array
     */
    CubicSpline(const float* points, int size, int offset=0);

    /**
     * Creates a spline from the given control points.
     *
     * The control points must be specified in the form
     *
     *      anchor, tangent, tangent, anchor, tangent ... anchor
     *
     * That is, starts and ends with anchors, and every two anchors have two
     * tangents (right of the first, left of the second) in between. As each
     * point is two floats, the size of the vector must be equal to 2 mod 6.
     *
     * The created spline is open.
     *
     * @param  points   The vector of control points as floats
     */
    CubicSpline(const std::vector<float>& points);

    /**
     * Creates a spline from the given control points.
     *
     * The control points must be specified in the form
     *
     *      anchor, tangent, tangent, anchor, tangent ... anchor
     *
     * That is, starts and ends with anchors, and every two anchors have two
     * tangents (right of the first, left of the second) in between. The
     * size of this vector must be equal to 1 mod 3.
     *
     * The created spline is open.
     *
     * @param  points   The vector of control points
     */
    CubicSpline(const std::vector<Vec2>& points);

    /**
     * Creates a copy of the given spline.
     *
     * @param  spline   The spline to copy
     */
    CubicSpline(const CubicSpline& spline);

    /**
     * Deletes this spline releasing all memory
     *
     * This spline does not retain ownership of any Poly2 allocated by
     * an alloc method. It is the callers responsibility to release
     * this memory when done with it.
     */
    ~CubicSpline() { }


#pragma mark Attribute Accessors
    /**
     * Returns the number of segments in this spline 
     *
     * Each segment is a bezier. To use the bezier methods associated with this
     * class, you will need to know the correct segment.
     *
     * @return the number of segments in this spline
     */
    int getSize() const { return _size; }
    
    /**
     * Returns true if the spline is closed.
     *
     * A closed spline is one where the first and last anchor are the same.
     * Hence the first and last tangents are tangents (right, and left, 
     * respectively) of the same point.  This is relevant for the setTangent()
     * method, particularly if the change is meant to be symmetric.
     *
     * A closed spline has no end. Therefore, anchors cannot be added to
     * a closed spline.  They may only be inserted between two other
     * anchors.
     *
     * @return true if the spline is closed
     */
    bool getClosed() const { return _closed; }
    
    /**
     * Sets whether the spline is closed.
     *
     * A closed spline is one where the first and last anchor are the same.
     * Hence the first and last tangents are tangents (right, and left,
     * respectively) of the same point.  This is relevant for the setTangent()
     * method, particularly if the change is meant to be symmetric.
     *
     * A closed spline has no end. Therefore, anchors cannot be added to
     * a closed spline.  They may only be inserted between two other
     * anchors.
     *
     * @param flag whether the spline is closed
     */
    void setClosed(bool flag);
    
    /**
     * Returns the spline point for parameter tp.
     *
     * A bezier spline is a parameterized curve. For a single bezier, it is
     * parameterized with tp in 0..1, with tp = 0 representing the first
     * anchor and tp = 1 representing the second. In the spline, we generalize
     * this idea, where tp is an anchor if it is an int, and is inbetween
     * the anchors floor(tp) and ceil(tp) otherwise.
     *
     * @param  tp   the parameterization value
     *
     * @return the spline point for parameter tp
     */
    Vec2 getPoint(float tp) const { return getPoint((int)tp,tp-(int)tp); }

    /**
     * Sets the spline point at parameter tp.
     *
     * A bezier spline is a parameterized curve. For a single bezier, it is
     * parameterized with tp in 0..1, with tp = 0 representing the first
     * anchor and tp = 1 representing the second. In the spline, we generalize
     * this idea, where tp is an anchor if it is an int, and is inbetween
     * the anchors floor(tp) and ceil(tp) otherwise.
     *
     * In this method, if tp is an int, it will just reassign the associated
     * anchor value.  Otherwise, this will insert a new anchor point at
     * that parameter.  This has a side-effect of changing the parameterization
     * values for the curve, as the number of beziers has increased.
     *
     * @param  tp       the parameterization value
     * @param  point    the new value to assign
     */
    void setPoint(float tp, const Vec2& point);
    
    /**
     * Returns the anchor point at the given index.
     *
     * If an open spline has n segments, then it has n+1 anchors. Similiarly,
     * a closed spline had n anchors.  The value index should be in the
     * appropriate range.
     *
     * @param  index    the anchor index (0..n+1 or 0..n)
     *
     * @return the anchor point at the given index.
     */
    Vec2 getAnchor(int index) const;
    
    /**
     * Sets the anchor point at the given index.
     *
     * This method will change both the anchor and its associated tangets.  
     * The new tangents will have the same relative change in position.
     * As a result, the bezier will still have the same shape locally.
     * This is the natural behavior for changing an anchor, as seen in
     * Adobe Illustrator.
     *
     * If an open spline has n segments, then it has n+1 anchors. Similiarly,
     * a closed spline had n anchors.  The value index should be in the
     * appropriate range.
     *
     * @param  index    the anchor index (0..n+1 or 0..n)
     * @param  point    the new value to assign
     */
    void setAnchor(int index, const Vec2& point);

    /** 
     * Returns the smoothness for the anchor point at the given index.
     *
     * A smooth anchor is one in which the derivative of the curve at the
     * anchor is continuous.  Practically, this means that the left and
     * right tangents are always parallel.  Only a non-smooth anchor may
     * form a "hinge".
     *
     * If an open spline has n segments, then it has n+1 anchors. Similiarly,
     * a closed spline had n anchors.  The value index should be in the
     * appropriate range.
     *
     * @param  index    the anchor index (0..n+1 or 0..n)
     *
     * @return the smoothness for the anchor point at the given index.
     */
    bool getSmooth(int index) const;
    
    /**
     * Sets the smoothness for the anchor point at the given index.
     *
     * A smooth anchor is one in which the derivative of the curve at the
     * anchor is continuous.  Practically, this means that the left and
     * right tangents are always parallel.  Only a non-smooth anchor may
     * form a "hinge".
     *
     * If you set a non-smooth anchor to smooth, it will adjust the
     * tangents accordingly.  In particular, it will average the two
     * tangents, making them parallel. This is the natural behavior for
     * changing an smoothness, as seen in Adobe Illustrator.
     *
     * If an open spline has n segments, then it has n+1 anchors. Similiarly,
     * a closed spline had n anchors.  The value index should be in the
     * appropriate range.
     *
     * @param  index    the anchor index (0..n+1 or 0..n)
     * @param  flag     the anchor smoothness
     *
     * @return the smoothness for the anchor point at the given index.
     */
    void setSmooth(int index, bool flag);

    /**
     * Returns the tangent at the given index.
     *
     * Tangents are specified as points, not vectors.  To get the tangent
     * vector for an anchor, you must subtract the anchor from its tangent
     * point.  Hence a curve is degenerate when the tangent and the
     * anchor are the same.
     *
     * If a spline has n segments, then it has 2n tangents. This is true
     * regardless of whether it is open or closed. The value index should 
     * be in the appropriate range. An even index is a right tangent, 
     * while an odd index is a left tangent. If the spline is closed, then
     * 2n-1 is the left tangent of the first point.
     *
     * @param  index    the tangent index (0..2n)
     *
     * @return the tangent at the given index.
     */
    Vec2 getTangent(int index) const;

    /**
     * Sets the tangent at the given index.
     *
     * Tangents are specified as points, not vectors.  To get the tangent
     * vector for an anchor, you must subtract the anchor from its tangent
     * point.  Hence a curve is degenerate when the tangent and the
     * anchor are the same.
     *
     * If the associated anchor point is smooth, changing the direction
     * of the tangent vector will also change the direction of the other
     * tangent vector (so that they remain parallel).  However, changing
     * only the magnitude will have no effect, unless symmetric is true.
     * In that case, it will modify the other tangent so that it has the
     * same magnitude and parallel direction. This is the natural behavior
     * for changing a tangent, as seen in Adobe Illustrator.
     *
     * If a spline has n segments, then it has 2n tangents. This is true
     * regardless of whether it is open or closed. The value index should
     * be in the appropriate range. An even index is a right tangent,
     * while an odd index is a left tangent. If the spline is closed, then
     * 2n-1 is the left tangent of the first point.
     *
     * @param  index     the tangent index (0..2n)
     * @param  tang      the new value to assign
     * @param  symmetric whether to make the other tangent symmetric
     */
    void setTangent(int index, const Vec2& tang, bool symmetric=false);
    
    /**
     * Returns the x-axis bezier polynomial for the given segment.
     *
     * Bezier polynomials define the curve parameterization. They are
     * two dimension polynomials that give a point.  Rather than
     * extend polynomial to support multidimensional data, we extract
     * each axis separately.
     *
     * We also cannot define a single polynomial for the entire spline, 
     * but we can do it for each segment.  The result is a cubic poly,
     * hence the name CubicSpline.
     *
     * @return the x-axis bezier polynomial for the given segment.
     */
    Polynomial getPolynomialX(int segment) const;

    /**
     * Returns the y-axis bezier polynomial for the given segment.
     *
     * Bezier polynomials define the curve parameterization. They are
     * two dimension polynomials that give a point.  Rather than
     * extend polynomial to support multidimensional data, we extract
     * each axis separately.
     *
     * We also cannot define a single polynomial for the entire spline,
     * but we can do it for each segment.  The result is a cubic poly,
     * hence the name CubicSpline.
     *
     * @return the y-axis bezier polynomial for the given segment.
     */
    Polynomial getPolynomialY(int segment) const;

    /**
     * Returns the spline control points.
     *
     * If the spline has n segments, then the list will have 6n+2 elements 
     * in it, representing the n+1 anchor points and the 2n tangents.
     * The values will alternate
     *
     *      anchor, tangent, tangent, anchor, tangent ... anchor
     *
     * This is true even if the curve is closed.  In that case, the
     * first and last anchor points will be the same.
     *
     * @return the spline control points.
     */
    const std::vector<Vec2> getControlPoints() const { return _points; }

    
#pragma mark Anchor Editting Methods
    /**
     * Adds the given point to the end of the spline, creating a new segment.
     *
     * The new segment will start at the previous end of the last spline and
     * extend to the given point. As closed splines have no end, this method
     * will fail on closed beziers. You should use insertAnchor instead for
     * closed beziers.
     *
     * This version of the method adds a degenerate tangent point for the
     * new anchor.
     *
     * @param  point    the new anchor point to add to the end
     *
     * @return the new number of segments in this spline
     */
    int addAnchor(const Vec2& point) { return addAnchor(point,point); }

    /**
     * Adds the given point to the end of the spline, creating a new segment.
     *
     * The new segment will start at the previous end of the last spline and
     * extend to the given point. As closed splines have no end, this method
     * will fail on closed beziers. You should use insertAnchor instead for
     * closed beziers.
     *
     * This value tang is the left tangent of the new anchor point.
     *
     * @param  point    the new anchor point to add to the end
     * @param  tang     the left tangent of the new anchor point
     *
     * @return the new number of segments in this spline
    */
    int addAnchor(const Vec2& point, const Vec2& tang);
    
    /**
     * Deletes the anchor point at the given index.
     *
     * The point is deleted as well as both of its tangents (left and right).
     * All remaining anchors after the deleted one will shift their indices 
     * down by one. Deletion is allowed on closed splines; the spline will
     * remain closed after deletion.
     *
     * If an open spline has n segments, then it has n+1 anchors. Similiarly,
     * a closed spline had n anchors.  The value index should be in the
     * appropriate range.
     *
     * @param  index    the anchor index to delete
     */
    void deleteAnchor(int index);
    
    /**
     * Inserts a new anchor point at parameter tp.
     *
     * Inserting an anchor point does not change the curve.  It just makes
     * an existing point that was not an anchor, now an anchor. This is the
     * natural behavior for inserting an index, as seen in Adobe Illustrator.
     *
     * A bezier spline is a parameterized curve. For a single bezier, it is
     * parameterized with tp in 0..1, with tp = 0 representing the first
     * anchor and tp = 1 representing the second. In the spline, we generalize
     * this idea, where tp is an anchor if it is an int, and is inbetween
     * the anchors floor(tp) and ceil(tp) otherwise.
     *
     * The tangents of the new anchor point will be determined by de Castlejau's.
     * This is the natural behavior for inserting an anchor mid bezier, as seen 
     * in Adobe Illustrator.
     *
     * @param  tp       the parameterization value
     */
    void insertAnchor(float param)  { insertAnchor((int)param,param-(int)param); }
    

#pragma mark Nearest Point Methods
    /**
     * Returns the nearest point on the spline to the given point.
     *
     * The value is effectively the projection of the point onto the curve.  We
     * compute this point using the projection polynomial, described at 
     *
     * http://jazzros.blogspot.com/2011/03/projecting-point-on-bezier-curve.html
     *
     * The point returned does not need to be an anchor point.  It can be anywhere
     * on the curve.  This allows us a way to select a non-anchor point with the
     * mouse (such as to add a new anchor point) in a level editor or other
     * program.
     *
     * @param  point    the point to project
     *
     * @return the nearest point on the spline to the given point.
     */
    Vec2 nearestPoint(const Vec2& point) const { return getPoint(nearestParameter(point)); }

    /**
     * Returns the parameterization of the nearest point on the spline.
     *
     * The value is effectively the projection of the point onto the parametrized 
     * curve. See getPoint() for an explanation of how the parameterization work. We
     * compute this value using the projection polynomial, described at
     *
     * http://jazzros.blogspot.com/2011/03/projecting-point-on-bezier-curve.html
     *
     * @param  point    the point to project
     *
     * @return the parameterization of the nearest point on the spline.
     */
    float nearestParameter(const Vec2& point) const;
    
    /**
     * Returns the index of the anchor nearest the given point.
     *
     * If there is no anchor whose distance to point is less than the square root
     * of threshold (we use lengthSquared for speed), then this method returns -1.
     *
     * @param  point        the point to compare
     * @param  threshold    the distance threshold for picking an anchor
     *
     * @return the index of the anchor nearest the given point.
     */
    int nearestAnchor(const Vec2& point, float threshold) const;

    /**
     * Returns the index of the tangent nearest the given point.
     *
     * If there is no tangent whose distance to point is less than the square root
     * of threshold (we use lengthSquared for speed), then this method returns -1.
     *
     * @param  point        the point to compare
     * @param  threshold    the distance threshold for picking a tangent
     *
     * @return the index of the tangent nearest the given point.
     */
    int nearestTangent(const Vec2& point, float threshold) const;

    
#pragma mark Polygon Approximation
    /**
     * Returns a list of vertices approximating this spline
     *
     * A polygon approximation is creating by recursively calling de Castlejau's
     * until we reach a stopping condition.  The stopping condition is determined
     * by the Criterion.  See that enum for a description of how the various
     * stopping conditions work.  The tolerance is the value associated with
     * the condition.  For example, for condition DISTANCE, tolerance is how
     * far the point can be away from the true curve.
     *
     * @param  tolerance    the error tolerance of the stopping condition
     * @param  criterion    the stopping condition criterion
     *
     * @return a list of vertices approximating this spline
     */
    std::vector<Vec2> approximate(float tolerance=DEFAULT_TOLERANCE,
                             Criterion criterion=Criterion::DISTANCE) const;

    /**
     * Returns a list of parameters for a polygon approximation
     *
     * A polygon approximation is creating by recursively calling de Castlejau's
     * until we reach a stopping condition.  The stopping condition is determined
     * by the Criterion.  See that enum for a description of how the various
     * stopping conditions work.  The tolerance is the value associated with
     * the condition.  For example, for condition DISTANCE, tolerance is how
     * far the point can be away from the true curve.
     *
     * Note that de Castlejau's changes the number of segments, hence changing
     * the way parameterization works. This parameter list is with respect to 
     * the original spline.  These parameters can be plugged into the method
     * getPoint() to retrieve the associated point.
     *
     * @param  tolerance    the error tolerance of the stopping condition
     * @param  criterion    the stopping condition criterion
     *
     * @return a list of parameters for a polygon approximation
     */
    std::vector<float> approximateParameters(float tolerance=DEFAULT_TOLERANCE,
                                       Criterion criterion=Criterion::DISTANCE) const;
    
    /**
     * Returns a list of tangent points for a polygon approximation
     *
     * A polygon approximation is creating by recursively calling de Castlejau's
     * until we reach a stopping condition.  The stopping condition is determined
     * by the Criterion.  See that enum for a description of how the various
     * stopping conditions work.  The tolerance is the value associated with
     * the condition.  For example, for condition DISTANCE, tolerance is how
     * far the point can be away from the true curve.
     *
     * These tangents are presented in control point order.  First, we have
     * the right tangent of the first point, then the left tangent of the 
     * second point, then the right, and so on.  If approximate() returned
     * n points, this method will return 2(n-1) tangents.
     *
     * @param  tolerance    the error tolerance of the stopping condition
     * @param  criterion    the stopping condition criterion
     *
     * @return a list of tangent points for a polygon approximation
     */
    std::vector<Vec2> approximateTangents(float tolerance=DEFAULT_TOLERANCE,
                                     Criterion criterion=Criterion::DISTANCE) const;
    
    /**
     * Returns a list of normals for a polygon approximation
     *
     * A polygon approximation is creating by recursively calling de Castlejau's
     * until we reach a stopping condition.  The stopping condition is determined
     * by the Criterion.  See that enum for a description of how the various
     * stopping conditions work.  The tolerance is the value associated with
     * the condition.  For example, for condition DISTANCE, tolerance is how
     * far the point can be away from the true curve.
     *
     * There is one normal per point. If approximate() returned n points, this 
     * method will also return n normals. The normals are determined by the
     * right tangents.  If the spline is open, then the normal of the last point
     * is determined by its left tangent.
     *
     * @param  tolerance    the error tolerance of the stopping condition
     * @param  criterion    the stopping condition criterion
     *
     * @return a list of normals for a polygon approximation
     */
    std::vector<Vec2> approximateNormals(float tolerance=DEFAULT_TOLERANCE,
                                    Criterion criterion=Criterion::DISTANCE) const;

    /**
     * Returns an expanded version of this spline
     *
     * When we use de Castlejau's to approximate the spline, it produces a list
     * of control points that are geometrically equal to this spline (e.g. ignoring 
     * parameterization). Instead of flattening this information to a polygon,
     * this method presents this data as a new spline.
     *
     * @param  tolerance    the error tolerance of the stopping condition
     * @param  criterion    the stopping condition criterion
     *
     * @return an expanded version of this spline
     */
    CubicSpline refine(float tolerance=DEFAULT_TOLERANCE,
                      Criterion criterion=Criterion::DISTANCE) const;
    
#pragma mark Rendering Data
    /**
     * Allocates a new polygon approximating this spline
     *
     * A polygon approximation is creating by recursively calling de Caslejau's
     * until we reach a stopping condition.  The stopping condition is determined
     * by the Criterion.  See that enum for a description of how the various
     * stopping conditions work.  The tolerance is the value associated with
     * the condition.  For example, for condition DISTANCE, tolerance is how 
     * far the point can be away from the true curve.
     *
     * The polygon will have either an OPEN or CLOSED traversal, depending on
     * whether the polygon is open or closed.
     *
     * This method allocates memory and releases ownership to the caller. This
     * decision was made because copying Poly2 is expensive (it needs exclusive
     * ownership of its data in order to be safe for the rendering pipline). It
     * is the responsibility of the caller to delete the polygon when done. As
     * TexturedNode and its subclasses copy the polygon before drawing, it is
     * safe to delete the polygon immediately after assigning it to a node.
     *
     * @param  tolerance    the error tolerance of the stopping condition
     * @param  criterion    the stopping condition criterion
     *
     * @return a newly allocated polygon approximating this spline
     */
    Poly2* allocPath(float tolerance=DEFAULT_TOLERANCE,
                     Criterion criterion=Criterion::DISTANCE) const;

    /**
     * Allocates lines for the anchor tangent vectors
     *
     * This method returns a vertex information for the tangent lines to this
     * spline. These are only the tangent lines for the anchors, not all of
     * the points on an approximated path.  This information may be passed
     * to a WireNode to provide a visual representation of the tangents (as
     * seen in Adobe Illustrator).
     *
     * This method allocates memory and releases ownership to the caller. This
     * decision was made because copying Poly2 is expensive (it needs exclusive
     * ownership of its data in order to be safe for the rendering pipline). It
     * is the responsibility of the caller to delete the polygon when done. As
     * TexturedNode and its subclasses copy the polygon before drawing, it is
     * safe to delete the polygon immediately after assigning it to a node.
     *
     * @return a newly set of lines for the anchor tangent vectors
     */
    Poly2* allocTangents() const;

    /**
     * Allocates handles for the anchor points
     *
     * This method returns a vertex information for handles at the anchor
     * points.  Handles are circular shapes of a given radius. This information
     * may be passed to a PolygonNode to provide a visual representation of the
     * anchor points (as seen in Adobe Illustrator).
     *
     * This method allocates memory and releases ownership to the caller. This
     * decision was made because copying Poly2 is expensive (it needs exclusive
     * ownership of its data in order to be safe for the rendering pipline). It
     * is the responsibility of the caller to delete the polygon when done. As
     * TexturedNode and its subclasses copy the polygon before drawing, it is
     * safe to delete the polygon immediately after assigning it to a node.
     *
     * @param  radius   the radius of each handle
     * @param  segments the number of segments in the handle "circle"
     *
     * @return a newly set of lines for the anchor tangent vectors
     */
    Poly2* allocAnchors(float radius, int segments=4) const;

    /**
     * Allocates handles for the tangent points
     *
     * This method returns a vertex information for handles at the tangent
     * points.  Handles are circular shapes of a given radius. This information
     * may be passed to a PolygonNode to provide a visual representation of the 
     * tangent points (as seen in Adobe Illustrator).
     *
     * This method allocates memory and releases ownership to the caller. This
     * decision was made because copying Poly2 is expensive (it needs exclusive
     * ownership of its data in order to be safe for the rendering pipline). It
     * is the responsibility of the caller to delete the polygon when done. As
     * TexturedNode and its subclasses copy the polygon before drawing, it is
     * safe to delete the polygon immediately after assigning it to a node.
     *
     * @param  radius   the radius of each handle
     * @param  segments the number of segments in the handle "circle"
     *
     * @return a newly set of lines for the anchor tangent vectors
     */
    Poly2* allocHandles(float radius, int segments=4) const;


#pragma mark Internal Helpers
protected:
    /**
     * Returns the spline point for parameter tp.
     *
     * This method is like the public getPoint(), except that it is restricted
     * to a single bezier segment.  A bezier is parameterized with tp in 0..1,
     * with tp = 0 representing the first anchor and tp = 1 representing the
     * second. This method is used by the public getPoint() to compute its value.
     *
     * @param  segment  the bezier segment to select from
     * @param  tp       the parameterization value
     *
     * @return the spline point for parameter tp
     */
    Vec2 getPoint(int segment, float tp) const;
    
    /**
     * Inserts a new anchor point at parameter tp.
     *
     * Inserting an anchor point does not change the curve.  It just makes
     * an existing point that was not an anchor, now an anchor. This is the
     * natural behavior for inserting an index, as seen in Adobe Illustrator.
     *
     * This version of insertAnchor() specifies the segment for insertion,
     * simplifying the parameterization. For a single bezier, it is
     * parameterized with tp in 0..1, with tp = 0 representing the first
     * anchor and tp = 1 representing the second.
     *
     * The tangents of the new anchor point will be determined by de Castlejau's.
     * This is the natural behavior for inserting an anchor mid bezier, as seen
     * in Adobe Illustrator.
     *
     * @param  segment  the bezier segment to insert into
     * @param  tp       the parameterization value
     */
    void insertAnchor(int segment, float param);
    
    /**
     * Applies de Castlejau's to the given segment, putting the result in left & right
     *
     * de Castlejau's takes a parameter tp in (0,1) and splits the bezier into two,
     * preserving the geometric information, but not the parameterization.  The control
     * points for the resulting two beziers are stored in left and right.
     *
     * @param  segment  the bezier segment of this spine
     * @param  tp       the parameter to split at
     * @param  left     vector to store the left bezier
     * @param  right    vector to store the right bezier
     */
    void subdivide(int segment, float tp, std::vector<Vec2>& left, std::vector<Vec2>& rght) const {
        subdivide(_points,6*segment,tp,left,rght);
    }

    /**
     * Applies de Castlejau's to a bezier, putting the result in left & right
     *
     * de Castlejau's takes a parameter tp in (0,1) and splits the bezier into two,
     * preserving the geometric information, but not the parameterization.  The control
     * points for the resulting two beziers are stored in left and right.
     *
     * This static method is not restricted to the current spline.  It can work
     * from any list of control points (and offset into those control points).
     * This is useful for recursive subdivision.
     *
     * @param  src      the control point list for the bezier
     * @param  soff     the offset into the control point list
     * @param  tp       the parameter to split at
     * @param  left     vector to store the left bezier
     * @param  right    vector to store the right bezier
     */
    static void subdivide(const std::vector<Vec2>& src, int soff, float tp,
                          std::vector<Vec2>& left, std::vector<Vec2>& rght);


    /**
     * Returns the projection polynomial for the given point.
     *
     * The projection polynomial is used to find the nearest value to point
     * on the spline, as described at
     *
     * http://jazzros.blogspot.com/2011/03/projecting-point-on-bezier-curve.html
     *
     * There is no one projection polynomial for the entire spline. Each
     * segment bezier has its own polynomial.
     *
     * @param  point    the point to project
     * @param  segment  the bezier segment to project upon
     */
    Polynomial getProjectionPolynomial(const Vec2& point, int segment) const;
    
    /**
     * Returns the parameterization of the nearest point on the bezier segment.
     *
     * The value is effectively the projection of the point onto the parametrized
     * curve. See getPoint() for an explanation of how the parameterization work.
     *
     * This version does not use the projection polynomial.  Instead, it picks
     * a parameter resolution and walks the entire length of the curve.  The
     * result is both slow and inexact (as the actual point may be in-between
     * chose parameters). This version is only picked when getProjectionFast
     * fails because of an error with root finding.
     *
     * The value returned is a pair of the parameter, and its distance value.
     * This allows us to compare this result to other segments, picking the
     * best value for the entire spline.
     *
     * @param  point    the point to project
     * @param  segment  the bezier segment to project upon
     *
     * @return the parameterization of the nearest point on the spline.
     */
    Vec2 getProjectionSlow(const Vec2& point, int segment) const;
    
    /**
     * Returns the parameterization of the nearest point on the bezier segment.
     *
     * The value is effectively the projection of the point onto the parametrized
     * curve. See getPoint() for an explanation of how the parameterization work.
     *
     * The value is effectively the projection of the point onto the parametrized
     * curve. See getPoint() for an explanation of how the parameterization work. We
     * compute this value using the projection polynomial, described at
     *
     * http://jazzros.blogspot.com/2011/03/projecting-point-on-bezier-curve.html
     *
     * The value returned is a pair of the parameter, and its distance value.
     * This allows us to compare this result to other segments, picking the
     * best value for the entire spline.
     *
     * This algorithm uses the projection polynomial, and searches for roots to
     * find the best (max of 5) candidates.  However, root finding may fail, 
     * do to singularities in Bairstow's Method.  If the root finder fails, then
     * the first element of the pair will be -1 (an invalid parameter).
     *
     * @param  point    the point to project
     * @param  segment  the bezier segment to project upon
     *
     * @return the parameterization of the nearest point on the spline.
     */
    Vec2 getProjectionFast(const Vec2& point, int segment) const;



#pragma mark Recursive Data Generation
private:
    /** The type of data to generate into a list of Vec2 */
    enum Buffer {
        /* No data, just count amounts */
        EMPTY,
        /* Put curve points into the list */
        POINTS,
        /* Put curve parameters into the list */
        PARAMETERS,
        /* Put tangent points into the list */
        TANGENTS,
        /* Put normals vectors into the list */
        NORMALS,
        /* Put all control points into the list */
        ALL
    };
    
    /**
     * Generates data via recursive use of de Castlejau's
     *
     * This function is a one-stop method for all of the polygon approximation
     * methods.  Instead of writing the same code over and over, we write a 
     * single method that takes a reference to a data buffer and a Buffer 
     * parameter (definining the type of data needed).
     *
     * The value int returned is the number of elements stored in buffer,
     * EXCEPT when bufferType is None.  In that case, it just returns the
     * number that would have been returned if POINTS were the type.
     *
     * This static method is not restricted to the current spline.  It can work
     * from any list of control points (and offset into those control points).
     * This is useful for recursive subdivision.
     * @param  src          the control point list for the bezier
     * @param  soff         the offset into the control point list
     * @param  tp           the parameter to split at
     * @param  tolerance    the error tolerance of the stopping condition
     * @param  criterion    the stopping condition criterion
     * @param  buffer       the buffer to store the generated data
     * @param  bufferType   the data type of the buffer
     * @param  depth        the current depth of the recursive call
     */
    static int generate_data(const std::vector<Vec2>& src, int soff, float tp,
                             float tolerance, Criterion criterion,
                             void* buffer, Buffer bufferType, int depth);

};

NS_CC_END
#endif /* defined(__CU_CUBIC_SPLINE_H__) */
