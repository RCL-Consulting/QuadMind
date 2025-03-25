#include "pch.h"
#include "MyVector.h"

#include "Ray.h"
#include "Msg.h"

#include <numbers>
#include <limits>

MyVector::MyVector( const MyVector& v )
{
	origin = std::make_unique<Node>( *v.origin );
	edge = v.edge;
	x = v.x;
	y = v.y;
}

MyVector::MyVector()
{
	origin = std::make_unique<Node>( Node::origin );
}

MyVector::MyVector( const Node& origin, double x, double y ):
	x( x ),
	y( y )
{
	this->origin = std::make_unique<Node>( origin );
}

MyVector::MyVector( const Ray& r, double length )
{
	this->origin = std::make_unique<Node>( *r.origin );
	x = length * r.x;
	y = length * r.y;
}

MyVector::MyVector( double angle, double length, const Node& origin )
{
	this->origin = std::make_unique<Node>( origin );
	this->x = length * cos( angle );
	this->y = length * sin( angle );
}

MyVector::MyVector( const Node& a, const Node& b )
{
	this->origin = std::make_unique<Node>( a );
	this->x = b.x - a.x;
	this->y = b.y - a.y;
}

MyVector
MyVector::operator=( const MyVector& v )
{
	origin = std::make_unique<Node>( *v.origin );
	x = v.x;
	y = v.y;
	edge = v.edge;
	return *this;
}

bool 
MyVector::equals( const MyVector& v )
{
	const double kZero = 1e-9;
	if ( abs( origin->x - origin->x ) > kZero || abs( origin->y - origin->y ) > kZero )
	{
		return false;
	}

	if ( abs( x / y - v.x / v.y ) > kZero )
	{
		return false;
	}

	return true;
}

double
MyVector::length()
{
	return sqrt( x * x + y * y );
}

double
MyVector::angle()
{
	// Math.acos returns values in the range 0.0 through pi, avoiding neg numbers.
	// If this is CW to x-axis, then return pos acos, else return neg acos
	if ( x == 0 && y > 0 )
	{
		return std::numbers::pi / 2.0;
	}
	else if ( x == 0 && y < 0 )
	{
		return -std::numbers::pi / 2.0;
	}
	else if ( y == 0 && x > 0 )
	{
		return 0;
	}
	else if ( y == 0 && x < 0 )
	{
		return std::numbers::pi;
	}
	else
	{
		double aLen = sqrt( x * x + y * y );
		if ( y > 0 )
		{
			return acos( (aLen * aLen + x * x - y * y) / (2 * aLen * x) );
		}
		else
		{
			return -acos( (aLen * aLen + x * x - y * y) / (2 * aLen * x) );
		}
	}
}

double 
MyVector::posAngle()
{
	// Math.acos returns values in the range 0.0 through pi, avoiding neg numbers.
	// If this is CW to x-axis, then return a number in the range pi through 2*pi
	if ( x == 0 && y > 0 )
	{
		return PIdiv2;
	}
	else if ( x == 0 && y < 0 )
	{
		return PIx3div2;
	}
	else if ( y == 0 && x > 0 )
	{
		return 0;
	}
	else if ( y == 0 && x < 0 )
	{
		return std::numbers::pi;
	}
	else
	{
		// double cLen= Math.sqrt(x*x + y*y);
		double aLen = sqrt( x * x + y * y );
		if ( y > 0 )
		{
			return acos( (aLen * aLen + x * x - y * y) / (2 * aLen * x) );
		}
		else
		{
			return PIx2 - acos( (aLen * aLen + x * x - y * y) / (2 * aLen * x) );
		}
	}
}

void 
MyVector::setLengthAndAngle( double length, double angle )
{
	this->x = length * cos( angle );
	this->y = length * sin( angle );
}

MyVector 
MyVector::plus( const MyVector& v )
{
	return MyVector( *origin, x + v.x, y + v.y );
}

MyVector 
MyVector::minus( const MyVector& v )
{
	return MyVector( *origin, x - v.x, y - v.y );
}

MyVector 
MyVector::mul( double d )
{
	return MyVector( *origin, d * x, d * y );
}

