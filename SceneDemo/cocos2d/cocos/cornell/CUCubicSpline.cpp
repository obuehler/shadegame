//
//  CUCubicSpline.cpp
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
//  Author: Walker White
//  Version: 11/24/15
//
#include "CUCubicSpline.h"
#include "CUPolynomial.h"


NS_CC_BEGIN

/** Maximum recursion depth for de Castlejau's */
#define MAX_DEPTH   8

/** Epsilon value for closenest tests */
#define EPSILON     1.0f/512.0f

/** Tolerance to identify a point as "smooth" */
#define SMOOTH_TOLERANCE    0.0001f


#pragma mark -
#pragma mark Constructors

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
CubicSpline::CubicSpline(const Vec2& start, const Vec2& end) {
	_points.push_back(start);
	_points.push_back(start);
	_points.push_back(end);
	_points.push_back(end);

	_smooth.push_back(true);
	_smooth.push_back(true);

	_size = 1;
	_closed = (start.x == end.x && start.y == end.y);
}

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
CubicSpline::CubicSpline(const float* points, int size, int offset) {
	CCASSERT(size - 2 % 6 != 0, "Constrol point array is the wrong size");

	_size = (size - 2) / 6;
	_closed = false;

	for (int ii = 0; ii < size / 2; ii++) {
		_points.push_back(Vec2(points[2 * ii + offset], points[2 * ii + offset + 1]));
	}

	_smooth.resize(_size + 1, true);
	_smooth[0] = true;
	_smooth[_size] = true;

	for (int ii = 1; ii < _size; ii++) {
		Vec2 temp0 = _points[3 * ii - 1] - _points[3 * ii];
		Vec2 temp1 = _points[3 * ii + 1] - _points[3 * ii];
		temp0.normalize();
		temp1.normalize();
		temp0 -= temp1;
		_smooth[ii] = (temp0.lengthSquared() < SMOOTH_TOLERANCE);
	}
}

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
CubicSpline::CubicSpline(const vector<float>& points) {
	CCASSERT(points.size() % 6 != 2, "Constrol point array is the wrong size");

	_size = ((int)points.size() - 2) / 6;
	_closed = false;

	for (int ii = 0; ii < _size / 2; ii++) {
		_points.push_back(Vec2(points[2 * ii], points[2 * ii + 1]));
	}

	_smooth.resize(_size + 1, true);
	_smooth[0] = true;
	_smooth[_size] = true;

	for (int ii = 1; ii < _size; ii++) {
		Vec2 temp0 = _points[3 * ii - 1] - _points[3 * ii];
		Vec2 temp1 = _points[3 * ii + 1] - _points[3 * ii];
		temp0.normalize();
		temp1.normalize();
		temp0 -= temp1;
		_smooth[ii] = (temp0.lengthSquared() < SMOOTH_TOLERANCE);
	}
}

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
CubicSpline::CubicSpline(const vector<Vec2>& points) {
	CCASSERT(points.size() % 3 != 1, "Constrol point array is the wrong size");

	_size = ((int)points.size() - 1) / 3;
	_closed = false;

	_points.assign(points.begin(), points.end());

	_smooth.resize(_size + 1, true);
	_smooth[0] = true;
	_smooth[_size] = true;

	for (int ii = 1; ii < _size; ii++) {
		Vec2 temp0 = _points[3 * ii - 1] - _points[3 * ii];
		Vec2 temp1 = _points[3 * ii + 1] - _points[3 * ii];
		temp0.normalize();
		temp1.normalize();
		temp0 -= temp1;
		_smooth[ii] = (temp0.lengthSquared() < SMOOTH_TOLERANCE);
	}
}

/**
* Creates a copy of the given spline.
*
* @param  spline   The spline to copy
*/
CubicSpline::CubicSpline(const CubicSpline& spline) {
	_size = spline._size;
	_closed = spline._closed;
	_points.assign(spline._points.begin(), spline._points.end());
	_smooth.assign(spline._smooth.begin(), spline._smooth.end());
}

