#pragma once

/**
 * This class holds information for edges, and has methods for handling issues
 * involving edges.
 */

#include "Constants.h"

#include <vector>
#include <string>
#include <array>

class Node;
class Element;
class Triangle;
class Quad;
class MyVector;

class Edge : public Constants
{
public:
	Node* leftNode = nullptr, * rightNode = nullptr; // This Edge has these two nodes
	Element *element1 = nullptr, *element2 = nullptr; // Belongs to these Elements (Quads/Triangles)
	Edge* leftFrontNeighbor=nullptr, *rightFrontNeighbor=nullptr;
	int level;

	static std::array<std::vector<Edge*>, 3> stateList;

	bool frontEdge = false;
	bool swappable = true;
	bool selectable = true;
	// Edge leftSide= null, rightSide= null; // Side edges when building a quad
	bool leftSide = false, rightSide = false; // Indicates if frontNeighbor is
	// to be used as side edge in quad
	double len; // length of this edge
	int color = 1; // Color.green;

	Edge() = default;
	Edge( Node* node1, Node* node2 );

	// Create a clone of Edge e with all the important fields
	Edge( Edge* e );

	bool equals( Edge* o );

	// Return a copy of the edge
	Edge* copy();

	static void clearStateList();

	// Removes an Edge from the stateLists
	// Returns true if the Edge was successfully removed, else false.
	bool removeFromStateList();

	// Removes an Edge from the stateLists
	// Returns true if the Edge was successfully removed, else false.
	bool removeFromStateList( int state );

	int getState();

	int getTrueState();

	bool alterLeftState( bool newLeftState );

	bool alterRightState( bool newRightState );

	// Determine whether the frontNeighbor is an appropriate side Edge for a future
	// Quad with Edge e as base Edge. Return the frontNeighbor if so, else null.
	// elem== null if n.boundaryNode()== true
	Edge* evalPotSideEdge( Edge* frontNeighbor, Node* n );

	// Determine the state bit at both Nodes and set the left and right side Edges.
	// If a state bit is set, then the corresponding front neighbor Edge must get
	// Edge this as a side Edge at that Node. If it is not set, then it must get a
	// null
	// value instead.

	// this: the edge to be classified (gets a state value, and is added to a
	// statelist)
	void classifyStateOfFrontEdge();

	bool isLargeTransition( Edge* e );

	// Select the next front to be processed. The selection criteria is:
	// Primary: the edge state
	// Secondary: the edge level
	// If the candidate edge is part of a large transition on the front where
	// longest - shortest length ratio > 2.5, and the candidate edge is not in
	// state 1-1, then the shorter edge is selected.
	static Edge* getNextFront();

	static void markAllSelectable();

	static void printStateLists();

	// If e.leftNode is leftmore than this.leftNode, return true, else false
	bool leftTo( Edge* e );

	bool isFrontEdge();

	std::string descr();

	void printMe();

	double length();

	// Replace this edge's node n1 with the node n2:
	bool replaceNode( Node* n1, Node* n2 );

	// Seam two edges together. The method assumes that they already have one common
	// node. For each edge in the edgeList of the otherNode (otherE) of edge e:
	// Provided that otherThis or the otherNode of otherE is not found in any edge
	// in
	// the edgeList of the otherNode of this edge (otherThis), then the edge is
	// added
	// to this' edgeList.

	// Assumes that the quad area defined by the three distinct nodes of the two
	// edges,
	// and the node in the top of the triangle adjacent the otherNodes (of the
	// common
	// node of the two edges), is empty.

	void seamWith( Edge* e );

	// Return the midpoint (represented by a new Node) of this edge:
	Node* midPoint();

	double computeLength();

	double length( double x1, double y1, double x2, double y2 );

	double length( Node* node1, Node* node2 );

	// Returns angle relative to the x-axis (which is directed from the origin (0,0)
	// to
	// the right, btw) at node n (which is leftNode or rightNode).
	// Range: <-180, 180>
	double angleAt( Node* n );

	// Returns the angle from this Edge to eEdge by summing the angles of the
	// Elements
	// adjacent Node n.
	double sumAngle( Element* sElem, Node* n, Edge* eEdge );

	// Return the first front Edge adjacent Node n starting check at
	// this Edge in Element sElem.
	Edge* firstFrontEdgeAt( Element* sElem, Node* n );

	// Compute the internal angle between this Edge and Edge edge at Node n.
	// Returns a positive value.
	double computePosAngle( Edge* edge, Node* n );

	// Compute the ccw directed angle between this Edge and Edge edge at Node n.
	// Returns a positive value in range [0, 2*PI>.
	double computeCCWAngle( Edge* edge );

	// Return a common node for edges this and e
	Node* commonNode( Edge* e );

	// Return a common element for edges this and e
	Element* commonElement( Edge* e );

	// Add this Edge to the nodes' edgeLists. Careful, there's no safety checks!
	void connectNodes();

	// Remove this Edge from the nodes' edgeLists. Careful, there's no safety
	// checks!
	void disconnectNodes();