MyVector
MyVector::div( double d )
{
	return MyVector( *origin, x / d, y / d ); 
}

double 
MyVector::computePosAngle( const MyVector& v )
{
	double aLen = sqrt( x * x + y * y );
	double bLen = sqrt( v.x * v.x + v.y * v.y );

	Node cOrigin( origin->x + x, origin->y + y );
	Node cEnd( v.origin->x + v.x, v.origin->y + v.y );

	double cxLen = cOrigin.x - cEnd.x;
	double cyLen = cOrigin.y - cEnd.y;
	double cLen = sqrt( cxLen * cxLen + cyLen * cyLen );

	return acos( (aLen * aLen + bLen * bLen - cLen * cLen) / (2 * aLen * bLen) );
}

double
MyVector::computeAngle( const MyVector& v )
{
	double aLen = sqrt( x * x + y * y );
	double bLen = sqrt( v.x * v.x + v.y * v.y );

	Node cOrigin( origin->x + x, origin->y + y );
	Node cEnd( v.origin->x + v.x, v.origin->y + v.y );

	double cxLen = cOrigin.x - cEnd.x;
	double cyLen = cOrigin.y - cEnd.y;
	double cLen = sqrt( cxLen * cxLen + cyLen * cyLen );

	// Math.acos returns values in the range 0.0 through pi, avoiding neg numbers.
	// If this is CW to v, then return pos acos, else return neg acos
	if ( this->isCWto( v ) )
	{
		return acos( (aLen * aLen + bLen * bLen - cLen * cLen) / (2 * aLen * bLen) );
	}
	else
	{
		return -acos( (aLen * aLen + bLen * bLen - cLen * cLen) / (2 * aLen * bLen) );
	}
}

double
MyVector::dot( const MyVector& v )
{
	// Must use computePosAngle because the dot product is commutative
	double theta = computePosAngle( v ); 
	double aLen = sqrt( x * x + y * y );
	double bLen = sqrt( v.x * v.x + v.y * v.y );
	return aLen * bLen * cos( theta );
}

double 
MyVector::dot( double vx, double vy )
{
	// Must use computePosAngle because the dot product is commutative
	double sqraLen = x * x + y * y;
	double aLen = sqrt( sqraLen );
	double sqrbLen = vx * vx + vy * vy;
	double bLen = sqrt( sqrbLen );

	double cxLen = x - vx;
	double cyLen = y - vy;
	double sqrcLen = cxLen * cxLen + cyLen * cyLen;

	double temp = (sqraLen + sqrbLen - sqrcLen) / (2 * aLen * bLen);
	return aLen * bLen * temp;
}

bool
MyVector::newcross( const MyVector& v )
{
	const double kZero = 1e-12;
	double d0x = x, d0y = y, d1x = v.x, d1y = v.y;
	double d0Xd1 = d0x * d1y - d1x * d0y;
	if ( abs( d0Xd1 ) < kZero )
	{
		return true;
	}
	else
	{
		return false;
	}

	/*BigDecimal d0x = new BigDecimal( x ), d0y = new BigDecimal( y ), d1x = new BigDecimal( v.x ), d1y = new BigDecimal( v.y );
	BigDecimal d0Xd1 = d0x.multiply( d1y ).subtract( d1x.multiply( d0y ) );
	if ( d0Xd1.compareTo( zero ) == 1 )
	{
		return true;
	}
	else
	{
		return false;
	}*/
}

double
MyVector::cross( const MyVector& v )
{
	return x * v.y - v.x * y;
}

double 
MyVector::cross( const Ray& r )
{
	return x * r.y - r.x * y;
}

