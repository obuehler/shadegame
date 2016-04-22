//
//  CUPolynomial.h
//  Cornell Extensions to Cocos2D
//
//  This module provides a class that represents a polynomial. It has basic methods
//  for evaluation and root finding.  The primary purpose of this class is to support
//  CubicBezier and other splines. However, we provide it publicly in case it is
//  useful for other applications.
//
//  Math data types are much lighter-weight than other objects, and are intended to
//  be copied.  That is why we do not use reference counting for these objects.
//
//  Author: Walker White
//  Version: 11/24/15
//
#ifndef __CU_POLYNOMIAL_H__
#define __CU_POLYNOMIAL_H__

#include <cocos2d.h>
#include <iostream>
#include <vector>

using namespace std;

NS_CC_BEGIN


#pragma mark -
#pragma mark Polynomial

/**
 * Class to represent a polynomial.
 *
 * A polynomial is a vector of floats.  This vector represents the polynomial from highest
 * degree to constant.  For example, the vector [1, -1, 2, 0, -3] is equivalent to
 *
 *    1*x^4  - 1*x^3  + 2*x^2 + 0*x - 3
 * 
 * Therefore, the degree of the polynomial is one less than the length of the list. 
 *
 * We make all of the vector methods still available. However, note that there is some danger
 * in using the vector methods carelessly.  In order to be well-formed, a polynomial vector
 * must have at least one element.  Furthermore, if it has more than one element, the first
 * element must be non-zero.  If this is not the case, we cannot guarantee that any of the
 * polynomial methods (such as root finding) will work properly. 
 *
 * This is the motivation for the isValid() and validate() methods.  If you believe there is
 * some possibility of the polynomial being corrupted, you should use these.
 */
class CC_DLL Polynomial : public vector<float> {
public:
#pragma mark Constructors
    /**
     * Creates a zero polynomial
     */
    Polynomial() : std::vector<float>(1,0) { }
    
    /**
     * Creates the polynomial x^d where is the degree.
     *
     * The first coefficient is 1.  All other coefficients are 0.
     *
     * @param  degree   the degree of the polynomial
     */
    Polynomial(long degree) : vector<float>(degree+1,0)	{
        (*this)[0] = 1;
    }
    
    /**
     * Creates the polynomial x^d where is the degree.
     *
     * The value is the coefficient of all terms.  This has a chance of making an invalid
     * polynomial (e.g. if value is 0).  However, this constructor does not enforce validity.
     * Hence it is a safe way to create a 0 polynomial with multiple terms.
     *
     * @param  degree   the degree of the polynomial
     * @param  value    the coefficient of each term
     */

    Polynomial(long degree, float value) : vector<float>(degree+1,value) {
    }

    /**
     * Creates a copy of the given polynomial.
     *
     * @param  poly   the polynomial to copy
     */
    Polynomial(const Polynomial& poly) : vector<float>(poly) {
    }
    
    /**
     * Creates a polynomial from the given iterators.
     *
     * The elements are copied in order.  A valid iterator must have at least one element,
     * and the first element cannot be 0 if there is more than one element.
     *
     * This constructor is provided for fast copying from other vectors.
     *
     * @param  first   the beginning iterator
     * @param  last    the terminating iterator
     */
    Polynomial(const_iterator first, const_iterator last) : vector<float>(first,last) {
        CCASSERT(isValid(), "The array data is invalid");
    }
    
    /**
     * Creates a polynomial from the given array.
     *
     * The elements are copied in order.  A valid array must have at least one element,
     * and the first element cannot be 0 if there is more than one element.
     *
     * @param  array   the array of polynomial coefficients
     * @param  size    the number of elements to copy from the array
     * @param  offset  the offset position to start in the array
     */
    Polynomial(float* array, unsigned int size, unsigned int offset=0) : vector<float>() {
        assign(array+offset,array+size+offset);
        CCASSERT(isValid(), "The array data is invalid");
    }
   
    /**
     * Deletes this polynomial
     */
    virtual ~Polynomial() { }

    
#pragma mark Accessors
    /**
     * Returns the degree of this polynomial.
     *
     * The degree is 1 less than the size.  We make the degree a long instead of size-type
     * so that it is safe to use in math formulas where the degree may go negative.
     *
     * @return the degree of this polynomial.
     */
    long degree() const  { return (long)size()-1; }

