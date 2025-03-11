#pragma once

/**
 * This class holds information for lines, and has methods for dealing with
 * line-related issues. The purpose of this class is solely to determine the
 * intersection point between two lines. The length of a line is, of course,
 * infinite.
 */

#include <string>

class Node;

class MyLine
{
	// n1: A point that the line passes through
	// n2: A point that the line passes through
	MyLine( Node* n1, Node* n2 );

	MyLine( Node* n1, double x, double y );

	double cross( const MyLine& l );

	Node* pointIntersectsAt( const MyLine& d1 );

	std::string descr() const;

	Node* ref;
	double x, y;
};
