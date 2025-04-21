#pragma once

#include "Constants.h"
#include "ArrayList.h"
#include "Element.h"
#include "Triangle.h"
#include "Node.h"
#include "Edge.h"

#include <memory>
#include <string>

class TopoCleanup;
class GlobalSmooth;

/**
 * This is a basic geometry class with methods for reading and writing meshes,
 * sorting Node lists, printing lists, topology inspection, etc.
 */

class GeomBasics :
	public Constants
{
public:
	inline static ArrayList<std::shared_ptr<Element>> elementList;
	inline static ArrayList<std::shared_ptr<Triangle>> triangleList;
	inline static ArrayList<std::shared_ptr<Node>> nodeList;
	inline static ArrayList<std::shared_ptr<Edge>> edgeList;

	inline static std::shared_ptr<Node> leftmost = nullptr, rightmost = nullptr, uppermost = nullptr, lowermost = nullptr;

	inline static bool m_step = false;

	inline static std::shared_ptr<TopoCleanup> topoCleanup = nullptr;
	inline static std::shared_ptr<GlobalSmooth> globalSmooth = nullptr;

	inline static std::string meshFilename = "";
	inline static std::string meshDirectory = ".";
	inline static bool meshLenOpt = false;
	inline static bool meshAngOpt = false;

	static void createNewLists();

	static void setParams( const std::string& filename,
						   const std::string& dir,
						   bool len, bool ang );

	/** Return the edgeList */
	static ArrayList<std::shared_ptr<Edge>> getEdgeList();

	/** Return the nodeList */
	static ArrayList<std::shared_ptr<Node>> getNodeList();

	/** Return the triangleList */
	static ArrayList<std::shared_ptr<Triangle>> getTriangleList();

	/** Return the elementList */
	static ArrayList<std::shared_ptr<Element>> getElementList();

private:
	inline static std::shared_ptr<GeomBasics> curMethod = nullptr;

public:
	static void setCurMethod( const std::shared_ptr<GeomBasics>& method );

	static const std::shared_ptr<GeomBasics>& getCurMethod();

	/** This method should be implemented in each of the subclasses. */
	virtual void step(){}

	/** Delete all the edges in the mesh. */
	static void clearEdges();

	/** Clear the nodeList, edgeList, triangleList and elementList. */
	static void clearLists();

	/** Update distortion metric for all elements in mesh. */
	static void updateMeshMetrics();

	/** @return a string containing the average and minimum element metrics. */
	static std::string meshMetricsReport();

	/** Find inverted elements and paint them with red colour. */
	static void detectInvertedElements();

	/** Output nr of tris and fake quads in mesh. */
	static void countTriangles();

	/** Output warnings if mesh is not consistent. */
	static void consistencyCheck();

	/** Load a mesh from a file. */
	static ArrayList<std::shared_ptr<Element>> loadMesh();

	/** Load a triangle mesh from a file. */
	static ArrayList<std::shared_ptr<Triangle>> loadTriangleMesh();

	/** A method to read node files. */
	static ArrayList<std::shared_ptr<Node>> loadNodes();

	/**
	 * Method for writing to a LaTeX drawing format (need the epic and eepic
	 * packages).
	 */
	static bool exportMeshToLaTeX( std::string filename,
								   int unitlength,
								   double xcorr,
								   double ycorr,
								   bool visibleNodes );

	/** Write all elements in elementList to a file. */
	static bool writeQuadMesh( const std::string& filename,
							   const ArrayList<std::shared_ptr<Element>>& list );

	/** Write all elements in elementList and triangleList to a file. */
	static bool writeMesh( const std::string& filename );

	/** Write all nodes in nodeList to a file. */
	static bool writeNodes( const std::string& filename );

	/** Find the leftmost, rightmost, uppermost, and lowermost nodes. */
	static void findExtremeNodes();

	/** Sort nodes left to right. Higher y-values are preferred to lower ones. */
	static ArrayList<std::shared_ptr<Node>> sortNodes( ArrayList<std::shared_ptr<Node>>& unsortedNodes );

private:
	inline static int cInd = 0;

	/**
	 * Method to assist the different load... methods.
	 *
	 * @param iline a comma-separated string
	 * @return the next double value from iline. If no more numbers, then return
	 *         NaN.
	 */
	static double nextDouble( const std::string& iline );

public:
	static void printVectors( const ArrayList<std::shared_ptr<MyVector>>& vectorList );

	static void printElements( const ArrayList<std::shared_ptr<Element>>& list );

	static void printTriangles( const ArrayList<std::shared_ptr<Triangle>>& triangleList );

	static void printQuads( const ArrayList<std::shared_ptr<Element>>& list );

	static void printEdgeList( const ArrayList<std::shared_ptr<Edge>>& list );

	static void printNodes( const ArrayList<std::shared_ptr<Node>>& nodeList );

	static void printValences();

	static void printValPatterns();

	static void printAnglesAtSurrondingNodes();

	/**
	 * Do inversion test and repair inversion if requiered
	 *
	 * @return true if any repairing was neccessary, else return false.
	 */
	static bool inversionCheckAndRepair( const std::shared_ptr<Node>& newN,
										 const std::shared_ptr<Node>& oldPos );

	/**
	 * Quad q is to be collapsed. Nodes n1 and n2 are two opposite nodes in q. This
	 * method tries to find a location inside the current q to which n1 and n2 can
	 * safely be relocated and joined without causing any adjacent elements to
	 * become inverted. The first candidate location is the centroid of the quad. If
	 * that location is not suitable, the method tries locations on the vectors from
	 * the centroid towards n1 and from the centroid towards n2. The first suitable
	 * location found is returned.
	 *
	 * @param q  the quad to be collapsed
	 * @param n1 the node in quad q that is to be joined with opposite node n2
	 * @param n2 the node in quad q that is to be joined with opposite node n1
	 * @return a position inside quad q to which both n1 and n2 can be relocated
	 *         without inverting any of their adjacent elements.
	 */
	static std::shared_ptr<Node> safeNewPosWhenCollapsingQuad( const std::shared_ptr<Quad>& q,
															   const std::shared_ptr<Node>& n1,
															   const std::shared_ptr<Node>& n2 );

	/**
	 * To be used only with all-triangle meshes.
	 *
	 * @return true if any zero area triangles were removed.
	 */
	bool repairZeroAreaTriangles();

	/**
	 * A method for fast computation of the cross product of two vectors.
	 *
	 * @param o1 origin of first vector
	 * @param p1 endpoint of first vector
	 * @param o2 origin of second vector
	 * @param p2 endpoint of second vector
	 * @return the cross product of the two vectors
	 */
	static double cross( const std::shared_ptr<Node>& o1,
						 const std::shared_ptr<Node>& p1,
						 const std::shared_ptr<Node>& o2,
						 const std::shared_ptr<Node>& p2 );
}; // End of class GeomBasics
