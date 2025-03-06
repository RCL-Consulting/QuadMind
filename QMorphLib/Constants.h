#pragma once

#include "Node.h"

/*
 * This class holds the program "constants". That is, they are given as
 * parameters to the Q-Morph implementation.
 */

class Constants
{
public:
	// A boolean indicating whether the triangle to quad conversion should run. 
	inline static const bool doTri2QuadConversion = true;
	// A boolean indicating whether the topological cleanup should run. 
	inline static const bool doCleanUp = true;
	// A boolean indicating whether the global smoothing should run. 
	inline static const bool doSmooth = true;

	// Some common constants
	inline static const double sqrt3x2 = 2.0 * sqrt( 3.0 );

	// Constants for quads
	inline static const int base = 0;
	inline static const int left = 1;
	inline static const int right = 2;
	inline static const int top = 3;

	// Some useful constants involving PI:
	// PI/6 or 30 degrees
	inline static const double PIdiv6 = java.lang.Math.PI / 6.0;
	// PI/2 or 90 degrees
	inline static const double PIdiv2 = java.lang.Math.PI / 2.0;
	// 3*PI/4 or 135 degrees
	inline static const double PIx3div4 = 3 * java.lang.Math.PI / 4.0;
	// 5*PI/4 or 225 degrees
	inline static const double PIx5div4 = 5 * java.lang.Math.PI / 4.0;
	// 3*PI/2 or 270 degrees
	inline static const double PIx3div2 = 3 * java.lang.Math.PI / 2.0;
	// 2*PI or 360 degrees
	inline static const double PIx2 = java.lang.Math.PI * 2.0;

	// Some useful constants holding the radian values of common angles in degrees
	// 6 degrees in radians
	inline static const double DEG_6 = Math.toRadians( 6 );
	// 150 degrees in radians
	inline static const double DEG_150 = Math.toRadians( 150 );
	// 160 degrees in radians
	inline static const double DEG_160 = Math.toRadians( 160 );
	// 179 degrees in radians
	inline static const double DEG_179 = Math.toRadians( 179 );
	// 180 degrees in radians
	inline static const double DEG_180 = Math.toRadians( 180 );
	// 200 degrees in radians
	inline static const double DEG_200 = Math.toRadians( 200 );

	// Constants for the seam, transition seam and transition split operations:
	// Note that we must have (EPSILON1 < EPSILON2)
	inline static const double EPSILON1 = java.lang.Math.PI * 0.04;
	inline static const double EPSILON2 = java.lang.Math.PI * 0.09;

	// The minimum size of the greatest angle in a chevron.
	inline static const double CHEVRONMIN = DEG_200;

	// Constants for side edge selection (EPSILON < EPSILONLARGER)
	inline static const double sqrt3div2 = Math.sqrt( 3.0 ) / 2.0;

	inline static const double EPSILON = java.lang.Math.PI / 6.0;
	inline static const double EPSILONLARGER = java.lang.Math.PI;

	// Constants for post smoothing
	// The node coincidence tolerance
	inline static const double COINCTOL = 0.01;
	// A value for the move tolerance. I don't know if it's any good.
	inline static const double MOVETOLERANCE = 0.01;
	// OBS tolerance. Should be 0.1, but may be adjusted to trigger OBS.
	inline static const double OBSTOL = 0.1;
	inline static const double DELTAFACTOR = 0.00001;
	inline static const double MYMIN = 0.05;
	// The maximum angle allowed in an element (not given in the paper)
	inline static const double THETAMAX = Math.toRadians( 200 );
	inline static const double TOL = 0.00001;
	inline static const double GAMMA = 0.8; // 0.8
	inline static const int MAXITER = 5;

	// And now some doubles to hold the default values of some of the above:
	inline static const double defaultE1Factor = 0.04;
	inline static const double defaultE2Factor = 0.09;
	inline static const double defaultCHEVRONMIN = DEG_200;
	inline static const double defaultCOINCTOL = 0.01; // The node coincidence tolerance
	inline static const double defaultMOVETOLERANCE = 0.01; // dunno if it's a good choice
	inline static const double defaultOBSTOL = 0.1;// should be 0.1, adjust to trigger OBS
	inline static const double defaultDELTAFACTOR = 0.00001;
	inline static const double defaultMYMIN = 0.05;
	inline static const double defaultTHETAMAX = DEG_200;
	inline static const double defaultTOL = 0.00001;
	inline static const double defaultGAMMA = 0.8; // 0.8
	inline static const int defaultMAXITER = 5;

	// The origin, only used as a reference.
}
