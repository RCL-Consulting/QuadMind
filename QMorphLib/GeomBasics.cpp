#include "pch.h"
#include "GeomBasics.h"

#include "Element.h"
#include "Edge.h"
#include "Node.h"
#include "Triangle.h"
#include "Quad.h"
#include "MyVector.h"

#include <fstream>

std::vector<Element*> GeomBasics::elementList;
std::vector<Triangle*> GeomBasics::triangleList;
std::vector<Node*> GeomBasics::nodeList;
std::vector<Edge*> GeomBasics::edgeList;

Node* GeomBasics::leftmost = nullptr;
Node* GeomBasics::rightmost = nullptr;
Node* GeomBasics::uppermost = nullptr;
Node* GeomBasics::lowermost = nullptr;

bool GeomBasics::m_step = false;

size_t GeomBasics::cInd = 0;

GeomBasics* GeomBasics::curMethod = nullptr;
TopoCleanup* GeomBasics::topoCleanup = nullptr;
GlobalSmooth* GeomBasics::globalSmooth = nullptr;

std::string GeomBasics::meshFilename = "";
std::string GeomBasics::meshDirectory = ".";

bool GeomBasics::meshLenOpt = false;
bool GeomBasics::meshAngOpt = false;

void
GeomBasics::createNewLists()
{
	elementList = std::vector<Element*>();
	triangleList = std::vector<Triangle*>();
	edgeList = std::vector<Edge*>();
	nodeList = std::vector<Node*>();
}

void
GeomBasics::setParams( const std::string& filename,
					   const std::string& dir,
					   bool len, bool ang )
{
	meshFilename = filename;
	meshDirectory = dir;
	meshLenOpt = len;
	meshAngOpt = ang;
}

std::vector<Edge*>&
GeomBasics::getEdgeList()
{
	return edgeList;
}

std::vector<Node*>& 
GeomBasics::getNodeList()
{
	return nodeList;
}

std::vector<Triangle*>&
GeomBasics::getTriangleList()
{
	return triangleList;
}

std::vector<Element*>&
GeomBasics::getElementList()
{
	return elementList;
}

void 
GeomBasics::setCurMethod( GeomBasics* method )
{
	curMethod = method;
}

GeomBasics*
GeomBasics::getCurMethod()
{
	return curMethod;
}

void 
GeomBasics::clearEdges()
{
	for ( auto curElem : elementList )
		curElem->disconnectEdges();

	for ( auto curEdge : edgeList )
		curEdge->disconnectNodes();

	for ( auto curNode : nodeList )
	{
		curNode->edgeList.clear();
	}

	std::for_each( elementList.begin(), elementList.end(), []( Element* e ) { delete e; } );
	elementList.clear();

	std::for_each( triangleList.begin(), triangleList.end(), []( Triangle* t ) { delete t; } );
	triangleList.clear();

	std::for_each( edgeList.begin(), edgeList.end(), []( Edge* e ) { delete e; } );
	edgeList.clear();
}

void 
GeomBasics::clearLists()
{
	std::for_each( nodeList.begin(), nodeList.end(), []( Node* n ) { delete n; } );
	nodeList.clear();
	
	std::for_each( edgeList.begin(), edgeList.end(), []( Edge* e ) { delete e; } );
	edgeList.clear();

	std::for_each( triangleList.begin(), triangleList.end(), []( Triangle* t ) { delete t; } );
	triangleList.clear();

	std::for_each( elementList.begin(), elementList.end(), []( Element* e ) { delete e; } );
	elementList.clear();
}

void
GeomBasics::updateMeshMetrics()
{
	if ( elementList.size() > 0 )
	{
		for ( auto element : elementList )
		{
			if ( element )
			{
				element->updateDistortionMetric();
			}
		}
	}
	else
	{
		for ( auto element : triangleList )
		{
			auto tri = static_cast<Triangle*>(element);
			if ( tri )
			{
				tri->updateDistortionMetric();
			}
		}
	}
}

std::string
GeomBasics::meshMetricsReport()
{
	double sumMetric = 0.0, minDM = std::numeric_limits<double>::max();//java.lang.Double.MAX_VALUE;
	int size = 0, nTris = 0, nQuads = 0;
	std::string s;
	uint8_t temp, no2valents = 0, no3valents = 0, no4valents = 0, no5valents = 0, no6valents = 0, noXvalents = 0;

	if ( elementList.size() > 0 )
	{

		for ( auto elem : elementList )
		{
			if ( elem != nullptr )
			{
				size++;
				if ( elem->IsATriangle() )
				{
					nTris++;
				}
				else if ( elem->IsAQuad() )
				{
					auto q = static_cast<Quad*>(elem);
					if ( q->isFake )
					{
						nTris++;
					}
					else
					{
						nQuads++;
					}
				}
				elem->updateDistortionMetric();
				sumMetric += elem->distortionMetric;
				if ( elem->distortionMetric < minDM )
				{
					minDM = elem->distortionMetric;
				}
			}
		}
	}

	if ( triangleList.size() > 0 )
	{
		for ( auto tri : triangleList )
		{
			if ( tri != nullptr )
			{
				size++;
				nTris++;
				tri->updateDistortionMetric();
				sumMetric += tri->distortionMetric;
				if ( tri->distortionMetric < minDM )
				{
					minDM = tri->distortionMetric;
				}
			}
		}

	}

	s = "Average distortion metric: " + std::to_string( sumMetric / size ) + "\n" + "Minimum distortion metric: " + std::to_string( minDM ) + "\n";

	for ( auto* n : nodeList )
	{

		temp = n->valence();
		if ( temp == 2 )
		{
			no2valents++;
		}
		else if ( temp == 3 )
		{
			no3valents++;
		}
		else if ( temp == 4 )
		{
			no4valents++;
		}
		else if ( temp == 5 )
		{
			no5valents++;
		}
		else if ( temp == 6 )
		{
			no6valents++;
		}
		else if ( temp > 6 )
		{
			noXvalents++;
		}
	}

	if ( no2valents > 0 )
	{
		s = s + "Number of 2-valent nodes: " + std::to_string( no2valents ) + "\n";
	}
	if ( no3valents > 0 )
	{
		s = s + "Number of 3-valent nodes: " + std::to_string( no3valents ) + "\n";
	}
	if ( no4valents > 0 )
	{
		s = s + "Number of 4-valent nodes: " + std::to_string( no4valents ) + "\n";
	}
	if ( no5valents > 0 )
	{
		s = s + "Number of 5-valent nodes: " + std::to_string( no5valents ) + "\n";
	}
	if ( no6valents > 0 )
	{
		s = s + "Number of 6-valent nodes: " + std::to_string( no6valents ) + "\n";
	}
	if ( noXvalents > 0 )
	{
		s = s + "Number of nodes with valence > 6: " + std::to_string( noXvalents ) + "\n";
	}

	s = s +
		"Number of quadrilateral elements: " + std::to_string( nQuads ) + "\n" +
		"Number of triangular elements: " + std::to_string( nTris ) + "\n" +
		"Number of edges: " + std::to_string( edgeList.size() ) + "\n" +
		"Number of nodes: " + std::to_string( nodeList.size() );

	return s;
}

