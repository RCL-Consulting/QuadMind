#include "pch.h"
#include "MeshLoader.h"

#include "Triangle.h"
#include "Node.h"
#include "Edge.h"

#include <fstream>

int MeshLoader::cInd = 0;

std::vector<Triangle*> MeshLoader::triangleList;
std::vector<Edge*> MeshLoader::edgeList;
std::vector<Node*> MeshLoader::nodeList;

class Point2D
{
public:

	Point2D( double x, double y ) : x( x ), y( y ) {}
	Point2D() : x( 0.0 ), y( 0.0 ) {}

	double x = 0.0;
	double y = 0.0;
	bool operator==( const Point2D& other ) const
	{
		const double kZero = 1e-12;
		return abs( x - other.x ) < kZero && abs( y - other.y ) < kZero;
	}
};

double
MeshLoader::nextDouble( const std::string& iline )
{
	auto List = splitString( iline, ',' );

	std::string ndouble;
	if ( cInd >= List.size() )
	{
		return std::numeric_limits<double>::quiet_NaN();
	}

	auto nInd = iline.find( ",", cInd );
	if ( nInd == std::string::npos )
	{
		nInd = iline.length();
	}

	try
	{
		++cInd;
		return std::stod( List[cInd] );
	}
	catch ( const std::invalid_argument& )
	{
		--cInd;
		throw std::runtime_error( "Invalid double value at the specified position" );
	}
	catch ( const std::out_of_range& )
	{
		--cInd;
		throw std::runtime_error( "Double value out of range at the specified position" );
	}
}

std::vector<Triangle*> 
MeshLoader::loadTriangleMesh( const std::string& meshDirectory, const std::string& meshFilename )
{
	triangleList.clear();
	edgeList.clear();
	// Use HashMap instead of ArrayList for nodes for faster lookup
	std::map<Point2D, Node*> nodeMap;
	std::vector<Node*> usNodeList; // Still keep a list to maintain order if needed, or can be removed if nodeMap
	// is sufficient.

	auto fis = std::ifstream( meshDirectory + meshFilename );
	try
	{

		std::string inputLine;
		std::getline( fis, inputLine );
		while ( fis.eof() )
		{
			cInd = 0;
			double x1 = nextDouble( inputLine );
			double y1 = nextDouble( inputLine );
			double x2 = nextDouble( inputLine );
			double y2 = nextDouble( inputLine );
			double x3 = nextDouble( inputLine );
			double y3 = nextDouble( inputLine );

			Node* node1 = getNode( nodeMap, usNodeList, x1, y1 );
			Node* node2 = getNode( nodeMap, usNodeList, x2, y2 );
			Node* node3 = getNode( nodeMap, usNodeList, x3, y3 );

			Edge* edge1 = getEdge( edgeList, node1, node2 );
			Edge* edge2 = getEdge( edgeList, node2, node3 );
			Edge* edge3 = getEdge( edgeList, node1, node3 );

			Triangle* t = new Triangle( edge1, edge2, edge3 );
			t->connectEdges();
			triangleList.push_back( t );
		}

	}
	catch ( ... )
	{
	}
	nodeList = usNodeList;
	return triangleList;
}

std::vector<Triangle*> 
MeshLoader::loadTriangleMeshFromArray( const std::vector<std::array<double, 6>>& triangleCoordinates,
									   bool meshLenOpt, bool meshAngOpt )
{
	triangleList.clear();
	edgeList.clear();
	std::map<Point2D, Node*> nodeMap;
	std::vector<Node*> usNodeList;

	for ( auto coords : triangleCoordinates )
	{
		double x1 = coords[0];
		double y1 = coords[1];
		double x2 = coords[2];
		double y2 = coords[3];
		double x3 = coords[4];
		double y3 = coords[5];

		Node* node1 = getNode( nodeMap, usNodeList, x1, y1 );
		Node* node2 = getNode( nodeMap, usNodeList, x2, y2 );
		Node* node3 = getNode( nodeMap, usNodeList, x3, y3 );

		Edge* edge1 = getEdge( edgeList, node1, node2 );
		Edge* edge2 = getEdge( edgeList, node2, node3 );
		Edge* edge3 = getEdge( edgeList, node1, node3 );

		Triangle* t = new Triangle( edge1, edge2, edge3 );
		t->connectEdges();
		triangleList.push_back( t );
	}
	nodeList = usNodeList; // Assign to class level nodeList if needed
	return triangleList;
}

Node* 
MeshLoader::getNode( std::map<Point2D, Node*>& nodeMap,
					 std::vector<Node*>& usNodeList,
					 double x, double y )
{
	Point2D point( x, y );
	Node* node = nodeMap.at( point );
	if ( node == nullptr )
	{
		node = new Node( x, y );
		nodeMap[point] = node;
		usNodeList.push_back( node );
	}
	return node;
}

Edge*
MeshLoader::getEdge( std::vector<Edge*>& edgeList, Node* node1, Node* node2 )
{
	Edge* edge = new Edge( node1, node2 );

	auto index = std::find( edgeList.begin(), edgeList.end(), edge );
	if ( index == edgeList.end() )
	{
		edgeList.push_back( edge );
	}
	else
	{
		edge = *index;
	}
	edge->leftNode->connectToEdge( edge );
	edge->rightNode->connectToEdge( edge );
	return edge;
}