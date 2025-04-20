#pragma once
#include "Element.h"

#include "Constants.h"
#include "ArrayList.h"

#include <memory>

/**
 * A class holding information for quadrilaterals, and with methods for the
 * handling of issues regarding quads.
 */

class Node;
class Edge;
class Triangle;

class Quad : 
	public Element,
	public std::enable_shared_from_this<Quad>
{
public:
	/** Create ordinary quad */
	Quad( const std::shared_ptr<Edge>& baseEdge,
		  const std::shared_ptr<Edge>& leftEdge,
		  const std::shared_ptr<Edge>& rightEdge,
		  const std::shared_ptr<Edge>& topEdge );

	/**
	 * Create ordinary quad. Use Edge e's two elements (Triangles) as basis. Not
	 * tested thoroughly!!!
	 */
	Quad( const std::shared_ptr<Edge>& e );

	/**
	 * Create a fake quad with 3 different nodes and edgeList[top]==edgeList[right]
	 */
	Quad( const std::shared_ptr<Triangle>& t );

	/**
	 * Create a simple quad for testing purposes only (nextCCWNode(),
	 * isStrictlyconvex()). Not tested thoroughly!!!
	 *
	 * @param e  is a diagonal edge
	 * @param n1 the first or the other two nodes in the quad
	 * @param n2 the second of the other two nodes in the quad
	 */
	Quad( const std::shared_ptr<Edge>& e,
		  const std::shared_ptr<Node>& n1,
		  const std::shared_ptr<Node>& n2 );

	/**
	 * Constructor to make life easier for elementWithExchangedNode(..) Create fake
	 * quad with only three nodes
	 */
	Quad( const std::shared_ptr<Node>& n1,
		  const std::shared_ptr<Node>& n2,
		  const std::shared_ptr<Node>& n3,
		  const std::shared_ptr<Node>& f );

	/** Constructor to make life easier for elementWithExchangedNode(..) */
	Quad( const std::shared_ptr<Node>& n1,
		  const std::shared_ptr<Node>& n2,
		  const std::shared_ptr<Node>& n3,
		  const std::shared_ptr<Node>& n4,
		  const std::shared_ptr<Node>& f );

	/**
	 * Create a simple quad for testing purposes only (constrainedLaplacianSmooth())
	 * Not tested thoroughly!!!
	 */
	std::shared_ptr<Element> elementWithExchangedNodes( const std::shared_ptr<Node>& original, 
														const std::shared_ptr<Node>& replacement ) override;

	/**
	 * @return true if the quad becomes inverted when node n1 is relocated to pos.
	 *         n2. Else return false.
	 */
	bool invertedWhenNodeRelocated( const std::shared_ptr<Node>& n1, 
									const std::shared_ptr<Node>& n2 );

	/**
	 * Test whether any neighboring elements becomes inverted if the quad is
	 * collapsed in a particular manner.
	 *
	 * @param n     a node holding the position for where the joined nodes are to be
	 *              located
	 * @param n1    the node in quad q that is to be joined with opposite node n2
	 * @param n2    the node in quad q that is to be joined with opposite node n1
	 * @param lK    the list of elements adjacent n1
	 * @param lKOpp the list of elements adjacent n2
	 * @return true if any elements adjacent to quad q becomes inverted when
	 *         collapsing quad q, joining its two opposite nodes n1 and n2 to the
	 *         position held by node n. Node n must be located somewhere inside quad
	 *         q.
	 */
	bool anyInvertedElementsWhenCollapsed( const std::shared_ptr<Node>& n,
										   const std::shared_ptr<Node>& n1,
										   const std::shared_ptr<Node>& n2,
										   const ArrayList<std::shared_ptr<Element>>& lK,
										   const ArrayList<std::shared_ptr<Element>>& lKOpp );

	bool equals( const std::shared_ptr<Constants>& elem ) const override;

	/** @return edge's index in this quad's edgeList */
	int indexOf( const std::shared_ptr<Edge>& e ) override;

	/**
	 * Collapse the quad by welding together two neighboring edges. For each edge in
	 * the edgeList of the otherNode (otherE2) of edge e2: Provided that otherE1 or
	 * the otherNode of otherE2 is not found in any edge in the edgeList of the
	 * otherNode of edge e1 (otherE1), then the edge is added to otherE1's edgeList.
	 * Assumes that the inner quad area is empty and that the edges of the quad
	 * don't have the quad as an element.
	 *
	 * @param e1 first edge
	 * @param e2 second edge, and the neighbor to edge e1 in this quad
	 */
	void closeQuad( const std::shared_ptr<Edge>& e1,
					const std::shared_ptr<Edge>& e2 );

	/**
	 * Create a new quad by combining this quad with quad q. The two quads must
	 * initially share two incident edges:
	 *
	 * @param e1 first common edge
	 * @param e2 second common edge
	 */
	std::shared_ptr<Quad> combine( const std::shared_ptr<Quad>& q,
								   const std::shared_ptr<Edge>& e1,
								   const std::shared_ptr<Edge>& e2 );

	/**
	 * Create a new triangle by combining this quad with Triangle t. The two
	 * elements must initially share two incident edges:
	 *
	 * @param e1 first common edge
	 * @param e2 second common edge
	 */
	std::shared_ptr<Triangle> combine( const std::shared_ptr<Triangle>& t,
									   const std::shared_ptr<Edge>& e1,
									   const std::shared_ptr<Edge>& e2 );

	/** @return neighbor element sharing edge e */
	std::shared_ptr<Element> neighbor( const std::shared_ptr<Edge>& e ) override;

	/**
	 * @return the edge in this quad that is a neighbor of the given node and edge.
	 */
	std::shared_ptr<Edge> neighborEdge( const std::shared_ptr<Node>& n,
										const std::shared_ptr<Edge>& e ) override;

	/** @return an edge in this quad that is a neighbor of the given node. */
	std::shared_ptr<Edge> neighborEdge( const std::shared_ptr<Node>& n );

	double angle( const std::shared_ptr<Edge>& e, 
				  const std::shared_ptr<Node>& n ) override;

	int angleIndex( int e1Index, int e2Index );

	int angleIndex( const std::shared_ptr<Edge>& e1, 
					const std::shared_ptr<Edge>& e2 ) override;

	int angleIndex( const std::shared_ptr<Node>& n ) override;

	double angle( const std::shared_ptr<Edge>& e1, 
				  const std::shared_ptr<Edge>& e2 ) override;

	bool hasNode( const std::shared_ptr<Node>& n ) override;

	bool hasEdge( const std::shared_ptr<Edge>& e ) override;

	bool boundaryQuad();

	/**
	 * Method to verify that this quad is a boundary diamond. A boundary diamond is
	 * defined as a quad with only one node on the boundary.
	 */
	bool boundaryDiamond();

	/** Get any node on the boundary that belongs to this quad. */
	std::shared_ptr<Node> getBoundaryNode();

	/**
	 * Method to verify that the quad has an area greater than 0. We simply check
	 * that the nodes of the element are not colinear.
	 */
	bool areaLargerThan0() override;

	/** Method to verify that the quad is convex. */
	bool isConvex();

	/**
	 * Method to verify that the quad is strictly convex, that is, convex in the
	 * common sense and in addition demanding that no three Nodes are colinear.
	 */
	bool isStrictlyConvex();

	/**
	 * @return true if the quad is a bowtie, defined as a quad with two opposite
	 *         edges that intersect.
	 */
	bool isBowtie();

	/**
	 * @return true if the quad is a chevron, defined as a quad with a greatest
	 *         angle that is greater than 200 degrees.
	 */
	bool isChevron();

	/**
	 * @return true if the largest angle of the quad is greater than 180 degrees.
	 */
	bool largestAngleGT180();

	/**
	 * The quad should be (strictly?) convex for this method to work correctly.
	 *
	 * @return the next ccw oriented Node of this Quad.
	 */
	std::shared_ptr<Node> nextCCWNodeOld( const std::shared_ptr<Node>& n );

	/**
	 * Not tested much yet, but should work very well in principle.
	 *
	 * @return the next node in the ccw direction around this quad.
	 */
	std::shared_ptr<Node> nextCCWNode( const std::shared_ptr<Node>& n );

	/**
	 * Update so that the edge connected to edgeList[base].leftNode is
	 * edgeList[left] and that the edge connected to edgeList[base].rightNode is
	 * edgeList[right]. The angle between base and left is at pos 0 in the ang
	 * array. The angle between right and base is at pos 1 in the ang array. The
	 * angle between left and top is at pos 2 in the ang array. The angle between
	 * top and right is at pos 3 in the ang array.
	 */
	void updateLR();

	/**
	 * Update the values in the ang array. Works correctly only for uninverted
	 * quads.
	 */
	void updateAngles() override;

	/** Update the values in the ang array except at the specified node. */
	void updateAnglesExcept( const std::shared_ptr<Node>& n );

	void updateAngle( const std::shared_ptr<Node>& n ) override;

	/** Method to test whether the quad is inverted. */
	bool inverted() override;

	/** Method to test whether the quad is inverted or its area is zero. */
	bool invertedOrZeroArea() override;

	/**
	 * Determines whether there is a concavity (angle > 180 degrees) at the
	 * specified node.
	 *
	 * @param n the node at the angle to investigate
	 * @return true if the element has a concavity at the specified node.
	 */
	bool concavityAt( const std::shared_ptr<Node>& n ) override;

	/**
	 * @return the centroid of this quad.... or at least a point *inside* the
	 *         quad... Assumes that the quad is not inverted.
	 */
	std::shared_ptr<Node> centroid();

	/**
	 * @param n a node in this quad
	 * @return the node on the opposite side of node n in the quad
	 */
	std::shared_ptr<Node> oppositeNode( const std::shared_ptr<Node>& n );

	/** @return the opposite Edge of Node n that is cw to the other opposite Edge */
	std::shared_ptr<Edge> cwOppositeEdge( const std::shared_ptr<Node>& n );

	/**
	 * @return the opposite Edge of Node n that is ccw to the other opposite Edge
	 */
	std::shared_ptr<Edge> ccwOppositeEdge( const std::shared_ptr<Node>& n );

	std::shared_ptr<Edge> oppositeEdge( const std::shared_ptr<Edge>& e );

	/**
	 * Check to see if any of the neighboring quad elements have become inverted
	 * NOTE 1: I might not need to check those elements that lies behind the front.
	 */
	bool invertedNeighbors();

	/** @return a list of all triangles adjacent to this quad. */
	ArrayList<std::shared_ptr<Triangle>> getAdjTriangles();

	/** @return a list of all nodes adjacent to this quad. */
	ArrayList<std::shared_ptr<Node>> getAdjNodes();

	void replaceEdge( const std::shared_ptr<Edge>& e, 
					  const std::shared_ptr<Edge>& replacement ) override;

	/**
	 * Make the Element pointers in each of the Edges in this Quad point to this
	 * Quad.
	 */
	void connectEdges() override;

	/**
	 * Release the element pointer of the edges in edgeList that pointed to this
	 * Quad.
	 */
	void disconnectEdges() override;

	/**
	 * @return an Edge that is common to both this Quad and Quad q at Node n. Return
	 *         null if none exists.
	 */
	std::shared_ptr<Edge> commonEdgeAt( const std::shared_ptr<Node>& n,
										const std::shared_ptr<Quad>& q );

	/**
	 * @param q a neighbor quad sharing an edge with this quad.
	 * @return an edge that is common to both this quad and quad q. Return null if
	 *         none exists.
	 */
	std::shared_ptr<Edge> commonEdge( const std::shared_ptr<Quad>& q );

	/**
	 * @return true if at least one of the edges connected to node n is a front
	 *         edge.
	 */
	bool hasFrontEdgeAt( const std::shared_ptr<Node>& n );

	/**
	 * @return the number of quad neighbors sharing an edge with this quad at node
	 *         n. This quad is not counted. Values are 0, 1, or 2.
	 */
	int nrOfQuadsSharingAnEdgeAt( const std::shared_ptr<Node>& n );

	/**
	 * Update the distortion metric according to the paper "An approach to Combined
	 * Laplacian and Optimization-Based Smoothing for Triangular, Quadrilateral and
	 * Quad-Dominant Meshes" by by Cannan, Tristano, and Staten
	 *
	 * return negative values for inverted quadrilaterals, else positive.
	 *         Equilateral quadrilaterals should return the maximum value of 1.
	 */
	 //
	 // This is a simple sketch of the quadrilateral with nodes and divided
	 // into four triangles:
	 //
	 // n3__________n4
	 // |\ /|
	 // | \ t4 / |
	 // | \ / |
	 // | t2 X t3 |
	 // | / \ |
	 // | / t1 \ |
	 // |/___________\|
	 // n1 n2
	 //
	 // Also, I tried to sketch the case where the quad has an angle > than 180
	 // degrees
	 // Note that t3 is part of t1 and that t4 is part of t2 in the sketch.
	 // t3 and t4 are inverted.
	 //
	 // n3
	 // |\ \
 	// | \ \
 	// | \t4 X
	// // |t2 \ / \
 	// | /\ t3 \
 	// | /t1 ---__n2
	 // |/______-----=
	 // n1
	void updateDistortionMetric() override;

	/** Test whether any nodes of the quad are coincident. */
	bool coincidentNodes( const std::shared_ptr<Node>& n1,
						  const std::shared_ptr<Node>& n2,
						  const std::shared_ptr<Node>& n3,
						  const std::shared_ptr<Node>& n4 );

	/** @return the size of the largest interior angle */
	double largestAngle() override;

	/** @return the node at the largest interior angle */
	std::shared_ptr<Node> nodeAtLargestAngle() override;

	/** @return the length of the longest Edge in the quad */
	double longestEdgeLength() override;

	/**
	 * @param first a triangle that is located inside the quad
	 * @return a list of triangles contained within the four edges of this quad.
	 */
	ArrayList<std::shared_ptr<Triangle>> trianglesContained( const std::shared_ptr<Triangle>& first );

	/**
	 * Test whether the quad contains a hole.
	 *
	 * @param tris the interior triangles
	 * @return true if there are one or more holes present within the four edges
	 *         defining the quad.
	 */
	bool containsHole( const ArrayList<std::shared_ptr<Element>>& tris );

	/** Set the color of the edges to green. */
	void markEdgesLegal() override;

	/** Set the color of the edges to red. */
	void markEdgesIllegal() override;

	/**
	 * Give a string representation of the quad.
	 *
	 * @return a string representation of the quad.
	 */
	std::string descr() override;

	/** Output a string representation of the quad. */
	void printMe() override;

	bool isFake = false;
};