void
GeomBasics::detectInvertedElements()
{
	for ( auto elem : elementList )
	{
		if ( elem != nullptr && elem->inverted() )
		{
			elem->markEdgesIllegal();
			Msg::warning( "Element " + elem->descr() + " is inverted." );
			Msg::warning( "It has firstNode " + elem->firstNode->descr() );
		}
	}

	for ( auto t : triangleList )
	{
		if ( t != nullptr && t->inverted() )
		{
			t->markEdgesIllegal();
			Msg::warning( "Triangle " + t->descr() + " is inverted." );
			Msg::warning( "It has firstNode " + t->firstNode->descr() );
		}
	}
}

void
GeomBasics::countTriangles()
{
	int fakes = 0, tris = 0;
	for ( auto elem : elementList )
	{
		if ( elem->IsAQuad() && static_cast<Quad*>(elem)->isFake )
		{
			fakes++;
		}
		else if ( elem->IsATriangle() )
		{
			tris++;
		}
	}

	Msg::debug( "Counted # of fake quads: " + fakes );
	Msg::debug( "Counted # of triangles: " + tris );
}

void
GeomBasics::consistencyCheck()
{
	Msg::debug( "Entering consistencyCheck()" );
	Node* n;
	for ( int i = 0; i < nodeList.size(); i++ )
	{
		n = nodeList.at( i );

		Edge* e;
		if ( n->edgeList.size() == 0 )
		{
			Msg::warning( "edgeList.size()== 0 for node " + n->descr() );
		}

		for ( int j = 0; j < n->edgeList.size(); j++ )
		{
			e = n->edgeList.at( j );
			if ( e == nullptr )
			{
				Msg::warning( "Node " + n->descr() + " has a null in its edgeList." );
			}
			else if( std::find(edgeList.begin(), edgeList.end(),e)==edgeList.end() )
			{
				Msg::warning( "Edge " + e->descr() + " found in the edgeList of Node " + n->descr() + ", but not in global edgeList" );
			}
		}
	}

	Edge* e;
	for ( int i = 0; i < edgeList.size(); i++ )
	{
		e = edgeList.at( i );
		if( std::find( e->leftNode->edgeList.begin(), e->leftNode->edgeList.end(), e ) == e->leftNode->edgeList.end() )
		{
			Msg::warning( "leftNode of edge " + e->descr() + " has not got that edge in its .edgeList" );
		}
		if ( std::find( e->rightNode->edgeList.begin(), e->rightNode->edgeList.end(), e ) == e->rightNode->edgeList.end() )
		{
			Msg::warning( "rightNode of edge " + e->descr() + " has not got that edge in its .edgeList" );
		}

		if ( e->element1 == nullptr && e->element2 == nullptr )
		{
			Msg::warning( "Edge " + e->descr() + " has null in both element pointers" );
		}

		if ( e->element1 == nullptr && e->element2 != nullptr )
		{
			Msg::warning( "Edge " + e->descr() + " has null in element1 pointer" );
		}

		if ( e->element1 != nullptr &&
			 !contains( triangleList, static_cast<Triangle*>(e->element1) ) &&
			 !contains( elementList, e->element1 ) )
		{
			Msg::warning( "element1 of edge " + e->descr() + " is not found in triangleList or elementList" );
		}

		if ( e->element2 != nullptr && 
			 !contains( triangleList, static_cast<Triangle*>(e->element2) ) &&
			 !contains( elementList, e->element2 ) )
		{
			Msg::warning( "element2 of edge " + e->descr() + " is not found in triangleList or elementList" );
		}

		if( !contains( nodeList, e->leftNode) )
		{
			Msg::warning( "leftNode of edge " + e->descr() + " not found in nodeList." );
		}
		if ( !contains( nodeList, e->rightNode ) )
		{
			Msg::warning( "rightNode of edge " + e->descr() + " not found in nodeList." );
		}
	}

	Node* na, *nb, *nc;
	double cross1;

	for ( auto t : triangleList )
	{
		if ( !t->edgeList[0]->hasElement( t ) )
		{
			Msg::warning( "edgeList[0] of triangle " + t->descr() + " has not got that triangle as an adjacent element" );
		}

		if ( !t->edgeList[1]->hasElement( t ) )
		{
			Msg::warning( "edgeList[1] of triangle " + t->descr() + " has not got that triangle as an adjacent element" );
		}

		if ( !t->edgeList[2]->hasElement( t ) )
		{
			Msg::warning( "edgeList[2] of triangle " + t->descr() + " has not got that triangle as an adjacent element" );
		}

		if ( t->edgeList[0]->commonNode( t->edgeList[1] ) == nullptr )
		{
			Msg::warning( "edgeList[0] and edgeList[1] of triangle " + t->descr() + " has no common Node" );
		}
		if ( t->edgeList[1]->commonNode( t->edgeList[2] ) == nullptr )
		{
			Msg::warning( "edgeList[1] and edgeList[2] of triangle " + t->descr() + " has no common Node" );
		}
		if ( t->edgeList[2]->commonNode( t->edgeList[0] ) == nullptr )
		{
			Msg::warning( "edgeList[2] and edgeList[0] of triangle " + t->descr() + " has no common Node" );
		}

		na = t->edgeList[0]->leftNode;
		nb = t->edgeList[0]->rightNode;
		nc = t->oppositeOfEdge( t->edgeList[0] );

		cross1 = cross( *na, *nc, *nb, *nc ); // The cross product nanc x nbnc

		if ( cross1 == 0 )
		{
			Msg::warning( "Degenerate triangle in triangleList, t= " + t->descr() );
		}
	}

	for ( auto element : elementList )
	{
		if ( element == nullptr )
		{
			Msg::debug( "elementList has a null-entry." );
		}
		else if ( element->IsAQuad() )
		{
			auto q = static_cast<Quad*>(element);

			if ( !q->edgeList[base]->hasElement( q ) )
			{
				Msg::warning( "edgeList[base] of quad " + q->descr() + " has not got that quad as an adjacent element" );
			}

			if ( !q->edgeList[left]->hasElement( q ) )
			{
				Msg::warning( "edgeList[left] of quad " + q->descr() + " has not got that quad as an adjacent element" );
			}

			if ( !q->edgeList[right]->hasElement( q ) )
			{
				Msg::warning( "edgeList[right] of quad " + q->descr() + " has not got that quad as an adjacent element" );
			}

			if ( !q->isFake && !q->edgeList[top]->hasElement( q ) )
			{
				Msg::warning( "edgeList[top] of quad " + q->descr() + " has not got that quad as an adjacent element" );
			}

			if ( q->edgeList[base]->commonNode( q->edgeList[left] ) == nullptr )
			{
				Msg::warning( "edgeList[base] and edgeList[left] of quad " + q->descr() + " has no common Node" );
			}
			if ( q->edgeList[base]->commonNode( q->edgeList[right] ) == nullptr )
			{
				Msg::warning( "edgeList[base] and edgeList[right] of quad " + q->descr() + " has no common Node" );
			}
			if ( !q->isFake && q->edgeList[left]->commonNode( q->edgeList[top] ) == nullptr )
			{
				Msg::warning( "edgeList[left] and edgeList[top] of quad " + q->descr() + " has no common Node" );
			}
			if ( !q->isFake && q->edgeList[right]->commonNode( q->edgeList[top] ) == nullptr )
			{
				Msg::warning( "edgeList[right] and edgeList[top] of quad " + q->descr() + " has no common Node" );
			}

			if ( q->isFake && q->edgeList[left]->commonNode( q->edgeList[right] ) == nullptr )
			{
				Msg::warning( "edgeList[left] and edgeList[right] of fake quad " + q->descr() + " has no common Node" );
			}
		}
	}

	Msg::debug( "Leaving consistencyCheck()" );

}

