#include "pch.h"
#include "MyVector.h"

#include "Node.h"
#include "Numbers.h"
#include "Msg.h"

#include <numbers>
#include <iostream>

//TODO: Tests
MyVector::MyVector( const std::shared_ptr<Node>& origin,
					double x,
					double y )
{
	this->origin = origin;
	this->x = x;
	this->y = y;
}

bool 
MyVector::operator==( const MyVector& v ) const
{
	if ( !rcl::equal( origin->x, v.origin->x ) || !rcl::equal( origin->y, v.origin->y ) )
	{
		return false;
	}

	if ( !rcl::equal( x / y, v.x / v.y ) )
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool 
MyVector::equals( const std::shared_ptr<Constants>& elem ) const
{
	auto vector = std::dynamic_pointer_cast<MyVector>(elem);
	if ( vector )
	{
		return *this == *vector;
	}
	return false;
}

//TODO: Implement this method
//TODO: Tests
MyVector::MyVector( const Ray& r,
					double length )
{
	/*origin = r.origin;
	x = length * r.x;
	y = length * r.y;*/
}

//TODO: Tests
MyVector::MyVector( double angle,
					double length,
					const std::shared_ptr<Node>& origin )
{
	this->origin = origin;
	this->x = length * std::cos( angle );
	this->y = length * std::sin( angle );
}

//TODO: Tests
MyVector::MyVector( const std::shared_ptr<Node>& a,
					const std::shared_ptr<Node>& b )
{
	this->origin = a;
	this->x = b->x - a->x;
	this->y = b->y - a->y;
}

double 
MyVector::length() const
{
	return std::sqrt( x * x + y * y );
}

double
MyVector::angle()
{
	// Math.acos returns values in the range 0.0 through pi, avoiding neg numbers.
	// If this is CW to x-axis, then return pos acos, else return neg acos
	if ( rcl::equal( x, 0.0 ) && y > 0 )
	{
		return std::numbers::pi / 2.0;
	}
	else if ( rcl::equal( x, 0.0 ) && y < 0 )
	{
		return -std::numbers::pi / 2.0;
	}
	else if ( rcl::equal( y, 0 ) && x > 0 )
	{
		return 0;
	}
	else if ( rcl::equal( y, 0.0 ) && x < 0 )
	{
		return std::numbers::pi;
	}
	else
	{
		double aLen = std::sqrt( x * x + y * y );
		if ( y > 0 )
		{
			return std::acos( (aLen * aLen + x * x - y * y) / (2 * aLen * x) );
		}
		else
		{
			return -std::acos( (aLen * aLen + x * x - y * y) / (2 * aLen * x) );
		}
	}
}

double
MyVector::posAngle()
{
	// Math.acos returns values in the range 0.0 through pi, avoiding neg numbers.
	// If this is CW to x-axis, then return a number in the range pi through 2*pi
	if ( rcl::equal( x, 0.0 ) && y > 0 )
	{
		return PIdiv2;
	}
	else if ( rcl::equal( x, 0.0 ) && y < 0 )
	{
		return PIx3div2;
	}
	else if ( rcl::equal( y, 0.0 ) && x > 0 )
	{
		return 0;
	}
	else if ( rcl::equal( y, 0.0 ) && x < 0 )
	{
		return std::numbers::pi;
	}
	else if ( rcl::equal( x, 0 ) && rcl::equal( y, 0 ) )
	{
		return 0;
	}
	else
	{
		double aLen = std::sqrt( x * x + y * y );
		if ( y > 0 )
		{
			return std::acos( (aLen * aLen + x * x - y * y) / (2 * aLen * x) );
		}
		else
		{
			return PIx2 - std::acos( (aLen * aLen + x * x - y * y) / (2 * aLen * x) );
		}
	}
}

void
MyVector::setLengthAndAngle( double length,
							 double angle )
{
	this->x = length * std::cos( angle );
	this->y = length * std::sin( angle );
}

MyVector
MyVector::plus( const MyVector& v )
{
	return MyVector( origin, x + v.x, y + v.y );
}

MyVector
MyVector::minus( const MyVector& v )
{
	return MyVector( origin, x - v.x, y - v.y );
}

MyVector 
MyVector::mul( double d )
{
	return MyVector( origin, d * x, d * y );
}

MyVector
MyVector::div( double d )
{
	if ( rcl::equal( d, 0.0 ) )
	{
		throw std::invalid_argument( "Division by zero" );
	}
	return MyVector( origin, x / d, y / d );
}

double
MyVector::computePosAngle( const MyVector& v )
{
	double aLen = std::sqrt( x * x + y * y );
	double bLen = std::sqrt( v.x * v.x + v.y * v.y );

	Node cOrigin( origin->x + x, origin->y + y );
	Node cEnd( v.origin->x + v.x, v.origin->y + v.y );

	double cxLen = cOrigin.x - cEnd.x;
	double cyLen = cOrigin.y - cEnd.y;
	double cLen = std::sqrt( cxLen * cxLen + cyLen * cyLen );

	if ( rcl::equal( aLen, 0.0 ) || rcl::equal(bLen,0.0))
		return 0;
	else if ( this->isCWto( v ) )
		return std::acos( (aLen * aLen + bLen * bLen - cLen * cLen) / (2 * aLen * bLen) );
	else
		return Constants::PIx2 - std::acos( (aLen * aLen + bLen * bLen - cLen * cLen) / (2 * aLen * bLen) );
}

double 
MyVector::computeAngle( const MyVector& v )
{
	double aLen = std::sqrt(x * x + y * y);
	double bLen = std::sqrt( v.x * v.x + v.y * v.y );

	Node cOrigin( origin->x + x, origin->y + y );
	Node cEnd( v.origin->x + v.x, v.origin->y + v.y );

	double cxLen = cOrigin.x - cEnd.x;
	double cyLen = cOrigin.y - cEnd.y;
	double cLen = std::sqrt( cxLen * cxLen + cyLen * cyLen );

	// std::acos returns values in the range 0.0 through pi, avoiding neg numbers.
	// If this is CW to v, then return pos acos, else return neg acos
	if ( this->isCWto( v ) )
	{
		return std::acos( (aLen * aLen + bLen * bLen - cLen * cLen) / (2 * aLen * bLen) );
	}
	else
	{
		return -std::acos( (aLen * aLen + bLen * bLen - cLen * cLen) / (2 * aLen * bLen) );
	}
}

//TODO: I think this should be name CWFrom not to
bool 
MyVector::isCWto( const MyVector& v )
{
	if ( *this == v )
	{
		return false; // A vector cannot be CW to itself
	}
	double thisR, vR;

	if ( x != 0 )
	{
		thisR = y / x;
	}
	else
	{
		thisR = -std::numeric_limits<double>::infinity();
	}

	if ( v.x != 0 )
	{
		vR = v.y / v.x;
	}
	else
	{
		vR = -std::numeric_limits<double>::infinity();
	}

	if ( x > 0 && y >= 0 )
	{ // ----- First quadrant -----
		if ( v.x > 0 && v.y >= 0 )
		{ // First quadrant
			if ( thisR <= vR )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if ( v.x <= 0 && v.y > 0 )
		{ // Second quadrant
			return true;
		}
		else if ( v.x < 0 && v.y <= 0 )
		{ // Third quadrant
			if ( thisR >= vR )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{ // Third quadrant
			return false;
		}
	}
	else if ( x <= 0 && y > 0 )
	{ // ----- Second quadrant -----
		if ( v.x > 0 && v.y >= 0 )
		{ // First quadrant
			return false;
		}
		else if ( v.x <= 0 && v.y > 0 )
		{ // Second quadrant
			if ( thisR <= vR )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if ( v.x < 0 && v.y <= 0 )
		{ // Third quadrant
			return true;
		}
		else
		{// if (v.x>=0 && v.y< 0) { // Fourth quadrant
			if ( thisR >= vR )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else if ( x < 0 && y <= 0 )
	{ // ----- Third quadrant -----
		if ( v.x > 0 && v.y >= 0 )
		{ // First quadrant
			if ( thisR >= vR )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if ( v.x <= 0 && v.y > 0 )
		{ // Second quadrant
			return false;
		}
		else if ( v.x < 0 && v.y <= 0 )
		{ // Third quadrant
			if ( thisR <= vR )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{ // Third quadrant
			return true;
		}

	}
	else
	{ // if (x>=0 && y< 0) { // ----- Fourth quadrant -----
		if ( v.x > 0 && v.y >= 0 )
		{ // First quadrant
			return true;
		}
		else if ( v.x <= 0 && v.y > 0 )
		{ // Second quadrant
			if ( thisR >= vR )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if ( v.x < 0 && v.y <= 0 )
		{ // Third quadrant
			return false;
		}
		else
		{// if (v.x>=0 && v.y< 0) { // Fourth quadrant
			if ( thisR <= vR )
			{ // <=
				return true;
			}
			else
			{ // <=
				return false;
			}
		}
	}
}

double
MyVector::cross( const MyVector& v )
{
	return x * v.y - v.x * y;
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Node>
MyVector::pointIntersectsAt( const MyVector& d1 )
{
	auto p0 = origin, p1 = d1.origin;
	MyVector delta( p0, p1->x - p0->x, p1->y - p0->y );
	MyVector d0 = *this;
	double d0crossd1 = d0.cross( d1 );

	if ( d0crossd1 == 0 )
	{
		return nullptr; // Does not intersect at all OR the same MyVector
	}
	else
	{
		double s = delta.cross( d1 ) / d0crossd1;
		double t = delta.cross( d0 ) / d0crossd1;
		if ( t < 0 || t > 1 || s < 0 || s > 1 )
		{
			return nullptr; // Intersects not at an edge point (but extension)
		}
		else
		{
			double x = d1.origin->x + t * d1.x;
			double y = d1.origin->y + t * d1.y;
			return std::make_shared<Node>( x, y );// Intersects at this edge point
		}
	}
}

double 
MyVector::dot( const MyVector& v )
{
	return dot( v.x, v.y );
}

double 
MyVector::dot( double vx, double vy )
{
	return x * vx + y * vy;
}

//TODO: Implement this method
//TODO: Tests
double 
MyVector::cross( const Ray& r )
{
	return 0.0;
	//return x * r.y - r.x * y;
}

std::string 
MyVector::descr() const
{
	return origin->descr() + ", (" + std::to_string( x + origin->x ) + ", " + std::to_string( y + origin->y ) + ")";
}

//TODO: This agorithm seems suspect
bool
MyVector::intersects( const MyVector& d1 )
{
	auto p0 = origin, p1 = d1.origin;
	MyVector delta( p0, p1->x - p0->x, p1->y - p0->y );
	MyVector d0 = *this;
	double d0crossd1 = d0.cross( d1 );

	if ( rcl::equal( d0crossd1, 0 ) )
	{ // Non-intersecting and parallel OR intersect in an interval
		if ( rcl::equal( delta.cross( d0 ), 0 ) )
		{ // Parallel and on the same line?

// Non-intersecting if both start and end of d0 is greater than or
// less than both start and end of d1.

// If x is okay to use...
			if ( !rcl::equal( d0.origin->x, d1.origin->x ) || !rcl::equal( d0.origin->x, d1.origin->x + d1.x ) )
			{
				double d0sx = d0.origin->x, d0ex = d0.origin->x + d0.x;
				double d1sx = d1.origin->x, d1ex = d1.origin->x + d1.x;
				if ( (d0sx < d1sx && d0sx < d1ex && d0ex < d1sx && d0ex < d1ex) || (d0sx > d1sx && d0sx > d1ex && d0ex > d1sx && d0ex > d1ex) )
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{ // ...no, use y instead
				double d0sy = d0.origin->y, d0ey = d0.origin->y + d0.y;
				double d1sy = d1.origin->y, d1ey = d1.origin->y + d1.y;
				if ( (d0sy < d1sy && d0sy < d1ey && d0ey < d1sy && d0ey < d1ey) || (d0sy > d1sy && d0sy > d1ey && d0ey > d1sy && d0ey > d1ey) )
				{
					return false;
				}
				else
				{
					return true;
				}
			}

		}
		else
		{
			Msg::debug( descr() + " doesn't intersect " + d1.descr() + " (1)" );
			return false;
		}
	}
	else
	{
		double s = delta.cross( d1 ) / d0crossd1;
		double t = delta.cross( d0 ) / d0crossd1;
		Msg::debug( "s: " + std::to_string( s ) );
		Msg::debug( "t: " + std::to_string( t ) );

		if ( t < 0 || t > 1 || s < 0 || s > 1 )
		{// Intersects not at an Edge point
			Msg::debug( descr() + " doesn't intersect " + d1.descr() + " (2)" );
			return false; // (but on the lines extending the Edges)
		}
		else
		{
			Msg::debug( this->descr() + " intersects " + d1.descr() + " (2)" );
			return true; // Intersects at an Edge point
		}
	}
}

//TODO: This agorithm seems suspect
bool 
MyVector::pointIntersects( const MyVector& d1 )
{
	auto p0 = origin, p1 = d1.origin;
	MyVector delta( p0, p1->x - p0->x, p1->y - p0->y );
	MyVector d0 = *this;
	double d0crossd1 = d0.cross( d1 );

	if ( rcl::equal( d0crossd1, 0.0 ) )
	{ // Non-intersecting and parallel OR intersect in an interval
		Msg::debug( this->descr() + " doesn't point intersect " + d1.descr() + " (1)" );
		return false;
	}
	else
	{
		double s = delta.cross( d1 ) / d0crossd1;
		double t = delta.cross( d0 ) / d0crossd1;

		if ( t < 0 || t > 1 || s < 0 || s > 1 )
		{// Intersects not at an Edge point
			Msg::debug( this->descr() + " doesn't point intersect " + d1.descr() + " (2)" );
			return false; // (but on the lines extending the Edges)
		}
		else
		{ // Intersects not at an Edge point
			return true; // Intersects at an Edge point
		}
	}
}

bool 
MyVector::innerpointIntersects( const MyVector& d1 )
{
	auto p0 = origin, p1 = d1.origin;
	MyVector delta( p0, p1->x - p0->x, p1->y - p0->y );
	MyVector d0 = *this;

	if ( d0.origin->equals( d1.origin ) || 
		 (rcl::equal( d0.origin->x, d1.origin->x + d1.x ) && rcl::equal( d0.origin->y, d1.origin->y + d1.y )) ||
		 (rcl::equal( d0.origin->x + d0.x, d1.origin->x ) && rcl::equal( d0.origin->y + d0.y, d1.origin->y ))||
		 (rcl::equal( d0.origin->x + d0.x, d1.origin->x + d1.x ) && rcl::equal( d0.origin->y + d0.y, d1.origin->y + d1.y )) )
	{
		Msg::debug( descr() + " doesn't intersect innerpoint of " + d1.descr() + " (0)" );
		return false;
	}

	double d0crossd1 = d0.cross( d1 );

	if ( d0crossd1 == 0 )
	{ // Non-intersecting and parallel OR intersect in an interval
		Msg::debug( this->descr() + " doesn't intersect in inner point of " + d1.descr() + " (1)" );
		return false;
	}
	else
	{
		double s = delta.cross( d1 ) / d0crossd1;
		double t = delta.cross( d0 ) / d0crossd1;

		Msg::debug( "innerpointIntersects(..): s: " + std::to_string( s ) );
		Msg::debug( "innerpointIntersects(..): t: " + std::to_string( t ) );

		if ( t <= 0 || t >= 1 || s <= 0 || s >= 1 )
		{
			Msg::debug( this->descr() + " doesn't innerintersect " + d1.descr() + " (2)" );
			return false; // Intersects not at an Edge point (but possibly an interval)
		}
		else
		{
			return true; // Intersects at an Edge point
		}
	}
}

void 
MyVector::printMe()
{
	std::cout << descr() << "\n";
}