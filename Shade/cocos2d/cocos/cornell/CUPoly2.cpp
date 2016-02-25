//
//  CUPoly2.cpp
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
//  Author: Walker White
//  Version: 11/15/15
//
#include "CUPoly2.h"
#include <algorithm>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;

NS_CC_BEGIN

#pragma mark -
#pragma mark TRIANGULATION PROTOTYPES

/**
 * Returns the indices for a triangulation of the given vertices.
 *
 * The indices are references to vertices in the given vertex array.  Vertex 0 is
 * the one with coordinates vertices[offset] and vertices[offset+1].  There will
 * be three times as many indices as triangles.
 *
 * This function uses ear-clipping triangulation. The function culls all degenerate
 * triangles from the result before returning.
 *
 * @param vertices  The array of vertices in this polygon
 * @param offset    The offset in vertices to start the polygon
 * @param count     The number of elements to use from vertices
 *
 * @return A vector of vertex indices representing a triangulation.
 */
vector<unsigned short> ear_triangulate(const vector<Vec2>& vertices);

/**
 * Returns the indices for a triangulation of the given vertices.
 *
 * The indices are references to vertices in the given vertex array.  Vertex 0 is
 * the one with coordinates vertices[offset] and vertices[offset+1].  There will
 * be three times as many indices as triangles.
 *
 * This function uses ear-clipping triangulation. The function culls all degenerate
 * triangles from the result before returning.
 *
 * This method differs from the other triangulation method in that it allows the
 * user to provide a vector for storing the triangulation.
 *
 * @param vertices  The array of vertices in this polygon
 * @param offset    The offset in vertices to start the polygon
 * @param count     The number of elements to use from vertices
 * @param output    The vector to store the output
 *
 * @return A reference to the output vector
 */
vector<unsigned short>& ear_triangulate(const vector<Vec2>& vertices, vector<unsigned short>& output);


#pragma mark -
#pragma mark PATH EXTRUSION

/** The number of segments to use in a rounded joint */
#define JOINT_PRECISION 10
/** The number of segments to use in a rounded cap */
#define CAP_PRECISION   10
/** PI/2; used for rounded joints and end caps */
#define PI_2 3.1415926535/2

/**
 * Computes the intersection of two lines.
 *
 * The lines are defined by line segments.  However the intersection is not
 * limited to the line segment.  It extends the segments to a full line as
 * part of its search.
 *
 * This function stores the point of intersection in the pointers px, py.  If
 * there is no intersection, these values are left unchanged and the function
 * returns false.
 *
 * @param   x1  Initial x-coordinate of the first line segment
 * @param   y1  Initial y-coordinate of the first line segment
 * @param   x2  Final x-coordinate of the first line segment
 * @param   y2  Final y-coordinate of the first line segment
 * @param   x3  Initial x-coordinate of the second line segment
 * @param   y3  Initial y-coordinate of the second line segment
 * @param   x4  Final x-coordinate of the second line segment
 * @param   y4  Final y-coordinate of the second line segment
 * @param   px  Pointer to store x-coordinate of intersection
 * @param   px  Pointer to store y-coordinate of intersection
 *
 * @return true if an intersection was found; false otherwise
 */
bool line_intersect(Vec2 v1, Vec2 v2, Vec2 v3, Vec2 v4, Vec2& p) {
    float u = (v1.x * v2.y - v1.y * v2.x);
    float v = (v3.x * v4.y - v3.y * v4.x);
    float denom = (v1.x - v2.x) * (v3.y - v4.y) - (v1.y - v2.y) * (v3.x - v4.x);
    if (denom == 0) {
        return false;
    }
    p = (u * (v3 - v4) - (v1 - v2) * v) / denom;
    return true;
}


