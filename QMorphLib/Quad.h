#pragma once

/**
 * A class holding information for quadrilaterals, and with methods for the
 * handling of issues regarding quads.
 */

#include "Element.h"

class Edge;
class Node;
class Triangle;

class Quad : public Element
{
public:
	// Create ordinary quad 
	Quad( Edge* baseEdge, Edge* leftEdge, Edge* rightEdge, Edge* topEdge );

	//
	// Create ordinary quad. Use Edge e's two elements (Triangles) as basis. Not
	// tested thoroughly!!!
	//
	Quad( Edge* e );

	//
	// Create a fake quad with 3 different nodes and edgeList[top]==edgeList[right]
	//
	Quad( Triangle* t );

	//
	// Create a simple quad for testing purposes only (nextCCWNode(),
	// isStrictlyconvex()). Not tested thoroughly!!!
	//
	// @param e  is a diagonal edge
	// @param n1 the first or the other two nodes in the quad
	// @param n2 the second of the other two nodes in the quad
	//
	Quad( Edge* e, Node* n1, Node* n2 );

private:
	//
	// Constructor to make life easier for elementWithExchangedNode(..) Create fake
	// quad with only three nodes
	//
	Quad( Node* n1, Node* n2, Node* n3, Node* f );

	// Constructor to make life easier for elementWithExchangedNode(..) 
	Quad( Node* n1, Node* n2, Node* n3, Node* n4, Node* f );

public:

	//
	// Create a simple quad for testing purposes only (constrainedLaplacianSmooth())
	// Not tested thoroughly!!!
	//
	Element* elementWithExchangedNodes( Node* original, Node* replacement );

	//
	// @return true if the quad becomes inverted when node n1 is relocated to pos.
	//         n2. Else return false.
	//
	bool invertedWhenNodeRelocated( Node* n1, Node* n2 );

	//
	// Test whether any neighboring elements becomes inverted if the quad is
	// collapsed in a particular manner.
	//
	// @param n     a node holding the position for where the joined nodes are to be
	//              located
	// @param n1    the node in quad q that is to be joined with opposite node n2
	// @param n2    the node in quad q that is to be joined with opposite node n1
	// @param lK    the list of elements adjacent n1
	// @param lKOpp the list of elements adjacent n2
	// @return true if any elements adjacent to quad q becomes inverted when
	//         collapsing quad q, joining its two opposite nodes n1 and n2 to the
	//         position held by node n. Node n must be located somewhere inside quad
	//         q.
	//
	bool anyInvertedElementsWhenCollapsed( Node* n, Node* n1, Node* n2,
										   const std::vector<Element*>& lK,
										   const std::vector<Element*>& lKOpp );

	bool equals( const Element* o ) const;

	// @return edge's index in this quad's edgeList 
	int indexOf( Edge* e );

	//
	// Collapse the quad by welding together two neighboring edges. For each edge in
	// the edgeList of the otherNode (otherE2) of edge e2: Provided that otherE1 or
	// the otherNode of otherE2 is not found in any edge in the edgeList of the
	// otherNode of edge e1 (otherE1), then the edge is added to otherE1's edgeList.
	// Assumes that the inner quad area is empty and that the edges of the quad
	// don't have the quad as an element.
	//
	// @param e1 first edge
	// @param e2 second edge, and the neighbor to edge e1 in this quad
	//
	void closeQuad( Edge* e1, Edge* e2 );

	//
	// Create a new quad by combining this quad with quad q. The two quads must
	// initially share two incident edges:
	//
	// @param e1 first common edge
	// @param e2 second common edge
	//
	Quad* combine( Quad* q, Edge* e1, Edge* e2 );

	//
	// Create a new triangle by combining this quad with Triangle t. The two
	// elements must initially share two incident edges:
	//
	// @param e1 first common edge
	// @param e2 second common edge
	//
	Triangle* combine( Triangle* t, Edge* e1, Edge* e2 );

	// @return neighbor element sharing edge e //
	Element* neighbor( Edge* e );

	//
	// @return the edge in this quad that is a neighbor of the given node and edge.
	//
	Edge* neighborEdge( Node* n, Edge* e );

	// @return an edge in this quad that is a neighbor of the given node. //
	Edge* neighborEdge( Node* n );

	double angle( Edge* e, Node* n );

	int angleIndex( int e1Index, int e2Index );

	int angleIndex( Edge* e1, Edge* e2 );

	int angleIndex( Node* n );

	double angle( Edge* e1, Edge* e2 );

	bool hasNode( Node* n );

	bool hasEdge( Edge* e );

	bool boundaryQuad();

	//
	// Method to verify that this quad is a boundary diamond. A boundary diamond is
	// defined as a quad with only one node on the boundary.
	//
	bool boundaryDiamond();

	// Get any node on the boundary that belongs to this quad. //
	Node* getBoundaryNode();

	//
	// Method to verify that the quad has an area greater than 0. We simply check
	// that the nodes of the element are not colinear.
	//
	bool areaLargerThan0();

	// Method to verify that the quad is convex. //
	bool isConvex();

	//
	// Method to verify that the quad is strictly convex, that is, convex in the
	// common sense and in addition demanding that no three Nodes are colinear.
	//
	bool isStrictlyConvex();

	//
	// @return true if the quad is a bowtie, defined as a quad with two opposite
	//         edges that intersect.
	//
	bool isBowtie();

	//
	// @return true if the quad is a chevron, defined as a quad with a greatest
	//         angle that is greater than 200 degrees.
	//
	bool isChevron();