    /**
     * Returns true if this polynomial is a constant.
     *
     * @return true if this polynomial is a constant.
     */
    bool constant() const  { return size() == 1; }

    /**
     * Returns true if the polynomial is valid.
     *
     * A valid polynomial is a vector of at least one element, and the first element cannot 
     * be 0 if there is more than one element.
     *
     * @return true if the polynomial is valid.
     */
    bool isValid() const { return size() == 1 || (size() > 1 && at(0) != 0); }
    
    /**
     * Returns true if the polynomial is the zero polynomial.
     *
     * The zero polynomial has exactly one element and the value is 0.  An invalid polynomial
     * will return false if this method is called.
     *
     * @return true if the polynomial is the zero polynomial.
     */
    bool isZero() const  { return size() == 1 && at(0) == 0; }

    
#pragma mark Basic Methods
    /**
     * Returns the derivative of this polynomial
     *
     * The derivative has degree one less than original, unless it the original has
     * degree 1.  In that case, the derivative is 0.
     *
     * @return the derivative of this polynomial
     */
    Polynomial derivative() const;

    /**
     * Returns the evaluation of the polynomial on the given value.
     *
     * Evaluation plugs the value in for the polynomial variable. 
     *
     * @return the evaluation of the polynomial on the given value.
     */
    float evaluate(float value) const;
    
    /**
     * Converts this polynomial into an equivalent valid polynomial.
     *
     * This method trims the zero values from the front of the vector until reaching
     * a non-zero value, or there is only one value left.
     */
    void validate();
    
    /**
     * Converts this polynomial into the associated mononomial.
     *
     * This method divides the polynomial by the coefficient of the first term. If
     * the polynomial is invalid, this method will fail.
     *
     * @return the coefficient divider of the original polynomial
     */
    float normalize();

    /**
     * Computes the roots of this polynomial using Bairstow's method
     *
     * Bairstow's method is an approximate root finding technique.  The value
     * epsilon is the error value for all of the roots.  A good description
     * of Bairstow's method can be found here:
     *
     *    http://nptel.ac.in/courses/122104019/numerical-analysis/Rathish-kumar/ratish-1/f3node9.html
     *
     * The roots are stored in the provided vector.  When complete, the vector
     * will have degree many elements.  If any root is complex, this method will
     * have added NaN in its place.
     *
     * It is possible for Bairstow's method to fail, which is why this method
     * has a return value.
     *
     * @param  roots    the vector to store the root values
     * @param  epsilon  the error tolerance for the root values
     *
     * @return true if Bairstow's method completes successfully
     */
    bool roots(vector<float>& roots, float epsilon) const;
    
    /**
     * Returns a string representation of this polynomial.
     *
     * There are two ways to represent a polynomial. One is in polynomial form, like
     * 
     *    x^4 - x^3 + 2x^2 - 3
     *
     * Alternatively, we could represent the same polynomial as its vector contents
     * [1, -1, 2, 0, -3].  This is the purpose of the optional parameter.
     *
     * @param  format   whether to format as a polynomial
     *
     * @return a string representation of this polynomial
     */
    string toString(bool format=true) const;

    
#pragma mark Operators
    /**
     * Adds the given polynomial in place.
     *
     * @param  other    The polynomial to add
     *
     * @return This polynomial, modified.
     */
    Polynomial& operator+=(const Polynomial& other);
    
    /**
     * Subtracts this polynomial by the given polynomial in place.
     *
     * @param  other    The polynomial to subtract
     *
     * @return This polynomial, modified.
     */
    Polynomial& operator-=(const Polynomial& other);
    
    /**
     * Multiplies the given polynomial in place.
     *
     * @param  other    The polynomial to multiply
     *
     * @return This polynomial, modified.
     */
    Polynomial& operator*=(const Polynomial& other) {
        return *this = (*this)*other;
    }
    
    /**
     * Divides this polynomial by the given polynomial in place.
     *
     * If other is not valid, then this method will fail.
     *
     * @param  other    The polynomial to divide by
     *
     * @return This polynomial, modified.
     */
    Polynomial& operator/=(const Polynomial& other);

    /**
     * Assigns this polynomial the division remainder of the other polynomial.
     *
     * If other is not valid, then this method will fail.
     *
     * @param  other    The polynomial to divide by
     *
     * @return This polynomial, modified.
     */
    Polynomial& operator%=(const Polynomial& other);