#pragma mark -
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
Poly2& Poly2::set(const vector<Vec2>& vertices) {
    _vertices.assign(vertices.begin(),vertices.end());
    _indices.clear();
    computeBounds();
    return *this;
}

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
Poly2& Poly2::set(const vector<Vec2>& vertices, const vector<unsigned short>& indices) {
    _vertices.assign(vertices.begin(),vertices.end());
    _indices.assign(indices.begin(),indices.end());
    computeBounds();
    return *this;
}

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
Poly2& Poly2::set(const vector<float>& vertices) {
    vector<Vec2>* ref = (vector<Vec2>*)&vertices;
    _vertices.assign(ref->begin(),ref->end());
    _indices.clear();
    
    computeBounds();
    return *this;
}

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
Poly2& Poly2::set(const vector<float>& vertices, const vector<unsigned short>& indices) {
    vector<Vec2>* ref = (vector<Vec2>*)&vertices;
    _vertices.assign(ref->begin(),ref->end());
    _indices.assign(indices.begin(),indices.end());
    computeBounds();
    return *this;
}

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
Poly2& Poly2::set(Vec2* vertices, int vertsize, int voffset) {
    _vertices.assign(vertices+voffset,vertices+voffset+vertsize);
    _indices.clear();

    computeBounds();
    return *this;
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
Poly2& Poly2::set(Vec2* vertices, int vertsize, unsigned short* indices, int indxsize,
                  int voffset, int ioffset) {
    _vertices.assign(vertices+voffset,vertices+voffset+vertsize);
    _indices.assign(indices+ioffset, indices+ioffset+indxsize);

    computeBounds();
    return *this;
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
Poly2& Poly2::set(const Poly2& poly) {
    _vertices.assign(poly._vertices.begin(),poly._vertices.end());
    _indices.assign(poly._indices.begin(),poly._indices.end());
    computeBounds();
    return *this;
}

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
Poly2& Poly2::set(const Rect& rect, bool index) {
    _vertices.resize(4,Vec2::ZERO);

    _vertices[0] = rect.origin;
    _vertices[1] = Vec2(rect.origin.x, rect.origin.y+rect.size.height);
    _vertices[2] = Vec2(rect.origin.x+rect.size.width, rect.origin.y+rect.size.height);
    _vertices[3] = Vec2(rect.origin.x+rect.size.width, rect.origin.y);
    
    if (index) {
        _indices.resize(6,0);
        _indices[0] = 0;
        _indices[1] = 1;
        _indices[2] = 2;
        _indices[3] = 0;
        _indices[4] = 2;
        _indices[5] = 3;
    } else {
        _indices.clear();
    }
    
    computeBounds();
    return *this;
}

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
Poly2& Poly2::setLine(const Vec2& origin, const Vec2& dest) {
    _vertices.clear();
    _vertices.push_back(origin);
    _vertices.push_back(dest);
    
    _indices.clear();
    _indices.push_back(1);
    _indices.push_back(2);

    computeBounds();
    return *this;
}

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
Poly2& Poly2::setEllipse(const Vec2& center, const Size& size, unsigned int segments) {
    const float coef = 2.0f * (float)M_PI/segments;
    
    _vertices.clear();
    Vec2 vert;
    for(unsigned int ii = 0; ii <= segments; ii++) {
        float rads = ii*coef;
        vert.x = 0.5f * size.width  * cosf(rads) + center.x;
        vert.y = 0.5f * size.height * sinf(rads) + center.y;
        _vertices.push_back(vert);
    }
    
    traverse(Traversal::CLOSED);
    
    computeBounds();
    return *this;
}

#pragma mark -
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
Poly2& Poly2::setIndices(const vector<unsigned short>& indices) {
    _indices.assign(indices.begin(), indices.end());
    return *this;
}

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
Poly2& Poly2::setIndices(unsigned short* indices, int indxsize, int ioffset) {
    _indices.assign(indices+ioffset, indices+ioffset+indxsize);
    return *this;
}

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
Poly2& Poly2::triangulate() {
    CCASSERT(_vertices.size() >= 3, "Not enough vertices to triangulate");

    _indices.clear();
    ear_triangulate(_vertices,_indices);
    return *this;
}

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
Poly2& Poly2::traverse(Traversal traversal) {
    _indices.clear();
    
    switch (traversal) {
        case Poly2::Traversal::OPEN:
        {
            _indices.clear();
            _indices.reserve(2*(_vertices.size()-1));
            for(int ii = 0; ii < _vertices.size()-1; ii++) {
                _indices.push_back(ii  );
                _indices.push_back(ii+1);
            }
            break;
        }
        case Poly2::Traversal::CLOSED:
        {
            _indices.clear();
            _indices.reserve(2*_vertices.size());
            for(int ii = 0; ii < _vertices.size()-1; ii++) {
                _indices.push_back(ii  );
                _indices.push_back(ii+1);
            }
            _indices.push_back(_vertices.size()-1);
            _indices.push_back(0);
            break;
        }
        case Poly2::Traversal::INTERIOR:
        {
            vector<unsigned short> indx;
            ear_triangulate(_vertices, indx);
            
            _indices.clear();
            _indices.reserve((int)(2*indx.size()));
            for(int ii = 0; ii < indx.size(); ii++) {
                int next = (ii % 3 == 2 ? ii-2 : ii+1);
                _indices.push_back(indx[ii  ]);
                _indices.push_back(indx[next]);
            }
            break;
        }
    }

    return *this;
}

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
Poly2* Poly2::extrude(Poly2& poly, float stroke, bool closed, Joint joint, Cap cap) {
    CCASSERT(stroke > 0, "invalid stroke value");
    poly._vertices.clear();
    poly._indices.clear();
    if (_vertices.size() == 0) {
        return &poly;
    }
    
    int count = (int)_vertices.size();
    
    // Closed paths have no cap;
    if (closed && count > 2) {
        count += 2;
        cap = Cap::NONE;
    }
    
    // Determine the number of vertices and indices we need.
    int vcount = (count - 1) * 4;
    int icount = (count - 1) * 6;
    
    switch (joint) {
        case Joint::BEVEL:
            icount += (count - 2) * 3;
            vcount += (count - 2);
            break;
        case Joint::ROUND:
            icount += (JOINT_PRECISION * 3) * (count - 2);
            vcount += (JOINT_PRECISION) * (count - 2);
            break;
        case Joint::MITRE:
            icount += (count - 2) * 6;
            vcount += (count - 2) * 2;
            break;
        case Joint::NONE:
            // Nothing to do.
            break;
    }
    
    switch (cap) {
        case Cap::SQUARE:
            icount += 12;
            vcount += 4;
            break;
        case Cap::ROUND:
            icount += (CAP_PRECISION * 3) * 2;
            vcount += (CAP_PRECISION) * 2;
            break;
        case Cap::NONE:
            // Nothing to do.
            break;
    }
    
    vector<Vec2> edgeverts;
    vector<unsigned short> edgeindx;
    edgeverts.reserve(vcount*2);
    edgeindx.reserve(icount);
    
    // Calculation time
    // Thanks Kivy guys for all the hard work.
    Vec2 c, pc;
    Vec2 s1, s4;
    Vec2 v1, v2, v3, v4;
    Vec2 p1, p2, p3, p4;
    
    float angle, sangle, pangle, pangle2;
    unsigned int pos, ppos, p2pos;
    
    angle = sangle = 0;
    pos = ppos = p2pos = 0;
    for(int ii = 0; ii < count-1; ii++) {
        Vec2 a = _vertices[  ii   % count];
        Vec2 b = _vertices[(ii+1) % count];
        
        if (ii > 0 && joint != Joint::NONE) {
            pc = c;
            p1 = v1; p2 = v2; p3 = v3; p4 = v4;
        }
        
        p2pos = ppos; ppos = pos;
        pangle2 = pangle; pangle = angle;
        
        // Calculate the orientation of the segment, between pi and -pi
        c = b - a;
        angle = atan2(c.y, c.x);
        float a1 = angle - PI_2;
        float a2 = angle + PI_2;
        
        // Calculate the position of the segment
        Vec2 temp1 = Vec2(cos(a1) * stroke, sin(a1) * stroke);
        Vec2 temp2 = Vec2(cos(a2) * stroke, sin(a2) * stroke);
        
        v1 = a+temp1;
        v4 = a+temp2;
        v2 = b+temp1;
        v3 = b+temp2;
        
        if (ii == 0) {
            s1 = v1; s4 = v4;
            sangle = angle;
        }
        
        edgeindx.push_back(pos  );
        edgeindx.push_back(pos+1);
        edgeindx.push_back(pos+2);
        edgeindx.push_back(pos  );
        edgeindx.push_back(pos+2);
        edgeindx.push_back(pos+3);
        
        // Textures, colors are set later.  Just initialize to safe values.
        edgeverts.push_back(v1);
        edgeverts.push_back(v2);
        edgeverts.push_back(v3);
        edgeverts.push_back(v4);
        pos += 4;
        
        // joint generation
        if (ii == 0 || joint == Joint::NONE) {
            continue; // Sigh
        }
        
        // calculate the angle of the previous and current segment
        float jangle = atan2(c.x * pc.y - c.y * pc.x,c.x * pc.x + c.y * pc.y);
        
        // in case of the angle is NULL, avoid the generation
        if (jangle == 0) {
            continue; // Sigh
        }
        
        switch (joint) {
            case Joint::BEVEL:
            {
                edgeverts.push_back(a);
                if (jangle < 0) {
                    edgeindx.push_back(p2pos+1);
                    edgeindx.push_back(ppos   );
                    edgeindx.push_back(pos    );
                } else {
                    edgeindx.push_back(p2pos+2);
                    edgeindx.push_back(ppos +3);
                    edgeindx.push_back(pos    );
                }
                pos += 1;
                break;
            }
            case Joint::MITRE:
            {
                edgeverts.push_back(a);
                if (jangle < 0) {
                    Vec2 temp;
                    if (!line_intersect(p1, p2, v1, v2, temp)) {
                        continue; // Sigh
                    }
                    edgeverts.push_back(temp);
                    edgeindx.push_back(pos    );
                    edgeindx.push_back(pos+1  );
                    edgeindx.push_back(p2pos+1);
                    edgeindx.push_back(pos    );
                    edgeindx.push_back(ppos   );
                    edgeindx.push_back(pos+1  );
                    pos += 2;
                } else {
                    Vec2 temp;
                    if (!line_intersect(p3, p4, v3, v4, temp)) {
                        continue; // Sigh
                    }
                    edgeverts.push_back(temp);
                    edgeindx.push_back(pos    );
                    edgeindx.push_back(pos+1  );
                    edgeindx.push_back(p2pos+2);
                    edgeindx.push_back(pos    );
                    edgeindx.push_back(ppos+3 );
                    edgeindx.push_back(pos+1  );
                    pos += 2;
                }
                break;
            }
            case Joint::ROUND:
            {
                // cap end
                float a0, step;
                unsigned int s_pos, e_pos;
                if (jangle < 0) {
                    a1 = pangle2 - PI_2;
                    a2 = angle + PI_2;
                    a0 = a2;
                    step = (abs(jangle)) / (float)JOINT_PRECISION;
                    s_pos = ppos + 3;
                    e_pos = p2pos + 1;
                } else {
                    a1 = angle - PI_2;
                    a2 = pangle2 + PI_2;
                    a0 = a1;
                    step = -(abs(jangle)) / (float)JOINT_PRECISION;
                    s_pos = ppos;
                    e_pos = p2pos + 2;
                }
                unsigned int opos = pos;
                edgeverts.push_back(a);
                pos += 1;
                for(int j = 0; j <  JOINT_PRECISION - 1; j++) {
                    edgeverts.push_back(a-Vec2(cos(a0 - step * j) * stroke,sin(a0 - step * j) * stroke));
                    if (j == 0) {
                        edgeindx.push_back(opos );
                        edgeindx.push_back(s_pos);
                        edgeindx.push_back(pos);
                    } else {
                        edgeindx.push_back(opos );
                        edgeindx.push_back(pos-1);
                        edgeindx.push_back(pos);
                    }
                    pos += 1;
                }
                edgeindx.push_back(opos );
                edgeindx.push_back(pos-1);
                edgeindx.push_back(e_pos);
                break;
            }
            case Joint::NONE:
                // Nothing to do
                break;
        }
    }
    
    // Process the caps
    switch (cap) {
        case Cap::SQUARE:
        {
            Vec2 temp = Vec2(cos(angle) * stroke,sin(angle) * stroke);
            edgeverts.push_back(v2+temp);
            edgeverts.push_back(v3+temp);
            edgeindx.push_back(ppos + 1);
            edgeindx.push_back(ppos + 2);
            edgeindx.push_back(pos  + 1);
            edgeindx.push_back(ppos + 1);
            edgeindx.push_back(pos);
            edgeindx.push_back(pos + 1);
            pos += 2;
            
            temp = Vec2(cos(sangle) * stroke,sin(sangle) * stroke);
            edgeverts.push_back(s1-temp);
            edgeverts.push_back(s4-temp);
            edgeindx.push_back(0);
            edgeindx.push_back(3);
            edgeindx.push_back(pos + 1 );
            edgeindx.push_back(0);
            edgeindx.push_back(pos);
            edgeindx.push_back(pos + 1 );
            pos += 2;
            break;
        }
        case Cap::ROUND:
        {
            // cap start
            float a1 = sangle - PI_2;
            float a2 = sangle + PI_2;
            float step = (a1 - a2) / (float)CAP_PRECISION;
            unsigned int opos = pos;
            c = _vertices[0];
            edgeverts.push_back(c);
            pos += 1;
            for(int i = 0; i < CAP_PRECISION - 1; i++) {
                Vec2 temp = Vec2(cos(a1 + step * i) * stroke,sin(a1 + step * i) * stroke);
                edgeverts.push_back(c+temp);
                if (i == 0) {
                    edgeindx.push_back(opos);
                    edgeindx.push_back(0);
                    edgeindx.push_back(pos);
                } else {
                    edgeindx.push_back(opos);
                    edgeindx.push_back(pos-1);
                    edgeindx.push_back(pos);
                }
                pos += 1;
            }
            edgeindx.push_back(opos );
            edgeindx.push_back(pos-1);
            edgeindx.push_back(3);
            
            // cap end
            a1 = angle - PI_2;
            a2 = angle + PI_2;
            step = (a2 - a1) / (float)CAP_PRECISION;
            opos = pos;
            c = _vertices[count-1];
            edgeverts.push_back(c);
            pos += 1;
            for(int i = 0; i < CAP_PRECISION - 1; i++) {
                Vec2 temp =  Vec2(cos(a1 + step * i) * stroke,sin(a1 + step * i) * stroke);
                edgeverts.push_back(c+temp);
                if (i == 0) {
                    edgeindx.push_back(opos  );
                    edgeindx.push_back(ppos+1);
                    edgeindx.push_back(pos);
                } else {
                    edgeindx.push_back(opos );
                    edgeindx.push_back(pos-1);
                    edgeindx.push_back(pos);
                }
                pos += 1;
            }
            edgeindx.push_back(opos);
            edgeindx.push_back(pos-1);
            edgeindx.push_back(ppos+2);
            break;
        }
        case Cap::NONE:
            // Nothing to do.
            break;
    }
    poly._vertices.assign(edgeverts.begin(), edgeverts.end());
    poly._indices.assign(edgeindx.begin(), edgeindx.end());
    poly.computeBounds();
    return &poly;
}

/*
Poly2* Poly2::extrude(Poly2& poly, float stroke, bool closed, Joint joint, Cap cap) {
    CCASSERT(stroke > 0, "invalid stroke value");
    poly._vertices.clear();
    poly._indices.clear();
    if (_vertices.size() == 0) {
        return &poly;
    }

#define _x(a) (2*(a))
#define _y(a) (2*(a)+1)
    
    int count = (int)_vertices.size();
    
    // Closed paths have no cap;
    if (closed and count > 2) {
        count += 2;
        cap = Cap::NONE;
    }
    
    // Determine the number of vertices and indices we need.
    int vcount = (count - 1) * 4;
    int icount = (count - 1) * 6;
    
    switch (joint) {
        case Joint::BEVEL:
            icount += (count - 2) * 3;
            vcount += (count - 2);
            break;
        case Joint::ROUND:
            icount += (JOINT_PRECISION * 3) * (count - 2);
            vcount += (JOINT_PRECISION) * (count - 2);
            break;
        case Joint::MITRE:
            icount += (count - 2) * 6;
            vcount += (count - 2) * 2;
            break;
        case Joint::NONE:
            // Nothing to do.
            break;
    }
    
    switch (cap) {
        case Cap::SQUARE:
            icount += 12;
            vcount += 4;
            break;
        case Cap::ROUND:
            icount += (CAP_PRECISION * 3) * 2;
            vcount += (CAP_PRECISION) * 2;
            break;
        case Cap::NONE:
            // Nothing to do.
            break;
    }
    
    float edgeverts[vcount*2];
    unsigned short edgeindx[icount];
    
    // Calculation time
    // Thanks Kivy guys for all the hard work.
    double ax, ay, bx, by, cx, cy, angle, a1, a2;
    double x1, y1, x2, y2, x3, y3, x4, y4;
    double sx1, sy1, sx4, sy4, sangle;
    double pcx, pcy, px1, py1, px2, py2, px3, py3, px4, py4, pangle, pangle2;
    double w = stroke;
    double ix, iy;
    unsigned int pos, p_pos, p_pos2, ipos;
    double jangle;
    
    angle = sangle = 0;
    pcx = pcy = cx = cy = ix = iy = 0;
    px1 = px2 = px3 = px4 = py1 = py2 = py3 = py4 = 0;
    sx1 = sy1 = sx4 = sy4 = 0;
    x1 = x2 = x3 = x4 = y1 = y2 = y3 = y4 = 0;
    double cos1 = 0, cos2 = 0, sin1 = 0, sin2 = 0;
    
    ipos = pos = p_pos = p_pos2 = 0;
    for(int i = 0; i < count-1; i++) {
        ax = _vertices[(i  ) % count].x;
        ay = _vertices[(i  ) % count].y;
        bx = _vertices[(i+1) % count].x;
        by = _vertices[(i+1) % count].y;
        
        if (i > 0 and joint != Joint::NONE) {
            pcx = cx; pcy = cy;
            px1 = x1; px2 = x2; px3 = x3; px4 = x4;
            py1 = y1; py2 = y2; py3 = y3; py4 = y4;
        }
        
        p_pos2 = p_pos; p_pos = pos;
        pangle2 = pangle; pangle = angle;
        
        // Calculate the orientation of the segment, between pi and -pi
        cx = bx - ax; cy = by - ay;
        angle = atan2(cy, cx);
        a1 = angle - PI_2; a2 = angle + PI_2;
        
        // Calculate the position of the segment
        cos1 = cos(a1) * w; sin1 = sin(a1) * w;
        cos2 = cos(a2) * w; sin2 = sin(a2) * w;
        x1 = ax + cos1; y1 = ay + sin1;
        x4 = ax + cos2; y4 = ay + sin2;
        x2 = bx + cos1; y2 = by + sin1;
        x3 = bx + cos2; y3 = by + sin2;
        
        if (i == 0) {
            sx1 = x1; sy1 = y1;
            sx4 = x4; sy4 = y4;
            sangle = angle;
        }
        
        edgeindx[ipos    ] = pos;
        edgeindx[ipos + 1] = pos + 1;
        edgeindx[ipos + 2] = pos + 2;
        edgeindx[ipos + 3] = pos;
        edgeindx[ipos + 4] = pos + 2;
        edgeindx[ipos + 5] = pos + 3;
        ipos += 6;
        
        // Textures, colors are set later.  Just initialize to safe values.
        edgeverts[_x(pos)] = x1;
        edgeverts[_y(pos)] = y1;
        pos += 1;
        edgeverts[_x(pos)] = x2;
        edgeverts[_y(pos)] = y2;
        pos += 1;
        edgeverts[_x(pos)] = x3;
        edgeverts[_y(pos)] = y3;
        pos += 1;
        edgeverts[_x(pos)] = x4;
        edgeverts[_y(pos)] = y4;
        pos += 1;
        
        // joint generation
        if (i == 0 or joint == Joint::NONE) {
            continue; // Sigh
        }
        
        // calculate the angle of the previous and current segment
        jangle = atan2(cx * pcy - cy * pcx,cx * pcx + cy * pcy);
        
        // in case of the angle is NULL, avoid the generation
        if (jangle == 0) {
            switch (joint) {
                case Joint::ROUND:
                    vcount -= JOINT_PRECISION;
                    icount -= JOINT_PRECISION * 3;
                    break;
                case Joint::BEVEL:
                    vcount -= 1;
                    icount -= 3;
                    break;
                case Joint::MITRE:
                    vcount -= 2;
                    icount -= 6;
                    break;
                case Joint::NONE:
                    // Nothing to do.
                    break;
            }
            continue; // Sigh
        }
        
        switch (joint) {
            case Joint::BEVEL:
            {
                edgeverts[_x(pos)] = ax;
                edgeverts[_y(pos)] = ay;
                if (jangle < 0) {
                    edgeindx[ipos  ] = p_pos2 + 1;
                    edgeindx[ipos+1] = p_pos;
                    edgeindx[ipos+2] = pos;
                } else {
                    edgeindx[ipos  ] = p_pos2+ 2;
                    edgeindx[ipos+1] = p_pos + 3;
                    edgeindx[ipos+2] = pos;
                }
                ipos += 3;
                pos += 1;
                break;
            }
            case Joint::MITRE:
            {
                edgeverts[_x(pos)] = ax;
                edgeverts[_y(pos)] = ay;
                if (jangle < 0) {
                    if (line_intersection(px1, py1, px2, py2, x1, y1, x2, y2, &ix, &iy) == 0) {
                        vcount -= 2;
                        icount -= 6;
                        continue; // Sigh
                    }
                    edgeverts[_x(pos+1)] = ix;
                    edgeverts[_y(pos+1)] = iy;
                    edgeindx[ipos  ] = pos;
                    edgeindx[ipos+1] = pos+1;
                    edgeindx[ipos+2] = p_pos2+1;
                    edgeindx[ipos+3] = pos;
                    edgeindx[ipos+4] = p_pos;
                    edgeindx[ipos+5] = pos+1;
                    ipos += 6;
                    pos += 2;
                } else {
                    if (line_intersection(px3, py3, px4, py4, x3, y3, x4, y4, &ix, &iy) == 0) {
                        vcount -= 2;
                        icount -= 6;
                        continue; // Sigh
                    }
                    edgeverts[_x(pos+1)] = ix;
                    edgeverts[_y(pos+1)] = iy;
                    edgeindx[ipos  ] = pos;
                    edgeindx[ipos+1] = pos+1;
                    edgeindx[ipos+2] = p_pos2+2;
                    edgeindx[ipos+3] = pos;
                    edgeindx[ipos+4] = p_pos+3;
                    edgeindx[ipos+5] = pos+1;
                    ipos += 6;
                    pos += 2;
                }
                break;
            }
            case Joint::ROUND:
            {
                // cap end
                double a0, step;
                double p_pos_str, p_pos_end;
                if (jangle < 0) {
                    a1 = pangle2 - PI_2;
                    a2 = angle + PI_2;
                    a0 = a2;
                    step = (abs(jangle)) / (float)JOINT_PRECISION;
                    p_pos_str = p_pos + 3;
                    p_pos_end = p_pos2 + 1;
                } else {
                    a1 = angle - PI_2;
                    a2 = pangle2 + PI_2;
                    a0 = a1;
                    step = -(abs(jangle)) / (float)JOINT_PRECISION;
                    p_pos_str = p_pos;
                    p_pos_end = p_pos2 + 2;
                }
                unsigned int s_pos = pos;
                edgeverts[_x(pos)] = ax;
                edgeverts[_y(pos)] = ay;
                pos += 1;
                for(int j = 0; j <  JOINT_PRECISION - 1; j++) {
                    edgeverts[_x(pos)] = ax - cos(a0 - step * j) * w;
                    edgeverts[_y(pos)] = ay - sin(a0 - step * j) * w;
                    if (j == 0) {
                        edgeindx[ipos  ] = s_pos;
                        edgeindx[ipos+1] = p_pos_str;
                        edgeindx[ipos+2] = pos;
                    } else {
                        edgeindx[ipos  ] = s_pos;
                        edgeindx[ipos+1] = pos-1;
                        edgeindx[ipos+2] = pos;
                    }
                    pos += 1;
                    ipos += 3;
                }
                edgeindx[ipos  ] = s_pos;
                edgeindx[ipos+1] = pos - 1;
                edgeindx[ipos+2] = p_pos_end;
                ipos += 3;
                break;
            }
            case Joint::NONE:
                // Nothing to do
                break;
        }
    }
    
    // Process the caps
    switch (cap) {
        case Cap::SQUARE:
        {
            edgeverts[_x(pos  )] = x2 + cos(angle) * w;
            edgeverts[_y(pos  )] = y2 + sin(angle) * w;
            edgeverts[_x(pos+1)] = x3 + cos(angle) * w;
            edgeverts[_y(pos+1)] = y3 + sin(angle) * w;
            edgeindx[ipos  ] = p_pos + 1;
            edgeindx[ipos+1] = p_pos + 2;
            edgeindx[ipos+2] = pos + 1;
            edgeindx[ipos+3] = p_pos + 1;
            edgeindx[ipos+4] = pos;
            edgeindx[ipos+5] = pos + 1;
            ipos += 6;
            pos += 2;
            edgeverts[_x(pos)  ] = sx1 - cos(sangle) * w;
            edgeverts[_y(pos)  ] = sy1 - sin(sangle) * w;
            edgeverts[_x(pos+1)] = sx4 - cos(sangle) * w;
            edgeverts[_y(pos+1)] = sy4 - sin(sangle) * w;
            edgeindx[ipos  ] = 0;
            edgeindx[ipos+1] = 3;
            edgeindx[ipos+2] = pos + 1;
            edgeindx[ipos+3] = 0;
            edgeindx[ipos+4] = pos;
            edgeindx[ipos+5] = pos + 1;
            ipos += 6;
            pos += 2;
            break;
        }
        case Cap::ROUND:
        {
            // cap start
            a1 = sangle - PI_2;
            a2 = sangle + PI_2;
            float step = (a1 - a2) / (float)CAP_PRECISION;
            unsigned int s_pos = pos;
            cx = _vertices[0].x; cy = _vertices[0].y;
            edgeverts[_x(pos)] = cx;
            edgeverts[_y(pos)] = cy;
            pos += 1;
            for(int i = 0; i < CAP_PRECISION - 1; i++) {
                edgeverts[_x(pos)] = cx + cos(a1 + step * i) * w;
                edgeverts[_y(pos)] = cy + sin(a1 + step * i) * w;
                if (i == 0) {
                    edgeindx[ipos  ] = s_pos;
                    edgeindx[ipos+1] = 0;
                    edgeindx[ipos+2] = pos;
                } else {
                    edgeindx[ipos  ] = s_pos;
                    edgeindx[ipos+1] = pos - 1;
                    edgeindx[ipos+2] = pos;
                }
                pos += 1;
                ipos += 3;
            }
            edgeindx[ipos  ] = s_pos;
            edgeindx[ipos+1] = pos - 1;
            edgeindx[ipos+2] = 3;
            ipos += 3;
            
            // cap end
            a1 = angle - PI_2;
            a2 = angle + PI_2;
            step = (a2 - a1) / (float)CAP_PRECISION;
            s_pos = pos;
            cx = _vertices[count-1].x;
            cy = _vertices[count-1].y;
            edgeverts[_x(pos)] = cx;
            edgeverts[_y(pos)] = cy;
            pos += 1;
            for(int i = 0; i < CAP_PRECISION - 1; i++) {
                edgeverts[_x(pos)] = cx + cos(a1 + step * i) * w;
                edgeverts[_y(pos)] = cy + sin(a1 + step * i) * w;
                if (i == 0) {
                    edgeindx[ipos  ] = s_pos;
                    edgeindx[ipos+1] = p_pos + 1;
                    edgeindx[ipos+2] = pos;
                } else {
                    edgeindx[ipos  ] = s_pos;
                    edgeindx[ipos+1] = pos - 1;
                    edgeindx[ipos+2] = pos;
                }
                pos += 1;
                ipos += 3;
            }
            edgeindx[ipos  ] = s_pos;
            edgeindx[ipos+1] = pos - 1;
            edgeindx[ipos+2] = p_pos + 2;
            ipos += 3;
            break;
        }
        case Cap::NONE:
            // Nothing to do.
            break;
    }
    Vec2* verts = (Vec2*)edgeverts;
    poly._vertices.assign(verts, verts+vcount);
    poly._indices.assign(edgeindx, edgeindx+icount);
    poly.computeBounds();
    return &poly;
}
*/

#pragma mark -
#pragma mark Polygon Operations

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
Poly2& Poly2::operator*=(float scale) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii] *= scale;
    }
    
    computeBounds();
    return *this;
}

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
Poly2& Poly2::operator*=(const Vec2& scale) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii].x *= scale.x;
        _vertices[ii].y *= scale.y;
    }
    
    computeBounds();
    return *this;
}

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
Poly2& Poly2::operator*=(const Mat4& transform) {
    Vec3 tmp;
    for(int ii = 0; ii < _vertices.size(); ii++) {
        transform.transformVector(_vertices[ii].x, _vertices[ii].y, 0.0f, 1.0f, &tmp);
        _vertices[ii].x = tmp.x;
        _vertices[ii].y = tmp.y;
    }
    
    computeBounds();
    return *this;
    
}

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
Poly2& Poly2::operator/=(float scale) {
    CCASSERT(scale != 0, "Division by 0");
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii].x /= scale;
        _vertices[ii].y /= scale;
    }
    
    computeBounds();
    return *this;
}

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
Poly2& Poly2::operator/=(const Vec2& scale) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii].x /= scale.x;
        _vertices[ii].y /= scale.y;
    }
    
    computeBounds();
    return *this;
}

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
Poly2& Poly2::operator+=(float offset) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii].x += offset;
        _vertices[ii].y += offset;
    }
    
    computeBounds();
    return *this;
}

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
Poly2& Poly2::operator+=(const Vec2& offset) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii] += offset;
    }
    
    computeBounds();
    return *this;
}


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
Poly2& Poly2::operator-=(float offset) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii].x -= offset;
        _vertices[ii].y -= offset;
    }
    
    computeBounds();
    return *this;
}

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
Poly2& Poly2::operator-=(const Vec2& offset) {
    for(int ii = 0; ii < _vertices.size(); ii++) {
        _vertices[ii] -= offset;
    }
    
    computeBounds();
    return *this;
}