double 
GeomBasics::cross( const Node& o1, const Node& p1, const Node& o2, const Node& p2 )
{
	double x1 = p1.x - o1.x;
	double x2 = p2.x - o2.x;
	double y1 = p1.y - o1.y;
	double y2 = p2.y - o2.y;
	return x1 * y2 - x2 * y1;
}

std::vector<Element*>
GeomBasics::loadMesh()
{
	Node* node1, *node2, *node3, *node4;
	Edge* edge1, *edge2, *edge3, *edge4;
	Triangle* t;
	Quad* q;

	elementList.clear();
	triangleList.clear();
	edgeList.clear();
	std::vector<Node*> usNodeList;

	try
	{
		std::ifstream fis( meshDirectory + "\\" + meshFilename);
		//BufferedReader in = new BufferedReader( new InputStreamReader( fis ) );
		double x1, x2, x3, x4, y1, y2, y3, y4;
		int i = 0;

		try
		{
			std::string inputLine;
			std::getline( fis, inputLine );
			while ( !fis.eof() )
			{
				cInd = 0;
				x1 = nextDouble( inputLine );
				y1 = nextDouble( inputLine );
				x2 = nextDouble( inputLine );
				y2 = nextDouble( inputLine );
				x3 = nextDouble( inputLine );
				y3 = nextDouble( inputLine );
				x4 = nextDouble( inputLine );
				y4 = nextDouble( inputLine );

				node1 = new Node( x1, y1 );
				auto NodeIter = find_equal( usNodeList, node1 );
				if ( NodeIter == usNodeList.end() )
				{
					usNodeList.push_back( node1 );
				}
				else
				{
					delete node1;
					node1 = *NodeIter;
				}

				node2 = new Node( x2, y2 );
				NodeIter = find_equal( usNodeList, node2 );
				if ( NodeIter == usNodeList.end() )
				{
					usNodeList.push_back( node2 );
				}
				else
				{
					delete node2;
					node2 = *NodeIter;
				}

				node3 = new Node( x3, y3 );
				NodeIter = find_equal( usNodeList, node3 );
				if ( NodeIter == usNodeList.end() )
				{
					usNodeList.push_back( node3 );
				}
				else
				{
					delete node3;
					node3 = *NodeIter;
				}
				
				edge1 = new Edge( node1, node2 );
				auto EdgeIter = find_equal( edgeList, edge1 );
				if ( EdgeIter == edgeList.end() )
				{
					edgeList.push_back( edge1 );
					edge1->connectNodes();
				}
				else
				{
					delete edge1;
					edge1 = *EdgeIter;
				}
				
				edge2 = new Edge( node1, node3 );
				EdgeIter = find_equal( edgeList, edge2 );
				if ( EdgeIter == edgeList.end() )
				{
					edgeList.push_back( edge2 );
					edge2->connectNodes();
				}
				else
				{
					delete edge2;
					edge2 = *EdgeIter;
				}
				
				if ( !std::isnan( x4 ) && !std::isnan( y4 ) )
				{
					node4 = new Node( x4, y4 );
					auto NodeIter = find_equal( usNodeList, node4 );
					if ( NodeIter == usNodeList.end() )
					{
						usNodeList.push_back( node4 );
					}
					else
					{
						delete node4;
						node4 = *NodeIter;
					}

					edge3 = new Edge( node2, node4 );
					EdgeIter = find_equal( edgeList, edge3 );
					if ( EdgeIter == edgeList.end() )
					{
						edgeList.push_back( edge3 );
						edge3->connectNodes();
					}
					else
					{
						delete edge3;
						edge3 = *EdgeIter;
					}

					edge4 = new Edge( node3, node4 );
					EdgeIter = find_equal( edgeList, edge4 );
					if ( EdgeIter == edgeList.end() )
					{
						edgeList.push_back( edge4 );
						edge4->connectNodes();
					}
					else
					{
						delete edge4;
						edge4 = *EdgeIter;
					}

					q = new Quad( edge1, edge2, edge3, edge4 );
					q->connectEdges();
					elementList.push_back( q );
				}
				else
				{
					edge3 = new Edge( node2, node3 );
					auto EdgeIter = find_equal( edgeList, edge3 );
					if ( EdgeIter == edgeList.end() )
					{
						edgeList.push_back( edge3 );
						edge3->connectNodes();
					}
					else
					{
						delete edge3;
						edge3 = *EdgeIter;
					}

					t = new Triangle( edge1, edge2, edge3 );
					t->connectEdges();
					triangleList.push_back( t );
				}
				std::getline( fis, inputLine );
			}
		}
		catch ( ... )
		{
			Msg::error( "Cannot read triangle-mesh data." );
		}
	}
	catch ( ... )
	{
		Msg::error( "File " + meshFilename + " not found." );
	}

	nodeList = usNodeList; // sortNodes(usNodeList);
	return elementList;
}

