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

	bool equals( const std::shared_ptr<Constants>& elem ) const override
	{
		return std::dynamic_pointer_cast<TopoCleanup>(elem) != nullptr;
	}

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

	/**
	 * Collapse a quad by joining two and two of its consecutive edges.
	 *
	 * @param q        the quad to be collapsed
	 * @param e1       an edge of q that has the node nK
	 * @param nK       the node that is to be joined with its opposite node in q
	 * @param centroid boolean indicating whether to look for a new pos for the
	 *                 joined nodes somewhere between the original positions,
	 *                 starting at the centroid of q, or to unconditionally try
	 *                 using the position of the node in q which is opposite to nK.
	 * @return the new current dart.
	 */
	std::shared_ptr<Dart> closeQuad( const std::shared_ptr<Quad>& q,
									 const std::shared_ptr<Edge>& e1,
									 const std::shared_ptr<Node>& nK,
									 bool centroid );

	/**
	 * Create a new quad by "opening" one at the specified node inside the specified
	 * quad. This effectively results in a splitting of the specified quad.
	 *
	 * @param q  the quad
	 * @param e  an edge in q (not used by method), but contained in returned dart
	 * @param n1 split q along the diagonal between n1 and its opposite node
	 */
	std::shared_ptr<Dart> openQuad( const std::shared_ptr<Quad>& q,
									const std::shared_ptr<Edge>& e,
									const std::shared_ptr<Node>& n1 );

	/**
	 * Create 2 new quads from 2 specified quads, in which the common edge of the
	 * given quads has been rotated one step in the CCW direction. Delete the old
	 * quads.
	 *
	 * @param qa  one of the two quads adjacent the edge to be switched, e1a
	 * @param e1a the edge to be switched
	 * @param n   one of e1a's nodes
	 * @return a dart representing the input dart after the operation is performed.
	 */
	std::shared_ptr<Dart> switchDiagonalCCW( const std::shared_ptr<Quad>& qa,
											 const std::shared_ptr<Edge>& e1a,
											 const std::shared_ptr<Node>& n );

	/**
	 * Create 2 new quads from 2 specified quads, in which the common edge of the
	 * given quads has been rotated one step in the CW direction. Delete the old
	 * quads. Update the nodes list.
	 *
	 * @param qa  one of the two quads adjacent the edge to be switched, e1a
	 * @param e1a the edge to be switched
	 * @param n   one of e1a's nodes
	 * @return a dart representing the input dart after the operation is performed.
	 */
	std::shared_ptr<Dart> switchDiagonalCW( const std::shared_ptr<Quad>& qa,
											const std::shared_ptr<Edge>& e1a,
											const std::shared_ptr<Node>& n );
	void globalSmooth();

};
