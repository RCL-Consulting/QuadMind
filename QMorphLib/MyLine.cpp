#include "pch.h"
#include "MyLine.h"

#include "Node.h"
#include "Msg.h"

MyLine::MyLine( Node* n1, Node* n2 )
{
	ref = n1;
	x = n2->x - n1->x;
	y = n2->y - n1->y;
}

MyLine::MyLine( Node* n1, double x, double y )
{
	ref = n1;
	this->x = x;
	this->y = y;
}

double
MyLine::cross( const MyLine& l )
{
	return x * l.y - l.x * y;
}

Node* 
MyLine::pointIntersectsAt( const MyLine& d1 )
{
	Msg::debug( "Entering MyLine.pointIntersectsAt(..)" );
	Node* p0 = ref, *p1 = d1.ref;
	MyLine delta( p0, p1->x - p0->x, p1->y - p0->y );
	MyLine d0 = *this;
	Msg::debug( "... d0:" + d0.descr() );
	Msg::debug( "... d1:" + d1.descr() );
	double d0crossd1 = d0.cross( d1 );

	if ( d0crossd1 == 0 )
	{ // Parallel and, alas, no pointintersection
		Msg::debug( "Leaving MyLine.pointIntersectsAt(..), returns null" );
		return nullptr;
	}
	else
	{
		double t = delta.cross( d0 ) / d0crossd1;

		double x = d1.ref->x + t * d1.x;
		double y = d1.ref->y + t * d1.y;
		Msg::debug( "Leaving MyLine.pointIntersectsAt(..), returns x: " + std::to_string( x ) + ", y:" + std::to_string( y ) );
		return new Node( x, y ); // Intersects at this line point
	}
}

std::string
MyLine::descr() const
{
	return ref->descr() + ", (" + std::to_string(x + ref->x) + ", " + std::to_string(y + ref->y) + ")";
}