double 
GeomBasics::nextDouble( const std::string& iline )
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
		return std::stod( List[cInd++] );
	}
	catch ( const std::invalid_argument& )
	{
		throw std::runtime_error( "Invalid double value at the specified position" );
	}
	catch ( const std::out_of_range& )
	{
		throw std::runtime_error( "Double value out of range at the specified position" );
	}
}

void
GeomBasics::printVectors( const std::vector<MyVector>& vectorList )
{
	if ( Msg::debugMode )
	{
		for ( MyVector v : vectorList )
		{
			v.printMe();
		}
	}
}

void
GeomBasics::printTriangles( const std::vector<Triangle*>& triangleList )
{
	Msg::debug( "triangleList: (size== " + std::to_string( triangleList.size() ) + ")" );
	printElements( triangleList );
}

void
GeomBasics::printQuads( const std::vector<Element*> list )
{
	Msg::debug( "quadList: (size== " + std::to_string( list.size() ) + ")" );
	printElements( list );
}

std::vector<Triangle*>
GeomBasics::loadTriangleMesh()
{
	Node* node1, *node2, *node3;
	Edge* edge1, *edge2, *edge3;
	Triangle* t;

	triangleList.clear();
	edgeList.clear();
	std::vector<Node*> usNodeList;

	try
	{
		std::ifstream fis( meshDirectory + meshFilename );
		double x1, x2, x3, y1, y2, y3, len1 = 0, len2 = 0, len3 = 0, ang1 = 0, ang2 = 0, ang3 = 0;

		try
		{
			std::string inputLine;
			std::getline( fis, inputLine );
			while ( !fis.eof() )
			{
				cInd = 0;
				x1 = nextDouble( inputLine );
				y1 = nextDouble( inputLine );
				x2 = nextDouble( inputLine );
				y2 = nextDouble( inputLine );
				x3 = nextDouble( inputLine );
				y3 = nextDouble( inputLine );

				node1 = new Node( x1, y1 );
				auto NodeIter = find_equal( usNodeList, node1 );
				if ( NodeIter == usNodeList.end() )
				{
					usNodeList.push_back( node1 );
				}
				else
				{
					delete node1;
					node1 = *NodeIter;
				}
				
				node2 = new Node( x2, y2 );
				NodeIter = find_equal( usNodeList, node2 );
				if ( NodeIter == usNodeList.end() )
				{
					usNodeList.push_back( node2 );
				}
				else
				{
					delete node2;
					node2 = *NodeIter;
				}
				
				node3 = new Node( x3, y3 );
				NodeIter = find_equal( usNodeList, node3 );
				if ( NodeIter != usNodeList.end() )
				{
					usNodeList.push_back( node3 );
				}
				else
				{
					delete node3;
					node3 = *NodeIter;
				}

				edge1 = new Edge( node1, node2 );
				auto EdgeIter = find_equal( edgeList, edge1 );
				if ( EdgeIter != edgeList.end() )
				{
					edgeList.push_back( edge1 );
				}
				else
				{
					delete edge1;
					edge1 = *EdgeIter;
				}
				edge1->leftNode->connectToEdge( edge1 );
				edge1->rightNode->connectToEdge( edge1 );

				edge2 = new Edge( node2, node3 );
				EdgeIter = find_equal( edgeList, edge2 );
				if ( EdgeIter != edgeList.end() )
				{
					edgeList.push_back( edge2 );
				}
				else
				{
					delete edge2;
					edge2 = *EdgeIter;
				}
				edge2->leftNode->connectToEdge( edge2 );
				edge2->rightNode->connectToEdge( edge2 );

				edge3 = new Edge( node1, node3 );
				EdgeIter = find_equal( edgeList, edge3 );
				if ( EdgeIter != edgeList.end() )
				{
					edgeList.push_back( edge3 );
				}
				else
				{
					delete edge3;
					edge3 = *EdgeIter;
				}
				edge3->leftNode->connectToEdge( edge3 );
				edge3->rightNode->connectToEdge( edge3 );

				if ( meshLenOpt )
				{
					len1 = nextDouble( inputLine );
					len2 = nextDouble( inputLine );
					len3 = nextDouble( inputLine );
				}

				if ( meshAngOpt )
				{
					ang1 = nextDouble( inputLine );
					ang2 = nextDouble( inputLine );
					ang3 = nextDouble( inputLine );
				}
				t = new Triangle( edge1, edge2, edge3, len1, len2, len3, ang1, ang2, ang3, meshLenOpt, meshAngOpt );
				t->connectEdges();
				triangleList.push_back( t );
				std::getline( fis, inputLine );
			}
		}
		catch ( ... )
		{
			Msg::error( "Cannot read triangle-mesh data." );
		}
	}
	catch ( ... )
	{
		Msg::error( "File " + meshFilename + " not found." );
	}
	nodeList = usNodeList; // sortNodes(usNodeList);
	return triangleList;
}