#pragma mark Attribute Accessors

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
void CubicSpline::setClosed(bool flag) {
	if (flag && (_points[0] != _points[3 * _size])) {
		addAnchor(_points[0]);
	}
	_closed = true;
}

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
Vec2 CubicSpline::getPoint(int segment, float tp) const {
	CCASSERT(segment >= 0 && segment < _size, "Illegal spline segment");
	CCASSERT(tp >= 0.0f && tp <= 1.0f, "Illegal segment parameter");

	if (segment == _size) {
		return _points[3 * segment];
	}

	int index = 6 * segment;
	float sp = (1 - tp);
	float a = sp*sp;
	float d = tp*tp;
	float b = 3 * tp*a;
	float c = 3 * sp*d;
	a = a*sp;
	d = d*tp;
	return a*_points[index] + b*_points[index + 1] + c*_points[index + 2] + d*_points[index + 3];
}

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
void CubicSpline::setPoint(float tp, const Vec2& point) {
	CCASSERT(tp >= 0 && tp <= _size, "Parameter out of bounds");
	CCASSERT(!_closed || tp < _size, "Parameter out of bounds for closed spline");

	int seg = (int)tp;
	if (seg == tp) {
		setAnchor(tp, point);
	}
	else {
		tp = tp - seg;
		insertAnchor(seg, tp);
		setAnchor(seg + 1, point);
	}
}

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
Vec2 CubicSpline::getAnchor(int index) const {
	CCASSERT(index >= 0 && index < _size, "Index out of bounds");
	CCASSERT(!_closed || index < _size - 1, "Index out of bounds for closed spline");
	return _points[3 * index];
}

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
void CubicSpline::setAnchor(int index, const Vec2& point) {
	CCASSERT(index >= 0 && index < _size, "Index out of bounds");
	CCASSERT(!_closed || index < _size - 1, "Index out of bounds for closed spline");

	Vec2 diff = point - _points[3 * index];

	// Adjust left tangents
	if (index > 0) {
		_points[3 * index - 1] = _points[3 * index - 1] + diff;
	}
	else if (_closed) {
		_points[3 * _size - 1] = _points[3 * _size - 1] + diff;
	}

	// Adjust right tangents
	if (index < _size) {
		_points[3 * index + 1] = _points[3 * index + 1] + diff;
	}
	else if (_closed) {
		_points[1] = _points[1] + diff;
	}

	_points[3 * index] = point;
}

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
bool CubicSpline::getSmooth(int index) const {
	CCASSERT(index >= 0 && index < _size, "Index out of bounds");
	CCASSERT(!_closed || index < _size - 1, "Index out of bounds for closed spline");
	return _smooth[index];
}

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
void CubicSpline::setSmooth(int index, bool flag) {
	CCASSERT(index >= 0 && index < _size, "Index out of bounds");
	CCASSERT(!_closed || index < _size - 1, "Index out of bounds for closed spline");

	_smooth[index] = flag;
	if (flag && index > 0 && index < _size) {
		Vec2 temp0 = _points[3 * index - 1] - _points[3 * index];
		Vec2 temp1 = _points[3 * index] - _points[3 * index + 1];
		float scale0 = temp0.length();
		float scale1 = temp1.length();

		// Average the vectors
		temp0.normalize();
		temp1.normalize();
		Vec2 temp2 = temp0.getMidpoint(temp1);
		temp2.normalize();

		// Scale them appropriately
		temp0.set(temp2);
		temp0.scale(scale0);
		temp1.set(temp2);
		temp1.scale(scale1);

		_points[3 * index - 1] = _points[3 * index] + temp0;
		_points[3 * index + 1] = _points[3 * index] - temp1;
	}
}

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
Vec2 CubicSpline::getTangent(int index) const {
	CCASSERT(index >= 0 && index < 2 * _size, "Index out of bounds");
	int spline = (index + 1) / 2;
	int anchor = 3 * spline;
	int tangt = (index % 2 == 1 ? anchor - 1 : anchor + 1);
	return _points[tangt];
}

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
void CubicSpline::setTangent(int index, const Vec2& tang, bool symmetric) {
	CCASSERT(index >= 0 && index < 2 * _size, "Index out of bounds");

	int spline = (index + 1) / 2;
	int anchor = 3 * spline;
	int tangt1 = (index % 2 == 1 ? anchor - 1 : anchor + 1);
	int tangt2 = (index % 2 == 1 ? anchor + 1 : anchor - 1);

	if (spline == 0) {
		tangt2 = (_closed ? 3 * _size - 1 : -1);
	}
	else if (spline == _size) {
		tangt2 = (_closed ? 1 : -1);
	}

	if (symmetric && tangt2 != -1) {
		Vec2 temp0 = _points[anchor] - tang;
		_points[tangt2] = _points[anchor] + temp0;
	}
	else if (_smooth[spline] && tangt2 != -1) {
		Vec2 temp0 = _points[anchor] - _points[tangt2];
		float d = temp0.length();

		temp0 = _points[anchor] - tang;
		temp0.normalize();
		temp0.scale(d);

		_points[tangt2] = _points[anchor] + temp0;
	}

	_points[tangt1] = tang;
}

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
Polynomial CubicSpline::getPolynomialX(int segment) const {
	CCASSERT(segment >= 0 && segment < _size, "Segment out of bounds");

	//(1-t)3 p0 + 3(1-t)2 t p1 + 3 (1-t) t2 p2 + t3 p3
	float p3 = _points[3 * segment + 3].x;
	float p2 = _points[3 * segment + 2].x;
	float p1 = _points[3 * segment + 1].x;
	float p0 = _points[3 * segment].x;

	Polynomial poly;
	poly.push_back(p3 + 3 * p1 - p0 - 3 * p2);
	poly.push_back(p0 + 6 * p1 + 3 * p2);
	poly.push_back(3 * p1 - 3 * p0);
	poly.push_back(p0);
	return poly;
}

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
Polynomial CubicSpline::getPolynomialY(int segment) const {
	CCASSERT(segment >= 0 && segment < _size, "Segment out of bounds");

	//(1-t)3 p0 + 3(1-t)2 t p1 + 3 (1-t) t2 p2 + t3 p3
	float p3 = _points[3 * segment + 3].y;
	float p2 = _points[3 * segment + 2].y;
	float p1 = _points[3 * segment + 1].y;
	float p0 = _points[3 * segment].y;

	Polynomial poly;
	poly.push_back(p3 + 3 * p1 - p0 - 3 * p2);
	poly.push_back(p0 + 6 * p1 + 3 * p2);
	poly.push_back(3 * p1 - 3 * p0);
	poly.push_back(p0);
	return poly;
}


