#include "pch.h"
#include "MeshLoader.h"	

#include "Triangle.h"
#include "Edge.h"
#include "Node.h"

#include <fstream>

// Define static member variables
ArrayList<std::shared_ptr<Triangle>> MeshLoader::triangleList;
ArrayList<std::shared_ptr<Edge>> MeshLoader::edgeList;
ArrayList<std::shared_ptr<Node>> MeshLoader::nodeList;

// Trim from start (in place)
void ltrim( std::string& s )
{
	s.erase( s.begin(), std::find_if( s.begin(), s.end(),
									  []( unsigned char ch ) { return !std::isspace( ch ); } ) );
}

// Trim from end (in place)
void rtrim( std::string& s )
{
	s.erase( std::find_if( s.rbegin(), s.rend(),
						   []( unsigned char ch ) { return !std::isspace( ch ); } ).base(), s.end() );
}

// Trim from both ends (in place)
void trim( std::string& s )
{
	ltrim( s );
	rtrim( s );
}

//TODO: Tests
ArrayList<std::shared_ptr<Triangle>>
MeshLoader::loadTriangleMesh( const std::string& meshDirectory,
							  const std::string& meshFilename )
{
	triangleList.clear();
	edgeList.clear();
	// Use HashMap instead of ArrayList for nodes for faster lookup
	std::map<Point2D, std::shared_ptr<Node>> nodeMap;
	ArrayList<std::shared_ptr<Node>> usNodeList; // Still keep a list to maintain order if needed, or can be removed if nodeMap
	// is sufficient.

	try 
	{
		std::ifstream fis( meshDirectory + meshFilename );
		std::string inputLine;
		std::getline( fis, inputLine ); 
		while ( !fis.eof() )
		{
			cInd = 0;
			double x1 = nextDouble( inputLine );
			double y1 = nextDouble( inputLine );
			double x2 = nextDouble( inputLine );
			double y2 = nextDouble( inputLine );
			double x3 = nextDouble( inputLine );
			double y3 = nextDouble( inputLine );

			auto node1 = getNode( nodeMap, usNodeList, x1, y1 );
			auto node2 = getNode( nodeMap, usNodeList, x2, y2 );
			auto node3 = getNode( nodeMap, usNodeList, x3, y3 );

			auto edge1 = getEdge( edgeList, node1, node2 );
			auto edge2 = getEdge( edgeList, node2, node3 );
			auto edge3 = getEdge( edgeList, node1, node3 );

			auto t = std::make_shared<Triangle>( edge1, edge2, edge3 );
			t->connectEdges();
			triangleList.add( t );

			std::getline( fis, inputLine );
		}

	}
	catch ( ... )
	{
	}
	nodeList = usNodeList;
	return triangleList;
}

//TODO: Tests
ArrayList<std::shared_ptr<Triangle>>
MeshLoader::loadTriangleMeshFromArray( const std::vector<std::array<double, 6>>& triangleCoordinates,
									   bool meshLenOpt,
									   bool meshAngOpt )
{
	triangleList.clear();
	edgeList.clear();
	std::map<Point2D, std::shared_ptr<Node>> nodeMap;
	ArrayList<std::shared_ptr<Node>> usNodeList;

	for ( auto& coords : triangleCoordinates )
	{
		double x1 = coords[0];
		double y1 = coords[1];
		double x2 = coords[2];
		double y2 = coords[3];
		double x3 = coords[4];
		double y3 = coords[5];

		auto node1 = getNode( nodeMap, usNodeList, x1, y1 );
		auto node2 = getNode( nodeMap, usNodeList, x2, y2 );
		auto node3 = getNode( nodeMap, usNodeList, x3, y3 );

		auto edge1 = getEdge( edgeList, node1, node2 );
		auto edge2 = getEdge( edgeList, node2, node3 );
		auto edge3 = getEdge( edgeList, node1, node3 );

		auto t = std::make_shared<Triangle>( edge1, edge2, edge3 );
		t->connectEdges();
		triangleList.add( t );
	}
	nodeList = usNodeList; // Assign to class level nodeList if needed
	return triangleList;
}

//TODO: Tests
std::shared_ptr<Node>
MeshLoader::getNode( std::map<Point2D, std::shared_ptr<Node>>& nodeMap,
					 ArrayList<std::shared_ptr<Node>>& usNodeList,
					 double x, double y )
{
	Point2D point( x, y );
	auto iter = nodeMap.find( point );
	if ( iter == nodeMap.end() )
	{
		auto node = std::make_shared<Node>( x, y );
		nodeMap[point] = node;
		usNodeList.add( node );
	}
	return iter->second;
}

//TODO: Tests
std::shared_ptr<Edge>
MeshLoader::getEdge( ArrayList<std::shared_ptr<Edge>>& edgeList,
					 const std::shared_ptr<Node>& node1,
					 const std::shared_ptr<Node>& node2 )
{
	auto edge = std::make_shared<Edge>( node1, node2 );
	auto index = edgeList.indexOf( edge ); // Assuming Edge class has proper equals() and hashCode()
	if ( index == -1 )
	{
		edgeList.add( edge );
	}
	else
	{
		edge = edgeList.get( index );
	}
	edge->leftNode->connectToEdge( edge );
	edge->rightNode->connectToEdge( edge );
	return edge;
}

//TODO: Tests
double
MeshLoader::nextDouble( const std::string& inputLine )
{
	int startIndex = cInd;
	while ( cInd < inputLine.length() && inputLine[cInd] != ' ' && inputLine[cInd] != '\t' && inputLine[cInd] != ',' )
	{
		cInd++;
	}
	auto doubleStr = inputLine.substr( startIndex, cInd );
	trim( doubleStr );
	while ( cInd < inputLine.length() && (inputLine[cInd] == ' ' || inputLine[cInd] == '\t' || inputLine[cInd] == ',') )
	{
		cInd++;
	}
	return std::stod( doubleStr );
}