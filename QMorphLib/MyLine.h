#pragma once

#include <memory>
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

	std::sheNode pointIntersectsAt( MyLine d1 )
	//{
	//	Msg.debug( "Entering MyLine.pointIntersectsAt(..)" );
	//	Node p0 = ref, p1 = d1.ref;
	//	MyLine delta = new MyLine( p0, p1.x - p0.x, p1.y - p0.y );
	//	MyLine d0 = this;
	//	Msg.debug( "... d0:" + d0.descr() );
	//	Msg.debug( "... d1:" + d1.descr() );
	//	double d0crossd1 = d0.cross( d1 );

	//	if ( d0crossd1 == 0 )
	//	{ // Parallel and, alas, no pointintersection
	//		Msg.debug( "Leaving MyLine.pointIntersectsAt(..), returns null" );
	//		return null;
	//	}
	//	else
	//	{
	//		double t = delta.cross( d0 ) / d0crossd1;

	//		double x = d1.ref.x + t * d1.x;
	//		double y = d1.ref.y + t * d1.y;
	//		Msg.debug( "Leaving MyLine.pointIntersectsAt(..), returns x: " + x + ", y:" + y );
	//		return new Node( x, y ); // Intersects at this line point
	//	}
	//}

	//public String descr()
	//{
	//	return ref.descr() + ", (" + (x + ref.x) + ", " + (y + ref.y) + ")";
	//}

	std::shared_ptr<Node> ref;
	double x, y;
};