bool
MyVector::isCWto( const MyVector& v )
{
	if ( equals( v ) )
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

Node* 
MyVector::pointIntersectsAt( const MyVector& d1 )
{
	/*Node p0 = *origin, p1 = *d1.origin;
	MyVector delta ( p0, p1.x - p0.x, p1.y - p0.y );
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
			return new Node( x, y );
		}
	}*/

	return nullptr;
}

bool 
MyVector::intersects( const MyVector& d1 )
{
	/*Node p0 = *origin, p1 = *d1.origin;
	MyVector delta( p0, p1.x - p0.x, p1.y - p0.y );
	MyVector d0 = *this;
	double d0crossd1 = d0.cross( d1 );

	const double kZero = 1e-12;

	if ( d0crossd1 == 0 )
	{ // Non-intersecting and parallel OR intersect in an interval
		if ( delta.cross( d0 ) == 0 )
		{ // Parallel and on the same line?

			// Non-intersecting if both start and end of d0 is greater than or
			// less than both start and end of d1.

			// If x is okay to use...
			if ( abs( d0.origin->x - d1.origin->x ) > kZero || abs( d0.origin->x - d1.origin->x + d1.x ) > kZero )
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
			Msg::debug( descr() + " intersects " + d1.descr() + " (2)" );
			return true; // Intersects at an Edge point
		}
	}*/
	return false;
}

bool
MyVector::pointIntersects( const MyVector& d1 )
{
	/*Node p0 = *origin, p1 = *d1.origin;
	MyVector delta( p0, p1.x - p0.x, p1.y - p0.y );
	MyVector d0 = *this;
	double d0crossd1 = d0.cross( d1 );

	if ( d0crossd1 == 0 )
	{ // Non-intersecting and parallel OR intersect in an interval
		Msg::debug( descr() + " doesn't point intersect " + d1.descr() + " (1)" );
		return false;
	}
	else
	{
		double s = delta.cross( d1 ) / d0crossd1;
		double t = delta.cross( d0 ) / d0crossd1;

		if ( t < 0 || t > 1 || s < 0 || s > 1 )
		{// Intersects not at an Edge point
			Msg::debug( descr() + " doesn't point intersect " + d1.descr() + " (2)" );
			return false; // (but on the lines extending the Edges)
		}
		else
		{ // Intersects not at an Edge point
			return true; // Intersects at an Edge point
		}
	}*/
	return false;
}

bool
MyVector::innerpointIntersects( const MyVector& d1 )
{
	/*Node p0 = *origin, p1 = *d1.origin;
	MyVector delta( p0, p1.x - p0.x, p1.y - p0.y );
	MyVector d0 = *this;

	const double kZero = 1e-12;

	//TODO: floating point comparisons are not reliable

	if ( d0.origin->equals( *d1.origin ) || (abs( d0.origin->x - d1.origin->x + d1.x ) < kZero && abs( d0.origin.y - d1.origin.y + d1.y ) < kZero)
		 || (d0.origin->x + d0.x == d1.origin->x && d0.origin->y + d0.y == d1.origin->y)
		 || (d0.origin->x + d0.x == d1.origin->x + d1.x && d0.origin->y + d0.y == d1.origin->y + d1.y) )
	{
		Msg::debug( descr() + " doesn't intersect innerpoint of " + d1.descr() + " (0)" );
		return false;
	}

	double d0crossd1 = d0.cross( d1 );

	if ( d0crossd1 == 0 )
	{ // Non-intersecting and parallel OR intersect in an interval
		Msg::debug( descr() + " doesn't intersect in inner point of " + d1.descr() + " (1)" );
		return false;
	}
	else
	{
		double s = delta.cross( d1 ) / d0crossd1;
		double t = delta.cross( d0 ) / d0crossd1;

		Msg::debug( "innerpointIntersects(..): s: " + s );
		Msg::debug( "innerpointIntersects(..): t: " + t );

		if ( t <= 0 || t >= 1 || s <= 0 || s >= 1 )
		{
			Msg::debug( descr() + " doesn't innerintersect " + d1.descr() + " (2)" );
			return false; // Intersects not at an Edge point (but possibly an interval)
		}
		else
		{
			return true; // Intersects at an Edge point
		}
	}*/
	return false;
}

std::string
MyVector::descr() const
{
	//return origin->descr() + ", (" + std::to_string(x + origin->x) + ", " + std::to_string(y + origin->y) + ")";
	return "";
}

void
MyVector::printMe() const
{
	std::cout << descr();
}