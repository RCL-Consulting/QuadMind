#pragma once

// ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ----
/**
 * This class is an implementation of the algorithm described in the paper "An
 * approach to Combined Laplacian and Optimization-Based Smoothing for
 * Triangular, Quadrilateral and Quad-Dominant Meshes" (1998) by Cannan,
 * Tristano, and Staten.
 *
 * The meshes produced by Q-Morph are indeed highly quad-dominant, with at most
 * one single triangle, so Q-Morph should work well with this algorithm.
 *
 * Note that the boundary layer smoothing is not implemented.
 *
 * @author Karl Erik Levik
 *
 */
 // ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ----

#include "GeomBasics.h"

class GlobalSmooth : public GeomBasics
{
	//
	// Compute the constrained Laplacian smoothed position of a node.
	//
	// @param n the node which is to be subjected to the smooth.
	// @return the smoothed position of node n.
	//
private:
	Node* constrainedLaplacianSmooth( Node* n );

	//
	// @return true if the new constrained-smoothed position is acceptable according
	//        to the criteria given in section 4.2 of the article.
	//
	bool acceptable( int N, int Nminus, int Nplus, int Nup,
					 int Ndown, int Ninverted, double deltaMy,
					 double theta );

	//
	// Compute the optimization-based smoothed position of a node. As described in
	// section 5 in the paper. Warning: The fields of the argument node will be
	// altered.
	//
	// @return a node with a position that is the optimaization-based smoothed
	//         position of node n.
	//
	Node* optBasedSmooth( Node* x, const std::vector<Element*>& elements );

	double maxModDim = 0.0;

public:
	// Initialize the object. //
	void init();

	// Perform the smoothing of the nodes in a step-wise manner. //
	void step();

	// The overall smoothing algorithm from section 3 in the paper. //
	void run();

}; // End of class GlobalSmooth
