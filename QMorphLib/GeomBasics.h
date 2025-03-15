#pragma once

/**
 * This is a basic geometry class with methods for reading and writing meshes,
 * sorting Node lists, printing lists, topology inspection, etc.
 */

#include "Constants.h"
#include "Msg.h"

#include <vector>
#include <string>

class Element;
class Triangle;
class Quad;
class Node;
class Edge;
class TopoCleanup;
class GlobalSmooth;
class MyVector;

class GeomBasics : public Constants
{
public:
	static std::vector<Element*> elementList;
	static std::vector<Triangle*> triangleList;
	static std::vector<Node*> nodeList;
	static std::vector<Edge*> edgeList;

	static Node* leftmost, * rightmost, * uppermost, * lowermost;

	static bool m_step;

	static TopoCleanup* topoCleanup;
	static GlobalSmooth* globalSmooth;

	static std::string meshFilename;
	static std::string meshDirectory;
	static bool meshLenOpt;
	static bool meshAngOpt;

	static void createNewLists();

	static void setParams( const std::string& filename,
						   const std::string& dir,
						   bool len, bool ang );

	// Return the edgeList //
	static std::vector<Edge*>& getEdgeList();

	// Return the nodeList //
	static std::vector<Node*>& getNodeList();

	// Return the triangleList //
	static std::vector<Triangle*>& getTriangleList();

	// Return the elementList /
	static std::vector<Element*>& getElementList();

private:
	static GeomBasics* curMethod;
	static size_t cInd;

public:
	static void setCurMethod( GeomBasics* method );

	static GeomBasics* getCurMethod();

	// This method should be implemented in each of the subclasses. //
	virtual void step()	{}

	// Delete all the edges in the mesh. //
	static void clearEdges();

	// Clear the nodeList, edgeList, triangleList and elementList. //
	static void clearLists();

	// Update distortion metric for all elements in mesh. //
	static void updateMeshMetrics();

	// @return a string containing the average and minimum element metrics. //
	static std::string meshMetricsReport();

	// Find inverted elements and paint them with red colour. //
	static void detectInvertedElements();

	// Output nr of tris and fake quads in mesh. //
	static void countTriangles();

	// Output warnings if mesh is not consistent. //
	static void consistencyCheck();

	// Load a mesh from a file. //
	static std::vector<Element*> loadMesh();

	// Load a triangle mesh from a file. //
	static std::vector<Triangle*> loadTriangleMesh();

	// A method to read node files. //
	static std::vector<Node*> loadNodes();

	//
	// Method for writing to a LaTeX drawing format (need the epic and eepic
	// packages).
	//
	static bool exportMeshToLaTeX( const std::string& filename,
								   int unitlength,
								   double xcorr,
								   double ycorr,
								   bool visibleNodes );

	// Write all elements in elementList to a file. //
	static bool writeQuadMesh( const std::string& filename,
							   std::vector<Element*>& list );

	// Write all elements in elementList and triangleList to a file. //
	static bool writeMesh( const std::string& filename );

	// Write all nodes in nodeList to a file. //
	static bool writeNodes( const std::string& filename );

	// Find the leftmost, rightmost, uppermost, and lowermost nodes. //
	static void findExtremeNodes();

	// Sort nodes left to right. Higher y-values are preferred to lower ones. //
	static std::vector<Node*> sortNodes( std::vector<Node*>& unsortedNodes );

	//
	// Method to assist the different load... methods.
	//
	// @param iline a comma-separated string
	// @return the next double value from iline. If no more numbers, then return
	//         NaN.
	//
private:
	static double nextDouble( const std::string& iline );

public:
	static void printVectors( const std::vector<MyVector>& vectorList );

	template <typename T>
	static void printElements( const std::vector<T*>& list )
	{
		if ( Msg::debugMode )
		{
			for ( auto element : list )
				element->printMe();
		}
	}

	static void printTriangles( const std::vector<Triangle*>& triangleList );

	static void printQuads( const std::vector<Element*> list );

	static void printEdgeList( const std::vector<Edge*>& list );

	static void printNodes( const std::vector<Node*>& nodeList );

	static void printValences();

	static void printValPatterns();

	static void printAnglesAtSurrondingNodes();

	//
	// Do inversion test and repair inversion if requiered
	//
	// @return true if any repairing was neccessary, else return false.
	//
	static bool inversionCheckAndRepair( Node* newN, Node* oldPos );

	//
	// Quad q is to be collapsed. Nodes n1 and n2 are two opposite nodes in q. This
	// method tries to find a location inside the current q to which n1 and n2 can
	// safely be relocated and joined without causing any adjacent elements to
	// become inverted. The first candidate location is the centroid of the quad. If
	// that location is not suitable, the method tries locations on the vectors from
	// the centroid towards n1 and from the centroid towards n2. The first suitable
	// location found is returned.
	//
	// @param q  the quad to be collapsed
	// @param n1 the node in quad q that is to be joined with opposite node n2
	// @param n2 the node in quad q that is to be joined with opposite node n1
	// @return a position inside quad q to which both n1 and n2 can be relocated
	//         without inverting any of their adjacent elements.
	//
	static Node* safeNewPosWhenCollapsingQuad( Quad* q, Node* n1, Node* n2 );

	//
	// To be used only with all-triangle meshes.
	//
	// @return true if any zero area triangles were removed.
	//
	bool repairZeroAreaTriangles();

	//
	// A method for fast computation of the cross product of two vectors.
	//
	// @param o1 origin of first vector
	// @param p1 endpoint of first vector
	// @param o2 origin of second vector
	// @param p2 endpoint of second vector
	// @return the cross product of the two vectors
	//
	static double cross( const Node& o1, const Node& p1, const Node& o2, const Node& p2 );
}; // End of class GeomBasics