#pragma mark Anchor Editting Methods
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
int CubicSpline::addAnchor(const Vec2& point, const Vec2& tang) {
	CCASSERT(!_closed, "Cannot append to closed curve");

	_points.resize(_points.size() + 3, Vec2::ZERO);
	_smooth.resize(_smooth.size() + 1, true);

	int pos = 3 * _size + 1;
	if (_smooth[_size]) {
		_points[pos] = 2 * _points[pos - 1] - _points[pos - 2];
	}
	else {
		_points[pos] = _points[pos - 1];
	}

	_points[pos + 1] = tang;
	_points[pos + 2] = point;
	_size++;
	_smooth[_size] = true;
	return _size;
}

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
void CubicSpline::deleteAnchor(int index) {
	CCASSERT(index >= 0 && index < _size, "Index out of bounds");
	CCASSERT(!_closed || index < _size - 1, "Index out of bounds for closed spline");

	// Shift everything left.
	_points.erase(_points.begin() + (3 * index), _points.begin() + (3 * (index + 1)));
	_smooth.erase(_smooth.begin() + index, _smooth.begin() + (index + 1));
	_size--;
}

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
void CubicSpline::insertAnchor(int segment, float param) {
	CCASSERT(segment >= 0 && segment < _size, "Illegal spline segment");
	CCASSERT(param > 0.0f && param < 1.0f, "Illegal insertion parameter");

	// Split the bezier.
	vector<Vec2> left;
	vector<Vec2> right;
	subdivide(segment, param, left, right);

	// Replace first segment with left
	copy(left.begin(), left.end(), _points.begin() + (3 * segment));

	// Now insert the right
	_points.insert(_points.begin() + (3 * segment + 1), right.begin(), right.end());
	_smooth.insert(_smooth.begin() + (segment + 1), true);
	_size++;
}


