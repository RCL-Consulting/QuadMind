#pragma once

#include "GeomBasics.h"

#include <memory>

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

class Node;

class GlobalSmooth : 
	public GeomBasics
{
private:
	/**
	 * Compute the constrained Laplacian smoothed position of a node.
	 *
	 * @param n the node which is to be subjected to the smooth.
	 * @return the smoothed position of node n.
	 */
	std::shared_ptr<Node> constrainedLaplacianSmooth( const std::shared_ptr<Node>& n );

	/**
	 * @return true if the new constrained-smoothed position is acceptable according
	 *         to the criteria given in section 4.2 of the article.
	 */
	bool acceptable( int N, int Nminus, int Nplus, 
					 int Nup, int Ndown, int Ninverted, 
					 double deltaMy, double theta );

	/**
	 * Compute the optimization-based smoothed position of a node. As described in
	 * section 5 in the paper. Warning: The fields of the argument node will be
	 * altered.
	 *
	 * @return a node with a position that is the optimaization-based smoothed
	 *         position of node n.
	 */
	std::shared_ptr<Node> optBasedSmooth( const std::shared_ptr<Node>& x,
										  const ArrayList<std::shared_ptr<Element>>& elements );

	double maxModDim = 0.0;

public:
	/** Initialize the object. */
	void init();

	/** Perform the smoothing of the nodes in a step-wise manner. */
	void step() override;

	/** The overall smoothing algorithm from section 3 in the paper. */
	void run();

	bool equals( const std::shared_ptr<Constants>& elem ) const override
	{
		return std::dynamic_pointer_cast<GlobalSmooth>(elem) != nullptr;
	}
}; // End of class GlobalSmooth
