#include "pch.h"
#include "GeomBasics.h"

#include "Element.h"
#include "Types.h"
#include "MyVector.h"

#include "Msg.h"

#include <fstream>
#include <string>

//TODO: Tests
void
GeomBasics::createNewLists()
{
	elementList.clear();
	triangleList.clear();
	edgeList.clear();
	nodeList.clear();
}

//TODO: Tests
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

//TODO: Tests
ArrayList<std::shared_ptr<Edge>>
GeomBasics::getEdgeList()
{
	return edgeList;
}

//TODO: Tests
ArrayList<std::shared_ptr<Node>> 
GeomBasics::getNodeList()
{
	return nodeList;
}

//TODO: Tests
ArrayList<std::shared_ptr<Triangle>> 
GeomBasics::getTriangleList()
{
	return triangleList;
}

//TODO: Tests
ArrayList<std::shared_ptr<Element>> 
GeomBasics::getElementList()
{
	return elementList;
}

//TODO: Tests
void 
GeomBasics::setCurMethod( const std::shared_ptr<GeomBasics>& method )
{
	curMethod = method;
}

//TODO: Tests
const std::shared_ptr<GeomBasics>&
GeomBasics::getCurMethod()
{
	return curMethod;
}

//TODO: Tests
void
GeomBasics::clearEdges()
{
	for ( auto curElem : elementList )
		curElem->disconnectEdges();

	for ( auto curEdge : edgeList )
		curEdge->disconnectNodes();

	for ( int i = 0; i < nodeList.size(); i++ )
	{
		auto& curNode = nodeList.get( i );
		curNode->edgeList.clear();
	}

	elementList.clear();
	triangleList.clear();
	edgeList.clear();
}

//TODO: Tests
void
GeomBasics::clearLists()
{
	nodeList.clear();
	edgeList.clear();
	triangleList.clear();
	elementList.clear();
}

//TODO: Tests
void
GeomBasics::updateMeshMetrics()
{
	if ( elementList.size() > 0 )
	{
		for ( auto elem : elementList )
		{
			if ( elem != nullptr )
				elem->updateDistortionMetric();
		}
	}
	else
	{
		for ( auto tri : triangleList )
		{
			if ( tri != nullptr )
			{
				tri->updateDistortionMetric();
			}
		}
	}
}

