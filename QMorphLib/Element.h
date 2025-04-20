#pragma once

#include "Constants.h"

#include <string>

/**
 * This class declares methods and variables that are common to quads and
 * triangles.
 */

class Edge;
class Node;

class Element : public Constants
{
public:

	/** An array of interior angles */
	std::vector<double> ang;
	/** An array of edges */
	std::vector<std::shared_ptr<Edge>> edgeList;
	/** Node used for determining inversion, amonst other things. */
	std::shared_ptr<Node> firstNode;
	/**
	 * Doubles to hold the cur. distortion metric and the metric after perturbation
	 */
	double distortionMetric, newDistortionMetric;
	/** Doubles to hold the gradient vector */
	double gX, gY;

	/** @return neighbor element sharing edge e */
	virtual std::shared_ptr<Element> neighbor( const std::shared_ptr<Edge>& e ) = 0;

	/** @return local angle inside element at Node n */
	virtual double angle( const std::shared_ptr<Edge>& e, 
						  const std::shared_ptr<Node>& n ) = 0;

	/** Compute and set the angles at the nodes of the element. */
	virtual void updateAngles() = 0;

	/**
	 * Compute and set the angle at this particular Node incident with this Element
	 * Edge
	 */
	virtual void updateAngle( const std::shared_ptr<Node>& n ) = 0;

	/** @return description string for element (list of node coordinates) */
	virtual std::string descr() = 0;

	/** Output description string for element (list of node coordinates) */
	virtual void printMe() = 0;

	/** Verify that the quad has the specified edge. */
	virtual bool hasEdge( const std::shared_ptr<Edge>& e ) = 0;

	/** Verify that the quad has the specified node. */
	virtual bool hasNode( const std::shared_ptr<Node>& n ) = 0;

	/** Verify that the area of the quad is greater than 0. */
	virtual bool areaLargerThan0() = 0;

	/** Return local neighboring edge at node n. */
	virtual std::shared_ptr<Edge> neighborEdge( const std::shared_ptr<Node>& n, 
												const std::shared_ptr<Edge>& e ) = 0;

	/** Return the index to this edge in this element's edgeList */
	virtual int indexOf( const std::shared_ptr<Edge>& e ) = 0;

	/** Return the index to this angle in this element's ang array */
	virtual int angleIndex( const std::shared_ptr<Edge>& e1, 
							const std::shared_ptr<Edge>& e2 ) = 0;

	virtual int angleIndex( const std::shared_ptr<Node>& n ) = 0;

	/** Return the angle between this Element's Edges e1 and e2. */
	virtual double angle( const std::shared_ptr<Edge>& e1, 
						  const std::shared_ptr<Edge>& e2 ) = 0;

	/** @return true if the element has become inverted */
	virtual bool inverted() = 0;

	/** @return true if the element has become inverted or its area is zero. */
	virtual bool invertedOrZeroArea() = 0;

	/** @return true if the element has a concavity at its Node n. */
	virtual bool concavityAt( const std::shared_ptr<Node>& n ) = 0;

	/** Replace one of the specified edges e with a replacement edge. */
	virtual void replaceEdge( const std::shared_ptr<Edge>& e, 
							  const std::shared_ptr<Edge>& replacement ) = 0;

	/** Make one element pointer of each Edge in edgeList point to this Element. */
	virtual void connectEdges() = 0;

	/**
	 * Point the element pointer of each Edge in edgeList that previously pointed to
	 * this Element to point to null.
	 */
	virtual void disconnectEdges() = 0;

	/** Create a simple element for testing purposes only. */
	virtual std::shared_ptr<Element> elementWithExchangedNodes( const std::shared_ptr<Node>& original, 
																const std::shared_ptr<Node>& replacement ) = 0;

	/**
	 * @return true if the quad becomes inverted when node n1 is relocated to pos.
	 *         n2. Else return false.
	 */
	virtual bool invertedWhenNodeRelocated( const std::shared_ptr<Node>& n1, 
											const std::shared_ptr<Node>& n2 ) = 0;

	/**
	 * Update the distortion metric according to the article "An approach to
	 * Combined Laplacian and Optimization-Based Smoothing for Triangular,
	 * Quadrilateral and Quad-Dominant Meshes" by by Cannan, Tristano, and Staten
	 */
	virtual void updateDistortionMetric() = 0;

	/** Return the length of the longest Edge. */
	virtual double longestEdgeLength() = 0;

	/** Return the size of the largest angle. */
	virtual double largestAngle() = 0;

	/** Return the node at the largest interior angle. */
	virtual std::shared_ptr<Node> nodeAtLargestAngle() = 0;

	/** Set the color of the edges to red. */
	virtual void markEdgesIllegal() = 0;

	/** Set the color of the edges to green. */
	virtual void markEdgesLegal() = 0;

	/**
	 * A method for fast computation of the cross product of two vectors.
	 *
	 * @param o1 origin of first vector
	 * @param p1 endpoint of first vector
	 * @param o2 origin of second vector
	 * @param p2 endpoint of second vector
	 * @return the cross product of the two vectors
	 */
	
	static double cross( const std::shared_ptr<Node>& o1,
						 const std::shared_ptr<Node>& p1,
						 const std::shared_ptr<Node>& o2,
						 const std::shared_ptr<Node>& p2 );
};