std::vector<Node*> 
GeomBasics::loadNodes()
{
	Node* node1, *node2, *node3, *node4;
	std::vector<Node*> usNodeList;

	try
	{
		std::ifstream fis( meshDirectory + meshFilename );
		double x1, x2, x3, y1, y2, y3, x4, y4;

		try
		{
			std::string inputLine;
			std::getline( fis, inputLine );
			while ( fis.eof() )
			{
				cInd = 0;
				x1 = nextDouble( inputLine );
				y1 = nextDouble( inputLine );
				x2 = nextDouble( inputLine );
				y2 = nextDouble( inputLine );
				x3 = nextDouble( inputLine );
				y3 = nextDouble( inputLine );
				x4 = nextDouble( inputLine );
				y4 = nextDouble( inputLine );

				if ( !std::isnan( x1 ) && !std::isnan( y1 ) )
				{
					node1 = new Node( x1, y1 );
					if ( !contains( usNodeList, node1 ) )
					{
						usNodeList.push_back( node1 );
					}
				}
				if ( !std::isnan( x2 ) && !std::isnan( y2 ) )
				{
					node2 = new Node( x2, y2 );
					if ( !contains( usNodeList, node2 ) )
					{
						usNodeList.push_back( node2 );
					}
				}
				if ( !std::isnan( x3 ) && !std::isnan( y3 ) )
				{
					node3 = new Node( x3, y3 );
					if ( !contains( usNodeList, node3 ) )
					{
						usNodeList.push_back( node3 );
					}
				}
				if ( !std::isnan( x4 ) && !std::isnan( y4 ) )
				{
					node4 = new Node( x4, y4 );
					if ( !contains( usNodeList, node4 ) )
					{
						usNodeList.push_back( node4 );
					}
				}
				std::getline( fis, inputLine );
			}
		}
		catch ( ... )
		{
			Msg::error( "Cannot read node file data." );
		}
	}
	catch ( ... )
	{
		Msg::error( "File " + meshFilename + " not found." );
	}

	// nodeList= sortNodes(usNodeList);
	nodeList = usNodeList;
	return usNodeList;
}

bool
GeomBasics::exportMeshToLaTeX( const std::string& filename,
							   int unitlength,
							   double xcorr,
							   double ycorr,
							   bool visibleNodes )
{
	Edge* edge;
	Node* n;
	int i;
	std::vector<Edge*> boundary;

	findExtremeNodes();

	// Collect boundary edges in a list
	for ( i = 0; i < edgeList.size(); i++ )
	{
		edge = edgeList.at( i );
		if ( edge->boundaryEdge() )
		{
			boundary.push_back( edge );
		}
	}

	try
	{
		std::ofstream fos( filename );
		double x1, x2, y1, y2;
		double width = rightmost->x - leftmost->x, height = uppermost->y - lowermost->y;

		try
		{
			fos << "% Include in the header of your file:\n";
			fos << "\n";
			fos << "% \\usepackage{epic, eepic}\n";
			fos << "\n"; 
			fos << "\n";
			fos << "\\begin{figure}[!Htbp]\n";
			fos << "\n";
			fos << "\\begin{center}\n";
			fos << "\n";
			fos << "\\setlength{\\unitlength}{" << unitlength << "mm}\n";
			fos << "\n";
			fos << "\\begin{picture}(" << width << "," << height << ")\n";
			fos << "\n"; 
			fos << "\\filltype{black}\n";
			fos << "\n"; 

			// All boundary edges...
			fos << "\\thicklines\n";
			fos << "\n";
			for ( i = 0; i < boundary.size(); i++ )
			{
				edge = boundary.at( i );

				x1 = edge->leftNode->x + xcorr;
				y1 = edge->leftNode->y + ycorr;
				x2 = edge->rightNode->x + xcorr;
				y2 = edge->rightNode->y + ycorr;

				fos << "\\drawline[1](" << x1 << "," << y1 << ")(" << x2 << "," << y2 << ")\n";
				fos << "\n";
			}

			// All other edges...
			fos << "\\thinlines\n";
			fos << "\n";
			for ( i = 0; i < edgeList.size(); i++ )
			{
				edge = edgeList.at( i );

				if ( !edge->boundaryEdge() )
				{
					x1 = edge->leftNode->x + xcorr;
					y1 = edge->leftNode->y + ycorr;
					x2 = edge->rightNode->x + xcorr;
					y2 = edge->rightNode->y + ycorr;

					fos << "\\drawline[1](" << x1 << "," << y1 << ")(" << x2 << "," << y2 << ")\n";
					fos << "\n";
				}
			}

			// All nodes...
			if ( visibleNodes )
			{
				for ( i = 0; i < nodeList.size(); i++ )
				{
					n = nodeList.at( i );
					fos << "\\put(" << (n->x + xcorr) << "," << (n->y + ycorr) << "){\\circle*{0.1}}\n";
					fos << "\n";
				}
			}

			fos << "\\end{picture}\n";
			fos << "\n";
			fos << "\\end{center}\n";
			fos << "\n";
			fos << "\\end{figure}\n";
			fos << "\n";

			fos.close();
		}
		catch ( ... )
		{
			Msg::error( "Cannot write quad-mesh data export file." );
		}
	}
	catch ( ... )
	{
		Msg::error( "File " + filename + " not found." );
	}
	return true;
}

void
GeomBasics::findExtremeNodes()
{
	// nodeList= sortNodes(nodeList);
	if ( nodeList.size() == 0 )
	{
		leftmost = nullptr;
		rightmost = nullptr;
		uppermost = nullptr;
		lowermost = nullptr;
		return;
	}

	leftmost = nodeList.at( 0 );
	rightmost = leftmost;
	uppermost = leftmost;
	lowermost = leftmost;

	Node* curNode;
	for ( int i = 1; i < nodeList.size(); i++ )
	{
		curNode = nodeList.at( i );

		if ( (curNode->x < leftmost->x) || (curNode->x == leftmost->x && curNode->y > leftmost->y) )
		{
			leftmost = curNode;
		}
		if ( (curNode->x > rightmost->x) || (curNode->x == rightmost->x && curNode->y < rightmost->y) )
		{
			rightmost = curNode;
		}

		if ( (curNode->y > uppermost->y) || (curNode->y == uppermost->y && curNode->x < uppermost->x) )
		{
			uppermost = curNode;
		}
		if ( (curNode->y < lowermost->y) || (curNode->y == lowermost->y && curNode->x < lowermost->x) )
		{
			lowermost = curNode;
		}
	}
}

