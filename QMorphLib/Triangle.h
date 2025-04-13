#pragma once
#include "Element.h"

/**
 * A class holding information for triangles, and with methods for the handling
 * of issues regarding triangles.
 */

class Triangle :
	public Element,
	public std::enable_shared_from_this<Triangle>
{
public:

	/**
	 * Creates a triangle (vertices are orientated CCW).
	 */
	Triangle( const std::shared_ptr<Edge>& edge1,
			  const std::shared_ptr<Edge>& edge2,
			  const std::shared_ptr<Edge>& edge3,
			  double len1, double len2, double len3,
			  double ang1, double ang2, double ang3,
			  bool lengthsOpt, bool anglesOpt );

	/**
	 * Creates a triangle (vertices are orientated CCW).
	 */
	Triangle( const std::shared_ptr<Edge>& edge1,
			  const std::shared_ptr<Edge>& edge2,
			  const std::shared_ptr<Edge>& edge3 );

	// Makes a copy of the given triangle
	Triangle( const Triangle& t );

	// Create a simple triangle for testing purposes only
	// (constrainedLaplacianSmooth()
	// and optBasedSmooth(..))
	// The triangle created is a copy of this, and each of the edges are also
	// copies.
	// One of the nodes in the triangle has been replaced.
	// The Edge.repleceNode(..) method updates the edge length.
	// *Not tested!!!*

	std::shared_ptr<Element> elementWithExchangedNodes( const std::shared_ptr<Node>& original,
														const std::shared_ptr<Node>& replacement );

	bool equals( const std::shared_ptr<Constants>& elem ) const override;

	double angle( const std::shared_ptr<Edge>& e, 
				  const std::shared_ptr<Node>& n ) override;

	double angle( const std::shared_ptr<Edge>& e1, 
				  const std::shared_ptr<Edge>& e2 ) override;

	void updateAttributes();

	void updateLengths();

	void updateAngle( const std::shared_ptr<Node>& n ) override;

	void updateAngles() override;

	bool hasEdge( const std::shared_ptr<Edge>& e ) override;

	bool hasNode( const std::shared_ptr<Node>& n ) override;

	// Return an Edge of this Triangle that is not Edge e.
	std::shared_ptr<Edge> otherEdge( const std::shared_ptr<Edge>& e );

	// Return the Edge of this Triangle that is not one of Edges e1 or e2.
	std::shared_ptr<Edge> otherEdge( const std::shared_ptr<Edge>& e1,
									 const std::shared_ptr<Edge>& e2 );

	// Returns neighbor element sharing edge e
	std::shared_ptr<Element> neighbor( const std::shared_ptr<Edge>& e ) override;

	// Returns the other edge belonging to this triangle that also share Node n
	// Returns null if not found
	std::shared_ptr<Edge> neighborEdge( const std::shared_ptr<Node>& n, 
										const std::shared_ptr<Edge>& e ) override;

	int indexOf( const std::shared_ptr<Edge>& e ) override;

	int angleIndex( int e1Index, int e2Index );

	int angleIndex( const std::shared_ptr<Node>& n ) override;

	int angleIndex( const std::shared_ptr<Edge>& e1, 
					const std::shared_ptr<Edge>& e2 ) override;

	std::shared_ptr<Edge> oppositeOfNode( const std::shared_ptr<Node>& n );

	std::shared_ptr<Node> oppositeOfEdge( const std::shared_ptr<Edge>& e );

	// Make the Element pointers in each of the Edges in this Triangle's edgeList
	// point to this Triangle.
	void connectEdges() override;

	// Release the element pointer of each Edge in edgeList that pointed to this
	// Element.
	void disconnectEdges() override;

	// Return the next ccw edge in this triangle.
	// Get the pos angles between e1 and each of the other two edges. Transform
	// these
	// angles into true angles. The largest angle indicates that this edge is ccw to
	// e1.
	std::shared_ptr<Edge> nextCCWEdge( const std::shared_ptr<Edge>& e1 );

	// We simply check that the nodes of the element are not collinear.
	bool areaLargerThan0() override;

	// Check if the old pos and the new pos of the node are on different sides
	// of the nodes opposite edge.
	bool inverted( const std::shared_ptr<Node>& oldN,
				   const std::shared_ptr<Node>& newN );

	// Return true if the triangle has become inverted
	bool inverted() override;

	// Return true if the triangle has become inverted or the area is zero
	bool invertedOrZeroArea() override;

	// Return true if the triangle area is zero.
	bool zeroArea();

	// Triangles don't have concavities, so return false.
	bool concavityAt( const std::shared_ptr<Node>& n ) override
	{
		return false;
	}

	void replaceEdge( const std::shared_ptr<Edge>& e, 
					  const std::shared_ptr<Edge>& replacement ) override;

	// Returns an edge that is on the boundary. Returns null if not found.
	std::shared_ptr<Edge> getBoundaryEdge();

	/**
	 * Update the distortion metric according to the article "An approach to
	 * Combined Laplacian and Optimization-Based Smoothing for Triangular,
	 * Quadrilateral and Quad-Dominant Meshes" by by Cannan, Tristano, and Staten.
	 *
	 * return negative values for inverted triangles, else positive. Equilateral
	 *         triangles will return the maximum value of 1.
	 */
	void updateDistortionMetric() override;

	/**
	 * See updateDistortionMetric().
	 *
	 * return negative values for inverted triangles, else positive.
	 */
	void updateDistortionMetric( double factor );

	// Return the size of the largest angle
	double largestAngle() override;

	// Return the node at the largest interior angle
	std::shared_ptr<Node> nodeAtLargestAngle() override;//}

	// Return the length of the longest Edge
	double longestEdgeLength() override;

	// Return the longest Edge
	std::shared_ptr<Edge> longestEdge();

	// Set the color of the edges to green.
	void markEdgesLegal() override;

	// Set the color of the edges to red.
	void markEdgesIllegal() override;

	std::string descr() override;

	void printMe() override;

	std::string toString();

};