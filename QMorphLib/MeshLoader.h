#pragma once
#include "Constants.h"

#include <vector>
#include <string>
#include <array>
#include <map>

class Triangle;
class Edge;
class Node;

class MeshLoader: public Constants
{
public:
	static std::vector<Triangle*> triangleList;
	static std::vector<Edge*> edgeList;
	static std::vector<Node*> nodeList;

	static std::vector<Triangle*> loadTriangleMesh( const std::string& meshDirectory, const std::string& meshFilename );

	//
	// Loads a triangle mesh from an array of triangle coordinates. The array should
	// be in the format double[n][6], where each row represents a triangle with
	// coordinates x1, y1, x2, y2, x3, y3.
	//
	// @param triangleCoordinates A 2D array of triangle coordinates.
	// @return A list of triangles representing the mesh, or null if an error
	//         occurred (e.g., invalid input array).
	//
	static std::vector<Triangle*> loadTriangleMeshFromArray( const std::vector<std::array<double, 6>>& triangleCoordinates,
															 bool meshLenOpt, bool meshAngOpt );

private:
	static Node* getNode( std::map<Point2D, Node*>& nodeMap,
						  std::vector<Node*>& usNodeList,
						  double x, double y );

	static Edge* getEdge( std::vector<Edge*>& edgeList, Node* node1, Node* node2 );

	static double nextDouble( const std::string& inputLine );
	/*{
		int startIndex = cInd;
		while ( cInd < inputLine.length() && inputLine.charAt( cInd ) != ' ' && inputLine.charAt( cInd ) != '\t' && inputLine.charAt( cInd ) != ',' )
		{
			cInd++;
		}
		String doubleStr = inputLine.substring( startIndex, cInd ).trim();
		while ( cInd < inputLine.length() && (inputLine.charAt( cInd ) == ' ' || inputLine.charAt( cInd ) == '\t' || inputLine.charAt( cInd ) == ',') )
		{
			cInd++;
		}
		return Double.parseDouble( doubleStr );
	}*/

	static int cInd;
};