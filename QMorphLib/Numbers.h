#pragma once
#include <cmath>

namespace rcl
{
	inline static constexpr double kZero = 1e-12;

	bool equal( double a, double b );
	int compareTo( double a, double b );
}