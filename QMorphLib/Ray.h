#pragma once

#include <memory>
#include <string>

class Node;
class Edge;
class MyVector;
/**
 * This class holds information for rays, and has methods for dealing with
 * ray-related issues. The purpose of this class is solely to determine the
 * intersection point between a ray (origin and direction) and a vector (origin
 * and x,y giving the direction, the length of the ray is considered to be
 * infinite).
 */

class Ray
{
public:
	// origin: the point where the ray starts
	// relEdge: an edge that the angle is relative to
	// angle: the angle relative to relEdge
	Ray( const Node& origin, const Edge& relEdge, double angle );

	// origin: the point where the ray starts
	// angle: the angle relative to relEdge
	// angle is relative to the x-axis
	Ray( const Node& origin, double angle );

	// origin: the point where the ray starts
	// passThrough: a point which the ray passes through
	Ray( const Node& origin, const Node& passThrough );

	Ray() = default;

	Ray( const Ray& );

	Ray& operator=( const Ray& r );

	double cross( const MyVector& v );

	Node* pointIntersectsAt( const MyVector& d1 );

	std::string values();

	std::string descr();

	void printMe();

	double x, y; // hyp= 1= sqrt(x^2 + y^2)
	std::unique_ptr<Node> origin = nullptr;
};
