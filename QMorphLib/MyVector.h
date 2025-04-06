#pragma once
#include "Constants.h"

class Edge;
class Node;
class Ray;

/**
 * This class holds information for vectors, and has methods for dealing with
 * vector-related issues.
 */

class MyVector : public Constants
{
public:
	std::shared_ptr<Node> origin = nullptr;
	double x = 0.0, y = 0.0;
	std::shared_ptr<Edge> edge = nullptr;

	MyVector() = default;

	/**
	 * @param origin the origin of the vector
	 * @param x      the x component
	 * @param y      the y component
	 */
	MyVector( const std::shared_ptr<Node>& origin,
			  double x,
			  double y );

	/**
	 * "Convert" a ray into a vector.
	 *
	 * @param r      a ray (we use the origin and direction found in this ray)
	 * @param length the length of the vector
	 */
	MyVector( const Ray& r,
			  double length );

	/**
	 * @param angle  the angle of the vector
	 * @param length the length of the vector
	 * @param origin the origin of the vector
	 */
	MyVector( double angle,
			  double length,
			  const std::shared_ptr<Node>& origin );

	/**
	 * @param a the origin of the vector
	 * @param b the endpoint of the vector
	 */
	MyVector( const std::shared_ptr<Node>& a, 
			  const std::shared_ptr<Node>& b );

	//TODO: C++ equivalent of Java's BigDecimal
//	private final static BigDecimal zero = new BigDecimal( 0.0 );
//	private final static BigDecimal one = new BigDecimal( 1.0 );

	bool operator==( const MyVector& v ) const;
	bool equals( const std::shared_ptr<Constants>& elem ) const override;

	/** @return length of vector */
	double length() const;

	/**
	 * @return angle relative to the x-axis (which is directed from the origin (0,0)
	 *         to the right, btw) in the range (-180, 180) in radians.
	 */
	double angle();

	/**
	 * @return angle relative to the x-axis (which is directed from the origin (0,0)
	 *         to the right, btw) in the range (0, 360) in radians.
	 */
	double posAngle();

	void setLengthAndAngle( double length,
							double angle );

	/** The origin is really overlooked here... I just use this.origin ... */
	MyVector plus( const MyVector& v );

	MyVector minus( const MyVector& v );

	MyVector mul( double d );

	MyVector div( double d );

	/**
	 * @param v another vector
	 * @return the smalles positive angle between this and v in radians
	 */
	double computePosAngle( const MyVector& v );

	/**
	 * @param v another vector
	 * @return the true angle between this and v in radians or NaN if length of this
	 *         or v is 0.
	 */
	double computeAngle( const MyVector& v );

	/**
	 * Compute the dot product
	 *
	 * @param v another vector
	 * @return the dot product defined as |this|*|v|*cos(theta)
	 */
	
	double dot( const MyVector& v );

	/**
	 * Compute the dot product. Much faster and more accurate than the
	 * MyVector.dot(MyVector) method.
	 *
	 * @param vx the x component of the other vector
	 * @param vy the y component of the other vector
	 * @return the dot product defined as |this|*|v|*cos(theta)
	 */
	double dot( double vx, double vy );

	/** Compute the cross product */
	double cross( const MyVector& v );

	/** Compute the cross product */
	double cross( const Ray& r );

	/**
	 * Methods to assist stupid Math.acos(..). Tested ok (simple test) Vectors with
	 * equal slopes *ARE* considered to be cw to each other.
	 *
	 * @param v another vector
	 * @return a boolean indicating whether this vector is clockwise (cw) to v
	 */
	bool isCWto( const MyVector& v );

	// Note: java.awt.geom.Line2D has an intersects method.. but no intersectsAt
	// method
	// Stupid!!!! So I had to make this... blood, sweat and tears, my friend...
	// Method derived from Intersection of Linear and Circular Components in 2D
	// Local copy at ~karll/hfag/teori/IntersectionLin2Cir2.pdf

	/**
	 * @return if vectors intersects at one point exactly, return a node for this
	 *         point Else return null.
	 */
	std::shared_ptr<Node> pointIntersectsAt( const MyVector& d1 );

	/** @return true if this and d1 intersect, else false. */
	bool intersects( const MyVector& d1 );

	/** @return true if this and d1 intersect in a single point, else false. */
	bool pointIntersects( const MyVector& d1 );

	/**
	 * @return true if this and d1 intersect in a inner point of each other, (that
	 *         is, a point that is not an endpoint, ) else return false.
	 */
	bool innerpointIntersects( const MyVector& d1 );

	 /** @return a string representation of this vector. */
	std::string descr() const;

	/** Print a string representation of this vector. */
	void printMe();
	
};