	//
	// @return true if the largest angle of the quad is greater than 180 degrees.
	//
	bool largestAngleGT180();

	//
	// The quad should be (strictly?) convex for this method to work correctly.
	//
	// @return the next ccw oriented Node of this Quad.
	//
	Node* nextCCWNodeOld( Node* n );

	//
	// Not tested much yet, but should work very well in principle.
	//
	// @return the next node in the ccw direction around this quad.
	//
	Node* nextCCWNode( Node* n );

	//
	// Update so that the edge connected to edgeList[base].leftNode is
	// edgeList[left] and that the edge connected to edgeList[base].rightNode is
	// edgeList[right]. The angle between base and left is at pos 0 in the ang
	// array. The angle between right and base is at pos 1 in the ang array. The
	// angle between left and top is at pos 2 in the ang array. The angle between
	// top and right is at pos 3 in the ang array.
	//
	void updateLR();

	//
	// Update the values in the ang array. Works correctly only for uninverted
	// quads.
	//
	void updateAngles();

	// Update the values in the ang array except at the specified node. //
	void updateAnglesExcept( Node* n );

	void updateAngle( Node* n );

	// Method to test whether the quad is inverted. //
	bool inverted();

	// Method to test whether the quad is inverted or its area is zero. //
	bool invertedOrZeroArea();

	//
	// Determines whether there is a concavity (angle > 180 degrees) at the
	// specified node.
	//
	// @param n the node at the angle to investigate
	// @return true if the element has a concavity at the specified node.
	//
	bool concavityAt( Node* n );

	//
	// @return the centroid of this quad.... or at least a point *inside* the
	//         quad... Assumes that the quad is not inverted.
	//
	Node* centroid();

	//
	// @param n a node in this quad
	// @return the node on the opposite side of node n in the quad
	//
	Node* oppositeNode( Node* n );

	// @return the opposite Edge of Node n that is cw to the other opposite Edge //
	Edge* cwOppositeEdge( Node* n );

	//
	// @return the opposite Edge of Node n that is ccw to the other opposite Edge
	//
	Edge* ccwOppositeEdge( Node* n );

	Edge* oppositeEdge( Edge* e );

	//
	// Check to see if any of the neighboring quad elements have become inverted
	// NOTE 1: I might not need to check those elements that lies behind the front.
	//
	bool invertedNeighbors();

	// @return a list of all triangles adjacent to this quad. //
	std::vector<Triangle*> getAdjTriangles();

	// @return a list of all nodes adjacent to this quad. //
	std::vector<Node*> getAdjNodes();

	void replaceEdge( Edge* e, Edge* replacement );

	//
	// Make the Element pointers in each of the Edges in this Quad point to this
	// Quad.
	//
	void connectEdges();

	//
	// Release the element pointer of the edges in edgeList that pointed to this
	// Quad.
	//
	void disconnectEdges();

	//
	// @return an Edge that is common to both this Quad and Quad q at Node n. Return
	//         null if none exists.
	//
	Edge* commonEdgeAt( Node* n, Quad* q );

	//
	// @param q a neighbor quad sharing an edge with this quad.
	// @return an edge that is common to both this quad and quad q. Return null if
	//         none exists.
	//
	Edge* commonEdge( Quad* q );

	//
	// @return true if at least one of the edges connected to node n is a front
	//         edge.
	//
	bool hasFrontEdgeAt( Node* n );

	//
	// @return the number of quad neighbors sharing an edge with this quad at node
	//         n. This quad is not counted. Values are 0, 1, or 2.
	//
	int nrOfQuadsSharingAnEdgeAt( Node* n );

	//
	// Update the distortion metric according to the paper "An approach to Combined
	// Laplacian and Optimization-Based Smoothing for Triangular, Quadrilateral and
	// Quad-Dominant Meshes" by by Cannan, Tristano, and Staten
	//
	// return negative values for inverted quadrilaterals, else positive.
	//         Equilateral quadrilaterals should return the maximum value of 1.
	//
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
	 // |t2 \ / \
 	// | /\ t3 \
 	// | /t1 ---__n2
	 // |/______-----=
	 // n1
	void updateDistortionMetric();

	// Test whether any nodes of the quad are coincident. //
	bool coincidentNodes( Node* n1, Node* n2, Node* n3, Node* n4 );

	// @return the size of the largest interior angle //
	double largestAngle();

	// @return the node at the largest interior angle //
	Node* nodeAtLargestAngle();

	// @return the length of the longest Edge in the quad //
	double longestEdgeLength();

	//
	// @param first a triangle that is located inside the quad
	// @return a list of triangles contained within the four edges of this quad.
	//
	std::vector<Triangle*> trianglesContained( Triangle* first );

	//
	// Test whether the quad contains a hole.
	//
	// @param tris the interior triangles
	// @return true if there are one or more holes present within the four edges
	//         defining the quad.
	//
	bool containsHole( const std::vector<Triangle*>& tris );

	// Set the color of the edges to green. //
	void markEdgesLegal();

	// Set the color of the edges to red. //
	void markEdgesIllegal();

	//
	// Give a string representation of the quad.
	//
	// @return a string representation of the quad.
	//
	std::string descr();

	// Output a string representation of the quad. //
	void printMe();
	
	bool isFake;
protected:
	bool fIsAQuad() const
	{
		return true;
	}

	bool fIsATriangle() const
	{
		return false;
	}
};