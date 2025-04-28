#pragma once

#include "GeomBasics.h"
#include "ArrayList.h"

#include <memory>

// ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ----
/**
 * This is the main class, implementing the triangle to quad conversion process.
 * The algorithm was invented by Steven J. Owen, Matthew L. Staten, Scott A.
 * Cannan, and Sunil Saigal and described in their paper "Advancing Front
 * Quadrilateral Meshing Using Triangle Transformations" (1998).
 *
 * @see <a href="http://www.andrew.cmu.edu/user/sowen/abstracts/Ow509.html"
 *      TARGET="_top">the abstract of the paper</a>
 * @author Karl Erik Levik
 *
 */
 // ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ---- ==== ----

class Node;
class Edge;
class Triangle;
class Quad;

class QMorph : 
	public GeomBasics,
	public std::enable_shared_from_this<QMorph>
{
private:
	ArrayList<std::shared_ptr<Edge>> frontList;
	bool finished = false;
	int level = 0;
	int nrOfFronts = 0;
	bool evenInitNrOfFronts = false;

public:
	/** Initialize the class */
	void init();

	///** Run the implementation on the given triangle mesh */
	void run();

	///** Step through the morphing process one front edge at the time. */
	void step() override;

	/** @return the frontList, that is, the list of front edges */
	const ArrayList<std::shared_ptr<Edge>>& getFrontList() { return frontList; }

	bool equals( const std::shared_ptr<Constants>& elem ) const override
	{
		return std::dynamic_pointer_cast<QMorph>(elem) != nullptr;
	}

private:
	/**
	 * Count the number of front edges in the new loops created if we were to create
	 * a new quad with edge b as base edge and one or both of edges l and r were
	 * promoted to front edges and one or both of their top nodes were located on
	 * the front. Return a byte signifying which, if any, of these loops contains an
	 * odd number of edges.
	 *
	 * @param b     the base edge of the quad we want to make. Should be a front
	 *              edge.
	 * @param l     the left edge of the quad we want to make
	 * @param r     the right edge of the quad we want to make
	 * @param lLoop boolean indicating whether to count the edges in the loop at l
	 * @param rLoop boolean indicating whether to count the edges in the loop at r
	 * @return a bit pattern indicating which, if any, of the resulting front loops
	 *         that have an odd number of edges. Also indicate if the two loops are
	 *         actually the same.
	 */
	uint8_t oddNOFEdgesInLoopsWithFEdge( const std::shared_ptr<Edge>& b,
										 const std::shared_ptr<Edge>& l,
										 const std::shared_ptr<Edge>& r,
										 bool lLoop, bool rLoop );

	bool  bothSidesInLoop = false;

	/**
	 * Supposing that the edges side and otherSide are promoted to front edges. The
	 * method parses a new loop involving the edges side, otherSide and possibly
	 * edge b. If two separate loops result from promoting the edges side and
	 * otherSide to front edges, then return the number of edges in the loop
	 * involving edges b and side. Otherwise, return the total number of edges in
	 * the loop involving edges side and otherSide.
	 *
	 * @param b         an edge that is considered for being promoted to a front
	 *                  edge
	 * @param side      one of b's front neighbors
	 * @param otherSide the other of b's front neighbors
	 * @return as described above.
	 */
	int countFrontsInNewLoopAt( const std::shared_ptr<Edge>& b,
								const std::shared_ptr<Edge>& side,
								const std::shared_ptr<Edge>& otherSide );

	/** Returns the number of front edges at the currently lowest level loop(s). */
	int countNOFrontsAtCurLowestLevel( const ArrayList<std::shared_ptr<Edge>>& frontList2 );

	/** Make sure the triangle mesh consists exclusively of triangles */
	bool verifyTriangleMesh( const ArrayList<std::shared_ptr<Triangle>>& triangleList );

	/**
	 * Construct a new quad
	 *
	 * @param e the base edge of this new quad
	 * @return the new quad
	 */
	std::shared_ptr<Quad> makeQuad( std::shared_ptr<Edge>& e );

	/**
	 * @param nK     front node to be smoothed
	 * @param nJ     node behind the front, connected to nK
	 * @param myQ    an arbitrary selected quad connected to node nK
	 * @param front1 a front edge adjacent nK
	 * @param front2 another front edge adjacent nK and part of the same loop as
	 *               front1
	 * @return a new node with the smoothed position
	 */
	std::shared_ptr<Node> smoothFrontNode( const std::shared_ptr<Node>& nK,
										   const std::shared_ptr<Node>& nJ,
										   const std::shared_ptr<Quad>& myQ,
										   const std::shared_ptr<Edge>& front1,
										   const std::shared_ptr<Edge>& front2 );

	/**
	 * Calculate smoothed position of node. (Called from localSmooth)
	 *
	 * @see #smoothFrontNode(Node, Node, Quad, Edge, Edge)
	 * @see Node#modifiedLWLaplacianSmooth()
	 * @param n Node to be smoothed
	 * @param q Quad to which n belongs
	 */
	std::shared_ptr<Node> getSmoothedPos( const std::shared_ptr<Node>& n,
										  const std::shared_ptr<Quad>& q );

	/**
	 * Smooth as explained in Owen's paper Each node in the newly formed quad is
	 * smoothed. So is every node directly connected to these.
	 */
	void localSmooth( const std::shared_ptr<Quad>& q,
					  const ArrayList<std::shared_ptr<Edge>>& frontList2 );

	/**
	 * Delete all interior triangles within the edges of this quad
	 *
	 * @param q    the quad
	 * @param tris the list of triangles to be deleted
	 */
	void clearQuad( const std::shared_ptr<Quad>& q,
					const ArrayList<std::shared_ptr<Triangle>>& tris );

	/**
	 * "Virus" that removes all triangles and their edges and nodes inside of this
	 * quad Assumes that only triangles are present, not quads, inside of q
	 */
	void clearQuad( const std::shared_ptr<Quad>& q,
					const std::shared_ptr<Triangle>& first );

	/** Updates fronts in fake quads (which are triangles, really) */
	int localFakeUpdateFronts( const std::shared_ptr<Quad>& q,
							   int lowestLevel,
							   ArrayList<std::shared_ptr<Edge>>& frontList2 );

	// Do some neccessary updating of the fronts before localSmooth(..) is run
	void preSmoothUpdateFronts( const std::shared_ptr<Quad>& q,
								ArrayList<std::shared_ptr<Edge>>& frontList );

	/**
	 * Define new fronts, remove old ones. Set new frontNeighbors. Reclassify front
	 * edges.
	 *
	 * @return nr of edges removed that belonged to the currently lowest level.
	 */
	int localUpdateFronts( const std::shared_ptr<Quad>& q,
						   int lowestLevel,
						   ArrayList<std::shared_ptr<Edge>>& frontList2 );

	const ArrayList<std::shared_ptr<Edge>> defineInitFronts( const ArrayList<std::shared_ptr<Edge>>& edgeList );

	void classifyStateOfAllFronts( ArrayList<std::shared_ptr<Edge>>& frontList2 );

	/** Performs seaming operation as described in Owen's paper */
	std::shared_ptr<Quad> doSeam( std::shared_ptr<Edge>& e1,
								  std::shared_ptr<Edge>& e2,
								  const std::shared_ptr<Node>& nK );

	/** Performs the transition seam operation as described in Owen's paper. */
	std::shared_ptr<Quad> doTransitionSeam( const std::shared_ptr<Edge>& e1,
											const std::shared_ptr<Edge>& e2,
											const std::shared_ptr<Node>& nK );

	/** Performs the transition split operation as described in Owen's paper. */
	std::shared_ptr<Quad> doTransitionSplit( const std::shared_ptr<Edge>& e1,
											 const std::shared_ptr<Edge>& e2,
											 const std::shared_ptr<Node>& nK );

	bool canSeam( const std::shared_ptr<Node>& n,
				  const std::shared_ptr<Edge>& e1,
				  const std::shared_ptr<Edge>& e2 );

	/**
	 * Check wether a seam operation is needed.
	 *
	 * @param e1 an edge
	 * @param e2 an edge that might need to be merged with e1
	 * @param n  the common Node of e1 and e2
	 * @param nQ number of Quads adjacent to Node n
	 */
	bool needsSeam( const std::shared_ptr<Edge>& e1,
					const std::shared_ptr<Edge>& e2,
					const std::shared_ptr<Node>& n,
					int nQ );

	/**
	 * @return not null if a special case was encountered. If so, this means that
	 *         that the base edge has been destroyed, and that a new one has to be
	 *         chosen.
	 */
	std::shared_ptr<Quad> handleSpecialCases( std::shared_ptr<Edge>& e );

	// jobber her...
	/**
	 * If not already set, this method sets the sideEdges, based on state. Called
	 * from makeQuad(Edge)
	 */
	std::array<std::shared_ptr<Edge>,2> makeSideEdges( const std::shared_ptr<Edge>& e );

	/** @return a list of potential side edges for this base edge at node n */
	ArrayList<std::shared_ptr<Edge>> getPotSideEdges( const std::shared_ptr<Edge>& baseEdge,
													  const std::shared_ptr<Node>& n );

	/**
	 * @param eF1    the base edge of the quad to be created
	 * @param nK     the node at eF1 that the side edge will be connected to
	 * @param altRSE list of candidate edges from which we might select a side edge
	 * @return A side edge: a reused edge OR one created in a swap/split operation
	 */
	std::shared_ptr<Edge> defineSideEdge( const std::shared_ptr<Edge>& eF1,
										  const std::shared_ptr<Node>& nK,
										  const std::shared_ptr<Edge>& leftSide,
										  const std::shared_ptr<Edge>& rightSide,
										  const ArrayList<std::shared_ptr<Edge>>& altRSE );
	/**
	 * Create an edge from nC to nD, or if it already exists, return that edge.
	 * Remove all edges intersected by the new edge. This is accomplished through a
	 * swapping procedure. All local and "global" updating is taken care of.
	 *
	 * @param nC the start node
	 * @param nD the end node
	 * @return null if the Edge could not be recovered. This might happen when the
	 *         line segment from nC to nD intersects a Quad or a boundary Edge.
	 */
	std::shared_ptr<Edge> recoverEdge( const std::shared_ptr<Node>& nC,
									   const std::shared_ptr<Node>& nD );
	//{
	//	Msg.debug( "Entering recoverEdge(Node, Node)..." );
	//	Edge S = new Edge( nD, nC );
	//	Msg.debug( "nC= " + nC.descr() );
	//	Msg.debug( "nD= " + nD.descr() );

	//	printEdgeList( nC.edgeList );

	//	if ( nC.edgeList.contains( S ) )
	//	{
	//		Edge edge = nC.edgeList.get( nC.edgeList.indexOf( S ) );
	//		Msg.debug( "recoverEdge returns edge " + edge.descr() + " (shortcut)" );
	//		return edge;
	//	}
	//	/* ---- First find the edges connecting nodes nC and nD: ---- */
	//	/* (Implementation of algorithm 2 in Owen) */
	//	ArrayList<Edge> intersectedEdges = new ArrayList<>();
	//	Edge eK, eKp1, eI = null, eJ = null, eN, eNp1;
	//	Element tK = null;
	//	Triangle tI = null, tIp1;
	//	MyVector vK, vKp1 = null, vI;
	//	MyVector vS = new MyVector( nC, nD );
	//	Node nI;

	//	ArrayList<MyVector> V = nC.ccwSortedVectorList();
	//	V.add( V.get( 0 ) ); // First add first edge to end of list to avoid crash in loops
	//	Msg.debug( "V.size()==" + V.size() );
	//	printVectors( V );

	//	// Aided by V, fill T with elements adjacent nC, in
	//	// ccw order (should work even for nodes with only two edges in their list):
	//	ArrayList<Element> T = new ArrayList<>();

	//	for ( int k = 0; k < V.size() - 1; k++ )
	//	{
	//		vK = V.get( k );
	//		vKp1 = V.get( k + 1 );
	//		eK = vK.edge;
	//		eKp1 = vKp1.edge;

	//		if ( eK == null || eKp1 == null )
	//		{
	//			Msg.debug( "eK eller eKp1 er null......" );
	//		}
	//		if ( !T.contains( eK.element1 ) && (eK.element1 == eKp1.element1 || eK.element1 == eKp1.element2) )
	//		{
	//			T.add( eK.element1 );
	//		}
	//		if ( eK.element2 != null && !T.contains( eK.element2 ) && (eK.element2 == eKp1.element2 || eK.element2 == eKp1.element1) )
	//		{
	//			T.add( eK.element2 );
	//		}
	//	}

	//	// Now, get the element attached to nC that contains a part of S, tI:
	//	Msg.debug( "T.size()==" + T.size() );
	//	for ( int k = 0; k < T.size(); k++ )
	//	{
	//		vK = V.get( k );
	//		vKp1 = V.get( k + 1 );

	//		tK = T.get( k );

	//		// We could optimize by using isCWto(..) directly instead of dot(..)
	//		// And I can't get the dot(...) to work properly anyway, sooo...

	//		Msg.debug( "vS==" + vS.descr() );
	//		Msg.debug( "vK==" + vK.descr() );
	//		Msg.debug( "vKp1==" + vKp1.descr() );
	//		Msg.debug( "tK=" + tK.descr() );
	//		// Msg.debug("vS.dot(vK)=="+vS.dot(vK)+" and vS.dot(vKp1)=="+vS.dot(vKp1));

	//		if ( !vS.isCWto( vK ) && vS.isCWto( vKp1 ) )
	//		{
	//			// tI= tK;
	//			// eI= vKp1.edge; // just something I tried.. no good
	//			break; // got it, escape from loop
	//		}
	//	}
	//	Msg.debug( "loop okei.." );
	//	if ( tK == null )
	//	{
	//		Msg.error( "Oida.. valgt tK er null" );
	//	}

	//	Msg.debug( "valgt tK er: " + tK.descr() );

	//	Element elemI, elemIp1;
	//	elemI = tK;
	//	if ( elemI instanceof Quad )
	//	{
	//		Msg.warning( "Leaving recoverEdge(..): intersecting quad, returning null." );
	//		return null;
	//	}
	//	else
	//	{ // elemI is a Triangle...
	//		tI = (Triangle)elemI;
	//		eI = tI.oppositeOfNode( nC );
	//	}
	//	if ( !eI.frontEdge )
	//	{
	//		intersectedEdges.add( eI );
	//	}
	//	else
	//	{
	//		Msg.warning( "Leaving recoverEdge: eI=" + eI.descr() + " is part of the front." );
	//		return null;
	//	}

	//	// Quad qI;
	//	while ( true )
	//	{
	//		elemIp1 = elemI.neighbor( eI );
	//		// tIp1= (Triangle)elem;
	//		Msg.debug( "elemIp1= " + elemIp1.descr() );

	//		if ( elemIp1.hasNode( nD ) )
	//		{
	//			break;
	//		}
	//		elemI = elemIp1;
	//		if ( elemI instanceof Triangle )
	//		{
	//			tI = (Triangle)elemI;
	//			nI = tI.oppositeOfEdge( eI );
	//			vI = new MyVector( nC, nI );
	//			eN = tI.nextCCWEdge( eI );
	//			eNp1 = tI.nextCWEdge( eI );

	//			Msg.debug( "eN= " + eN.descr() );
	//			Msg.debug( "eNp1= " + eNp1.descr() );
	//			// if (vS.dot(vI)<0) // Not convinced that dot(..) works properly
	//			if ( vS.isCWto( vI ) )
	//			{
	//				eI = eN;
	//			}
	//			else
	//			{
	//				eI = eNp1;
	//			}

	//			if ( !eI.frontEdge )
	//			{
	//				intersectedEdges.add( eI );
	//			}
	//			else
	//			{
	//				Msg.warning( "Leaving recoverEdge: eI=" + eI.descr() + " is part of the front." );
	//				return null;
	//			}
	//		}
	//		else
	//		{ // elemI is instanceof Quad
	//			Msg.warning( "Leaving recoverEdge: intersecting quad." );
	//			return null;
	//		}
	//	}

	//	/* ---- ---- ---- ---- ---- ---- */

	//	/* ---- Secondly, do the actual recovering of the top edge ---- */
	//	if ( intersectedEdges.size() == 0 )
	//	{
	//		eJ = S;
	//		Msg.warning( "recoverEdge: intersectedEdges.size()== 0" );
	//	}
	//	else if ( intersectedEdges.size() == 1 )
	//	{
	//		// According to alg. 2 in Owen, intersectedEdges always contains at least
	//		// one edge.
	//		eI = intersectedEdges.get( 0 );
	//		if ( eI.equals( S ) )
	//		{
	//			Msg.debug( "Leaving recoverEdge: returns edge " + eI.descr() );
	//			return eI;
	//		}
	//	}

	//	Msg.debug( "recoverEdge: intersectedEdges.size()==" + intersectedEdges.size() );

	//	// When this loop is done, the edge should be recovered
	//	Element oldEIElement1, oldEIElement2;
	//	ArrayList<Element> removeList = new ArrayList<>();
	//	Triangle t;
	//	Quad q;
	//	int index;
	//	int n = intersectedEdges.size();
	//	// for (int i=0; i< n; i++) {
	//	Element old1, old2;
	//	Node na, nb, nc, nd;
	//	Edge qe1, qe2;

	//	while ( intersectedEdges.size() > 0 )
	//	{
	//		eI = intersectedEdges.get( 0 );
	//		Msg.debug( "eI= " + eI.descr() );

	//		// We must avoid creating inverted or degenerate triangles.
	//		q = new Quad( eI );
	//		Msg.debug( "eI.element1= " + eI.element1.descr() );
	//		Msg.debug( "eI.element2= " + eI.element2.descr() );
	//		old1 = eI.element1;
	//		old2 = eI.element2;

	//		na = ((Triangle)old1).oppositeOfEdge( eI );
	//		nb = ((Triangle)old2).oppositeOfEdge( eI );
	//		nc = eI.leftNode;
	//		nd = eI.rightNode;

	//		double cross1 = cross( na, nc, nb, nc ); // The cross product nanc x nbnc
	//		double cross2 = cross( na, nd, nb, nd ); // The cross product nand x nbnd

	//		// For a stricly convex quad, the nodes must be located on different sides of
	//		// eI:
	//		if ( (cross1 > 0 && cross2 < 0) || (cross1 < 0 && cross2 > 0) /* q.isStrictlyConvex() */ )
	//		{
	//			// ... but this seems ok
	//			Msg.debug( "...cross1: " + cross1 );
	//			Msg.debug( "...cross2: " + cross2 );

	//			eI.swappable = true;
	//			eJ = eI.getSwappedEdge();

	//			Msg.debug( "eJ= " + eJ.descr() );
	//			eI.swapToAndSetElementsFor( eJ );

	//			Msg.debug( "eJ.element1==" + eJ.element1.descr() );
	//			Msg.debug( "eJ.element2==" + eJ.element2.descr() );

	//			if ( eJ.element1.inverted() )
	//			{
	//				Msg.error( "eJ.element1 is inverted: " + eJ.element1.descr() );
	//			}

	//			if ( eJ.element2.inverted() )
	//			{
	//				Msg.error( "eJ.element2 is inverted: " + eJ.element2.descr() );
	//			}

	//			// Add old triangles to removeList...
	//			if ( !removeList.contains( old1 ) )
	//			{
	//				removeList.add( old1 );
	//				Msg.debug( "...adding element " + old1.descr() + " to removeList" );
	//			}

	//			if ( !removeList.contains( old2 ) )
	//			{
	//				removeList.add( old2 );
	//				Msg.debug( "...adding element " + old2.descr() + " to removeList" );
	//			}

	//			// ... and replace with new ones:
	//			triangleList.add( (Triangle)eJ.element1 );
	//			Msg.debug( "Added element: " + eJ.element1.descr() );
	//			triangleList.add( (Triangle)eJ.element2 );
	//			Msg.debug( "Added element: " + eJ.element2.descr() );

	//			// Update "global" edge list
	//			Msg.debug( "...removing edge " + eI.descr() );
	//			edgeList.remove( edgeList.indexOf( eI ) );
	//			edgeList.add( eJ );

	//			intersectedEdges.remove( 0 );
	//			MyVector vEj = new MyVector( eJ.leftNode, eJ.rightNode );

	//			if ( !eJ.hasNode( nC ) && !eJ.hasNode( nD ) && vEj.innerpointIntersects( vS ) )
	//			{
	//				Msg.debug( "recoverEdge: vEj: " + vEj.descr() + " is innerpoint-intersecting vS: " + vS.descr() );
	//				intersectedEdges.add( eJ );
	//			}

	//		}
	//		else if ( intersectedEdges.size() > 1 && eI.swappable )
	//		{
	//			Msg.debug( "recoverEdge(..): ok, moving edge last, eI=" + eI.descr() );
	//			Msg.debug( "Quad is " + q.descr() );
	//			intersectedEdges.remove( 0 ); // put eI last in the list
	//			intersectedEdges.add( eI );
	//			eI.swappable = false;
	//		}
	//		else
	//		{
	//			// We have to give up
	//			Msg.warning( "Leaving recoverEdge: Cannot swap edge - non-convex quad." );

	//			for ( Edge intersectedEdge : intersectedEdges )
	//			{
	//				eI = intersectedEdge;
	//				eI.swappable = true;
	//			}
	//			// Remove all triangles found in the removeList from triangleList:
	//			for ( Element element : removeList )
	//			{
	//				t = (Triangle)element;
	//				index = triangleList.indexOf( t );
	//				if ( index != -1 )
	//				{
	//					Msg.debug( "Removing triangle " + t.descr() );
	//					triangleList.remove( index );
	//				}
	//			}
	//			return null;
	//		}
	//	}

	//	// Remove all triangles found in the removeList from triangleList:
	//	for ( Element element : removeList )
	//	{
	//		t = (Triangle)element;
	//		index = triangleList.indexOf( t );
	//		if ( index != -1 )
	//		{
	//			Msg.debug( "Removing triangle " + t.descr() );
	//			triangleList.remove( index );
	//		}
	//	}

	//	/* ---- ---- ---- ---- ---- ---- */

	//	// eJ should be the recovered edge now, according to fig.6(d) in Owen.
	//	// From the swapToAndSetElementsFor(..) method, eJ has already got its
	//	// adjacent triangles. So everything should be juuuuust fine by now...
	//	Msg.debug(
	//		"Leaving recoverEdge(Edge e): returns edge " + eJ.descr() + " with element1= " + eJ.element1.descr() + " and element2= " + eJ.element2.descr() );
	//	return eJ;
	//}

};