bool 
GeomBasics::writeQuadMesh( const std::string& filename,
						   std::vector<Element*>& list )
{
	try
	{
		std::ofstream fos( filename );
		double x1, x2, x3, x4, y1, y2, y3, y4;

		try
		{
			for ( auto element : list )
			{
				if ( element->IsAQuad() )
				{
					auto q = static_cast<Quad*>(element);
					x1 = q->edgeList[base]->leftNode->x;
					y1 = q->edgeList[base]->leftNode->y;
					x2 = q->edgeList[base]->rightNode->x;
					y2 = q->edgeList[base]->rightNode->y;
					x3 = q->edgeList[left]->otherNode( q->edgeList[base]->leftNode )->x;
					y3 = q->edgeList[left]->otherNode( q->edgeList[base]->leftNode )->y;
					x4 = q->edgeList[right]->otherNode( q->edgeList[base]->rightNode )->x;
					y4 = q->edgeList[right]->otherNode( q->edgeList[base]->rightNode )->y;

					fos <<
						x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " <<
						y3 << ", " << x4 << ", " << y4 << "\n";
				}
				else
				{
					auto t = static_cast<Triangle*>(element);
					x1 = t->edgeList[0]->leftNode->x;
					y1 = t->edgeList[0]->leftNode->y;
					x2 = t->edgeList[0]->rightNode->x;
					y2 = t->edgeList[0]->rightNode->y;
					if ( !t->edgeList[1]->leftNode->equals( t->edgeList[0]->leftNode ) && !t->edgeList[1]->leftNode->equals( t->edgeList[0]->rightNode ) )
					{
						x3 = t->edgeList[1]->leftNode->x;
						y3 = t->edgeList[1]->leftNode->y;
					}
					else
					{
						x3 = t->edgeList[1]->rightNode->x;
						y3 = t->edgeList[1]->rightNode->y;
					}
					fos << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " << y3 << "\n";;
				}
				fos << "\n";
			}
			fos.close();
		}
		catch ( ... )
		{
			Msg::error( "Cannot write quad-mesh data." );
		}
	}
	catch ( ... )
	{
		Msg::error( "File " + filename + " not found." );
	}
	return true;
}

bool 
GeomBasics::writeMesh( const std::string& filename )
{

	std::ofstream fos;
	try
	{
		fos = std::ofstream( filename );
	}
	catch ( ... )
	{
		Msg::error( "File " + filename + " not found." );
		return false;
	}

	double x1, x2, x3, x4, y1, y2, y3, y4;

	for ( auto t : triangleList )
	{
		x1 = t->edgeList[0]->leftNode->x;
		y1 = t->edgeList[0]->leftNode->y;
		x2 = t->edgeList[0]->rightNode->x;
		y2 = t->edgeList[0]->rightNode->y;
		if ( !t->edgeList[1]->leftNode->equals( t->edgeList[0]->leftNode ) && !t->edgeList[1]->leftNode->equals( t->edgeList[0]->rightNode ) )
		{
			x3 = t->edgeList[1]->leftNode->x;
			y3 = t->edgeList[1]->leftNode->y;
		}
		else
		{
			x3 = t->edgeList[1]->rightNode->x;
			y3 = t->edgeList[1]->rightNode->y;
		}
		try
		{
			fos << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " << y3 << "\n";
			fos << "\n";
		}
		catch ( ... )
		{
			Msg::error( "Cannot write quad-mesh data." );
		}
	}

	for ( auto element : elementList )
	{

		if ( element->IsAQuad() )
		{
			auto q = static_cast<Quad*>(element);

			x1 = q->edgeList[base]->leftNode->x;
			y1 = q->edgeList[base]->leftNode->y;
			x2 = q->edgeList[base]->rightNode->x;
			y2 = q->edgeList[base]->rightNode->y;
			x3 = q->edgeList[left]->otherNode( q->edgeList[base]->leftNode )->x;
			y3 = q->edgeList[left]->otherNode( q->edgeList[base]->leftNode )->y;
			if ( !q->isFake )
			{
				x4 = q->edgeList[right]->otherNode( q->edgeList[base]->rightNode )->x;
				y4 = q->edgeList[right]->otherNode( q->edgeList[base]->rightNode )->y;
				try
				{
					fos << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " 
						<< y3 << ", " << x4 << ", " << y4 << "\n";
					fos << "\n";
				}
				catch ( ... )
				{
					Msg::error( "Cannot write quad-mesh data." );
				}

			}
			else
			{
				try
				{
					fos << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " << y3 << "\n";
				}
				catch ( ... )
				{
					Msg::error( "Cannot write quad-mesh data." );
				}
			}
		}
		else if ( element->IsATriangle() )
		{
			auto t = static_cast<Triangle*>(element);
			x1 = t->edgeList[0]->leftNode->x;
			y1 = t->edgeList[0]->leftNode->y;
			x2 = t->edgeList[0]->rightNode->x;
			y2 = t->edgeList[0]->rightNode->y;
			if ( !t->edgeList[1]->leftNode->equals( t->edgeList[0]->leftNode ) && !t->edgeList[1]->leftNode->equals( t->edgeList[0]->rightNode ) )
			{
				x3 = t->edgeList[1]->leftNode->x;
				y3 = t->edgeList[1]->leftNode->y;
			}
			else
			{
				x3 = t->edgeList[1]->rightNode->x;
				y3 = t->edgeList[1]->rightNode->y;
			}
			try
			{
				fos << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " << y3 << "\n";
				fos << "\n";
			}
			catch ( ... )
			{
				Msg::error( "Cannot write quad-mesh data." );
			}
		}
	}

	try
	{
		fos.close();
	}
	catch ( ... )
	{
		Msg::error( "Cannot write quad-mesh data." );
	}
	return true;
}

