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

	///**
	// * @return the number of quad neighbors sharing an edge with this quad at node
	// *         n. This quad is not counted. Values are 0, 1, or 2.
	// */
	//public int nrOfQuadsSharingAnEdgeAt( Node n )
	//{
	//	int count = 0;

	//	if ( edgeList[left].hasNode( n ) )
	//	{
	//		if ( neighbor( edgeList[left] ) instanceof Quad )
	//		{
	//			count++;
	//		}
	//		if ( edgeList[base].hasNode( n ) )
	//		{
	//			if ( neighbor( edgeList[base] ) instanceof Quad )
	//			{
	//				count++;
	//			}
	//		}
	//		else if ( neighbor( edgeList[top] ) instanceof Quad )
	//		{
	//			count++;
	//		}
	//		return count;
	//	}
	//	else if ( edgeList[right].hasNode( n ) )
	//	{
	//		if ( neighbor( edgeList[right] ) instanceof Quad )
	//		{
	//			count++;
	//		}
	//		if ( edgeList[base].hasNode( n ) )
	//		{
	//			if ( neighbor( edgeList[base] ) instanceof Quad )
	//			{
	//				count++;
	//			}
	//		}
	//		else if ( neighbor( edgeList[top] ) instanceof Quad )
	//		{
	//			count++;
	//		}

	//		return count;
	//	}
	//	return count;
	//}

	///**
	// * Update the distortion metric according to the paper "An approach to Combined
	// * Laplacian and Optimization-Based Smoothing for Triangular, Quadrilateral and
	// * Quad-Dominant Meshes" by by Cannan, Tristano, and Staten
	// *
	// * return negative values for inverted quadrilaterals, else positive.
	// *         Equilateral quadrilaterals should return the maximum value of 1.
	// */
	// //
	// // This is a simple sketch of the quadrilateral with nodes and divided
	// // into four triangles:
	// //
	// // n3__________n4
	// // |\ /|
	// // | \ t4 / |
	// // | \ / |
	// // | t2 X t3 |
	// // | / \ |
	// // | / t1 \ |
	// // |/___________\|
	// // n1 n2
	// //
	// // Also, I tried to sketch the case where the quad has an angle > than 180
	// // degrees
	// // Note that t3 is part of t1 and that t4 is part of t2 in the sketch.
	// // t3 and t4 are inverted.
	// //
	// // n3
	// // |\ \
 //	// | \ \
 //	// | \t4 X
	// // |t2 \ / \
 //	// | /\ t3 \
 //	// | /t1 ---__n2
	// // |/______-----=
	// // n1
	//@Override
	//	public void updateDistortionMetric()
	//{
	//	Msg.debug( "Entering Quad.updateDistortionMetric()" );

	//	if ( isFake )
	//	{
	//		double AB = edgeList[base].len, CB = edgeList[left].len, CA = edgeList[right].len;

	//		Node a = edgeList[base].commonNode( edgeList[right] ), b = edgeList[base].commonNode( edgeList[left] ), c = edgeList[left].commonNode( edgeList[right] );
	//		MyVector vCA = new MyVector( c, a ), vCB = new MyVector( c, b );

	//		double temp = sqrt3x2 * Math.abs( vCA.cross( vCB ) ) / (CA * CA + AB * AB + CB * CB);
	//		if ( inverted() )
	//		{
	//			distortionMetric = -temp;
	//		}
	//		else
	//		{
	//			distortionMetric = temp;
	//		}

	//		Msg.debug( "Leaving Quad.updateDistortionMetric(): " + distortionMetric );
	//		return;
	//	}

	//	Node n1 = edgeList[base].leftNode;
	//	Node n2 = edgeList[base].rightNode;
	//	Node n3 = edgeList[left].otherNode( n1 );
	//	Node n4 = edgeList[right].otherNode( n2 );

	//	// The two diagonals
	//	Edge e1 = new Edge( n1, n4 );
	//	Edge e2 = new Edge( n2, n3 );

	//	// The four triangles
	//	Triangle t1 = new Triangle( edgeList[base], edgeList[left], e2 );
	//	Triangle t2 = new Triangle( edgeList[base], e1, edgeList[right] );
	//	Triangle t3 = new Triangle( edgeList[top], edgeList[right], e2 );
	//	Triangle t4 = new Triangle( edgeList[top], e1, edgeList[left] );

	//	// Place the firstNodes correctly
	//	t1.firstNode = firstNode;
	//	if ( firstNode == n1 )
	//	{
	//		t2.firstNode = n1;
	//		t3.firstNode = n4;
	//		t4.firstNode = n4;
	//	}
	//	else
	//	{
	//		t2.firstNode = n2;
	//		t3.firstNode = n3;
	//		t4.firstNode = n3;
	//	}

	//	// Compute and get alpha values for each triangle
	//	t1.updateDistortionMetric( 4.0 );
	//	t2.updateDistortionMetric( 4.0 );
	//	t3.updateDistortionMetric( 4.0 );
	//	t4.updateDistortionMetric( 4.0 );

	//	double alpha1 = t1.distortionMetric, alpha2 = t2.distortionMetric, alpha3 = t3.distortionMetric, alpha4 = t4.distortionMetric;

	//	int invCount = 0;
	//	if ( alpha1 < 0 )
	//	{
	//		invCount++;
	//	}
	//	if ( alpha2 < 0 )
	//	{
	//		invCount++;
	//	}
	//	if ( alpha3 < 0 )
	//	{
	//		invCount++;
	//	}
	//	if ( alpha4 < 0 )
	//	{
	//		invCount++;
	//	}

	//	double temp12 = Math.min( alpha1, alpha2 );
	//	double temp34 = Math.min( alpha3, alpha4 );
	//	double alphaMin = Math.min( temp12, temp34 );
	//	double negval = 0;

	//	if ( invCount >= 3 )
	//	{
	//		if ( invCount == 3 )
	//		{
	//			negval = 2.0;
	//		}
	//		else
	//		{
	//			negval = 3.0;
	//		}
	//	}
	//	else if ( ang[0] < DEG_6 || ang[1] < DEG_6 || ang[2] < DEG_6 || ang[3] < DEG_6 || coincidentNodes( n1, n2, n3, n4 ) || invCount == 2 )
	//	{
	//		negval = 1.0;
	//	}

	//	distortionMetric = alphaMin - negval;
	//	Msg.debug( "Leaving Quad.updateDistortionMetric(): " + distortionMetric );
	//}

	///** Test whether any nodes of the quad are coincident. */
	//private boolean coincidentNodes( Node n1, Node n2, Node n3, Node n4 )
	//{
	//	Msg.debug( "Entering Quad.coincidentNodes(..)" );
	//	double x12diff = n2.x - n1.x;
	//	double y12diff = n2.y - n1.y;
	//	double x13diff = n3.x - n1.x;
	//	double y13diff = n3.y - n1.y;
	//	double x14diff = n4.x - n1.x;
	//	double y14diff = n4.y - n1.y;

	//	double x23diff = n3.x - n2.x;
	//	double y23diff = n3.y - n2.y;
	//	double x24diff = n4.x - n2.x;
	//	double y24diff = n4.y - n2.y;

	//	double x34diff = n4.x - n3.x;
	//	double y34diff = n4.y - n3.y;

	//	// Using Pythagoras: hyp^2= kat1^2 + kat2^2
	//	double l12 = Math.sqrt( x12diff * x12diff + y12diff * y12diff );
	//	double l13 = Math.sqrt( x13diff * x13diff + y13diff * y13diff );
	//	double l14 = Math.sqrt( x14diff * x14diff + y14diff * y14diff );
	//	double l23 = Math.sqrt( x23diff * x23diff + y23diff * y23diff );
	//	double l24 = Math.sqrt( x24diff * x24diff + y24diff * y24diff );
	//	double l34 = Math.sqrt( x34diff * x34diff + y34diff * y34diff );

	//	if ( l12 < COINCTOL || l13 < COINCTOL || l14 < COINCTOL || l23 < COINCTOL || l24 < COINCTOL || l34 < COINCTOL )
	//	{
	//		Msg.debug( "Leaving Quad.coincidentNodes(..), returning true" );
	//		return true;
	//	}
	//	else
	//	{
	//		Msg.debug( "Leaving Quad.coincidentNodes(..), returning false" );
	//		return false;
	//	}
	//}

	///** @return the size of the largest interior angle */
	//@Override
	//	public double largestAngle()
	//{
	//	double cand = ang[0];
	//	if ( ang[1] > cand )
	//	{
	//		cand = ang[1];
	//	}
	//	if ( ang[2] > cand )
	//	{
	//		cand = ang[2];
	//	}
	//	if ( ang[3] > cand )
	//	{
	//		cand = ang[3];
	//	}
	//	return cand;
	//}

	///** @return the node at the largest interior angle */
	//@Override
	//	public Node nodeAtLargestAngle()
	//{
	//	Node candNode = edgeList[base].leftNode;
	//	double cand = ang[0];

	//	if ( ang[1] > cand )
	//	{
	//		candNode = edgeList[base].rightNode;
	//		cand = ang[1];
	//	}
	//	if ( ang[2] > cand )
	//	{
	//		candNode = edgeList[left].otherNode( edgeList[base].leftNode );
	//		cand = ang[2];
	//	}
	//	if ( ang[3] > cand )
	//	{
	//		candNode = edgeList[right].otherNode( edgeList[base].rightNode );
	//	}
	//	return candNode;
	//}

	///** @return the length of the longest Edge in the quad */
	//@Override
	//	public double longestEdgeLength()
	//{
	//	double t1 = Math.max( edgeList[base].len, edgeList[left].len );
	//	double t2 = Math.max( t1, edgeList[right].len );
	//	return Math.max( t2, edgeList[top].len );
	//}

	///**
	// * @param first a triangle that is located inside the quad
	// * @return a list of triangles contained within the four edges of this quad.
	// */
	//public ArrayList trianglesContained( Triangle first )
	//{
	//	Msg.debug( "Entering trianglesContained(..)" );
	//	ArrayList tris = new ArrayList();
	//	Element neighbor;
	//	Triangle cur;
	//	Edge e;

	//	tris.add( first );
	//	for ( int j = 0; j < tris.size(); j++ )
	//	{
	//		cur = (Triangle)tris.get( j );
	//		Msg.debug( "...parsing triangle " + cur.descr() );

	//		for ( int i = 0; i < 3; i++ )
	//		{
	//			e = cur.edgeList[i];
	//			if ( !hasEdge( e ) )
	//			{
	//				neighbor = cur.neighbor( e );
	//				if ( neighbor != null && !tris.contains( neighbor ) )
	//				{
	//					tris.add( neighbor );
	//				}
	//			}
	//		}
	//	}
	//	Msg.debug( "Leaving trianglesContained(..)" );
	//	return tris;
	//}

	///**
	// * Test whether the quad contains a hole.
	// *
	// * @param tris the interior triangles
	// * @return true if there are one or more holes present within the four edges
	// *         defining the quad.
	// */
	//public boolean containsHole( ArrayList tris )
	//{
	//	Triangle t;

	//	if ( tris.size() == 0 )
	//	{
	//		return true; // Corresponds to a quad defined on a quad-shaped hole
	//	}

	//	for ( Object element : tris )
	//	{
	//		t = (Triangle)element;
	//		if ( t.edgeList[0].boundaryEdge() && !hasEdge( t.edgeList[0] ) || t.edgeList[1].boundaryEdge() && !hasEdge( t.edgeList[1] )
	//			 || t.edgeList[2].boundaryEdge() && !hasEdge( t.edgeList[2] ) )
	//		{
	//			return true;
	//		}
	//	}
	//	return false;
	//}

	///** Set the color of the edges to green. */
	//@Override
	//	public void markEdgesLegal()
	//{
	//	edgeList[base].color = java.awt.Color.green;
	//	edgeList[left].color = java.awt.Color.green;
	//	edgeList[right].color = java.awt.Color.green;
	//	edgeList[top].color = java.awt.Color.green;
	//}

	///** Set the color of the edges to red. */
	//@Override
	//	public void markEdgesIllegal()
	//{
	//	edgeList[base].color = java.awt.Color.red;
	//	edgeList[left].color = java.awt.Color.red;
	//	edgeList[right].color = java.awt.Color.red;
	//	edgeList[top].color = java.awt.Color.red;
	//}

	///**
	// * Give a string representation of the quad.
	// *
	// * @return a string representation of the quad.
	// */
	//@Override
	//	public String descr()
	//{
	//	Node node1, node2, node3, node4;
	//	node1 = edgeList[base].leftNode;
	//	node2 = edgeList[base].rightNode;
	//	node3 = edgeList[left].otherNode( node1 );
	//	node4 = edgeList[right].otherNode( node2 );

	//	return node1.descr() + ", " + node2.descr() + ", " + node3.descr() + ", " + node4.descr();
	//}

	///** Output a string representation of the quad. */
	//@Override
	//	public void printMe()
	//{
	//	System.out.println( descr() + ", inverted(): " + inverted() + ", ang[0]: " + Math.toDegrees( ang[0] ) + ", ang[1]: " + Math.toDegrees( ang[1] )
	//						+ ", ang[2]: " + Math.toDegrees( ang[2] ) + ", ang[3]: " + Math.toDegrees( ang[3] ) + ", firstNode is " + firstNode.descr() );
	//}

	bool isFake = false;
};