	// Remove this Edge from the nodes' edgeLists. Safety checks...
	void tryToDisconnectNodes();

	void connectToTriangle( Triangle* triangle );

	void connectToQuad( Quad* q );

	void connectToElement( Element* elem );

	// element1 should never be null:
	void disconnectFromElement( Element* elem );

	//
	// @param wrongNode a node that we don't want returned
	// @return a node opposite to this edge in an adjacent triangle
	//
	Node* oppositeNode( Node* wrongNode );

	// Construct an Edge that is a unit normal to this Edge.
	// (Remember to add the new Node to nodeList if you want to keep it.)
	// nB: one of the nodes on this edge (leftNode or rightNode)

	Edge* unitNormalAt( Node* n );

	Edge* getSwappedEdge();

	//
	// Swap diagonal between the edge's two triangles and update locally (To be used
	// with getSwappedEdge())
	//
	void swapToAndSetElementsFor( Edge* e );

	MyVector getVector();

	MyVector getVector( Node* origin );

	bool bordersToTriangle();

	bool boundaryEdge();

	bool boundaryOrTriangleEdge();

	bool hasNode( Node* n );

	bool hasElement( Element* elem );

	bool hasFalseFrontNeighbor();

	bool hasFrontNeighbor( Edge* e );

	Node* otherNode( Node* n );

	//
	// Extend this edge at a given node and to a given lenth.
	//
	// @param length the new length of this edge
	// @param nJ     the node from which the edge is extended
	// @return the other node on the new edge
	//
	Node* otherNodeGivenNewLength( double length, Node* nJ );

	// Prefers the left node if they are at equal y positions
	Node* upperNode();

	// Prefers the right node if they are at equal y positions
	Node* lowerNode();

	//
	// Return true if the 1-orbit around this.commonNode(e) through quad startQ from
	// edge this to edge e doesn't contain any triangle elements. If node n lies on
	// the boundary, and the orbit contains a hole, the orbit simply skips the hole
	// and continues on the other side.
	//
	bool noTrianglesInOrbit( Edge* e, Quad* startQ );

	Edge* findLeftFrontNeighbor( const std::vector<Edge*>& frontList2 );

	Edge* findRightFrontNeighbor( const std::vector<Edge*>& frontList2 );

	// Set the appropriate front neighbor to edge e. 
	void setFrontNeighbor( Edge* e );

	// Returns true if the frontEdgeNeighbors are changed. 
	bool setFrontNeighbors( const std::vector<Edge*>& frontList2 );

	void promoteToFront( int level, std::vector<Edge*>& frontList );

	bool removeFromFront( std::vector<Edge*>& frontList );

	//
	// Halve this Edge by introducing a new Node at the midpoint, and create two
	// Edges from this midpoint to the each of the two opposite Nodes of Edge this:
	// one in element1 and one in element2. Also create two new Edges from Node mid
	// to the two Nodes of Edge this. Create four new Triangles. Update everything
	// (also remove this Edge from edgeList and disconnect the nodes).
	//
	// @return the new Edge incident with Node ben.
	//
	Edge* splitTrianglesAt( Node* nN, Node* ben,
							std::vector<Triangle*>& triangleList,
							std::vector<Edge*>& edgeList );

	//
	// Make new triangles by introducing new Edges at this' midpoint.
	//
	// @return the "lower" (the one incident with the baseEdge) of the two edges
	//         created from splitting this edge.
	//
	Edge* splitTrianglesAtMyMidPoint( std::vector<Triangle*>& triangleList,
									  std::vector<Edge*>& edgeList,
									  std::vector<Node*>& nodeList,
									  Edge* baseEdge );

	//
	// Find the next edge adjacent a quad element, starting at this edge which is
	// part of a given element and which is adjacent a given node. Note that the
	// method stops if the boundary is encountered.
	//
	// @param n         the node
	// @param startElem
	// @return the first edge of a quad found when parsing around node n, starting
	//         at edge e in element startElem and moving in the direction from e to
	//         e's neighbor edge at n in startElem. If startElem happens to be a
	//         quad, the method won't consider that particular quad. If
	//         unsuccessful, the method returns null.
	//
	Edge* nextQuadEdgeAt( Node* n, Element* startElem );

	// Returns a neighboring element that is a quad. When applied to inner front
	// edges,
	// there are, of course, only one possible quad to return.
	Quad* getQuadElement();

	// Returns a neighboring element that is a triangle. The method should work as
	// long
	// as it is applied to a front edge.
	Triangle* getTriangleElement();

	// Return the neighboring quad that is also a neighbor of edge e. //
	Quad* getQuadWithEdge( Edge* e );

	// Return the front neighbor edge at node n. //
	Edge* frontNeighborAt( Node* n );

	// @return the front neighbor next to this (not the prev edge).//
	Edge* nextFrontNeighbor( Edge* prev );

	// Return a neighbor edge at node n, that is front edge according to the
	// definition,
	// and that is part of the same loop as this edge.
	// Assumes that this is a true front edge.
	Edge* trueFrontNeighborAt( Node* n );

	std::string toString();
};