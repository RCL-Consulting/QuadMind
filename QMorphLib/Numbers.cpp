#include "pch.h"
#include "Numbers.h"

bool
rcl::equal( double a, double b )
{
	return std::abs( a - b ) < kZero;
}

int
rcl::compareTo( double a, double b )
{
    if ( std::fabs( a - b ) < kZero ) return 0;
    return (a < b) ? -1 : 1;
}