#pragma mark Nearest Point Methods
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
float CubicSpline::nearestParameter(const Vec2& point) const {
	float tmin = -1;
	float dmin = -1;
	int smin = -1;

	for (int ii = 0; ii < _size; ii++) {
		Vec2 pair = getProjectionFast(point, ii);
		if (pair.x == -1) {
			pair = getProjectionSlow(point, ii);
		}
		if (smin == -1 || pair.y < dmin) {
			tmin = pair.x; dmin = pair.y; smin = ii;
		}
	}

	return smin + tmin;
}

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
int CubicSpline::nearestAnchor(const Vec2& point, float threshold) const {
	float best = std::numeric_limits<float>::infinity();
	int	 index = -1;

	for (int ii = 0; ii <= _size; ii++) {
		Vec2 temp0 = _points[3 * ii] - point;
		float d = temp0.lengthSquared();
		if (d < threshold && d < best) {
			best = d;
			index = ii;
		}
	}
	return index;
}

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
int CubicSpline::nearestTangent(const Vec2& point, float threshold) const {
	float best = std::numeric_limits<float>::infinity();
	int	 index = -1;

	for (int ii = 0; ii < _size; ii++) {
		Vec2 temp0 = _points[3 * ii + 1] - point;
		float d = temp0.lengthSquared();
		if (d < threshold && d < best) {
			best = d;
			index = 2 * ii + 1; // Right side of index ii.
		}
		temp0 = _points[3 * ii + 2] - point;
		d = temp0.lengthSquared();
		if (d < threshold && d < best) {
			best = d;
			index = 2 * ii + 2; // Left side of index ii+1
		}
	}
	return index;
}


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
vector<Vec2> CubicSpline::approximate(float tolerance, Criterion criterion) const {
	vector<Vec2> buffer;
	for (int ii = 0; ii < _size; ii++) {
		generate_data(_points, 3 * ii, ii, tolerance, criterion, &buffer, Buffer::POINTS, 0);
	}

	// Push back last point
	buffer.push_back(_points[3 * _size]);
	return buffer;
}

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
vector<float> CubicSpline::approximateParameters(float tolerance, Criterion criterion) const {
	vector<float> buffer;
	for (int ii = 0; ii < _size; ii++) {
		generate_data(_points, 3 * ii, ii, tolerance, criterion, &buffer, Buffer::PARAMETERS, 0);
	}

	// Push back last parameter
	buffer.push_back(_size);
	return buffer;

}

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
vector<Vec2> CubicSpline::approximateTangents(float tolerance, Criterion criterion) const {
	vector<Vec2> buffer;
	for (int ii = 0; ii < _size; ii++) {
		generate_data(_points, 3 * ii, ii, tolerance, criterion, &buffer, Buffer::TANGENTS, 0);
	}

	return buffer;
}

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
vector<Vec2> CubicSpline::approximateNormals(float tolerance, Criterion criterion) const {
	vector<Vec2> buffer;
	for (int ii = 0; ii < _size; ii++) {
		generate_data(_points, 3 * ii, ii, tolerance, criterion, &buffer, Buffer::NORMALS, 0);
	}

	// Push back the last normal.
	int offset = 3 * _size;
	Vec2 temp0 = _points[offset] - _points[offset - 1];;
	temp0.set(-temp0.y, temp0.x);
	temp0.normalize();

	buffer.push_back(temp0);
	return buffer;
}

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
CubicSpline CubicSpline::refine(float tolerance, Criterion criterion) const {
	vector<Vec2> buffer;
	for (int ii = 0; ii < _size; ii++) {
		generate_data(_points, 3 * ii, ii, tolerance, criterion, &buffer, Buffer::ALL, 0);
	}

	// Push back last point
	buffer.push_back(_points[3 * _size]);
	return CubicSpline(buffer);
}