#pragma mark -
#pragma mark Internal Helpers

/**
 * Compute the bounding box for this polygon.
 *
 * The bounding box is the minimal rectangle that contains all of the vertices in
 * this polygon.  It is recomputed whenever the vertices are set.
 */
void Poly2::computeBounds() {
    float minx, maxx;
    float miny, maxy;

    minx = _vertices[0].x;
    maxx = _vertices[0].x;
    miny = _vertices[0].y;
    maxy = _vertices[0].y;
    for(auto it = _vertices.begin()+1; it != _vertices.end(); ++it) {
        if (it->x < minx) {
            minx = it->x;
        } else if (it->x > maxx) {
            maxx = it->x;
        }
        if (it->y < miny) {
            miny = it->y;
        } else if (it->y > maxy) {
            maxy = it->y;
        }
    }
    
    _bounds.origin.x = minx;
    _bounds.origin.y = miny;
    _bounds.size.width  = maxx-minx;
    _bounds.size.height = maxy-miny;
}


#pragma mark -
#pragma mark Ear-Clipping Triangulation

/** Computes the previous index in a vector, treating it as a circular queue */
#define PREV(i,idx) ((i == 0 ? (int)idx.size() : i) - 1)
/** Computes the next index in a vector, treating it as a circular queue */
#define NEXT(i,idx) ((i + 1) % (int)idx.size())

