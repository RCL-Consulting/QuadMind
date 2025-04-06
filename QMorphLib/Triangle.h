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

	std::shared_ptr<Edge> nextCWEdge( const std::shared_ptr<Edge>& e1 );
	//{
	//	Node e1commone2, e1commone3;
	//	Edge e2, e3;
	//	MyVector v1Forv2, v1Forv3, v2, v3;
	//	if ( e1 == edgeList[0] )
	//	{
	//		e2 = edgeList[1];
	//		e3 = edgeList[2];
	//	}
	//	else if ( e1 == edgeList[1] )
	//	{
	//		e2 = edgeList[0];
	//		e3 = edgeList[2];
	//	}
	//	else if ( e1 == edgeList[2] )
	//	{
	//		e2 = edgeList[0];
	//		e3 = edgeList[1];
	//	}
	//	else
	//	{
	//		Msg.error( "Edge " + e1.descr() + " is not part of triangle" );
	//		return null;
	//	}

	//	e1commone2 = e1.commonNode( e2 );
	//	e1commone3 = e1.commonNode( e3 );

	//	v2 = new MyVector( e1commone2, e2.otherNode( e1commone2 ) );
	//	v3 = new MyVector( e1commone3, e3.otherNode( e1commone3 ) );
	//	v1Forv2 = new MyVector( e1commone2, e1.otherNode( e1commone2 ) );
	//	v1Forv3 = new MyVector( e1commone3, e1.otherNode( e1commone3 ) );

	//	// Positive angles between e1 and each of the other two edges:
	//	double ang1, ang2;
	//	ang1 = ang[angleIndex( e1, e2 )];
	//	ang2 = ang[angleIndex( e1, e3 )];

	//	// Transform into true angles:
	//	e1commone2 = e1.commonNode( e2 );
	//	e1commone3 = e1.commonNode( e3 );

	//	v2 = new MyVector( e1commone2, e2.otherNode( e1commone2 ) );
	//	v3 = new MyVector( e1commone3, e3.otherNode( e1commone3 ) );
	//	v1Forv2 = new MyVector( e1commone2, e1.otherNode( e1commone2 ) );
	//	v1Forv3 = new MyVector( e1commone3, e1.otherNode( e1commone3 ) );

	//	if ( v2.isCWto( v1Forv2 ) )
	//	{
	//		ang1 += Math.PI;
	//	}
	//	if ( v3.isCWto( v1Forv3 ) )
	//	{
	//		ang2 += Math.PI;
	//	}

	//	if ( ang2 > ang1 )
	//	{
	//		return e2;
	//	}
	//	else
	//	{
	//		return e3;
	//	}
	//}

	//// We simply check that the nodes of the element are not collinear.
	//@Override
	//	public boolean areaLargerThan0()
	//{
	//	Node na = edgeList[0].leftNode;
	//	Node nb = edgeList[0].rightNode;
	//	Node nc = oppositeOfEdge( edgeList[0] );

	//	if ( cross( na, nc, nb, nc ) != 0 )
	//	{ // The cross product nanc x nbnc
	//		return true;
	//	}
	//	else
	//	{ // The cross product nanc x nbnc
	//		return false;
	//	}

	//	/*
	//	 * Node n= oppositeOfEdge(edgeList[0]); if (n.onLine(edgeList[0])) return false;
	//	 * else return true;
	//	 */
	//}

	//// Check if the old pos and the new pos of the node are on different sides
	//// of the nodes opposite edge.
	//public boolean inverted( Node oldN, Node newN )
	//{
	//	Edge e = oppositeOfNode( newN );
	//	// Check with edge e:
	//	double oldN_e_det = (e.leftNode.x - oldN.x) * (e.rightNode.y - oldN.y) - (e.leftNode.y - oldN.y) * (e.rightNode.x - oldN.x);
	//	double newN_e_det = (e.leftNode.x - newN.x) * (e.rightNode.y - newN.y) - (e.leftNode.y - newN.y) * (e.rightNode.x - newN.x);

	//	// If different sign, or 0, they are inverted:
	//	if ( oldN_e_det >= 0 )
	//	{
	//		if ( newN_e_det <= 0 || oldN_e_det == 0 )
	//		{
	//			Msg.debug( "Triangle.inverted(..): Triangle " + descr() + " is inverted" );
	//			return true;
	//		}
	//	}
	//	else if ( newN_e_det >= 0 )
	//	{
	//		Msg.debug( "Triangle.inverted(..): Triangle " + descr() + " is inverted" );
	//		return true;
	//	}
	//	return false;
	//}

	//// Return true if the triangle has become inverted
	//@Override
	//	public boolean inverted()
	//{
	//	Node a, b, c;
	//	a = firstNode;
	//	b = edgeList[0].otherNode( a );
	//	c = edgeList[1].rightNode;
	//	if ( c == a || c == b )
	//	{
	//		c = edgeList[1].leftNode;
	//	}

	//	if ( cross( a, c, b, c ) < 0 )
	//	{
	//		// if (!ac.newcross(bc))
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	//// Return true if the triangle has become inverted or the area is zero
	//@Override
	//	public boolean invertedOrZeroArea()
	//{
	//	Node a, b, c;
	//	a = firstNode;
	//	b = edgeList[0].otherNode( a );
	//	c = edgeList[1].rightNode;
	//	if ( c == a || c == b )
	//	{
	//		c = edgeList[1].leftNode;
	//	}

	//	if ( cross( a, c, b, c ) <= 0 )
	//	{
	//		// if (!ac.newcross(bc))
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	//// Return true if the triangle area is zero.
	//public boolean zeroArea()
	//{
	//	Node a, b, c;
	//	a = firstNode;
	//	b = edgeList[0].otherNode( a );
	//	c = edgeList[1].rightNode;
	//	if ( c == a || c == b )
	//	{
	//		c = edgeList[1].leftNode;
	//	}

	//	if ( cross( a, c, b, c ) == 0 )
	//	{
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	//// Triangles don't have concavities, so return false.
	//@Override
	//	public boolean concavityAt( Node n )
	//{
	//	return false;
	//}

	//@Override
	//	public void replaceEdge( Edge e, Edge replacement )
	//{
	//	edgeList[indexOf( e )] = replacement;
	//}

	//// Returns an edge that is on the boundary. Returns null if not found.
	//public Edge getBoundaryEdge()
	//{
	//	if ( edgeList[0].element2 == null )
	//	{
	//		return edgeList[0];
	//	}
	//	else if ( edgeList[1].element2 == null )
	//	{
	//		return edgeList[1];
	//	}
	//	else if ( edgeList[2].element2 == null )
	//	{
	//		return edgeList[2];
	//	}
	//	else
	//	{
	//		return null;
	//	}
	//}

	///**
	// * Update the distortion metric according to the article "An approach to
	// * Combined Laplacian and Optimization-Based Smoothing for Triangular,
	// * Quadrilateral and Quad-Dominant Meshes" by by Cannan, Tristano, and Staten.
	// *
	// * return negative values for inverted triangles, else positive. Equilateral
	// *         triangles will return the maximum value of 1.
	// */
	//@Override
	//	public void updateDistortionMetric()
	//{
	//	updateDistortionMetric( sqrt3x2 );
	//}

	///**
	// * See updateDistortionMetric().
	// *
	// * return negative values for inverted triangles, else positive.
	// */
	//public void updateDistortionMetric( double factor )
	//{
	//	Msg.debug( "Entering Triangle.updateDistortionMetric(..)" );
	//	double AB = edgeList[0].len, CB = edgeList[1].len, CA = edgeList[2].len;
	//	Node a = edgeList[2].commonNode( edgeList[0] ), b = edgeList[0].commonNode( edgeList[1] ), c = edgeList[2].commonNode( edgeList[1] );
	//	MyVector vCA = new MyVector( c, a ), vCB = new MyVector( c, b );

	//	double temp = factor * Math.abs( vCA.cross( vCB ) ) / (CA * CA + AB * AB + CB * CB);
	//	if ( inverted() )
	//	{
	//		distortionMetric = -temp;
	//	}
	//	else
	//	{
	//		distortionMetric = temp;
	//	}
	//	Msg.debug( "Leaving Triangle.updateDistortionMetric(..): " + distortionMetric );
	//}

	//// Return the size of the largest angle
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
	//	return cand;
	//}

	//// Return the node at the largest interior angle
	//@Override
	//	public Node nodeAtLargestAngle()
	//{
	//	Node candNode = edgeList[0].leftNode;
	//	double cand = ang[angleIndex( candNode )], temp;

	//	temp = ang[angleIndex( edgeList[0].rightNode )];
	//	if ( temp > cand )
	//	{
	//		candNode = edgeList[0].rightNode;
	//		cand = temp;
	//	}
	//	temp = ang[angleIndex( oppositeOfEdge( edgeList[0] ) )];
	//	if ( temp > cand )
	//	{
	//		candNode = oppositeOfEdge( edgeList[0] );
	//	}
	//	return candNode;
	//}

	//// Return the length of the longest Edge
	//@Override
	//	public double longestEdgeLength()
	//{
	//	double temp = Math.max( edgeList[0].len, edgeList[1].len );
	//	return Math.max( temp, edgeList[2].len );
	//}

	//// Return the longest Edge
	//public Edge longestEdge()
	//{
	//	Edge temp;
	//	if ( edgeList[0].len > edgeList[1].len )
	//	{
	//		temp = edgeList[0];
	//	}
	//	else
	//	{
	//		temp = edgeList[1];
	//	}

	//	if ( edgeList[2].len > temp.len )
	//	{
	//		return edgeList[2];
	//	}
	//	else
	//	{
	//		return temp;
	//	}
	//}

	//// Set the color of the edges to green.
	//@Override
	//	public void markEdgesLegal()
	//{
	//	edgeList[0].color = java.awt.Color.green;
	//	edgeList[1].color = java.awt.Color.green;
	//	edgeList[2].color = java.awt.Color.green;
	//}

	//// Set the color of the edges to red.
	//@Override
	//	public void markEdgesIllegal()
	//{
	//	edgeList[0].color = java.awt.Color.red;
	//	edgeList[1].color = java.awt.Color.red;
	//	edgeList[2].color = java.awt.Color.red;
	//}

	//@Override
	//	public String descr()
	//{
	//	Node node1, node2, node3;
	//	node1 = edgeList[0].leftNode;
	//	node2 = edgeList[0].rightNode;
	//	node3 = edgeList[1].rightNode;
	//	if ( node3 == node1 || node3 == node2 )
	//	{
	//		node3 = edgeList[1].leftNode;
	//	}

	//	return node1.descr() + ", " + node2.descr() + ", " + node3.descr();
	//}

	//@Override
	//	public void printMe()
	//{
	//	if ( inverted() )
	//	{
	//		System.out.println( descr() + ", inverted" );
	//	}
	//	else
	//	{
	//		System.out.println( descr() + ", not inverted" );
	//	}

	//}

	//@Override
	//	public String toString()
	//{
	//	return descr();
	//}

};