#pragma mark Rendering Data
/**
* Fills in the vertex and index data for a single handle.
*
* Handles are circular shapes of a given radius. This information may be
* passed to a PolygonNode to provide a visual representation of the
* anchor points (as seen in Adobe Illustrator).
*
* @param  point    The location of the handle point
* @param  index    The index seed for the vertex array
* @param  radius   the radius of each handle
* @param  segments the number of segments in the handle "circle"
* @param  vertices the array storing the vertex data
* @param  indices  the array storing the index data
*/
void fillHandle(const Vec2& point, int index, float radius, int segments,
	float* vertices, unsigned short* indices) {
	int voff = 2 * index*(segments + 2);
	int ioff = 3 * index*segments;

	// Add the center
	vertices[voff] = point.x;
	vertices[voff + 1] = point.y;

	// Add everything else
	float coef = 2.0f * (float)M_PI / segments;
	for (int jj = 0; jj <= segments; jj++) {
		float rads = jj*coef;
		vertices[voff + 2 * (jj + 1)] = 0.5f * radius * cosf(rads) + point.x;
		vertices[voff + 2 * (jj + 1) + 1] = 0.5f * radius * sinf(rads) + point.y;
	}

	for (int jj = 0; jj < segments; jj++) {
		indices[ioff + 3 * jj] = index*(segments + 2);
		indices[ioff + 3 * jj + 1] = index*(segments + 2) + jj + 1;
		indices[ioff + 3 * jj + 2] = index*(segments + 2) + jj + 2;
	}
}

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
Poly2* CubicSpline::allocPath(float tolerance, Criterion criterion) const {
	Poly2* result = new Poly2(approximate(tolerance, criterion));
	result->traverse(_closed ? Poly2::Traversal::CLOSED : Poly2::Traversal::OPEN);
	return result;
}

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
Poly2* CubicSpline::allocTangents() const {
	Poly2* result = new Poly2(_points);
	unsigned short* indx = new unsigned short[4 * _size];
	for (int ii = 0; ii < _size; ii++) {
		indx[4 * ii] = 3 * ii;
		indx[4 * ii + 1] = 3 * ii + 1;
		indx[4 * ii + 2] = 3 * ii + 2;
		indx[4 * ii + 3] = 3 * ii + 3;
	}
	result->setIndices(indx, 4 * _size);
	delete[] indx;
	return result;
}

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
Poly2* CubicSpline::allocAnchors(float radius, int segments) const {
	int last = (_closed ? _size - 1 : _size);
	int vertsize = 2 * (segments + 2)*(last + 1);
	float* vertices = new float[vertsize];

	int indxsize = 3 * segments*(last + 1);
	unsigned short* indices = new unsigned short[indxsize];

	for (int ii = 0; ii <= last; ii++) {
		fillHandle(_points[3 * ii], ii, radius, segments, vertices, indices);
	}

	delete[] vertices;
	delete[] indices;

	return new Poly2(vertices, vertsize, indices, indxsize);
}

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
Poly2* CubicSpline::allocHandles(float radius, int segments) const {
	int vertsize = 2 * (segments + 2)*(2 * _size);
	float* vertices = new float[vertsize];

	int indxsize = 3 * segments*(2 * _size);
	unsigned short* indices = new unsigned short[indxsize];

	for (int ii = 0; ii < _size; ii++) {
		fillHandle(_points[3 * ii + 1], 2 * ii, radius, segments, vertices, indices);
		fillHandle(_points[3 * ii + 2], 2 * ii + 1, radius, segments, vertices, indices);
	}

	delete[] vertices;
	delete[] indices;

	return new Poly2(vertices, vertsize, indices, indxsize);
}