/// Ear-clipping functions
/// This is a simple implementation for the ear cutting algorithm to triangulate simple
/// polygons.  It will not handle polygons with holes or with self intersections.
///
/// This implementation is largely copied from the LibGDX implementation by Nicolas
/// Gramlich, Eric Spits, Thomas Cate, and Nathan Sweet.

/**
 * Enumeration of vertex types (for triangulation purposes)
 *
 * A vertex type is classified by the area spanned by this vertex and its adjacent
 * neighbors.  If the interior angle is outside of the polygon, it is CONCAVE.  If it
 * is inside the polygon, it is CONVEX.
 */
enum VertexType {
    /** Vertex and its immediate neighbors form a concave polygon */
    CONCAVE    = -1,
    /** Vertex is colinear with its immediate neighbors */
    TANGENTIAL = 0,
    /** Vertex and its immediate neighbors form a convex polygon */
    CONVEX     = 1
};

/**
 * Classify the vertex p2 according to its immediate neighbors.
 *
 * If the interior angle is outside of the polygon, it is CONCAVE.  If it is inside
 * the polygon, it is CONVEX.
 *
 * @param p1    The previous vertex
 * @param p2    The current vertex
 * @param p3    The next vertex
 *
 * @return the VertexType of vertex p2
 */
VertexType compute_spanned_area_type(const Vec2& p1, const Vec2& p2, const Vec2& p3) {
    float area = p1.x * (p3.y - p2.y);
    area += p2.x * (p1.y - p3.y);
    area += p3.x * (p2.y - p1.y);
    VertexType result = (area < 0 ? CONCAVE : (area > 0 ? CONVEX : TANGENTIAL));
    return result;
}