bool 
GeomBasics::writeNodes( const std::string& filename )
{
	try
	{
		std::ofstream fos( filename );
		double x, y;

		try
		{
			for ( auto n : nodeList )
			{
				x = n->x;
				y = n->y;
				fos << x << ", " << y << "\n";
				fos << "\n";
			}
			fos.close();
		}
		catch ( ... )
		{
			Msg::error( "Cannot write node data." );
		}
	}
	catch ( ... )
	{
		Msg::error( "Could not open file " + filename );
	}
	return true;
}

std::vector<Node*> 
GeomBasics::sortNodes( std::vector<Node*>& unsortedNodes )
{
	std::vector<Node*> sortedNodes;
	Node* curNode, *candNode;
	while ( unsortedNodes.size() > 0 )
	{
		curNode = unsortedNodes.at( 0 );
		for ( int i = 1; i < unsortedNodes.size(); i++ )
		{
			candNode = unsortedNodes.at( i );
			if ( candNode->x < curNode->x || (candNode->x == curNode->x && candNode->y < curNode->y) )
			{
				curNode = candNode;
			}
		}
		sortedNodes.push_back( curNode );
		unsortedNodes.erase( unsortedNodes.begin() );
	}

	// Find the leftmost, rightmost, uppermost, and lowermost nodes.
	leftmost = sortedNodes.front();
	rightmost = sortedNodes.back();
	uppermost = leftmost;
	lowermost = leftmost;

	for ( int i = 1; i < sortedNodes.size(); i++ )
	{
		curNode = sortedNodes.at( i );
		if ( curNode->y > uppermost->y )
		{
			uppermost = curNode;
		}
		if ( curNode->y < lowermost->y )
		{
			lowermost = curNode;
		}
	}

	return sortedNodes;
}

void 
GeomBasics::printEdgeList( const std::vector<Edge*>& list )
{
	if ( Msg::debugMode )
	{
		for ( auto edge : list )
			edge->printMe();
	}
}

void 
GeomBasics::printNodes( const std::vector<Node*>& nodeList )
{
	if ( Msg::debugMode )
	{
		Msg::debug( "nodeList:" );
		for ( auto node : nodeList )
			node->printMe();
	}
}

void
GeomBasics::printValences()
{
	for ( auto n : nodeList )
	{
		Msg::debug( "Node " + n->descr() + " has valence " + std::to_string( n->valence() ) );
	}
}

void
GeomBasics::printValPatterns()
{
	for ( auto n : nodeList )
	{
		if ( !n->boundaryNode() )
		{
			auto neighbors = n->ccwSortedNeighbors();
			n->createValencePattern( neighbors );
			Msg::debug( "Node " + n->descr() + " has valence pattern " + n->valDescr() );
		}
	}
}

void 
GeomBasics::printAnglesAtSurrondingNodes()
{
	for ( auto n : nodeList )
	{
		if ( !n->boundaryNode() )
		{
			auto neighbors = n->ccwSortedNeighbors();
			n->createValencePattern( neighbors );
			auto angles = n->surroundingAngles( neighbors, n->pattern[0] - 2 );

			Msg::debug( "Angles at the nodes surrounding node " + n->descr() + ":" );
			for ( int j = 0; j < n->pattern[0] - 2; j++ )
			{
				Msg::debug( "angles[" + std::to_string( j ) + "]== " + std::to_string( toDegrees * angles[j] ) + " (in degrees)" );
			}
		}
	}
}

bool
GeomBasics::inversionCheckAndRepair( Node* newN, Node* oldPos )
{
	Msg::debug( "Entering inversionCheckAndRepair(..), node oldPos: " + oldPos->descr() );

	auto elements = newN->adjElements();
	if ( newN->invertedOrZeroAreaElements( elements ) )
	{
		if ( !newN->incrAdjustUntilNotInvertedOrZeroArea( oldPos, elements ) )
		{
			for ( auto element : elements )
			{
				if ( element->invertedOrZeroArea() )
				{
					Msg::error( "It seems that an element was inverted initially: " + element->descr() );
					break;
				}
			}

			return false;
		}
		Msg::debug( "Leaving inversionCheckAndRepair(..)" );
		return true;
	}
	else
	{
		Msg::debug( "Leaving inversionCheckAndRepair(..)" );
		return false;
	}
}

