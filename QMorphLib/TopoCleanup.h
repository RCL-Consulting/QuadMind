#pragma once

#include "GeomBasics.h"

#include "Dart.h"
#include "ArrayList.h"

#include <memory>

// ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ----
/**
 * This class constitutes a simple implementation of the cleanup process as
 * outlined by Paul Kinney: "CleanUp: Improving Quadrilateral Finite Element
 * Meshes" (1997). Please note that this is not a complete and accurate
 * implementation, as it had to be somewhat adapted to work with the Q-Morph
 * implementation. E.g. the size cleanup is not implemented. Neither is cleaning
 * up bowties and the mesh topology inspection. Furthermore, the number of
 * cleanup patterns is limited to those described in Kinney's paper.<br>
 * <br>
 * TODO<br>
 * Draw all cases and test them. The ones running correctly: Connectivity -
 * stdCase1a - stdCase1b - stdCase2a - stdCase2b - stdCase3a - stdCase3b -
 * case1a og case1b - case2 - case3 - case4 - case5
 *
 * Boundary: - diamond - case1 - case2 - case3 - case4 - boundary angle > 150
 * degrees with one and two row transition
 *
 * Shape: - case1 - case2
 *
 * TODO: All good, but: - Why do I now see so little effect from class
 * GlobalSmooth???
 *
 * @author Karl Erik Levik
 *
 */
 // ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ----

class Element;
class Node;