/**
 * Returns true if the vertices are arranged clockwise about the interior.
 *
 * @param vertices  The array of vertices to check
 *
 * @return true if the vertices are arranged clockwise about the interior
 */
bool are_vertices_clockwise (const vector<Vec2>& vertices) {
    if (vertices.size() <= 2) {
        return false;
    }
    
    float area = 0;
    Vec2 p1, p2;
    for (int i = 0, n = (int)vertices.size() - 3; i < n; i += 2) {
        p1 = vertices[i  ];
        p2 = vertices[i+1];
        area += p1.x * p2.y - p2.x * p1.y;
    }
    p1 = vertices[vertices.size()-1];
    p2 = vertices[0];
    return area + p1.x * p2.y - p2.x * p1.y < 0;
}

/**
 * Remove an ear tipe from the naive triangulation, adding it to the output.
 *
 * This function modifies both indices and types, removing the clipped triangle.
 * The triangle is defined by the given index and its immediate neighbors on
 * either side.
 *
 * @param earTipIndex  The index indentifying the triangle
 * @param indices      The indices for the naive triangulation
 * @param types        The vertex types corresponding to the triangulation
 * @param output       The vector to store the clipped triangle
 */
void cut_ear_tip(int earTipIndex, vector<unsigned short>& indices, vector<VertexType>& types,
                 vector<unsigned short>& output) {
    output.push_back(indices[PREV(earTipIndex, indices)]);
    output.push_back(indices[earTipIndex]);
    output.push_back(indices[NEXT(earTipIndex, indices)]);
    
    indices.erase(indices.begin() + earTipIndex);
    types.erase(types.begin() + earTipIndex);
}