Node*
GeomBasics::safeNewPosWhenCollapsingQuad( Quad* q, Node* n1, Node* n2 )
{
	Msg::debug( "Entering safeNewPosWhenCollapsingQuad(..)" );

	auto n = q->centroid();
	MyVector back2n1( *n, *n1 ), back2n2( *n, *n2 );
	double startX = n->x, startY = n->y;
	double xstepn1 = back2n1.x / 50.0, ystepn1 = back2n1.y / 50.0, xstepn2 = back2n2.x / 50.0, ystepn2 = back2n2.y / 50.0;

	int steps2n1, steps2n2, i;
	std::vector<Element*> l1 = n1->adjElements(), l2 = n2->adjElements();

	if ( !q->anyInvertedElementsWhenCollapsed( n, n1, n2, l1, l2 ) )
	{
		Msg::debug( "Leaving safeNewPosWhenCollapsingQuad(..): found" );
		return n;
	}

	// Calculate the parameters for direction n to n1
	if ( abs( xstepn1 ) < COINCTOL || abs( ystepn1 ) < COINCTOL )
	{
		Msg::debug( "...ok, resorting to use of minimum increment" );
		if ( abs( back2n1.x ) < abs( back2n1.y ) )
		{
			if ( back2n1.x < 0 )
			{
				xstepn1 = -COINCTOL;
			}
			else
			{
				xstepn1 = COINCTOL;
			}

			// abs(ystepn1/xstepn1) = abs(n1.y/n1.x)
			ystepn1 = abs( n1->y ) * COINCTOL / abs( n1->x );
			if ( back2n1.y < 0 )
			{
				ystepn1 = -ystepn1;
			}

			steps2n1 = (int)(back2n1.x / xstepn1);
		}
		else
		{
			if ( back2n1.y < 0 )
			{
				ystepn1 = -COINCTOL;
			}
			else
			{
				ystepn1 = COINCTOL;
			}

			// abs(xstepn1/ystepn1) = abs(n1.x/n1.y)
			xstepn1 = abs( n1->x ) * COINCTOL / abs( n1->y );
			if ( back2n1.x < 0 )
			{
				xstepn1 = -xstepn1;
			}

			steps2n1 = (int)(back2n1.y / ystepn1);
		}
	}
	else
	{
		xstepn1 = back2n1.x / 50.0;
		ystepn1 = back2n1.x / 50.0;
		steps2n1 = 50;
	}

	// Calculate the parameters for direction n to n2
	if ( abs( xstepn2 ) < COINCTOL || abs( ystepn2 ) < COINCTOL )
	{
		Msg::debug( "...ok, resorting to use of minimum increment" );
		if ( abs( back2n2.x ) < abs( back2n2.y ) )
		{
			if ( back2n2.x < 0 )
			{
				xstepn2 = -COINCTOL;
			}
			else
			{
				xstepn2 = COINCTOL;
			}

			// abs(ystepn2/xstepn2) = abs(n2.y/n2.x)
			ystepn2 = abs( n2->y ) * COINCTOL / abs( n2->x );
			if ( back2n2.y < 0 )
			{
				ystepn2 = -ystepn2;
			}

			steps2n2 = (int)(back2n2.x / xstepn2);
		}
		else
		{
			if ( back2n2.y < 0 )
			{
				ystepn2 = -COINCTOL;
			}
			else
			{
				ystepn2 = COINCTOL;
			}

			// abs(xstepn2/ystepn2) = abs(n2.x/n2.y)
			xstepn2 = abs( n2->x ) * COINCTOL / abs( n2->y );
			if ( back2n2.x < 0 )
			{
				xstepn2 = -xstepn2;
			}

			steps2n2 = (int)(back2n2.y / ystepn2);
		}
	}
	else
	{
		xstepn2 = back2n2.x / 50.0;
		ystepn2 = back2n2.x / 50.0;
		steps2n2 = 50;
	}

	Msg::debug( "...back2n1.x is: " + std::to_string( back2n1.x ) );
	Msg::debug( "...back2n1.y is: " + std::to_string( back2n1.y ) );
	Msg::debug( "...xstepn1 is: " + std::to_string( xstepn1 ) );
	Msg::debug( "...ystepn1 is: " + std::to_string( ystepn1 ) );

	Msg::debug( "...back2n2.x is: " + std::to_string( back2n2.x ) );
	Msg::debug( "...back2n2.y is: " + std::to_string( back2n2.y ) );
	Msg::debug( "...xstepn2 is: " + std::to_string( xstepn2 ) );
	Msg::debug( "...ystepn2 is: " + std::to_string( ystepn2 ) );

	// Try to find a location
	for ( i = 1; i <= steps2n1 || i <= steps2n2; i++ )
	{
		if ( i <= steps2n1 )
		{
			n->x = startX + xstepn1 * i;
			n->y = startY + ystepn1 * i;
			if ( !q->anyInvertedElementsWhenCollapsed( n, n1, n2, l1, l2 ) )
			{
				Msg::debug( "Leaving safeNewPosWhenCollapsingQuad(..): found" );
				return n;
			}
		}
		if ( i <= steps2n2 )
		{
			n->x = startX + xstepn2 * i;
			n->y = startY + ystepn2 * i;
			if ( !q->anyInvertedElementsWhenCollapsed( n, n1, n2, l1, l2 ) )
			{
				Msg::debug( "Leaving safeNewPosWhenCollapsingQuad(..): found" );
				return n;
			}
		}
	}

	Msg::debug( "Leaving safeNewPosWhenCollapsingQuad(..): not found" );
	return nullptr;
}

bool 
GeomBasics::repairZeroAreaTriangles()
{
	Msg::debug( "Entering GeomBasics.repairZeroAreaTriangles()" );
	bool res = false;
	Edge* e, * eS, * e1, * e2;

	for ( int i = 0; i < triangleList.size(); i++ )
	{
		if ( !triangleList.at( i )->IsATriangle() )
		{
			continue;
		}
		auto t = triangleList.at( i );
		if ( t->zeroArea() )
		{
			e = t->longestEdge();
			e1 = t->otherEdge( e );
			e2 = t->otherEdge( e, e1 );
			res = true;

			Msg::debug( "...longest edge is " + e->descr() );
			if ( !e->boundaryEdge() )
			{
				Msg::debug( "...longest edge not on boundary!" );
				auto old1 = std::find( triangleList.begin(), triangleList.end(), e->element1 );
				auto old2 = std::find( triangleList.begin(), triangleList.end(), e->element2 );
				eS = e->getSwappedEdge();
				e->swapToAndSetElementsFor( eS );

				*old1 = nullptr;
				*old2 = nullptr;

				triangleList.push_back( static_cast<Triangle*>(eS->element1) );
				triangleList.push_back( static_cast<Triangle*>(eS->element2) );

				edgeList.erase( std::find( edgeList.begin(), edgeList.end(), e ) );
				edgeList.push_back( eS );
			}
			else
			{
				// The zero area triangle has its longest edge on the boundary...
				// Then we can just remove the triangle and the long edge!
				// Note that we now get a new boundary node...
				Msg::debug( "...longest edge is on boundary!" );
				auto Iter = std::find( edgeList.begin(), edgeList.end(), e );
				*Iter = nullptr;
				t->disconnectEdges();
				edgeList.erase( Iter );
				e->disconnectNodes();
			}

		}
	}

	// Remove those entries that were set to null above.
	int i = 0;
	do
	{
		auto t = triangleList.at( i );
		if ( t == nullptr )
		{
			triangleList.erase( triangleList.begin() + i );
		}
		else
		{
			i++;
		}
	} while ( i < triangleList.size() );

	Msg::debug( "Leaving GeomBasics.repairZeroAreaTriangles()" );
	return res;
}