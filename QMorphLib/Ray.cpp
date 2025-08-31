#include "pch.h"
#include "Ray.h"

#include "Node.h"
#include "Edge.h"
#include "Msg.h"
#include "MyVector.h"

#include "Numbers.h"

#include <iostream>

//TODO: Tests
Ray::Ray( const std::shared_ptr<Node>& origin,
		  const std::shared_ptr<Edge>& relEdge,
		  double angle )
{
	this->origin = origin;
	double temp = relEdge->angleAt( origin );
	Msg::debug( "relEdge.angleAt(origin)==" + std::to_string( Constants::toDegrees * temp ) + " degrees" );
	double ang = temp + angle;
	Msg::debug( "Ray(..): relEdge.angleAt(origin)+ angle== " + std::to_string( Constants::toDegrees * ang ) + " degrees" );

	this->x = std::cos( ang );
	this->y = std::sin( ang );
}

Ray::Ray( const std::shared_ptr<Node>& origin, double angle )
{
	this->origin = origin;
	this->x = std::cos( angle );
	this->y = std::sin( angle );
}

Ray::Ray( const std::shared_ptr<Node>& origin, 
		  const std::shared_ptr<Node>& passThrough )
{
	this->origin = origin;
	double tempx = passThrough->x - origin->x;
	double tempy = passThrough->y - origin->y;
	double hyp = std::sqrt( tempx * tempx + tempy * tempy );

	this->x = tempx / hyp;
	this->y = tempy / hyp;
}

double 
Ray::cross( const MyVector& v )
{
	return x * v.y - v.x * y;
}

std::shared_ptr<Node>
Ray::pointIntersectsAt( const MyVector& d1 )
{
	auto p0 = origin;        // this ray's origin
	auto p1 = d1.origin;     // d1's origin
	MyVector delta( p0, p1->x - p0->x, p1->y - p0->y ); // vector from p0 to p1
	Ray d0 = *this;

	double d0crossd1 = d0.cross( d1 );

	if ( rcl::equal( d0crossd1, 0.0 ) )
	{
		if ( rcl::equal( delta.cross( d0 ), 0.0 ) )
		{ // Parallel and collinear
			// Check if they touch
			if ( d0.origin->equals( d1.origin ) ||
				 (rcl::equal( d0.origin->x, d1.origin->x + d1.x ) &&
				   rcl::equal( d0.origin->y, d1.origin->y + d1.y )) )
			{
				return d0.origin;
			}
		}
		return nullptr; // Parallel but not collinear
	}
	else
	{
		double s = delta.cross( d1 ) / d0crossd1;
		double t = delta.cross( d0 ) / d0crossd1;
		if ( t < 0 || t > 1 || s < 0 )
		{
			return nullptr; // Intersection not within ray1 or vector2 bounds
		}
		else
		{
			double x = d1.origin->x + t * d1.x;
			double y = d1.origin->y + t * d1.y;
			return std::make_shared<Node>( x, y );
		}
	}
}

std::string
Ray::values()
{
	return origin->descr() + ", x= " + std::to_string( x ) + ", y= " + std::to_string( y );
}

std::string 
Ray::descr()
{
	return origin->descr() + ", (" + std::to_string(x + origin->x) + ", " + std::to_string(y + origin->y) + ")";
}

void 
Ray::printMe()
{
	Msg::debug( descr() );
}