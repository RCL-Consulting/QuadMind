#pragma once

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

#include "GeomBasics.h"

class Edge;
class Quad;
class Node;

class QMorph : public GeomBasics
{
private:
	std::vector<Edge*> frontList;
	bool finished = false;
	int level = 0;
	int nrOfFronts = 0;
	bool evenInitNrOfFronts = false;

public:
	// Initialize the class //
	void init();

	// Run the implementation on the given triangle mesh //
	void run();

	// Step through the morphing process one front edge at the time. //
	void step();

	// @return the frontList, that is, the list of front edges //
	const std::vector<Edge*>& getFrontList()
	{
		return frontList;
	}

private:
	// Count the number of front edges in the new loops created if we were to create
	// a new quad with edge b as base edge and one or both of edges l and r were
	// promoted to front edges and one or both of their top nodes were located on
	// the front. Return a byte signifying which, if any, of these loops contains an
	// odd number of edges.
	//
	// @param b     the base edge of the quad we want to make. Should be a front
	//              edge.
	// @param l     the left edge of the quad we want to make
	// @param r     the right edge of the quad we want to make
	// @param lLoop boolean indicating whether to count the edges in the loop at l
	// @param rLoop boolean indicating whether to count the edges in the loop at r
	// @return a bit pattern indicating which, if any, of the resulting front loops
	//         that have an odd number of edges. Also indicate if the two loops are
	//         actually the same.
	//
	uint8_t oddNOFEdgesInLoopsWithFEdge( Edge* b, Edge* l, Edge* r, bool lLoop, bool rLoop );

	bool bothSidesInLoop = false;

	//
	// Supposing that the edges side and otherSide are promoted to front edges. The
	// method parses a new loop involving the edges side, otherSide and possibly
	// edge b. If two separate loops result from promoting the edges side and
	// otherSide to front edges, then return the number of edges in the loop
	// involving edges b and side. Otherwise, return the total number of edges in
	// the loop involving edges side and otherSide.
	//
	// @param b         an edge that is considered for being promoted to a front
	//                  edge
	// @param side      one of b's front neighbors
	// @param otherSide the other of b's front neighbors
	// @return as described above.
	//
	int countFrontsInNewLoopAt( Edge* b, Edge* side, Edge* otherSide );

	// Returns the number of front edges at the currently lowest level loop(s). //
	int countNOFrontsAtCurLowestLevel( const std::vector<Edge*>& frontList2 );

	// Make sure the triangle mesh consists exclusively of triangles //
	bool verifyTriangleMesh( const std::vector<Triangle*>& triangleList )
	{
		//		Object o;
		//		for (Object element : triangleList) {
		//			o = element;
		//			if (!(o instanceof Triangle)) {
		//				return false;
		//			}
		//		}
		return true;
	}

	//
	// Construct a new quad
	//
	// @param e the base edge of this new quad
	// @return the new quad
	//
	Quad* makeQuad( Edge* e );

	//
	// @param nK     front node to be smoothed
	// @param nJ     node behind the front, connected to nK
	// @param myQ    an arbitrary selected quad connected to node nK
	// @param front1 a front edge adjacent nK
	// @param front2 another front edge adjacent nK and part of the same loop as
	//               front1
	// @return a new node with the smoothed position
	//
	Node* smoothFrontNode( Node* nK, Node* nJ, Quad* myQ, Edge* front1, Edge* front2 );

	//
	// Calculate smoothed position of node. (Called from localSmooth)
	//
	// @see #smoothFrontNode(Node, Node, Quad, Edge, Edge)
	// @see Node#modifiedLWLaplacianSmooth()
	// @param n Node to be smoothed
	// @param q Quad to which n belongs
	//
	Node* getSmoothedPos( Node* n, Quad* q );

	//
	// Smooth as explained in Owen's paper Each node in the newly formed quad is
	// smoothed. So is every node directly connected to these.
	///
	void localSmooth( Quad* q, const std::vector<Edge*>& frontList2 );

	//
	// Delete all interior triangles within the edges of this quad
	//
	// @param q    the quad
	// @param tris the list of triangles to be deleted
	//
	void clearQuad( Quad* q, const std::vector<Triangle*>& tris );

	//
	// "Virus" that removes all triangles and their edges and nodes inside of this
	// quad Assumes that only triangles are present, not quads, inside of q
	//
	void clearQuad( Quad* q, Triangle* first );

	// Updates fronts in fake quads (which are triangles, really) //
	int localFakeUpdateFronts( Quad* q, int lowestLevel, std::vector<Edge*>& frontList2 );

	// Do some neccessary updating of the fronts before localSmooth(..) is run
	void preSmoothUpdateFronts( Quad* q, const std::vector<Edge*>& frontList );

	//
	// Define new fronts, remove old ones. Set new frontNeighbors. Reclassify front
	// edges.
	//
	// @return nr of edges removed that belonged to the currently lowest level.
	//
	int localUpdateFronts( Quad* q, int lowestLevel, std::vector<Edge*>& frontList2 );

	std::vector<Edge*> defineInitFronts( const std::vector<Edge*>& edgeList );

	void classifyStateOfAllFronts( const std::vector<Edge*>& frontList2 );

	// Performs seaming operation as described in Owen's paper //
	Quad* doSeam( Edge* e1, Edge* e2, Node* nK );

	// Performs the transition seam operation as described in Owen's paper. //
	Quad* doTransitionSeam( Edge* e1, Edge* e2, Node* nK );

	// Performs the transition split operation as described in Owen's paper. //
	Quad* doTransitionSplit( Edge* e1, Edge* e2, Node* nK );

	bool canSeam( Node* n, Edge* e1, Edge* e2 );

	//
	// Check wether a seam operation is needed.
	//
	// @param e1 an edge
	// @param e2 an edge that might need to be merged with e1
	// @param n  the common Node of e1 and e2
	// @param nQ number of Quads adjacent to Node n
	//
	bool needsSeam( Edge* e1, Edge* e2, Node* n, int nQ );

	//
	// @return not null if a special case was encountered. If so, this means that
	//         that the base edge has been destroyed, and that a new one has to be
	//         chosen.
	//
	Quad* handleSpecialCases( Edge* e );

	// jobber her...
	//
	// If not already set, this method sets the sideEdges, based on state. Called
	// from makeQuad(Edge)
	//
	std::array<Edge*, 2> makeSideEdges( Edge* e );

	// @return a list of potential side edges for this base edge at node n //
	std::vector<Edge*> getPotSideEdges( Edge* baseEdge, Node* n );

	//
	// @param eF1    the base edge of the quad to be created
	// @param nK     the node at eF1 that the side edge will be connected to
	// @param altRSE list of candidate edges from which we might select a side edge
	// @return A side edge: a reused edge OR one created in a swap/split operation
	//
	Edge* defineSideEdge( Edge* eF1, Node* nK,
						  Edge* leftSide, Edge* rightSide,
						  const std::vector<Edge*>& altRSE );

	//
	// Create an edge from nC to nD, or if it already exists, return that edge.
	// Remove all edges intersected by the new edge. This is accomplished through a
	// swapping procedure. All local and "global" updating is taken care of.
	//
	// @param nC the start node
	// @param nD the end node
	// @return null if the Edge could not be recovered. This might happen when the
	//         line segment from nC to nD intersects a Quad or a boundary Edge.
	//
	Edge* recoverEdge( Node* nC, Node* nD );
};