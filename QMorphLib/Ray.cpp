#include "pch.h"
#include "Ray.h"

#include "Node.h"
#include "Edge.h"
#include "Msg.h"
#include "Constants.h"
#include "MyVector.h"

Ray::Ray( const Node& origin, const Edge& relEdge, double angle )
{
	this->origin = std::make_unique<Node>( origin );
	double temp = relEdge.angleAt( &origin );
	Msg::debug( "relEdge.angleAt(origin)==" + std::to_string( Constants::toDegrees * temp ) + " degrees" );
	double ang = temp + angle;
	Msg::debug( "Ray(..): relEdge.angleAt(origin)+ angle== " + std::to_string( Constants::toDegrees * ang ) + " degrees" );

	this->x = cos( ang );
	this->y = sin( ang );
}

Ray::Ray( const Node& origin, double angle )
{
	this->origin = std::make_unique<Node>( origin );
	this->x = cos( angle );
	this->y = sin( angle );
}

Ray::Ray( const Node& origin, const Node& passThrough )
{
	this->origin = std::make_unique<Node>( origin );
	double tempx = passThrough.x - origin.x;
	double tempy = passThrough.y - origin.y;
	double hyp = sqrt( tempx * tempx + tempy * tempy );

	this->x = tempx / hyp;
	this->y = tempy / hyp;
}

Ray::Ray( const Ray& r )
{
	this->origin = std::make_unique<Node>( *r.origin );
	this->x = r.x;
	this->y = r.y;
}

Ray&
Ray::operator=( const Ray& r )
{
	this->origin = std::make_unique<Node>( *r.origin );
	this->x = r.x;
	this->y = r.y;
	return *this;
}

double 
Ray::cross( const MyVector& v )
{
	return x * v.y - v.x * y;
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
	std::cout << descr();
}

Node* 
Ray::pointIntersectsAt( const MyVector& d1 )
{
	auto p0 = *origin, p1 = *d1.origin;
	MyVector delta( p0, p1.x - p0.x, p1.y - p0.y );
	Ray d0 = *this;
	double d0crossd1 = d0.cross( d1 );

	if ( d0crossd1 == 0 )
	{
		if ( delta.cross( d0 ) == 0 )
		{ // Parallel
			if ( d0.origin->equals( d1.origin.get() ) || 
				 (d0.origin->x == d1.origin->x + d1.x && d0.origin->y == d1.origin->y + d1.y) )
			{
				return new Node( *d0.origin );
			}
		}
		return nullptr;
	}
	else
	{
		double s = delta.cross( d1 ) / d0crossd1;
		double t = delta.cross( d0 ) / d0crossd1;
		if ( t < 0 || t > 1 || s < 0 )
		{
			return nullptr; // Intersects not at a ray/vector point
		}
		else
		{
			double x = d1.origin->x + t * d1.x;
			double y = d1.origin->y + t * d1.y;
			return new Node( x, y ); // Intersects at this ray point
		}
	}
	return nullptr;
}