//TODO: Tests
std::string
GeomBasics::meshMetricsReport()
{
	double sumMetric = 0.0, minDM = std::numeric_limits<double>::max();
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
				if ( rcl::instanceOf<Triangle>( elem ) )
				{
					nTris++;
				}
				else if ( rcl::instanceOf<Quad>( elem ) )
				{
					const auto& q = std::dynamic_pointer_cast<Quad>(elem);
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
		for ( auto tri: triangleList )
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

	for ( auto n : nodeList )
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

	s = s + "Number of quadrilateral elements: " + std::to_string( nQuads ) + "\n" + "Number of triangular elements: " + std::to_string( nTris ) + "\n" + "Number of edges: " + std::to_string( edgeList.size() )
		+ "\n" + "Number of nodes: " + std::to_string( nodeList.size() );

	return s;
}

//TODO: Tests
void
GeomBasics::detectInvertedElements()
{
	int i;
	for ( i = 0; i < elementList.size(); i++ )
	{
		auto& elem = elementList.get( i );
		if ( elem != nullptr && elem->inverted() )
		{
			elem->markEdgesIllegal();
			Msg::warning( "Element " + elem->descr() + " is inverted." );
			Msg::warning( "It has firstNode " + elem->firstNode->descr() );
		}
	}

	for ( i = 0; i < triangleList.size(); i++ )
	{
		auto& t = triangleList.get( i );
		if ( t != nullptr && t->inverted() )
		{
			t->markEdgesIllegal();
			Msg::warning( "Triangle " + t->descr() + " is inverted." );
			Msg::warning( "It has firstNode " + t->firstNode->descr() );
		}
	}
}

//TODO: Tests
void
GeomBasics::countTriangles()
{
	int fakes = 0, tris = 0;
	for ( auto elem : elementList )
	{
		auto Q = std::dynamic_pointer_cast<Quad>(elem);
		if ( Q && Q->isFake )
		{
			fakes++;
		}
		else if ( rcl::instanceOf<Triangle>( elem ) )
		{
			tris++;
		}
	}

	Msg::debug( "Counted # of fake quads: " + std::to_string( fakes ) );
	Msg::debug( "Counted # of triangles: " + std::to_string( tris ) );
}

//TODO: Tests
void
GeomBasics::consistencyCheck()
{
	Msg::debug( "Entering consistencyCheck()" );
	for ( int i = 0; i < nodeList.size(); i++ )
	{
		const auto&n = nodeList.get( i );

		if ( n->edgeList.size() == 0 )
		{
			Msg::warning( "edgeList.size()== 0 for node " + n->descr() );
		}

		for ( int j = 0; j < n->edgeList.size(); j++ )
		{
			const auto& e = n->edgeList.get( j );
			if ( e == nullptr )
			{
				Msg::warning( "Node " + n->descr() + " has a null in its edgeList." );
			}
			else if ( edgeList.indexOf( e ) == -1 )
			{
				Msg::warning( "Edge " + e->descr() + " found in the edgeList of Node " + n->descr() + ", but not in global edgeList" );
			}
		}
	}

	for ( int i = 0; i < edgeList.size(); i++ )
	{
		const auto& e = edgeList.get( i );
		if ( e->leftNode->edgeList.indexOf( e ) == -1 )
		{
			Msg::warning( "leftNode of edge " + e->descr() + " has not got that edge in its .edgeList" );
		}
		if ( e->rightNode->edgeList.indexOf( e ) == -1 )
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

		if ( e->element1 != nullptr && !triangleList.contains( std::dynamic_pointer_cast<Triangle>(e->element1) ) && !elementList.contains( e->element1 ) )
		{
			Msg::warning( "element1 of edge " + e->descr() + " is not found in triangleList or elementList" );
		}

		if ( e->element2 != nullptr && !triangleList.contains( std::dynamic_pointer_cast<Triangle>(e->element2) ) && !elementList.contains( e->element2 ) )
		{
			Msg::warning( "element2 of edge " + e->descr() + " is not found in triangleList or elementList" );
		}

		if ( nodeList.indexOf( e->leftNode ) == -1 )
		{
			Msg::warning( "leftNode of edge " + e->descr() + " not found in nodeList." );
		}
		if ( nodeList.indexOf( e->rightNode ) == -1 )
		{
			Msg::warning( "rightNode of edge " + e->descr() + " not found in nodeList." );
		}
	}

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

		const auto& na = t->edgeList[0]->leftNode;
		const auto& nb = t->edgeList[0]->rightNode;
		const auto& nc = t->oppositeOfEdge( t->edgeList[0] );

		cross1 = cross( na, nc, nb, nc ); // The cross product nanc x nbnc

		if ( cross1 == 0 /* !t.areaLargerThan0() */ )
		{
			Msg::warning( "Degenerate triangle in triangleList, t= " + t->descr() );
		}
	}

	for ( auto elem : elementList )
	{
		if ( elem == nullptr )
		{
			Msg::debug( "elementList has a null-entry." );
		}
		else if ( const auto& q = std::dynamic_pointer_cast<Quad>(elem) )
		{
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

//TODO: Tests
double 
GeomBasics::cross( const std::shared_ptr<Node>& o1,
				   const std::shared_ptr<Node>& p1,
				   const std::shared_ptr<Node>& o2,
				   const std::shared_ptr<Node>& p2 )
{
	double x1 = p1->x - o1->x;
	double x2 = p2->x - o2->x;
	double y1 = p1->y - o1->y;
	double y2 = p2->y - o2->y;
	return x1 * y2 - x2 * y1;
}

double
GeomBasics::nextDouble( const std::string& iline )
{
	std::string ndouble;
	if ( cInd > iline.length() )
	{
		return std::numeric_limits<double>::infinity();
	}

	auto nInd = iline.find( ',', cInd );
	if ( nInd == std::string::npos )
	{
		nInd = iline.length();
	}

	ndouble = iline.substr( cInd, nInd );
	cInd = static_cast<int>(nInd + 1);
	return std::stod( ndouble );
}

//TODO: Tests
ArrayList<std::shared_ptr<Element>>
GeomBasics::loadMesh()
{
	elementList.clear();
	triangleList.clear();
	edgeList.clear();
	ArrayList<std::shared_ptr<Node>> usNodeList;

	try
	{
		std::ifstream fis( meshDirectory + meshFilename );
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

				auto node1 = std::make_shared<Node>( x1, y1 );
				if ( !usNodeList.contains( node1 ) )
				{
					usNodeList.add( node1 );
				}
				else
				{
					node1 = usNodeList.get( usNodeList.indexOf( node1 ) );
				}

				auto node2 = std::make_shared<Node>( x2, y2 );
				if ( !usNodeList.contains( node2 ) )
				{
					usNodeList.add( node2 );
				}
				else
				{
					node2 = usNodeList.get( usNodeList.indexOf( node2 ) );
				}

				auto node3 = std::make_shared<Node>( x3, y3 );
				if ( !usNodeList.contains( node3 ) )
				{
					usNodeList.add( node3 );
				}
				else
				{
					node3 = usNodeList.get( usNodeList.indexOf( node3 ) );
				}

				auto edge1 = std::make_shared<Edge>( node1, node2 );
				if ( !edgeList.contains( edge1 ) )
				{
					edgeList.add( edge1 );
					edge1->connectNodes();
				}
				else
				{
					edge1 = edgeList.get( edgeList.indexOf( edge1 ) );
				}

				auto edge2 = std::make_shared<Edge>( node1, node3 );
				if ( !edgeList.contains( edge2 ) )
				{
					edgeList.add( edge2 );
					edge2->connectNodes();
				}
				else
				{
					edge2 = edgeList.get( edgeList.indexOf( edge2 ) );
				}

				if ( !std::isnan( x4 ) && !std::isnan( y4 ) )
				{
					auto node4 = std::make_shared<Node>( x4, y4 );
					if ( !usNodeList.contains( node4 ) )
					{
						usNodeList.add( node4 );
					}
					else
					{
						node4 = usNodeList.get( usNodeList.indexOf( node4 ) );
					}

					auto edge3 = std::make_shared<Edge>( node2, node4 );
					if ( !edgeList.contains( edge3 ) )
					{
						edgeList.add( edge3 );
						edge3->connectNodes();
					}
					else
					{
						edge3 = edgeList.get( edgeList.indexOf( edge3 ) );
					}

					auto edge4 = std::make_shared<Edge>( node3, node4 );
					if ( !edgeList.contains( edge4 ) )
					{
						edgeList.add( edge4 );
						edge4->connectNodes();
					}
					else
					{
						edge4 = edgeList.get( edgeList.indexOf( edge4 ) );
					}

					auto q = std::make_shared<Quad>( edge1, edge2, edge3, edge4 );
					q->connectEdges();
					elementList.add( q );
				}
				else
				{
					auto edge3 = std::make_shared<Edge>( node2, node3 );
					if ( !edgeList.contains( edge3 ) )
					{
						edgeList.add( edge3 );
						edge3->connectNodes();
					}
					else
					{
						edge3 = edgeList.get( edgeList.indexOf( edge3 ) );
					}

					auto t = std::make_shared<Triangle>( edge1, edge2, edge3 );
					t->connectEdges();
					triangleList.add( t );
					// elementList.add(t);
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

//TODO: Tests
ArrayList<std::shared_ptr<Triangle>>
GeomBasics::loadTriangleMesh()
{
	triangleList.clear();
	edgeList.clear();
	ArrayList<std::shared_ptr<Node>> usNodeList;

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

				auto node1 = std::make_shared<Node>( x1, y1 );
				if ( !usNodeList.contains( node1 ) )
				{
					usNodeList.add( node1 );
				}
				else
				{
					node1 = usNodeList.get( usNodeList.indexOf( node1 ) );
				}

				auto node2 = std::make_shared<Node>( x2, y2 );
				if ( !usNodeList.contains( node2 ) )
				{
					usNodeList.add( node2 );
				}
				else
				{
					node2 = usNodeList.get( usNodeList.indexOf( node2 ) );
				}

				auto node3 = std::make_shared<Node>( x3, y3 );
				if ( !usNodeList.contains( node3 ) )
				{
					usNodeList.add( node3 );
				}
				else
				{
					node3 = usNodeList.get( usNodeList.indexOf( node3 ) );
				}

				auto edge1 = std::make_shared<Edge>( node1, node2 );
				if ( !edgeList.contains( edge1 ) )
				{
					edgeList.add( edge1 );
				}
				else
				{
					edge1 = edgeList.get( edgeList.indexOf( edge1 ) );
				}
				edge1->leftNode->connectToEdge( edge1 );
				edge1->rightNode->connectToEdge( edge1 );

				auto edge2 = std::make_shared<Edge>( node2, node3 );
				if ( !edgeList.contains( edge2 ) )
				{
					edgeList.add( edge2 );
				}
				else
				{
					edge2 = edgeList.get( edgeList.indexOf( edge2 ) );
				}
				edge2->leftNode->connectToEdge( edge2 );
				edge2->rightNode->connectToEdge( edge2 );

				auto edge3 = std::make_shared<Edge>( node1, node3 );
				if ( !edgeList.contains( edge3 ) )
				{
					edgeList.add( edge3 );
				}
				else
				{
					edge3 = edgeList.get( edgeList.indexOf( edge3 ) );
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
				auto t = std::make_shared<Triangle>( edge1, edge2, edge3, len1, len2, len3, ang1, ang2, ang3, meshLenOpt, meshAngOpt );
				t->connectEdges();
				triangleList.add( t );
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

//TODO: Tests
ArrayList<std::shared_ptr<Node>> 
GeomBasics::loadNodes()
{
	ArrayList<std::shared_ptr<Node>> usNodeList;

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
					auto node1 = std::make_shared<Node>( x1, y1 );
					if ( !usNodeList.contains( node1 ) )
					{
						usNodeList.add( node1 );
					}
				}
				if ( !std::isnan( x2 ) && !std::isnan( y2 ) )
				{
					auto node2 = std::make_shared<Node>( x2, y2 );
					if ( !usNodeList.contains( node2 ) )
					{
						usNodeList.add( node2 );
					}
				}
				if ( !std::isnan( x3 ) && !std::isnan( y3 ) )
				{
					auto node3 = std::make_shared<Node>( x3, y3 );
					if ( !usNodeList.contains( node3 ) )
					{
						usNodeList.add( node3 );
					}
				}
				if ( !std::isnan( x4 ) && !std::isnan( y4 ) )
				{
					auto node4 = std::make_shared<Node>( x4, y4 );
					if ( !usNodeList.contains( node4 ) )
					{
						usNodeList.add( node4 );
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

//TODO: Tests
bool
GeomBasics::exportMeshToLaTeX( std::string filename,
							   int unitlength,
							   double xcorr,
							   double ycorr,
							   bool visibleNodes )
{
	ArrayList<std::shared_ptr<Edge>> boundary;

	findExtremeNodes();

	// Collect boundary edges in a list
	for ( auto i = 0; i < edgeList.size(); i++ )
	{
		const auto& edge = edgeList.get( i );
		if ( edge->boundaryEdge() )
		{
			boundary.add( edge );
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
			fos << "% \\usepackage{epic, eepic}\n\n";
			fos << "\\begin{figure}[!Htbp]\n";
			fos << "\\begin{center}\n";
			fos << "\\setlength{\\unitlength}{" << unitlength << "mm}\n";
			fos << "\\begin{picture}(" << width << "," << height << ")\n";
			fos << "\\filltype{black}\n";

			// All boundary edges...
			fos << "\\thicklines\n";
			for ( auto i = 0; i < boundary.size(); i++ )
			{
				const auto& edge = boundary.get( i );

				x1 = edge->leftNode->x + xcorr;
				y1 = edge->leftNode->y + ycorr;
				x2 = edge->rightNode->x + xcorr;
				y2 = edge->rightNode->y + ycorr;

				fos << "\\drawline[1](" << x1 << "," << y1 << ")(" << x2 << "," << y2 << ")\n";
			}

			// All other edges...
			fos << "\\thinlines\n";
			for ( auto i = 0; i < edgeList.size(); i++ )
			{
				const auto& edge = edgeList.get( i );

				if ( !edge->boundaryEdge() )
				{
					x1 = edge->leftNode->x + xcorr;
					y1 = edge->leftNode->y + ycorr;
					x2 = edge->rightNode->x + xcorr;
					y2 = edge->rightNode->y + ycorr;

					fos << "\\drawline[1](" << x1 << "," << y1 << ")(" << x2 << "," << y2 << ")\n";
				}
			}

			// All nodes...
			if ( visibleNodes )
			{
				for ( auto i = 0; i < nodeList.size(); i++ )
				{
					const auto& n = nodeList.get( i );
					fos << "\\put(" << (n->x + xcorr) << "," << (n->y + ycorr) << "){\\circle*{0.1}}\n";
				}
			}

			fos << "\\end{picture}\n";
			fos << "\\end{center}\n";
			fos << "\\end{figure}\n";

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

//TODO: Tests
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

	leftmost = nodeList.get( 0 );
	rightmost = leftmost;
	uppermost = leftmost;
	lowermost = leftmost;

	for ( int i = 1; i < nodeList.size(); i++ )
	{
		const auto& curNode = nodeList.get( i );

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

//TODO: Tests
bool 
GeomBasics::writeQuadMesh( const std::string& filename,
						   const ArrayList<std::shared_ptr<Element>>& list )
{
	try
	{
		std::ofstream fos( filename );
		double x1, x2, x3, x4, y1, y2, y3, y4;

		try
		{
			for ( auto elem : list )
			{
				if ( auto q = std::dynamic_pointer_cast<Quad>(elem) )
				{
					x1 = q->edgeList[base]->leftNode->x;
					y1 = q->edgeList[base]->leftNode->y;
					x2 = q->edgeList[base]->rightNode->x;
					y2 = q->edgeList[base]->rightNode->y;
					x3 = q->edgeList[left]->otherNode( q->edgeList[base]->leftNode )->x;
					y3 = q->edgeList[left]->otherNode( q->edgeList[base]->leftNode )->y;
					x4 = q->edgeList[right]->otherNode( q->edgeList[base]->rightNode )->x;
					y4 = q->edgeList[right]->otherNode( q->edgeList[base]->rightNode )->y;

					fos << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " << y3 << ", " << x4 << ", " << y4;
				}
				else if ( auto t = std::dynamic_pointer_cast<Triangle>(elem) )
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
					fos << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " << y3;
				}
				fos << "\n";
			}
			fos << "\n";
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

//TODO: Tests
bool 
GeomBasics::writeMesh( const std::string& filename )
{
	try
	{
		std::ofstream fos( filename );
		double x1, x2, x3, x4, y1, y2, y3, y4;

		if ( triangleList.size() )
		{
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
				}
				catch ( ... )
				{
					Msg::error( "Cannot write quad-mesh data." );
				}

			}
		}

		if ( elementList.size() )
		{
			for ( auto element : elementList )
			{
				if ( auto q = std::dynamic_pointer_cast<Quad>(element) )
				{
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
							fos << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " << y3 << ", " << x4 << ", " << y4 << "\n";
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
							fos << x1 << ", " << y1 << ", " << x2 << ", " << y2 << ", " << x3 << ", " << y3;
						}
						catch ( ... )
						{
							Msg::error( "Cannot write quad-mesh data." );
						}
					}
				}
				else if ( auto t = std::dynamic_pointer_cast<Triangle>(element) )
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
					}
					catch ( ... )
					{
						Msg::error( "Cannot write quad-mesh data." );
					}
				}
			}
		}

		fos.close();
	}
	catch ( ... )
	{
		Msg::error( "File " + filename + " not found." );
	}
	return true;
}

//TODO: Tests
bool 
GeomBasics::writeNodes( const std::string& filename )
{
	try
	{
		std::ofstream fos( filename );
		double x, y;

		try
		{
			if ( nodeList.size() )
			{
				for ( auto n : nodeList )
				{
					x = n->x;
					y = n->y;
					fos << x << ", " << y << "\n";
				}
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

ArrayList<std::shared_ptr<Node>>
GeomBasics::sortNodes( ArrayList<std::shared_ptr<Node>>& unsortedNodes )
{
	ArrayList<std::shared_ptr<Node>> sortedNodes;
	
	while ( unsortedNodes.size() > 0 )
	{
		auto curNode = unsortedNodes.get( 0 );
		for ( int i = 1; i < unsortedNodes.size(); i++ )
		{
			const auto& candNode = unsortedNodes.get( i );
			if ( candNode->x < curNode->x || (candNode->x == curNode->x && candNode->y < curNode->y) )
			{
				curNode = candNode;
			}
		}
		sortedNodes.add( curNode );
		unsortedNodes.remove( unsortedNodes.indexOf( curNode ) );
	}

	// Find the leftmost, rightmost, uppermost, and lowermost nodes.
	leftmost = sortedNodes.get( 0 );
	rightmost = sortedNodes.get( sortedNodes.size() - 1 );
	uppermost = leftmost;
	lowermost = leftmost;

	for ( int i = 1; i < sortedNodes.size(); i++ )
	{
		auto curNode = sortedNodes.get( i );
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

//TODO: Tests
void
GeomBasics::printVectors( const ArrayList<std::shared_ptr<MyVector>>& vectorList )
{
	if ( Msg::debugMode )
	{
		for ( auto v : vectorList )
		{
			v->printMe();
		}
	}
}

//TODO: Tests
void
GeomBasics::printElements( const ArrayList<std::shared_ptr<Element>>& list )
{
	if ( Msg::debugMode )
	{
		for ( auto elem: list )
		{
			elem->printMe();
		}
	}
}

//TODO: Tests
void 
GeomBasics::printTriangles( const ArrayList<std::shared_ptr<Triangle>>& triangleList )
{
	Msg::debug( "triangleList: (size== " + std::to_string( triangleList.size() ) + ")" );
	if ( Msg::debugMode )
	{
		for ( auto elem : triangleList )
		{
			elem->printMe();
		}
	}
}

//TODO: Tests
void 
GeomBasics::printQuads( const ArrayList<std::shared_ptr<Element>>& list )
{
	Msg::debug( "quadList: (size== " + std::to_string( list.size() ) + ")" );
	printElements( list );
}

//TODO: Tests
void
GeomBasics::printEdgeList( const ArrayList<std::shared_ptr<Edge>>& list )
{
	if ( Msg::debugMode )
	{
		for ( auto edge : list )
		{
			edge->printMe();
		}
	}
}

//TODO: Tests
void 
GeomBasics::printNodes( const ArrayList<std::shared_ptr<Node>>& nodeList )
{
	if ( Msg::debugMode )
	{
		Msg::debug( "nodeList:" );
		for ( auto node : nodeList )
		{
			node->printMe();
		}
	}
}

//TODO: Tests
void
GeomBasics::printValences()
{
	for ( auto n : nodeList )
	{
		Msg::debug( "Node " + n->descr() + " has valence " + std::to_string( n->valence() ) );
	}
}

//TODO: Tests
void
GeomBasics::printValPatterns()
{
	std::vector<std::shared_ptr<Node>> neighbors;
	for ( auto n : nodeList )
	{
		if ( !n->boundaryNode() )
		{
			neighbors = n->ccwSortedNeighbors();
			n->createValencePattern( neighbors );
			Msg::debug( "Node " + n->descr() + " has valence pattern " + n->valDescr() );
		}
	}
}

//TODO: Tests
void 
GeomBasics::printAnglesAtSurrondingNodes()
{
	std::vector<std::shared_ptr<Node>> neighbors;
	std::vector<double> angles;
	for ( auto n : nodeList )
	{
		if ( !n->boundaryNode() )
		{
			neighbors = n->ccwSortedNeighbors();
			n->createValencePattern( neighbors );
			angles = n->surroundingAngles( neighbors, n->pattern[0] - 2 );

			Msg::debug( "Angles at the nodes surrounding node " + n->descr() + ":" );
			for ( int j = 0; j < n->pattern[0] - 2; j++ )
			{
				Msg::debug( "angles[" + std::to_string( j ) + "]== " + std::to_string( toDegrees * angles[j] ) + " (in degrees)" );
			}
		}
	}
}

//TODO: Tests
bool
GeomBasics::inversionCheckAndRepair( const std::shared_ptr<Node>& newN,
									 const std::shared_ptr<Node>& oldPos )
{
	Msg::debug( "Entering inversionCheckAndRepair(..), node oldPos: " + oldPos->descr() );
	auto elements = newN->adjElements();
	if ( newN->invertedOrZeroAreaElements( elements ) )
	{
		if ( !newN->incrAdjustUntilNotInvertedOrZeroArea( oldPos, elements ) )
		{

			for ( auto elem : elements )
			{
				if ( elem->invertedOrZeroArea() )
				{
					Msg::error( "It seems that an element was inverted initially: " + elem->descr() );
					return false;
				}
			}	
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

//TODO: Tests
std::shared_ptr<Node> 
GeomBasics::safeNewPosWhenCollapsingQuad( const std::shared_ptr<Quad>& q,
										  const std::shared_ptr<Node>& n1,
										  const std::shared_ptr<Node>& n2 )
{
	Msg::debug( "Entering safeNewPosWhenCollapsingQuad(..)" );

	auto n = q->centroid();
	MyVector back2n1( n, n1 ), back2n2( n, n2 );
	double startX = n->x, startY = n->y;
	double xstepn1 = back2n1.x / 50.0, ystepn1 = back2n1.y / 50.0, xstepn2 = back2n2.x / 50.0, ystepn2 = back2n2.y / 50.0;
	int steps2n1, steps2n2, i;
	auto l1 = n1->adjElements(), l2 = n2->adjElements();

	if ( !q->anyInvertedElementsWhenCollapsed( n, n1, n2, l1, l2 ) )
	{
		Msg::debug( "Leaving safeNewPosWhenCollapsingQuad(..): found" );
		return n;
	}

	// Calculate the parameters for direction n to n1
	if ( std::abs( xstepn1 ) < COINCTOL || std::abs( ystepn1 ) < COINCTOL )
	{
		Msg::debug( "...ok, resorting to use of minimum increment" );
		if ( std::abs( back2n1.x ) < std::abs( back2n1.y ) )
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
			ystepn1 = std::abs( n1->y ) * COINCTOL / std::abs( n1->x );
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
			xstepn1 = std::abs( n1->x ) * COINCTOL / std::abs( n1->y );
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
	if ( std::abs( xstepn2 ) < COINCTOL || std::abs( ystepn2 ) < COINCTOL )
	{
		Msg::debug( "...ok, resorting to use of minimum increment" );
		if ( std::abs( back2n2.x ) < std::abs( back2n2.y ) )
		{
			if ( back2n2.x < 0 )
			{
				xstepn2 = -COINCTOL;
			}
			else
			{
				xstepn2 = COINCTOL;
			}

			ystepn2 = std::abs( n2->y ) * COINCTOL / std::abs( n2->x );
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
			xstepn2 = std::abs( n2->x ) * COINCTOL / std::abs( n2->y );
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
	
	for ( int i = 0; i < triangleList.size(); i++ )
	{
		auto t = triangleList.get( i );
		if ( t->zeroArea() )
		{
			auto e = t->longestEdge();
			const auto& e1 = t->otherEdge( e );
			const auto& e2 = t->otherEdge( e, e1 );
			res = true;

			Msg::debug( "...longest edge is " + e->descr() );
			if ( !e->boundaryEdge() )
			{
				Msg::debug( "...longest edge not on boundary!" );
				const auto& old1 = std::dynamic_pointer_cast<Triangle>(e->element1);
				const auto& old2 = std::dynamic_pointer_cast<Triangle>(e->element2);
				const auto& eS = e->getSwappedEdge();
				e->swapToAndSetElementsFor( eS );

				triangleList.set( triangleList.indexOf( old1 ), nullptr );
				triangleList.set( triangleList.indexOf( old2 ), nullptr );

				triangleList.add( std::dynamic_pointer_cast<Triangle>(eS->element1) );
				triangleList.add( std::dynamic_pointer_cast<Triangle>(eS->element2) );

				edgeList.remove( edgeList.indexOf( e ) );
				edgeList.add( eS );
			}
			else
			{
				// The zero area triangle has its longest edge on the boundary...
				// Then we can just remove the triangle and the long edge!
				// Note that we now get a new boundary node...
				Msg::debug( "...longest edge is on boundary!" );
				triangleList.set( triangleList.indexOf( t ), nullptr );
				t->disconnectEdges();
				edgeList.remove( edgeList.indexOf( e ) );
				e->disconnectNodes();
			}

		}
	}

	// Remove those entries that were set to null above.
	int i = 0;
	do
	{
		auto t = triangleList.get( i );
		if ( t == nullptr )
		{
			triangleList.remove( i );
		}
		else
		{
			i++;
		}
	} while ( i < triangleList.size() );

	Msg::debug( "Leaving GeomBasics.repairZeroAreaTriangles()" );
	return res;
}