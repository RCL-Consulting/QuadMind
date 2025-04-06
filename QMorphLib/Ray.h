#pragma once

#include <memory>
#include <string>

/**
 * This class holds information for rays, and has methods for dealing with
 * ray-related issues. The purpose of this class is solely to determine the
 * intersection point between a ray (origin and direction) and a vector (origin
 * and x,y giving the direction, the length of the ray is considered to be
 * infinite).
 */

class Node;
class Edge;
class MyVector;

class Ray
{
public:
	// origin: the point where the ray starts
	// relEdge: an edge that the angle is relative to
	// angle: the angle relative to relEdge
	Ray( const std::shared_ptr<Node>& origin,
		 const std::shared_ptr<Edge>& relEdge,
		 double angle );

	// origin: the point where the ray starts
	// angle: the angle relative to relEdge
	// angle is relative to the x-axis
	Ray( const std::shared_ptr<Node>& origin, double angle );

	// origin: the point where the ray starts
	// passThrough: a point which the ray passes through
	Ray( const std::shared_ptr<Node>& origin, 
		 const std::shared_ptr<Node>& passThrough );

	double cross( const MyVector& v );

	std::shared_ptr<Node> pointIntersectsAt( const MyVector& d1 );

	std::string values();

	std::string descr();

	void printMe();
	//{
	//	System.out.println( descr() );
	//}

	double x, y; // hyp= 1= sqrt(x^2 + y^2)
	std::shared_ptr<Node> origin;
};
