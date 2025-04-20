#pragma once

#include <memory>
#include <string>

class Node;
/**
 * This class holds information for lines, and has methods for dealing with
 * line-related issues. The purpose of this class is solely to determine the
 * intersection point between two lines. The length of a line is, of course,
 * infinite.
 */

class MyLine
{
public:
	// n1: A point that the line passes through
	// n2: A point that the line passes through
	MyLine( const std::shared_ptr<Node>& n1,
			const std::shared_ptr<Node>& n2 );

	MyLine( const std::shared_ptr<Node>& n1,
			double x,
			double y );

	double cross( const MyLine& l );

	std::shared_ptr<Node> pointIntersectsAt( const MyLine& d1 );

	std::string descr() const;

	std::shared_ptr<Node> ref;
	double x, y;
};