/**
 * Returns true if the specified triangle is an eartip.
 *
 * The triangle is defined by the given index and its immediate neighbors on
 * either side.
 *
 * @param earTipIndex  The index indentifying the triangle
 * @param vertices     The vertex coordinates of the polygon
 * @param indices      The indices for the naive triangulation
 * @param types        The types corresponding to each vertex
 *
 * @return true if the specified triangle is an eartip
 */
bool is_ear_tip (int earTipIndex, const vector<Vec2>& vertices, vector<unsigned short>& indices,
                 vector<VertexType>& types) {
    if (types[earTipIndex] == CONCAVE) {
        return false;
    }
    
    int prevIndex = PREV(earTipIndex, indices);
    int nextIndex = NEXT(earTipIndex, indices);
    
    int p1 = indices[prevIndex];
    int p2 = indices[earTipIndex];
    int p3 = indices[nextIndex];
    
    Vec2 v1 = vertices[p1];
    Vec2 v2 = vertices[p2];
    Vec2 v3 = vertices[p3];
    
    // Check if any point is inside the triangle formed by previous, current and next vertices.
    // Only consider vertices that are not part of this triangle, or else we'll always find one inside.
    for (int i = NEXT(nextIndex, indices); i != prevIndex; i = NEXT(i, indices)) {
        // Concave vertices can obviously be inside the candidate ear, but so can tangential vertices
        // if they coincide with one of the triangle's vertices.
        if (types[i] != CONVEX) {
            int v = indices[i];
            Vec2 vt = vertices[v];
            // Because the polygon has clockwise winding order, the area sign will be positive if the point
            // is strictly inside. It will be 0 on the edge, which we want to include as well.
            // note: check the edge defined by p1->p3 first since this fails _far_ more then the other 2 checks.
            if (compute_spanned_area_type(v3, v1, vt) >= 0) {
                if (compute_spanned_area_type(v1, v2, vt) >= 0) {
                    if (compute_spanned_area_type(v2, v3, vt) >= 0) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

/**
 * Returns a candidate ear-tip triangle
 *
 * The triangle is defined by the given index and its immediate neighbors on
 * either side.  A triangle is a candidate if the defining vertex is convex
 * or tangential.
 *
 * @param vertices  The vertex coordinates of the polygon
 * @param indices   The indices for the naive triangulation
 * @param types     The types corresponding to each vertex
 *
 * @return a candidate ear-tip triangle
 */
int find_ear_tip (const vector<Vec2>& vertices, vector<unsigned short>& indices, vector<VertexType>& types) {
    for (int i = 0; i < indices.size(); i++) {
        if (is_ear_tip(i, vertices, indices, types)) {
            return i;
        }
    }
    
    // Desperate mode: if no vertex is an ear tip, we are dealing with a degenerate polygon
    // (e.g. nearly collinear). Note that the input was not necessarily degenerate, but we could
    // have made it so by clipping some valid ears.
    
    // Idea taken from Martin Held, "FIST: Fast industrial-strength triangulation of polygons",
    // Algorithmica (1998), http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.115.291
    
    // Return a convex or tangential vertex if one exists.
    for (int i = 0; i < indices.size(); i++) {
        if (types[i] != CONCAVE) {
            return i;
        }
    }
    
    return 0; // If all vertices are concave, just return the first one.
}

/**
 * Returns the classification for the vertex at the given index.
 *
 * A vertex type is classified by the area spanned by this vertex and its adjacent
 * neighbors.  If the interior angle is outside of the polygon, it is CONCAVE.  If it
 * is inside the polygon, it is CONVEX.
 *
 * @param index     The vertex index for vertices
 * @param vertices  The vertex coordinates of the polygon
 * @param indices   The indices for the naive triangulation
 *
 * @return the classification for the vertex at the given index
 */
VertexType classify_vertex (int index, const vector<Vec2>& vertices, vector<unsigned short>& indices) {
    int prev = indices[PREV(index, indices)];
    int curr = indices[index];
    int next = indices[NEXT(index, indices)];
    VertexType result = compute_spanned_area_type(vertices[prev],vertices[curr],vertices[next]);
    return result;
}

/**
 * Computes the indices for a triangulation of the given vertices.
 *
 * The indices are references to vertices in the given vertex array.  Vertex 0 is
 * the one with coordinates vertices[offset] and vertices[offset+1].  There will
 * be three times as many indices as triangles.
 *
 * This function uses ear-clipping triangulation. The function culls all degenerate
 * triangles from the result before returning.
 *
 * @param vertices  The array of vertices in this polygon
 * @param indices   The naive triangulation for this polygon
 * @param types     The types corresponding to each vertex
 * @param output    The vector to store the triangulation
 *
 * @return A vector of vertex indices representing a triangulation.
 */
void compute_triangulation(const vector<Vec2>& vertices, vector<unsigned short>& indices,
                           vector<VertexType>& types, vector<unsigned short>& output) {
    while (indices.size() > 3) {
        int earTipIndex = find_ear_tip(vertices, indices, types);
        cut_ear_tip(earTipIndex, indices, types, output);
        
        // The type of the two vertices adjacent to the clipped vertex may have changed.
        int prevIndex = PREV(earTipIndex, indices);
        int nextIndex = earTipIndex == indices.size() ? 0 : earTipIndex;
        types[prevIndex] = classify_vertex(prevIndex, vertices, indices);
        types[nextIndex] = classify_vertex(nextIndex, vertices, indices);
    }
    
    if (indices.size() == 3) {
        output.push_back(indices[0]);
        output.push_back(indices[1]);
        output.push_back(indices[2]);
    }
}

/**
 * Removes colinear vertices from the given triangulation.
 *
 * Because we permit tangential vertices as ear-clips, this triangulator will occasionally
 * return colinear vertices.  This will crash OpenGL, so we remove them.
 *
 * @param vertices The polygon vertices
 * @param indices The triangulation indices
 *
 * @return reference to the triangulation indices
 */
vector<unsigned short>& trim_colinear(const vector<Vec2>& vertices, vector<unsigned short>& indices) {
    int colinear = 0;
    for(int ii = 0; ii < indices.size()/3-colinear; ii++) {
        float t1 = vertices[indices[3*ii  ]].x*(vertices[indices[3*ii+1]].y-vertices[indices[3*ii+2]].y);
        float t2 = vertices[indices[3*ii+1]].x*(vertices[indices[3*ii+2]].y-vertices[indices[3*ii  ]].y);
        float t3 = vertices[indices[3*ii+2]].x*(vertices[indices[3*ii  ]].y-vertices[indices[3*ii+1]].y);
        if (fabs(t1+t2+t3) < 0.0000001f) {
            iter_swap(indices.begin()+(3*ii  ), indices.end()-(3*colinear+3));
            iter_swap(indices.begin()+(3*ii+1), indices.end()-(3*colinear+2));
            iter_swap(indices.begin()+(3*ii+2), indices.end()-(3*colinear+1));
            colinear++;
        }
    }
    
    if (colinear > 0) {
        indices.erase(indices.end()-3*colinear,indices.end());
    }
    return indices;
}

/**
 * Returns the indices for a triangulation of the given vertices.
 *
 * The indices are references to vertices in the given vertex list. There will
 * be three times as many indices as triangles.
 *
 * This function uses ear-clipping triangulation. The function culls all degenerate
 * triangles from the result before returning.
 *
 * This method differs from the other triangulation method in that it allows the
 * user to provide a vector for storing the triangulation.
 *
 * @param vertices  The vector of vertices in this polygon
 * @param output    The vector to store the output
 *
 * @return A reference to the output vector
 */
vector<unsigned short>& ear_triangulate(const vector<Vec2>& vertices, vector<unsigned short>& output) {
    
    int vcount = (int)vertices.size();
    vector<unsigned short> indices;
    indices.reserve(vcount);
    indices.resize(vcount,0);
    
    if (are_vertices_clockwise(vertices)) {
        for (short i = 0; i < vcount; i++) {
            indices[i] = i;
        }
    } else {
        for (int i = 0, n = vcount - 1; i < vcount; i++) {
            indices[i] = (short)(n - i); // Reversed.
        }
    }
    
    vector<VertexType> types;
    types.reserve(vcount);
    for (int i = 0; i < vcount; ++i) {
        types.push_back(classify_vertex(i, vertices, indices));
    }
    
    // A polygon with n vertices has a triangulation of n-2 triangles.
    output.clear();
    output.reserve(MAX(0, vcount - 2) * 3);
    compute_triangulation(vertices, indices, types, output);
    return trim_colinear(vertices, output);
}

/**
 * Returns the indices for a triangulation of the given vertices.
 *
 * The indices are references to vertices in the given vertex list. There will
 * be three times as many indices as triangles.
 *
 * This function uses ear-clipping triangulation. The function culls all degenerate
 * triangles from the result before returning.
 *
 * @param vertices  The vector of vertices in this polygon
 *
 * @return A vector of vertex indices representing a triangulation.
 */
vector<unsigned short> ear_triangulate(const vector<Vec2>& vertices) {
    vector<unsigned short> result;
    return ear_triangulate(vertices, result);
}

NS_CC_END
