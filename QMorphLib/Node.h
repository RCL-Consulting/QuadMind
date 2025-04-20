#pragma once

#include "Constants.h"
#include "ArrayList.h"
#include "Numbers.h"

#include <memory>
#include <vector>
#include <string>

class Edge;
class MyVector;
class Element;
class Triangle;

/**
 * This class holds information for nodes, and has methods for the management of
 * issues regarding nodes.
 */

class Node:
	public Constants,
	public std::enable_shared_from_this<Node>
{
public:
	/** Boolean indicating whether the node has been moved by the OBS */
	bool movedByOBS = false; // Used by the smoother
	/** The coordinates */
	double x, y;
	/** A valence pattern for this node */
	std::vector<uint8_t> pattern;
	// byte state= 0; // For front Nodes only
	ArrayList<std::shared_ptr<Edge>> edgeList;
	Color color = Color::Cyan;

	/** Create new node with position (x,y). */
	Node( double x, double y )
		: x( x )
		, y( y )
	{
	}

	bool equals( const std::shared_ptr<Constants>& elem ) const override;
	bool operator==( const Node& other ) const
	{
		return rcl::equal( other.x, x ) && rcl::equal( other.y, y );
	}

	/** @return a "real" copy of this node with a shallow copy of its edgeList. */
	std::shared_ptr<Node> copy();

	/** @return a new node with the same position as this. */
	std::shared_ptr<Node> copyXY();

	/** Relocate this node to the same position as n. */
	void setXY( const Node& n );

	/** Relocate this node to position (x,y) */
	void setXY( double x, double y );

	void update();

	void updateLRinEdgeList();

	/** Change the position of the node to position (x,y) */
	void moveToPos( double x, double y );

	/** Change the position of the node to the position of the specified node */
	void moveTo( const Node& n );

	/** Update all lengths of edges around this Node */
	void updateEdgeLengths();

	void updateState() {}

	/**
	 * Update (almost) all angles in all elements adjacent to this Node. Note: still
	 * experimental, not tested thoroughly.
	 */
	void updateAngles();
	
	double cross( const Node& n );

	void connectToEdge( const std::shared_ptr<Edge>& edge );

	// Rewrite of ccwSortedEdgeList().
	// We use vector representations instead of the edges directly.
	ArrayList<std::shared_ptr<MyVector>> ccwSortedVectorList();

	 /**
	  * Assumes: b0 and b1 form a convex boundary, but not neccessarily *strictly*
	  * convex
	  *
	  * @param b0 First boundary edge
	  * @param b1 Second boundary edge
	  */
	ArrayList<std::shared_ptr<Edge>> calcCCWSortedEdgeList( const std::shared_ptr<Edge>& b0,
															const std::shared_ptr<Edge>& b1 );
	

	/**
	 * Note: *ALL* nodes in a neighboring quad is regarded as neighbors, not only
	 * those that are directly connected to this node by edges.
	 *
	 * @return a ccw sorted list of the neighboring nodes to this, but returns null
	 *         if this node is part of any triangle.
	 */
	std::vector<std::shared_ptr<Node>> ccwSortedNeighbors();
	
	double meanNeighborEdgeLength();

	int nrOfAdjElements();

	ArrayList<std::shared_ptr<Element>> adjElements();

	int nrOfAdjQuads();
	
	ArrayList<std::shared_ptr<Element>> adjQuads();

	int nrOfAdjTriangles();

	// Hmm. Should I include fake quads as well?
	ArrayList<std::shared_ptr<Triangle>> adjTriangles();

	/**
	 * Classic Laplacian smooth. Of course, to be run on internal nodes only.
	 *
	 * @return the vector from the old to the new position.
	 */
	MyVector laplacianMoveVector();

	/**
	 * Classic Laplacian smooth. Of course, to be run on internal nodes only.
	 *
	 * @return the new position of node
	 */
	std::shared_ptr<Node> laplacianSmooth();

	/**
	 * Classic Laplacian smooth, but exclude the given neighbor node from the
	 * calculation. Of course, to be run on internal nodes only.
	 *
	 * @param node the node to be excluded
	 * @return the new position of node
	 */
	std::shared_ptr<Node> laplacianSmoothExclude( const std::shared_ptr<Node>& node );

	/**
	 * Run this on internal nodes (not part of the boundary or front) Does a
	 * modified length weighted Laplacian smooth.
	 *
	 * @return a new node with the smoothed position.
	 */
	std::shared_ptr<Node> modifiedLWLaplacianSmooth();

	int nrOfFrontEdges();

	/**
	 * An implementation of an algorithm described in a paper by Blacker and
	 * Stephenson.
	 *
	 * @param nJ     the other node that lies behind this node (not on the
	 *               front/boundary)
	 * @param ld     length from this to nJ
	 * @param front1 front/boundary neighbor edge to this
	 * @param front2 front/boundary neighbor edge to this
	 * @return a new node (with a smoothed positing) that can replace this node.
	 */
	std::shared_ptr<Node> blackerSmooth( const std::shared_ptr<Node>& nJ,
										 const std::shared_ptr<Edge>& front1,
										 const std::shared_ptr<Edge>& front2,
										 double ld );

	/**
	 * Performs an angular smoothness adjustment as described in the paper by
	 * Blacker and Stephenson. Assumes that this is a node that lies on the
	 * front/boundary.
	 *
	 * @param nJ the node connected to this, that lies behind the front/boundary
	 * @param f1 front/boundary neighbor edge to this
	 * @param f2 front/boundary neighbor edge to this
	 * @return a vector that should replace the edge between this and nJ
	 */
	MyVector angularSmoothnessAdjustment( const std::shared_ptr<Node>& nJ,
										  const std::shared_ptr<Edge>& f1,
										  const std::shared_ptr<Edge>& f2,
										  double ld );

	/**
	 * Test whether any of the adjacent elements has become inverted or their areas
	 * are zero.
	 *
	 * @param elements the list of elements to parse
	 * @return true if the movement of a node has caused any of it's adjacent
	 *         elements to become inverted or get an area of size zero.
	 */
	bool invertedOrZeroAreaElements( const ArrayList<std::shared_ptr<Element>>& elements );

	/**
	 * Incrementally adjust the location of the node (along a vector) until none of
	 * it's neighboring elements are inverted. Use increments of size vector
	 * component divided by 50 in each direction, unless ONE of these increments is
	 * less than a given lower limit. If so, the increments in the direction of the
	 * shortest component should be equal to that limit, while the other direction
	 * is dictated by the first, of course.
	 *
	 * @return true on success else false.
	 */
	bool incrAdjustUntilNotInvertedOrZeroArea( const std::shared_ptr<Node>& old,
											   const ArrayList<std::shared_ptr<Element>>& elements );

	/** @return true if the node is part of the boundary of the mesh. */
	bool boundaryNode();

	/**
	 * @return true if the node is part of the boundary of the mesh or a triangle.
	 */
	bool boundaryOrTriangleNode();

	/** @return true if the node is truely a part of the front. */
	bool frontNode();

	/**
	 * @param known is the front edge that is already known. (Use null if no such
	 *              edge is known.)
	 * @return a front edge found in this node's edgelist.
	 */
	std::shared_ptr<Edge> anotherFrontEdge( const std::shared_ptr<Edge>& known );

	/**
	 * @param known is the boundary edge that is already known. (Use null if no such
	 *              edge is known.)
	 * @return a boundary edge found in this node's edgelist.
	 */
	std::shared_ptr<Edge> anotherBoundaryEdge( const std::shared_ptr<Edge>& known );

	double length( const std::shared_ptr<Node>& n );

	double length( double x, double y );

	/**
	 * Determine if a node is on the line (of infinite length) that e is a part of.
	 */
	bool onLine( const std::shared_ptr<Edge>& e );

	/**
	 * Determine if a node is in a given halfplane. The method is based on the
	 * determinant as described in Schewchuk's paper.
	 *
	 * @return 1 if this Node is in the halfplane defined by Triangle t and Edge e,
	 *         0 if the Node is on Edge e, -1 if the node is not in the halfplane
	 *         defined by Triangle t and Edge e.
	 */
	int inHalfplane( const std::shared_ptr<Triangle>& t,
					 const std::shared_ptr<Edge>& e );

	// @return 1 if this Node is on the same side of Edge e as Node n is,
	// 0 if this Node is on the line that extends Edge e, and
	// -1 if this Node is on the other side of Edge e than Node n is.
	int inHalfplane( const std::shared_ptr<Edge>& e,
					 const std::shared_ptr<Node>& n );

	// @return 1 if this Node is on the same side of the line (l1, l2) as Node n is,
	// 0 if this Node is on the line that extends line (l1, l2), and
	// -1 if this Node is on the other side of line (l1, l2) than Node n is.
	int inHalfplane( const std::shared_ptr<Node>& l1, 
					 const std::shared_ptr<Node>& l2, 
					 const std::shared_ptr<Node>& n );

	/**
	 * Test to see if this Node lies in the plane bounded by the two parallel lines
	 * intersecting the Nodes of Edge e that are normal to Edge e.
	 */
	bool inBoundedPlane( const std::shared_ptr<Edge>& e );

	/**
	 * Return true if the circle intersecting the Nodes p1, p2, and p3 contains this
	 * Node in its interior. p1, p2, p3, and p4 are ccw sorted. Note that testing
	 * for convexity of the quad should not be necessary.
	 */
	bool inCircle( const std::shared_ptr<Node>& p1,
				   const std::shared_ptr<Node>& p2,
				   const std::shared_ptr<Node>& p3 );

	/**
	 * Pretending this and n has the same location, copy the edges in n's edgelist
	 * that this node doesn't already have, and put them into this node's edgeList.
	 * If this and n have any common edges, these must be removed.
	 */
	void merge( const std::shared_ptr<Node>& n );

	ArrayList<std::shared_ptr<Edge>> frontEdgeList();

	/**
	 * Parse the edgeList to look for Edge e.
	 *
	 * @return true if found, else false
	 */
	bool hasEdge( const std::shared_ptr<Edge>& e );

	/** Determine the node valence. */
	uint8_t valence();

	/** Calculate the valence pattern for this node and its neighbors. */
	void createValencePattern( const std::vector<std::shared_ptr<Node>>& ccwNodes );

	/** Calculate the valence pattern for this node and its neighbors. */
	void createValencePattern( uint8_t ccwNodesSize,
							   const std::vector<std::shared_ptr<Node>>& ccwNodes );

	/**
	 * Return # of irregular nodes in the valence pattern (nodes whose valence!= 4)
	 * Note that calcMyValencePattern() must be called before calling this method.
	 */
	int irregNeighborNodes();

	/**
	 * Compare the valence pattern of this node to the special pattern in pattern2.
	 * In pattern2, the following codes apply:<br>
	 * <ul>
	 * <li>14 means 4- (4 or less)
	 * <li>24 means 4+ (4 or more)
	 * <li>5 means 5 or more
	 * <li>0 means any number
	 * </ul>
	 * Note that the length of the patterns must be an even number. Also note that
	 * the patterns have to be aligned in a 2-byte fashion. (This means that the
	 * index into the node's pattern where they start matching have to be an even
	 * number.)
	 *
	 * @param pattern2 A valence pattern
	 * @return If they match then return the index of the valence value in the
	 *         node's pattern that corresponds to the first valence value in
	 *         pattern2, otherwise return -1. Note that calcMyValencePattern() must
	 *         be called before calling this method.
	 */
	int patternMatch( const std::vector<uint8_t>& pattern2 );

	/**
	 * Compare the valence pattern of this node to the special pattern in pattern2.
	 * Also make sure that the tagged nodes in vertexPat are vertices. (That is, the
	 * interior angles must be greater than any other interior angles around this
	 * node.) In pattern2, the following codes apply:<br>
	 * <ul>
	 * <li>14 means 4- (4 or less)
	 * <li>24 means 4+ (4 or more)
	 * <li>5 means 5 or more
	 * <li>0 means any number
	 * </ul>
	 * Note that the length of the patterns must be an even number. Also note that
	 * the patterns have to be aligned in a 2-byte fashion. (This means that the
	 * index into the node's pattern where they start matching have to be an even
	 * number.)
	 *
	 * @param pattern2 A valence pattern
	 * @return If they match then return the index of the valence value in the
	 *         node's pattern that corresponds to the first valence value in
	 *         pattern2, otherwise return -1. Note that calcMyValencePattern() must
	 *         be called before calling this method.
	 */
	int patternMatch( const std::vector<uint8_t>& pattern2,
					  const std::vector<bool>& vertexPat,
					  const std::vector<double>& angles );

	/**
	 * Confirm whether the nodes having the given interior angles have the correct
	 * vertex pattern.
	 *
	 * @param start     start index for the ang array
	 * @param ang       an array of interior angles
	 * @param vertexPat a boolean array indicating which angles are at actual
	 *                  vertices
	 * @param len       the length of the two arrays
	 * @return True if the pattern matches. Otherwise false.
	 */
	bool fitsVertexPat( uint8_t start,
						const std::vector<double>& ang,
						const std::vector<bool>& vertexPat,
						int len );

	/**
	 * Fill the angles array with the angles at the opposite nodes.
	 *
	 * @param ccwNeighbors the surrounding nodes in ccw order
	 * @param len          the length of
	 * @return an array of doubles
	 */
	std::vector<double> surroundingAngles( const std::vector<std::shared_ptr<Node>>& ccwNeighbors,
										   int len );

	/**
	 * Compare the valence pattern of this boundary node to the special pattern in
	 * pattern2. In pattern2, the following codes apply:<br>
	 * <ul>
	 * <li>14 means 4- (4 or less)
	 * <li>24 means 4+ (4 or more)
	 * <li>5 means 5 or more
	 * <li>0 means any number
	 * </ul>
	 * Note that calcMyValencePattern() must be called before calling this method.
	 *
	 * @param pattern2 A valence pattern
	 * @param bpat     a boolean pattern indicating which nodes are located on the
	 *                 boundary
	 * @return If they match then return the true, otherwise return false.
	 */
	bool boundaryPatternMatch( const std::vector<uint8_t>& pattern2,
							   const std::vector<bool>& bpat,
							   const std::vector<std::shared_ptr<Node>>& ccwNeighbors );

	/**
	 * Compare the valence pattern of this internal node to the special pattern in
	 * pattern2. The boundary pattern must also fit. In pattern2, the following
	 * codes apply:<br>
	 * <ul>
	 * <li>14 means 4- (4 or less)
	 * <li>24 means 4+ (4 or more)
	 * <li>5 means 5 or more
	 * <li>0 means any number
	 * </ul>
	 * Note that calcMyValencePattern() must be called before calling this method.
	 *
	 * @param pattern2     A valence pattern
	 * @param bpat         a boolean pattern indicating which nodes are located on
	 *                     the boundary
	 * @param ccwNeighbors the neighbor nodes in ccw order
	 * @return If they match then return the true, otherwise return false.
	 */
	int boundaryPatternMatchSpecial( const std::vector<uint8_t>& pattern2,
									 const std::vector<bool>& bpat,
									 const std::vector<std::shared_ptr<Node>>& ccwNeighbors );

	std::shared_ptr<Edge> commonEdge( const std::shared_ptr<Node>& n );

	bool replaceWithStdMesh();

	/**
	 * Give a string representation of the node.
	 *
	 * @return a string representation of the node.
	 */
	std::string descr();

	std::string valDescr();

	/** Output a string representation of the node. */
	void printMe();

	std::string toString();
};