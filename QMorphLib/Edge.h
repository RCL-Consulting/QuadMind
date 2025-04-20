#pragma once

#include "Constants.h"
#include "ArrayList.h"

#include <array>

/**
 * This class holds information for edges, and has methods for handling issues
 * involving edges.
 */

class Node;
class Element;
class Edge;
class Triangle;
class Quad;
class MyVector;

class Edge : 
	public Constants,
	public std::enable_shared_from_this<Edge>
{
public:
	std::shared_ptr<Node> leftNode, rightNode; // This Edge has these two nodes
	std::shared_ptr<Element> element1 = nullptr, element2 = nullptr; // Belongs to these Elements (Quads/Triangles)
	std::shared_ptr<Edge> leftFrontNeighbor, rightFrontNeighbor;
	int level;

	static std::array<ArrayList<std::shared_ptr<Edge>>, 3> stateList;

	bool frontEdge = false;
	bool swappable = true;
	bool selectable = true;
	// Edge leftSide= null, rightSide= null; // Side edges when building a quad
	bool leftSide = false, rightSide = false; // Indicates if frontNeighbor is
	// to be used as side edge in quad
	double len; // length of this edge
	Color color = Color::Green;

	Edge( const std::shared_ptr<Node>& node1,
		  const std::shared_ptr<Node>& node2 );

	// Create a clone of Edge e with all the important fields
	Edge( const Edge& e );

public:
	bool equals( const std::shared_ptr<Constants>& elem ) const override;
	bool operator==( const Edge& e ) const;

	// Return a copy of the edge
	std::shared_ptr<Edge> copy();

	static void clearStateList();

	// Removes an Edge from the stateLists
	// Returns true if the Edge was successfully removed, else false.
	bool removeFromStateList();

	// Removes an Edge from the stateLists
	// Returns true if the Edge was successfully removed, else false.
	bool removeFromStateList( int state );

	int getState();

	bool alterLeftState( bool newLeftState );
	bool alterRightState( bool newRightState );

	// Determine whether the frontNeighbor is an appropriate side Edge for a future
	// Quad with Edge e as base Edge. Return the frontNeighbor if so, else null.
	// elem== null if n.boundaryNode()== true
	std::shared_ptr<Edge> evalPotSideEdge( const std::shared_ptr<Edge>& frontNeighbor,
										   const std::shared_ptr<Node>& n );

	// Determine the state bit at both Nodes and set the left and right side Edges.
	// If a state bit is set, then the corresponding front neighbor Edge must get
	// Edge this as a side Edge at that Node. If it is not set, then it must get a
	// null
	// value instead.

	// this: the edge to be classified (gets a state value, and is added to a
	// statelist)
	void classifyStateOfFrontEdge();

	bool isLargeTransition( const std::shared_ptr<Edge>& e );
	
	// Select the next front to be processed. The selection criteria is:
	// Primary: the edge state
	// Secondary: the edge level
	// If the candidate edge is part of a large transition on the front where
	// longest - shortest length ratio > 2.5, and the candidate edge is not in
	// state 1-1, then the shorter edge is selected.
	static std::shared_ptr<Edge> getNextFront();
	
	static void markAllSelectable();

	static void printStateLists();

	// If e.leftNode is leftmore than this.leftNode, return true, else false
	bool leftTo( const std::shared_ptr<Edge>& e );

	bool isFrontEdge();

	std::string descr();

	void printMe();

	double length()	{ return len; }

	// Replace this edge's node n1 with the node n2:
	bool replaceNode( const std::shared_ptr<Node>& n1,
					  const std::shared_ptr<Node>& n2 );

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

	void seamWith( const std::shared_ptr<Edge>& e );

	// Return the midpoint (represented by a new Node) of this edge:
	std::shared_ptr<Node> midPoint();

	double computeLength();

	static double length( double x1, double y1, double x2, double y2 );

	static double length( const std::shared_ptr<Node>& node1,
						  const std::shared_ptr<Node>& node2 );

	// Returns angle relative to the x-axis (which is directed from the origin (0,0)
	// to
	// the right, btw) at node n (which is leftNode or rightNode).
	// Range: <-180, 180>
	double angleAt( const std::shared_ptr<Node>& n );

	// Returns the angle from this Edge to eEdge by summing the angles of the
	// Elements
	// adjacent Node n.
	double sumAngle( const std::shared_ptr<Element>& sElem,
					 const std::shared_ptr<Node>& n,
					 const std::shared_ptr<Edge>& eEdge );

	// Return the first front Edge adjacent Node n starting check at
	// this Edge in Element sElem.
	std::shared_ptr<Edge> firstFrontEdgeAt( const std::shared_ptr<Element>& sElem,
											const std::shared_ptr<Node>& n );

	// Compute the internal angle between this Edge and Edge edge at Node n.
	// Returns a positive value.
	double computePosAngle( const std::shared_ptr<Edge>& edge,
							const std::shared_ptr<Node>& n );

	// Compute the ccw directed angle between this Edge and Edge edge at Node n.
	// Returns a positive value in range [0, 2*PI>.
	double computeCCWAngle( const std::shared_ptr<Edge>& edge );

	// Return a common node for edges this and e
	std::shared_ptr<Node> commonNode( const std::shared_ptr<Edge> e );

	// Return a common element for edges this and e
	std::shared_ptr<Element> commonElement( const std::shared_ptr<Edge>& e );

	// Add this Edge to the nodes' edgeLists. Careful, there's no safety checks!
	void connectNodes();

	// Remove this Edge from the nodes' edgeLists. Careful, there's no safety
	// checks!
	void disconnectNodes();

	// Remove this Edge from the nodes' edgeLists. Safety checks...
	void tryToDisconnectNodes();

	void connectToTriangle( const std::shared_ptr<Triangle>& triangle );

	void connectToQuad( const std::shared_ptr<Quad>& q );

	void connectToElement( const std::shared_ptr<Element>& elem );

	// element1 should never be null:
	void disconnectFromElement( const std::shared_ptr<Element>& elem );

	/**
	 * @param wrongNode a node that we don't want returned
	 * @return a node opposite to this edge in an adjacent triangle
	 */
	std::shared_ptr<Node> oppositeNode( const std::shared_ptr<Node>& wrongNode );

	// Construct an Edge that is a unit normal to this Edge.
	// (Remember to add the new Node to nodeList if you want to keep it.)
	// nB: one of the nodes on this edge (leftNode or rightNode)

	/*
	 * C b ____----x ___---- | b=1 ___---- | x----------------------x A c B
	 *
	 * angle(B)= PI/2
	 *
	 */
	 // xdiff= xB - xA
	 // ydiff= yB - yA
	 // a= 1, c= sqrt(xdiff^2 + ydiff^2)
	 // xC = xA + b* cos(alpha +ang.A)
	 // = xA + xdiff - ydiff*a/c
	 // = xB - ydiff*a/c
	 // = xB - ydiff/c
	 //
	 // yC = yA + b* sin(alpha + ang.A)
	 // = yA + ydiff + xdiff*a/c
	 // = yB + xdiff*a/c
	 // = yB + xdiff/c
	 //
	std::shared_ptr<Edge> unitNormalAt( const std::shared_ptr<Node>& n );

	std::shared_ptr<Edge> getSwappedEdge();

	/**
	 * Swap diagonal between the edge's two triangles and update locally (To be used
	 * with getSwappedEdge())
	 */
	void swapToAndSetElementsFor( const std::shared_ptr<Edge>& e );

	MyVector getVector();

	MyVector getVector( const std::shared_ptr<Node>& origin );

	bool bordersToTriangle();

	bool boundaryEdge();

	bool boundaryOrTriangleEdge();

	bool hasNode( const std::shared_ptr<Node>& n );

	bool hasElement( const std::shared_ptr<Element>& elem );

	bool hasFalseFrontNeighbor();

	bool hasFrontNeighbor( const std::shared_ptr<Edge>& e );

	std::shared_ptr<Node> otherNode( const std::shared_ptr<Node>& n );

	/**
	 * Extend this edge at a given node and to a given lenth.
	 *
	 * @param length the new length of this edge
	 * @param nJ     the node from which the edge is extended
	 * @return the other node on the new edge
	 */
	std::shared_ptr<Node> otherNodeGivenNewLength( double length,
												   const std::shared_ptr<Node>& nJ );

	// Prefers the left node if they are at equal y positions
	std::shared_ptr<Node> upperNode();

	// Prefers the right node if they are at equal y positions
	std::shared_ptr<Node> lowerNode();

	/**
	 * Return true if the 1-orbit around this.commonNode(e) through quad startQ from
	 * edge this to edge e doesn't contain any triangle elements. If node n lies on
	 * the boundary, and the orbit contains a hole, the orbit simply skips the hole
	 * and continues on the other side.
	 */
	bool noTrianglesInOrbit( const std::shared_ptr<Edge>& e,
							 const std::shared_ptr<Quad>& startQ );

	std::shared_ptr<Edge> findLeftFrontNeighbor( const ArrayList<std::shared_ptr<Edge>>& frontList2 );

	std::shared_ptr<Edge> findRightFrontNeighbor( const ArrayList<std::shared_ptr<Edge>>& frontList2 );

	/** Set the appropriate front neighbor to edge e. */
	void setFrontNeighbor( const std::shared_ptr<Edge>& e );

	/** Returns true if the frontEdgeNeighbors are changed. */
	bool setFrontNeighbors( const ArrayList<std::shared_ptr<Edge>>& frontList2 );

	void promoteToFront( int level,
						 ArrayList<std::shared_ptr<Edge>>& frontList );

	bool removeFromFront( ArrayList<std::shared_ptr<Edge>>& frontList2 );

	/**
	 * Halve this Edge by introducing a new Node at the midpoint, and create two
	 * Edges from this midpoint to the each of the two opposite Nodes of Edge this:
	 * one in element1 and one in element2. Also create two new Edges from Node mid
	 * to the two Nodes of Edge this. Create four new Triangles. Update everything
	 * (also remove this Edge from edgeList and disconnect the nodes).
	 *
	 * @return the new Edge incident with Node ben.
	 */
	std::shared_ptr<Edge> splitTrianglesAt( const std::shared_ptr<Node>& nN,
											const std::shared_ptr<Node>& ben,
											ArrayList<std::shared_ptr<Triangle>>& triangleList,
											ArrayList<std::shared_ptr<Edge>>& edgeList,
											const ArrayList<std::shared_ptr<Node>>& nodeList );

	/**
	 * Make new triangles by introducing new Edges at this' midpoint.
	 *
	 * @return the "lower" (the one incident with the baseEdge) of the two edges
	 *         created from splitting this edge.
	 */
	std::shared_ptr<Edge> splitTrianglesAtMyMidPoint( ArrayList<std::shared_ptr<Triangle>>& triangleList,
													  ArrayList<std::shared_ptr<Edge>>& edgeList,
													  ArrayList<std::shared_ptr<Node>>& nodeList,
													  const std::shared_ptr<Edge>& baseEdge );

	/**
	 * Find the next edge adjacent a quad element, starting at this edge which is
	 * part of a given element and which is adjacent a given node. Note that the
	 * method stops if the boundary is encountered.
	 *
	 * @param n         the node
	 * @param startElem
	 * @return the first edge of a quad found when parsing around node n, starting
	 *         at edge e in element startElem and moving in the direction from e to
	 *         e's neighbor edge at n in startElem. If startElem happens to be a
	 *         quad, the method won't consider that particular quad. If
	 *         unsuccessful, the method returns null.
	 */
	std::shared_ptr<Edge> nextQuadEdgeAt( const std::shared_ptr<Node>& n,
										  const std::shared_ptr<Element>& startElem );

	// Returns a neighboring element that is a quad. When applied to inner front
	// edges,
	// there are, of course, only one possible quad to return.
	std::shared_ptr<Quad> getQuadElement();

	// Returns a neighboring element that is a triangle. The method should work as
	// long
	// as it is applied to a front edge.
	std::shared_ptr<Triangle> getTriangleElement();

	/** Return the neighboring quad that is also a neighbor of edge e. */
	std::shared_ptr<Quad> getQuadWithEdge( const std::shared_ptr<Edge>& e );

	/** Return the front neighbor edge at node n. */
	std::shared_ptr<Edge> frontNeighborAt( const std::shared_ptr<Node>& n );

	/** @return the front neighbor next to this (not the prev edge). */
	std::shared_ptr<Edge> nextFrontNeighbor( const std::shared_ptr<Edge>& prev );

	// Return a neighbor edge at node n, that is front edge according to the
	// definition,
	// and that is part of the same loop as this edge.
	// Assumes that this is a true front edge.
	std::shared_ptr<Edge> trueFrontNeighborAt( const std::shared_ptr<Node>& n );

	std::string toString();
};
  