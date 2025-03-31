#pragma once

#include "Element.h"

/**
 * A class holding information for triangles, and with methods for the handling
 * of issues regarding triangles.
 */

class Triangle : public Element
{
public:
	
	//
	// Creates a triangle (vertices are orientated CCW).
	//
	Triangle( Edge* edge1, Edge* edge2, Edge* edge3,
			  double len1, double len2, double len3,
			  double ang1, double ang2, double ang3,
			  bool lengthsOpt, bool anglesOpt );

	//
	// Creates a triangle (vertices are orientated CCW).
	//
	Triangle( Edge* edge1, Edge* edge2, Edge* edge3 );

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

	Element* elementWithExchangedNodes( Node* original, Node* replacement );

	// Return true if the quad becomes inverted when node n is relocated to pos.
	// (x,y).
	// Else return false.
	bool invertedWhenNodeRelocated( Node* n1, Node* n2 );

	bool equals( const Element* t ) const;

	double angle( Edge* e, Node* n );

	double angle( Edge* e1, Edge* e2 );

	void updateAttributes();

	void updateLengths();

	void updateAngle( Node* n );

	void updateAngles();

	bool hasEdge( Edge* e );

	bool hasNode( Node* n );

	// Return an Edge of this Triangle that is not Edge e.
	Edge* otherEdge( Edge* e );

	// Return the Edge of this Triangle that is not one of Edges e1 or e2.
	Edge* otherEdge( Edge* e1, Edge* e2 );

	// Returns neighbor element sharing edge e
	Element* neighbor( Edge* e );

	// Returns the other edge belonging to this triangle that also share Node n
	// Returns null if not found
	Edge* neighborEdge( Node* n, Edge* e );

	int indexOf( Edge* e );

	int angleIndex( int e1Index, int e2Index );

	int angleIndex( Node* n );

	int angleIndex( Edge* e1, Edge* e2 );

	Edge* oppositeOfNode( Node* n );

	Node* oppositeOfEdge( Edge* e );

	// Make the Element pointers in each of the Edges in this Triangle's edgeList
	// point to this Triangle.
	void connectEdges();

	// Release the element pointer of each Edge in edgeList that pointed to this
	// Element.
	void disconnectEdges();

	// Return the next ccw edge in this triangle.
	// Get the pos angles between e1 and each of the other two edges. Transform
	// these
	// angles into true angles. The largest angle indicates that this edge is ccw to
	// e1.
	Edge* nextCCWEdge( Edge* e1 );

	Edge* nextCWEdge( Edge* e1 );

	// We simply check that the nodes of the element are not collinear.
	bool areaLargerThan0();

	// Check if the old pos and the new pos of the node are on different sides
	// of the nodes opposite edge.
	bool inverted( Node* oldN, Node* newN );

	// Return true if the triangle has become inverted
	bool inverted();

	// Return true if the triangle has become inverted or the area is zero
	bool invertedOrZeroArea();

	// Return true if the triangle area is zero.
	bool zeroArea();

	// Triangles don't have concavities, so return false.
	bool concavityAt( Node* n )
	{
		return false;
	}

	void replaceEdge( Edge* e, Edge* replacement );

	// Returns an edge that is on the boundary. Returns null if not found.
	Edge* getBoundaryEdge();

	//
	// Update the distortion metric according to the article "An approach to
	// Combined Laplacian and Optimization-Based Smoothing for Triangular,
	// Quadrilateral and Quad-Dominant Meshes" by by Cannan, Tristano, and Staten.
	//
	// return negative values for inverted triangles, else positive. Equilateral
	//         triangles will return the maximum value of 1.
	//
	void updateDistortionMetric();

	//
	// See updateDistortionMetric().
	//
	// return negative values for inverted triangles, else positive.
	//
	void updateDistortionMetric( double factor );

	// Return the size of the largest angle
	double largestAngle();

	// Return the node at the largest interior angle
	Node* nodeAtLargestAngle();

	// Return the length of the longest Edge
	double longestEdgeLength();

	// Return the longest Edge
	Edge* longestEdge();

	// Set the color of the edges to green.
	void markEdgesLegal();

	// Set the color of the edges to red.
	void markEdgesIllegal();

	std::string descr();

	void printMe();

	std::string toString();
protected:
	bool fIsAQuad() const override
	{
		return false;
	}

	bool fIsATriangle() const override
	{
		return true;
	}
};