class TopoCleanup :
	public GeomBasics,
	public std::enable_shared_from_this<TopoCleanup>
{
private:
	Dart d;
	/** A dart used when traversing the mesh in cleanup operations. */
	int count = 0;
	ArrayList<std::shared_ptr<Element>> deleteList;
	ArrayList<std::shared_ptr<Node>> nodes;

public:
	/** Initialize the object */
	void init();

	/** Main loop for global topological clean-up */
	void run();

private:
	bool elimChevsFinished = false;
	bool connCleanupFinished = false;
	bool boundaryCleanupFinished = false;
	bool shapeCleanupFinished = false;

	int passNum = 0;

public:
	/** Method for stepping through the implementation one step at the time. */
	void step() override;

private:
	/** Initial pass to detect and cleanup chevrons. */
	void elimChevsStep();

	/**
	 * The chevron is deleted along with one of its neighbors. A new node is created
	 * and the deleted quads are replaced with three new ones surrounding the new
	 * node. The neighbor chosen for deletion is the one that, when replaced by the
	 * new quads, yields the optimal node valences.
	 *
	 * @param q the chevron to be eliminated
	 */
	void eliminateChevron( const std::shared_ptr<Quad>& q );

	/**
	 * Combine with neighbor and fill with "fill_3" as defined in the paper by
	 * P.Kinney Note that the method doesn't remove q and its neighbor from
	 * elementList.
	 *
	 * @param q    the first quad
	 * @param e    the edge which is adjacent to both q and its neighbor
	 * @param n    a node belonging to q, e, and one of the three new edges to be
	 *             created
	 * @param safe boolean indicating whether a safe pos must be attempted for the
	 *             new node
	 */
	std::shared_ptr<Dart> fill3( const std::shared_ptr<Quad>& q,
								 const std::shared_ptr<Edge>& e,
								 const std::shared_ptr<Node>& n,
								 bool safe );
	/**
	 * Combine with neighbor and fill with "fill_4" as defined in paper by P.Kinney
	 * Note that the method doesn't remove q and its neighbor from elementList.
	 *
	 * @param q  the first of the two quads to be combined
	 * @param e  the common edge of q and the second quad
	 * @param n2 one of the nodes of edge e and whose opposite node in q will not
	 *           get connected to any new edge.
	 */
	std::shared_ptr<Dart> fill4( const std::shared_ptr<Quad>& q,
								 const std::shared_ptr<Edge>& e,
								 const std::shared_ptr<Node>& n2 );

	/**
	 * Replace the specified surrounding mesh elements (quads) with some other
	 * specified quads. This is accomplished by applying a composition of alpha
	 * iterators and mesh modify codes.
	 *
	 * @param startDart the dart where the composition is to start.
	 * @param fillPat   a composition of alpha iterators and mesh modification codes
	 *                  that moves around on the existing mesh and modifies it
	 *                  according to the codes in the composition. The format of the
	 *                  byte array is:<br>
	 *                  [Total number of bytes in array], [number of quads to be
	 *                  created], [number of quads to be deleted], [iterators and
	 *                  modify codes]. In addition to the normal alpha iterator
	 *                  codes 0,1,2, we have mesh modify codes:
	 *                  <ul>
	 *                  <li>Code 0 for alpha iterator 0
	 *                  <li>Code 1 for alpha iterator 1
	 *                  <li>Code 2 for alpha iterator 2
	 *                  <li>Code 3 for closing the current quad, new pos of cur.
	 *                  node at the opposite node
	 *                  <li>Code 4 for closing the current quad, new pos of cur.
	 *                  node midway to oppos. node
	 *                  <li>Code 5 for filling cur. quad and neighbour with fill_3
	 *                  <li>Code 6 for filling cur. quad and neighbour with fill_4
	 *                  <li>Code 7 for splitting cur. quad into two new quads along
	 *                  diagonal from cur. node
	 *                  <li>Code 8 for switching cur. edge clockwise
	 *                  <li>Code 9 for switching cur. edge counter-clockwise
	 */
	void applyComposition( const std::shared_ptr<Dart>& startDart,
						   const std::vector<uint8_t>& fillPat );

	/*
	 * The different cases: Their valence patterns, vertex patterns, boundary
	 * patterns and compositions of mesh modification codes.
	 *
	 * In the valence patterns, the following codes apply: 4- (code 14) means a
	 * valence of 4 or less 4+ (code 24) means a valence of 4 or more 5 means a
	 * valence of 5 or more 0 means the valence is ignored and unchanged and is
	 * usually drawn as valence 4 1st value in pattern is total length of pattern
	 * 2nd value is the valence of the central node The rest of the valences then
	 * follow in ccw order around the central node
	 */

	 /* The connectivity cases */
	inline static std::vector<uint8_t> stdCase1 = { 12, 5, 24, 3, 24, 3, 4, 0, 4, 0, 4, 3 };
	inline static std::vector<bool> stdVertexCase1 = { true, false, true, false, false, true, false, true, false, false }; // ok...
	inline static std::vector<uint8_t> stdComp1 = { 4, 5, 1, 9 };
	inline static std::vector<uint8_t> stdCase2a = { 14, 6, 14, 24, 4, 3, 4, 24, 14, 3, 24, 3, 24, 3 };
	inline static std::vector<bool> stdVertexCase2a = { false, true, false, false, false, true, false, false, true, false, true, false }; // ok...
	inline static std::vector<uint8_t> stdComp2a = { 22, 2, 1, 5, 2, 1, 8, 2, 1, 0, 2, 1, 0, 5, 0, 2, 1, 2, 0, 1, 9, 5 };
	inline static std::vector<uint8_t> stdCase2b = { 14, 6, 24, 3, 4, 3, 24, 3, 4, 0, 4, 0, 4, 3 };
	inline static std::vector<bool> stdVertexCase2b = { true, false, false, false, true, false, false, true, false, true, false, false }; // ok...
	inline static std::vector<uint8_t> stdComp2b = { 9, 5, 1, 2, 1, 5, 1, 0, 5 };
	inline static std::vector<uint8_t> stdCase3a = { 12, 5, 24, 3, 4, 0, 4, 0, 4, 0, 4, 3 };
	inline static std::vector<bool> stdVertexCase3a = { true, false, false, true, false, true, false, true, false, false }; // ok...
	inline static std::vector<uint8_t> stdComp3a = { 2, 5 };
	inline static std::vector<uint8_t> stdCase3b = { 12, 5, 24, 0, 4, 3, 24, 3, 24, 3, 24, 3 };
	inline static std::vector<bool> stdVertexCase3b = { false, true, false, false, true, false, true, false, true, false }; // ok...
	inline static std::vector<uint8_t> stdComp3b = { 18, 1, 2, 1, 0, 8, 5, 0, 1, 2, 0, 1, 9, 2, 1, 0, 1, 9 };
	inline static std::vector<uint8_t> case1a = { 12, 5, 3, 4, 4, 3, 0, 0, 0, 0, 0, 0 }; // mirror of case1b
	inline static std::vector<uint8_t> comp1a = { 3, 1, 9 };
	inline static std::vector<uint8_t> case1b = { 12, 5, 4, 4, 3, 0, 0, 0, 0, 0, 0, 3 }; // mirror of case1a
	inline static std::vector<uint8_t> comp1b = { 2, 8 };
	inline static std::vector<uint8_t> case2 = { 12, 5, 3, 4, 24, 4, 3, 0, 0, 0, 0, 0 };
	inline static std::vector<uint8_t> comp2 = { 4, 1, 0, 5 };
	inline static std::vector<uint8_t> case3 = { 8, 3, 24, 3, 24, 0, 0, 0 };
	inline static std::vector<bool> vertexCase3 = { false, false, false, true, false, true };
	inline static std::vector<bool> ipat3 = { true, true, true, false, false, false };
	inline static std::vector<uint8_t> comp3 = { 2, 4 };
	inline static std::vector<uint8_t> case4 = { 8, 3, 3, 5, 4, 5, 4, 4 };
	inline static std::vector<bool> ipat4 = { true, false, false, false, false, false };
	inline static std::vector<uint8_t> comp4 = { 5, /*Consider 4 instead of 3...nah, 3 is best here*/ 0, 3, 0, 3 };
	inline static std::vector<uint8_t> case5 = { 10, 4, 3, 4, 4, 3, 4, 4, 4, 5 };
	inline static std::vector<bool> ipat5 = { true, true, true, true, true, false, false, false };
	inline static std::vector<uint8_t> comp5 = { 13, /*Consider 4 instead of 3...*/	4, 0, 3, 2, 0, 1, 0, 2, 1, 4, 0, 3 };

	/* The boundary cases */
	inline static std::vector<uint8_t> bcase1a = { 9, 5, 4, 3, 5, 4, 3, 4, 4 };
	inline static std::vector<bool> bpat1a = { true, true, false, true, true, false, false, true };
	inline static std::vector<uint8_t> bcomp1a = { 3, 1, 8 };
	inline static std::vector<uint8_t> bcase1b = { 9, 5, 4, 24, 3, 4, 5, 3, 4 };
	inline static std::vector<bool> bpat1b = { true, true, false, false, true, true, false, true };
	inline static std::vector<uint8_t> bcomp1b = { 5, 1, 2, 1, 9 };
	inline static std::vector<uint8_t> bcase2a = { 7, 4, 4, 3, 5, 4, 4 };
	inline static std::vector<bool> bpat2a = { true, true, false, true, true, true };
	inline static std::vector<uint8_t> bcomp2a = { 3, 1, 5 };
	inline static std::vector<uint8_t> bcase2b = { 7, 4, 4, 4, 5, 3, 4 };
	inline static std::vector<bool> bpat2b = { true, true, true, true, false, true };
	inline static std::vector<uint8_t> bcomp2b = { 3, 1, 5 };

	inline static std::vector<uint8_t> bcase3 = { 10, 4, 5, 4, 3, 4, 5, 4, 3, 4 };
	inline static std::vector<bool> bpat3 = { false, true, false, false, true, true, false, false, true };
	inline static std::vector<uint8_t> bcomp3 = { 12, 8, 2, 0, 1, 0, 2, 3, 1, 0, 2, 3 };
	inline static std::vector<uint8_t> bcase4 = { 9, 5, 4, 3, 5, 3, 5, 3, 4 };
	inline static std::vector<bool> bpat4 = { true, true, false, true, true, true, false, true };
	inline static std::vector<uint8_t> bcomp4 = { 7, 1, 2, 1, 5, 1, 8 };

	/** Perform one more step of connectivity cleanup. */
	void connCleanupStep();

	/** Method to confirm whether marked nodes are truely internal . */
	bool internalNodes( const std::vector<bool>& ipat,
						const std::vector<std::shared_ptr<Node>>& ccwNodes,
						int index, int len );

	void addNodes( const std::vector<std::shared_ptr<Node>>& arrayOfNodes, int len );

	bool bcaseTriQFin = false;
	bool bcaseValPat1Fin = false;
	bool bcaseValPat2Fin = false;
	bool bcaseValPat3Fin = false;
	bool bcaseValPat4Fin = false;
	bool bcaseDiamondFin = false;

	/** Perform one more steps of boundary cleanup. */
	void boundaryCleanupStep();

	bool shape1stTypeFin = false;

	/** The shape cleanup */
	void shapeCleanupStep();

	/**
	 * Return the dart with node c, the edge connecting c and the node at pos. i in
	 * neighbors, and the quad with that edge and node at pos. i+1
	 *
	 * @param c         the central node
	 * @param neighbors array of neighboring nodes to c
	 * @param i         index into neighbors
	 */
	std::shared_ptr<Dart> getDartAt( const std::shared_ptr<Node>& c,
									 const std::vector<std::shared_ptr<Node>>& neighbors,
									 int i );

//	/**
//	 * Collapse a quad by joining two and two of its consecutive edges.
//	 *
//	 * @param q        the quad to be collapsed
//	 * @param e1       an edge of q that has the node nK
//	 * @param nK       the node that is to be joined with its opposite node in q
//	 * @param centroid boolean indicating whether to look for a new pos for the
//	 *                 joined nodes somewhere between the original positions,
//	 *                 starting at the centroid of q, or to unconditionally try
//	 *                 using the position of the node in q which is opposite to nK.
//	 * @return the new current dart.
//	 */
//	private Dart closeQuad( Quad q, Edge e1, Node nK, boolean centroid )
//	{
//		Msg.debug( "Entering closeQuad(..)" );
//		Dart d = new Dart();
//		Element nElem = q.neighbor( e1 ); // Save for later...
//		Node nKOpp = q.oppositeNode( nK );
//		Node nKp1 = e1.otherNode( nK ); // , nKm1= eKm1.otherNode(nK);
//		Edge e2 = q.neighborEdge( nKp1, e1 ), e4 = q.neighborEdge( nK, e1 );
//
//		List<Element> lK = nK.adjElements(), lKOpp = nKOpp.adjElements();
//		Node n = null;
//		int i;
//
//		if ( centroid )
//		{
//			n = safeNewPosWhenCollapsingQuad( q, nK, nKOpp );
//			if ( n == null )
//			{
//				Msg.debug( "Leaving closeQuad(..), returning null!" );
//				return null;
//			}
//		}
//		else
//		{
//			if ( q.anyInvertedElementsWhenCollapsed( nKOpp, nK, nKOpp, lK, lKOpp ) )
//			{
//				Msg.debug( "Leaving closeQuad(..), returning null!" );
//				return null;
//			}
//			else
//			{
//				n = nKOpp;
//			}
//		}
//		elementList.remove( elementList.indexOf( q ) );
//
//		edgeList.remove( edgeList.indexOf( e1 ) ); // e2
//		edgeList.remove( edgeList.indexOf( q.neighborEdge( nK, e1 ) ) ); // (nKOpp, e2)
//		q.disconnectEdges();
//		q.closeQuad( e2, e1 ); // (e1,e2)
//
//		nKOpp.setXY( n ); // nK.setXY(n);
//		nodeList.remove( nodeList.indexOf( nK ) ); // nKOpp
//		i = nodes.indexOf( nK );
//		if ( i != -1 )
//		{
//			nodes.set( i, null ); // nKOpp
//		}
//		nKOpp.update(); // nK.update();
//
//		d.elem = nElem;
//		d.e = e2; // e1;
//		d.n = nKOpp; // nK;
//
//		Msg.debug( "Leaving closeQuad(..)" );
//		return d;
//	}
//
//	/**
//	 * Create a new quad by "opening" one at the specified node inside the specified
//	 * quad. This effectively results in a splitting of the specified quad.
//	 *
//	 * @param q  the quad
//	 * @param e  an edge in q (not used by method), but contained in returned dart
//	 * @param n1 split q along the diagonal between n1 and its opposite node
//	 */
//	private Dart openQuad( Quad q, Edge e, Node n1 )
//	{
//		Msg.debug( "Entering openQuad(..)" );
//		Dart d = new Dart();
//		Node c = q.centroid();
//		Edge e1 = q.neighborEdge( n1 );
//		Edge e2 = q.neighborEdge( n1, e1 );
//
//		Node n2 = e2.otherNode( n1 );
//		Edge e3 = q.neighborEdge( n2, e2 );
//		Node n3 = e3.otherNode( n2 );
//		Edge e4 = q.neighborEdge( n3, e3 );
//		Node n4 = e4.otherNode( n3 );
//
//		Edge e1New = new Edge( c, n1 );
//		Edge e4New = new Edge( c, n3 );
//
//		e1New.connectNodes();
//		e4New.connectNodes();
//
//		q.replaceEdge( e1, e1New );
//		q.replaceEdge( e4, e4New );
//		e1.disconnectFromElement( q );
//		e4.disconnectFromElement( q );
//
//		e1New.connectToQuad( q );
//		e4New.connectToQuad( q );
//
//		Quad qNew;
//		if ( e1.leftNode == n1 )
//		{
//			qNew = new Quad( e1, e1New, e4, e4New );
//		}
//		else
//		{
//			qNew = new Quad( e1, e4, e1New, e4New );
//		}
//
//		qNew.connectEdges();
//
//		edgeList.add( e1New );
//		edgeList.add( e4New );
//
//		elementList.add( qNew );
//		c.color = java.awt.Color.red; // Indicate it was created during clean-up
//		nodeList.add( c );
//		nodes.add( c );
//
//		q.updateLR();
//
//		d.elem = q;
//		d.e = e;
//		d.n = n1;
//
//		Msg.debug( "Leaving openQuad(..)" );
//		return d;
//	}
//
//	/**
//	 * Create 2 new quads from 2 specified quads, in which the common edge of the
//	 * given quads has been rotated one step in the CCW direction. Delete the old
//	 * quads.
//	 *
//	 * @param qa  one of the two quads adjacent the edge to be switched, e1a
//	 * @param e1a the edge to be switched
//	 * @param n   one of e1a's nodes
//	 * @return a dart representing the input dart after the operation is performed.
//	 */
//	private Dart switchDiagonalCCW( Quad qa, Edge e1a, Node n )
//	{
//		Msg.debug( "Entering switchDiagonalCCW(..)" );
//		Dart d = new Dart();
//		Node n1a, n2a, n3a, n4a, n1b, n2b, n3b, n4b;
//		Edge e2a, e3a, e4a, e1b, e2b, e3b, e4b;
//		Edge eNew, l, r;
//		Quad q1, q2;
//
//		Quad qb = (Quad)qa.neighbor( e1a );
//		int qaIndex = elementList.indexOf( qa ), qbIndex = elementList.indexOf( qb );
//
//		// First get the edges of qa in ccw order:
//		n2a = qa.nextCCWNode( e1a.leftNode );
//		if ( n2a == e1a.rightNode )
//		{
//			n1a = e1a.leftNode;
//		}
//		else
//		{
//			n1a = e1a.rightNode;
//			n2a = e1a.leftNode;
//		}
//
//		e2a = qa.neighborEdge( n2a, e1a );
//		n3a = e2a.otherNode( n2a );
//		e3a = qa.neighborEdge( n3a, e2a );
//		n4a = e3a.otherNode( n3a );
//		e4a = qa.neighborEdge( n4a, e3a );
//
//		// Now get the edges of qb in ccw order:
//		e1b = e1a;
//		n2b = qb.nextCCWNode( e1b.leftNode );
//		if ( n2b == e1b.rightNode )
//		{
//			n1b = e1b.leftNode;
//		}
//		else
//		{
//			n1b = e1b.rightNode;
//			n2b = e1b.leftNode;
//		}
//		e2b = qb.neighborEdge( n2b, e1b );
//		n3b = e2b.otherNode( n2b );
//		e3b = qb.neighborEdge( n3b, e2b );
//		n4b = e3b.otherNode( n3b );
//		e4b = qb.neighborEdge( n4b, e3b );
//
//		Node nOld, smoothed;
//		// Check to see if the switch will violate the mesh topology:
//		if ( e4a.sumAngle( qa, n1a, e2b ) >= Math.PI )
//		{ // if angle >= 180 degrees...
//			if ( n1a.boundaryNode() )
//			{ // exit if node on boundary
//				Msg.debug( "Leaving switchDiagonalCCW(..): returning null" );
//				return null;
//			}
//
//			// ...then try smoothing the pos of the node:
//			nOld = new Node( n1a.x, n1a.y );
//			smoothed = n1a.laplacianSmoothExclude( n2a );
//			if ( !n1a.equals( smoothed ) )
//			{
//				n1a.setXY( smoothed.x, smoothed.y );
//				inversionCheckAndRepair( n1a, nOld );
//				n1a.update();
//			}
//
//			if ( e4a.sumAngle( qa, n1a, e2b ) >= Math.PI )
//			{ // Still angle >= 180 degrees?
//				Msg.debug( "Leaving switchDiagonalCCW(..): returning null" );
//				return null;
//			}
//		}
//
//		if ( e2a.sumAngle( qa, n2a, e4b ) >= Math.PI )
//		{ // if angle >= 180 degrees...
//			if ( n2a.boundaryNode() )
//			{ // exit if node on boundary
//				Msg.debug( "Leaving switchDiagonalCCW(..): returning null" );
//				return null;
//			}
//
//			// ...then try smoothing the pos of the node:
//			nOld = new Node( n2a.x, n2a.y );
//			smoothed = n2a.laplacianSmoothExclude( n1a );
//			if ( !n2a.equals( smoothed ) )
//			{
//				n2a.setXY( smoothed.x, smoothed.y );
//				inversionCheckAndRepair( n2a, nOld );
//				n2a.update();
//			}
//
//			if ( e2a.sumAngle( qa, n2a, e4b ) >= Math.PI )
//			{ // Still angle >= 180 degrees?
//				Msg.debug( "Leaving switchDiagonalCCW(..): returning null" );
//				return null;
//			}
//		}
//		// The new diagonal:
//		eNew = new Edge( n3a, n3b );
//
//		// Create the new quads:
//		l = qa.neighborEdge( e4a.leftNode, e4a );
//		r = qa.neighborEdge( e4a.rightNode, e4a );
//		if ( l == e1a )
//		{
//			l = e2b;
//		}
//		else
//		{
//			r = e2b;
//		}
//		q1 = new Quad( e4a, l, r, eNew );
//
//		l = qb.neighborEdge( e4b.leftNode, e4b );
//		r = qb.neighborEdge( e4b.rightNode, e4b );
//		if ( l == e1b )
//		{
//			l = e2a;
//		}
//		else
//		{
//			r = e2a;
//		}
//		q2 = new Quad( e4b, l, r, eNew );
//
//		qa.disconnectEdges();
//		qb.disconnectEdges();
//		e1a.disconnectNodes();
//		q1.connectEdges();
//		q2.connectEdges();
//		eNew.connectNodes();
//
//		// Update lists:
//		edgeList.set( edgeList.indexOf( e1a ), eNew );
//
//		elementList.set( qaIndex, q1 );
//		elementList.set( qbIndex, q2 );
//
//		d.elem = q1;
//		d.e = eNew;
//		if ( n == n1a )
//		{
//			d.n = n3b;
//		}
//		else
//		{
//			d.n = n3a;
//		}
//
//		Msg.debug( "Leaving switchDiagonalCCW(..)" );
//		return d;
//	}
//
//	/**
//	 * Create 2 new quads from 2 specified quads, in which the common edge of the
//	 * given quads has been rotated one step in the CW direction. Delete the old
//	 * quads. Update the nodes list.
//	 *
//	 * @param qa  one of the two quads adjacent the edge to be switched, e1a
//	 * @param e1a the edge to be switched
//	 * @param n   one of e1a's nodes
//	 * @return a dart representing the input dart after the operation is performed.
//	 */
//	private Dart switchDiagonalCW( Quad qa, Edge e1a, Node n )
//	{
//		Msg.debug( "Entering switchDiagonalCW(..)" );
//		Dart d = new Dart();
//		Node n1a, n2a, n3a, n4a, n1b, n2b, n3b, n4b;
//		Edge e2a, e3a, e4a, e1b, e2b, e3b, e4b;
//		Edge eNew, l, r;
//		Quad q1, q2;
//
//		Quad qb = (Quad)qa.neighbor( e1a );
//		int qaIndex = elementList.indexOf( qa ), qbIndex = elementList.indexOf( qb );
//
//		// First get the edges of qa in ccw order:
//		n2a = qa.nextCCWNode( e1a.leftNode );
//		if ( n2a == e1a.rightNode )
//		{
//			n1a = e1a.leftNode;
//		}
//		else
//		{
//			n1a = e1a.rightNode;
//			n2a = e1a.leftNode;
//		}
//		e2a = qa.neighborEdge( n2a, e1a );
//		n3a = e2a.otherNode( n2a );
//		e3a = qa.neighborEdge( n3a, e2a );
//		n4a = e3a.otherNode( n3a );
//		e4a = qa.neighborEdge( n4a, e3a );
//
//		// Now get the edges of qb in ccw order:
//		e1b = e1a;
//		n2b = qb.nextCCWNode( e1b.leftNode );
//		if ( n2b == e1b.rightNode )
//		{
//			n1b = e1b.leftNode;
//		}
//		else
//		{
//			n1b = e1b.rightNode;
//			n2b = e1b.leftNode;
//		}
//		e2b = qb.neighborEdge( n2b, e1b );
//		n3b = e2b.otherNode( n2b );
//		e3b = qb.neighborEdge( n3b, e2b );
//		n4b = e3b.otherNode( n3b );
//		e4b = qb.neighborEdge( n4b, e3b );
//
//		Node nOld, smoothed;
//		// Check to see if the switch will violate the mesh topology:
//		if ( e4a.sumAngle( qa, n1a, e2b ) >= Math.PI )
//		{ // if angle >= 180 degrees...
//// ...then try smoothing the pos of the node:
//			nOld = new Node( n1a.x, n1a.y );
//			smoothed = n1a.laplacianSmooth();
//			if ( !n1a.equals( smoothed ) )
//			{
//				n1a.moveTo( smoothed );
//				inversionCheckAndRepair( n1a, nOld );
//				n1a.update();
//			}
//
//			if ( e4a.sumAngle( qa, n1a, e2b ) >= Math.PI )
//			{ // Still angle >= 180 degrees?
//				Msg.debug( "Leaving switchDiagonalCW(..): returning null" );
//				return null;
//			}
//		}
//
//		// Check to see if the switch will violate the mesh topology:
//		if ( e2a.sumAngle( qa, n2a, e4b ) >= Math.PI )
//		{ // if angle >= 180 degrees...
//// ...then try smoothing the pos of the node:
//			nOld = new Node( n2a.x, n2a.y );
//			smoothed = n2a.laplacianSmooth();
//			if ( !n2a.equals( smoothed ) )
//			{
//				n2a.moveTo( smoothed );
//				inversionCheckAndRepair( n2a, nOld );
//				n2a.update();
//			}
//
//			if ( e2a.sumAngle( qa, n2a, e4b ) >= Math.PI )
//			{ // Still angle >= 180 degrees?
//				Msg.debug( "Leaving switchDiagonalCW(..): returning null" );
//				return null;
//			}
//		}
//
//		// The new diagonal:
//		eNew = new Edge( n4a, n4b );
//
//		// Create the new quads:
//		l = qa.neighborEdge( e2a.leftNode, e2a );
//		r = qa.neighborEdge( e2a.rightNode, e2a );
//		if ( l == e1a )
//		{
//			l = e4b;
//		}
//		else
//		{
//			r = e4b;
//		}
//		q1 = new Quad( e2a, l, r, eNew );
//
//		l = qb.neighborEdge( e2b.leftNode, e2b );
//		r = qb.neighborEdge( e2b.rightNode, e2b );
//		if ( l == e1b )
//		{
//			l = e4a;
//		}
//		else
//		{
//			r = e4a;
//		}
//		q2 = new Quad( e2b, l, r, eNew );
//
//		qa.disconnectEdges();
//		qb.disconnectEdges();
//		e1a.disconnectNodes();
//		q1.connectEdges();
//		q2.connectEdges();
//		eNew.connectNodes();
//
//		// Update lists:
//		edgeList.set( edgeList.indexOf( e1a ), eNew );
//
//		elementList.set( qaIndex, q1 );
//		elementList.set( qbIndex, q2 );
//
//		d.elem = q1;
//		d.e = eNew;
//		if ( n == n1a )
//		{
//			d.n = n4a;
//		}
//		else
//		{
//			d.n = n4b;
//		}
//
//		Msg.debug( "Leaving switchDiagonalCW(..)" );
//		return d;
//	}
//
//	private void globalSmooth()
//	{
//		Msg.debug( "Entering TopoCleanup.globalSmoth()" );
//		Node n, nn, nOld;
//
//		for ( Object element : nodeList )
//		{
//			n = (Node)element;
//
//			if ( !n.boundaryNode() )
//			{
//
//				// Try smoothing the pos of the node:
//				nOld = new Node( n.x, n.y );
//				nn = n.laplacianSmooth();
//				if ( !n.equals( nn ) )
//				{
//					n.setXY( nn.x, nn.y );
//					inversionCheckAndRepair( n, nOld );
//					n.update();
//				}
//			}
//		}
//		Msg.debug( "Leaving TopoCleanup.globalSmoth()" );
//	}

};
