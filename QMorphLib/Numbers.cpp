#include "pch.h"
#include "Numbers.h"

bool
rcl::equal( double a, double b )
{
	return std::abs( a - b ) < kZero;
}