    /**
     * Returns the sum of this polynomial and other.
     *
     * @param  other    The polynomial to add
     *
     * @return the sum of this polynomial and other.
     */
    Polynomial operator+(const Polynomial& other) const {
        return Polynomial(*this) += other;
    }

    /**
     * Returns the result of subtracting other from this.
     *
     * @param  other    The polynomial to subtract
     *
     * @return the result of subtracting other from this.
     */
    Polynomial operator-(const Polynomial& other) const {
        return Polynomial(*this) -= other;
    }

    /**
     * Returns the product of this polynomial and other.
     *
     * @param  other    The polynomial to multiply
     *
     * @return the product of this polynomial and other.
     */
    Polynomial operator*(const Polynomial& other) const;

    /**
     * Returns the result of dividing this polynomial by other.
     *
     * @param  other    The polynomial to divide by
     *
     * @return the result of dividing this polynomial by other.
     */
    Polynomial operator/(const Polynomial& other) const {
        return Polynomial(*this) /= other;
    }
    
    /**
     * Returns the remainder when dividing this polynomial by other.
     *
     * @param  other    The polynomial to divide by
     *
     * @return the remainder when dividing this polynomial by other.
     */
    Polynomial operator%(const Polynomial& other) const {
        return Polynomial(*this) %= other;
    }

    /**
     * Adds the given constant in place.
     *
     * @param  value    The value to add
     *
     * @return This polynomial, modified.
     */
    Polynomial& operator+=(float value);
    
    /**
     * Subtracts this polynomial by the given value in place.
     *
     * @param  value    The value to subtract
     *
     * @return This polynomial, modified.
     */
    Polynomial& operator-=(float value);
    
    /**
     * Multiplies the given value in place.
     *
     * @param  value    The value to multiply
     *
     * @return This polynomial, modified.
     */
    Polynomial& operator*=(float value);
    
    /**
     * Divides this polynomial by the given value in place.
     *
     * If value is zero, then this method will fail.
     *
     * @param  value    The value to divide by
     *
     * @return This polynomial, modified.
     */
    Polynomial& operator/=(float value);

    /**
     * Assigns this polynomial the division remainder of value.
     *
     * If value is zero, then this method will fail.
     *
     * @param  value    The value to divide by
     *
     * @return This polynomial, modified.
     */
    Polynomial& operator%=(float value);
    
    /**
     * Returns the sum of this polynomial and value.
     *
     * @param  value    The value to add
     *
     * @return the sum of this polynomial and value.
     */
    Polynomial operator+(float value) const {
        return Polynomial(*this) += value;
    }

    /**
     * Returns the result of subtracting value from this.
     *
     * @param  value    The value to subtract
     *
     * @return the result of subtracting value from this.
     */
    Polynomial operator-(float value) const {
        return Polynomial(*this) += value;
    }
    
    /**
     * Returns the product of this polynomial and value.
     *
     * @param  value    The value to multiply
     *
     * @return the product of this polynomial and value.
     */
    Polynomial operator*(float value) const {
        return Polynomial(*this) += value;
    }

    /**
     * Returns the result of dividing this polynomial by value.
     *
     * @param  value    The value to divide by
     *
     * @return the result of dividing this polynomial by value.
     */
    Polynomial operator/(float value) const {
        return Polynomial(*this) /= value;
    }
    
    /**
     * Returns the remainder when dividing this polynomial by value.
     *
     * @param  value    The value to divide by
     *
     * @return the remainder when dividing this polynomial by value.
     */
    Polynomial operator%(float value) const {
        return Polynomial(*this) /= value;
    }
    
    
#pragma mark Friend Functions
    /**
     * Returns the sum of the polynomial and value.
     *
     * @param  left    The value to add
     * @param  right   The polynomial
     *
     * @return the sum of the polynomial and value.
     */
    friend Polynomial operator+(float left, const Polynomial& right);

    /**
     * Returns the result of subtracting the polynomial from value.
     *
     * @param  left    The initial value
     * @param  right   The polynomial to subtract
     *
     * @return the result of subtracting the polynomial from value.
     */
    friend Polynomial operator-(float left, const Polynomial& right);
    
    /**
     * Returns the product of the polynomial and value.
     *
     * @param  left    The value to multiply
     * @param  right   The polynomial
     *
     * @return the product of the polynomial and value.
     */
    friend Polynomial operator*(float left, const Polynomial& right);