#pragma mark Internal Helpers
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
void CubicSpline::subdivide(const vector<Vec2>& src, int soff, float tp,
	vector<Vec2>& left, vector<Vec2>& rght) {
	// Cross bar
	Vec2 h = (1 - tp)*src[soff + 1] + tp*src[soff + 2];

	// FIRST HALF
	left.resize(4, Vec2::ZERO);
	left[0] = src[soff];
	left[1] = (1 - tp)*src[soff] + tp*src[soff + 1];
	left[2] = (1 - tp)*left[1] + tp*h;

	// SECOND HALF
	rght.resize(4, Vec2::ZERO);
	rght[3] = src[soff + 3];
	rght[2] = (1 - tp)*src[soff + 2] + tp*src[soff + 3];
	rght[1] = (1 - tp)*h + tp*rght[2];
	rght[0] = (1 - tp)*left[2] + tp*rght[1];

	left[3] = rght[0];
}

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
Polynomial CubicSpline::getProjectionPolynomial(const Vec2& point, int segment) const {
	CCASSERT(segment >= 0 && segment < _size, "Illegal spline segment");

	Vec2 a = _points[3 * segment + 3] - 3 * _points[3 * segment + 2] + 3 * _points[3 * segment + 1] - _points[3 * segment];
	Vec2 b = 3 * _points[3 * segment + 2] - 6 * _points[3 * segment + 1] + 3 * _points[3 * segment];
	Vec2 c = 3 * (_points[3 * segment + 1] - _points[3 * segment]);
	Vec2 p = _points[3 * segment] - point;

	Polynomial result(5);
	result[0] = 3.0f*a.dot(a);                  // Q5
	result[1] = 5.0f*a.dot(b);					// Q4
	result[2] = 4.0f*a.dot(c) + 2.0f*b.dot(b);	// Q3
	result[3] = 3.0f*b.dot(c) + 3.0f*a.dot(p);	// Q2
	result[4] = c.dot(c) + 2.0f*b.dot(p);			// Q1
	result[5] = c.dot(p);                       // Q0
	return result;
}

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
Vec2 CubicSpline::getProjectionSlow(const Vec2& point, int segment) const {
	Vec2 result(-1, -1);

	int RESOLUTION = (1 << MAX_DEPTH);
	for (int jj = 0; jj < RESOLUTION; jj++) {
		float t = ((float)jj) / RESOLUTION;
		Vec2 temp0 = getPoint(t, segment);
		temp0 -= point;
		float d = temp0.lengthSquared();
		if (result.x == -1 || d < result.y) {
			result.x = t; result.y = d;
		}
	}

	// Compare the last point.
	Vec2 temp0 = _points[3 * _size] - point;
	float d = temp0.lengthSquared();
	if (d < result.y) {
		result.x = 1.0f; result.y = d;
	}
	return result;
}

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
Vec2 CubicSpline::getProjectionFast(const Vec2& point, int segment) const {
	vector<float> roots;

	Polynomial poly = getProjectionPolynomial(point, segment);

	float epsilon = 1.0f / (1 << (MAX_DEPTH + 1));
	bool success = poly.roots(roots, epsilon);
	if (!success) {
		// This will kick us to the slow method
		return Vec2(-1, 0);
	}

	Vec2 result;

	// Now compare the candidates
	result.x = 0.0f;
	Vec2 compare = getPoint(1.0f, segment) - point;
	result.y = compare.lengthSquared();

	float t = 1.0f;
	compare = getPoint(1.0f, segment) - point;
	float d = compare.lengthSquared();
	if (d < result.y) {
		result.set(t, d);
	}

	// Check the roots
	int RESOLUTION = (1 << MAX_DEPTH);
	for (auto it = roots.begin(); it != roots.end(); ++it) {
		float r = *it;
		if (r != nanf("") && r > 0 && r < 1) {
			// Convert to nearest step.
			t = r*RESOLUTION;
			t = roundf(t);
			t /= RESOLUTION;
			compare = getPoint(t, segment) - point;
			d = compare.lengthSquared();
			if (d < result.y) {
				result.set(t, d);
			}
		}
	}

	return result;
}


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
int CubicSpline::generate_data(const vector<Vec2>& src, int soff, float tp,
	float tolerance, Criterion criterion,
	void* buffer, Buffer bufferType, int depth) {
	// Do not go to far
	bool terminate = (depth >= 8) || (soff >= src.size() - 1);

	// Check if we are at the bottom level
	if (!terminate && criterion == Criterion::SPACING) {
		Vec2 temp0 = src[soff + 3] - src[soff];             // p3 - p0
		terminate = temp0.length() < tolerance;
	}
	else if (!terminate && (criterion == Criterion::DISTANCE || criterion == Criterion::FLAT)) {
		Vec2 temp0 = src[soff + 3] - src[soff];             // p3 - p0
		float leng = 1.0f;
		if (criterion == Criterion::FLAT) {
			leng = temp0.length();
		}

		Vec2 temp1 = src[soff + 1] - src[soff];             // p1 - p0
		temp1.normalize();
		float scale = temp0.dot(temp1);
		temp1 *= scale;
		temp0 -= temp1;

		terminate = (temp0.length() < tolerance*leng);

		temp0 = src[soff] - src[soff + 3];                // p0 - p3
		temp1 = src[soff + 2] - src[soff + 3];                // p2 - p3
		temp1.normalize();
		scale = temp0.dot(temp1);
		temp1 *= scale;
		temp0 -= temp1;

		terminate = terminate && (temp0.length() < tolerance*leng);
	}

	// Add the first point if terminating.
	int result = 0;
	if (terminate) {
		switch (bufferType) {
		case Buffer::EMPTY:
			return 1;
		case Buffer::POINTS:
		{
			vector<Vec2>* pbuffer = (vector<Vec2>*)buffer;
			pbuffer->push_back(src[soff]);
			return 1;
		}
		case Buffer::PARAMETERS:
		{
			vector<float>* mbuffer = (vector<float>*)buffer;
			mbuffer->push_back(tp);
			return 1;
		}
		case Buffer::TANGENTS:
		{
			vector<Vec2>* tbuffer = (vector<Vec2>*)buffer;
			tbuffer->push_back(src[soff + 1]);
			tbuffer->push_back(src[soff + 2]);
			return 2;
		}
		case Buffer::NORMALS:
		{
			Vec2 norm = src[soff + 1] - src[soff];
			norm.set(-norm.y, norm.x);
			norm.normalize();

			// Add the _points.
			vector<Vec2>* nbuffer = (vector<Vec2>*)buffer;
			nbuffer->push_back(norm);
			return 1;
		}
		case Buffer::ALL:
		{
			vector<Vec2>* abuffer = (vector<Vec2>*)buffer;
			abuffer->push_back(src[soff]);
			abuffer->push_back(src[soff + 1]);
			abuffer->push_back(src[soff + 2]);
			return 3;
		}
		}
		// We are just counting.
		return 1;
	}

	vector<Vec2> left;
	vector<Vec2> rght;
	subdivide(src, soff, 0.5, left, rght);

	// Recursive calls
	float sp = tp + 1.0f / (1 << (depth + 1));
	result = generate_data(left, 0, tp, tolerance, criterion, buffer, bufferType, depth + 1);
	result += generate_data(rght, 0, sp, tolerance, criterion, buffer, bufferType, depth + 1);
	return result;
}

NS_CC_END