    /**
     * Returns the result of dividing value by the polynomial.
     *
     * The result will always be 0, unless the polynomial is a
     * constant.
     *
     * @param  left    The initial value
     * @param  right   The polynomial to divide by
     *
     * @return the result of dividing value by the polynomial.
     */
    friend Polynomial operator/(float left, const Polynomial& right);
    
    /**
     * Returns the remainder when dividing value by the polynomial.
     *
     * The value will be the polynomial unless the polynomial is constant.
     *
     * @param  left    The initial value
     * @param  right   The polynomial to divide by
     *
     * @return the remainder when dividing value by the polynomial.
     */
    friend Polynomial operator%(float left, const Polynomial& right);
    
    /**
     * Outputs this polynomial to the given output stream.
     *
     * This function uses the toString() method to convert the polynomial
     * into a string
     *
     * @param  os   the output stream
     * @param  poly the polynomial to ouput
     *
     * @return the output stream
     */
    friend ostream&   operator<<(ostream& os, const Polynomial& poly);

    
#pragma mark Internal Helpers
protected:
    /**
     * Returns the product of polynomials a and b.
     *
     * This method multiplies the two polynomials with a nested for-loop. It
     * is O(nm) where n is the degree of a and m the degree of b.  It is, 
     * however, faster on small polynomials.
     *
     * @param  s    The first polynomial to muliply
     * @param  b    The second polynomial to muliply
     *
     * @return the product of polynomials a and b
     */
    static Polynomial iterative_multiply(const Polynomial& a, const Polynomial& b);
    
    /**
     * Returns the product of polynomials a and b.
     *
     * This method multiplies the two polynomials with recursively using a 
     * divide-and-conquer algorithm. The algorithm is described here:
     *
     *  http://algorithm.cs.nthu.edu.tw/~course/Extra_Info/Divide%20and%20Conquer_supplement.pdf
     *
     * This algorithm is θ(n) where n is the maximum degree of a and b.  It is, however,
     * slower on small polynomials.
     *
     * @param  s    The first polynomial to muliply
     * @param  b    The second polynomial to muliply
     *
     * @return the product of polynomials a and b
     */
    static Polynomial recursive_multiply(const Polynomial& a, const Polynomial& b);
    
    /**
     * Returns the synthetic division of this polynomial by other
     *
     * This method is adopted from the python code provided at 
     *
     *   https://en.wikipedia.org/wiki/Synthetic_division
     *
     * Synthetic division preserves the length of the vector.  The beginning is the
     * result, and the tail is the remainder.  This value must be broken up to implement
     * the / and % operators.  However, some algorithms (like Bairstow's method) 
     * prefer this method just the way it is.
     *
     * @param  other    the polynomial to divide by
     *
     * @return the synthetic division of this polynomial by other
     */
    Polynomial& synthetic_divide(const Polynomial& other);
    
    /**
     * Uses Bairstow's method to find a quadratic polynomial dividing this one.
     *
     * Bairstow's method iteratively divides this polynomial by quadratic factors, 
     * until it finds one that divides it within epsilon.  This method can fail
     * (takes to many iterations; the Jacobian is singular), hence the return value.
     * For more information, see 
     *
     *    http://nptel.ac.in/courses/122104019/numerical-analysis/Rathish-kumar/ratish-1/f3node9.html
     *
     * When calling this method, quad must be provided as an initial guess, while
     * result can be empty.  This method will modify both quad and result. quad is 
     * the final quadratic divider. result is the result of the division.
     *
     * @param  quad     the final quadratic divisor chosen
     * @param  result   the result of the final division
     * @param  epsilon  the error tolerance for quad
     *
     * @return true if Bairstow's method completes successfully
     */
    bool bairstow_factor(Polynomial& quad, Polynomial& result, float epsilon) const;
    
    /**
     * Solve for the roots of this polynomial with the quadratic formula.
     *
     * Obviously, this method will fail if the polynomial is not quadratic. The roots are 
     * added to the provided vector (the original contents are not erased). If any root is
     * complex, this method will have added NaN in its place.
     *
     * @param  roots    the vector to store the root values
     */
    void solve_quadratic(vector<float>& roots) const;
};

NS_CC_END

#endif /* defined(__CU_POLYNOMIAL_H__) */
