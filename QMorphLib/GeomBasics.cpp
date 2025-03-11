#include "pch.h"
#include "GeomBasics.h"

#include "Element.h"
#include "Edge.h"
#include "Node.h"
#include "Triangle.h"
#include "Quad.h"
#include "Msg::h"

std::vector<Element*> GeomBasics::elementList;
std::vector<Triangle*> GeomBasics::triangleList;
std::vector<Node*> GeomBasics::nodeList;
std::vector<Edge*> GeomBasics::edgeList;

Node* GeomBasics::leftmost = nullptr;
Node* GeomBasics::rightmost = nullptr;
Node* GeomBasics::uppermost = nullptr;
Node* GeomBasics::lowermost = nullptr;

bool GeomBasics::m_step = false;

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
	int i;
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
			 !triangleList.contains( e->element1 ) &&
			 !elementList.contains( e->element1 ) )
		{
			Msg::warning( "element1 of edge " + e->descr() + " is not found in triangleList or elementList" );
		}

		if ( e->element2 != nullptr && 
			 !triangleList.contains( e->element2 ) &&
			 !elementList.contains( e->element2 ) )
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

	Triangle t;
	Node na, nb, nc;
	double cross1;

	for ( Object element : triangleList )
	{
		t = (Triangle)element;
		if ( !t.edgeList[0].hasElement( t ) )
		{
			Msg::warning( "edgeList[0] of triangle " + t.descr() + " has not got that triangle as an adjacent element" );
		}

		if ( !t.edgeList[1].hasElement( t ) )
		{
			Msg::warning( "edgeList[1] of triangle " + t.descr() + " has not got that triangle as an adjacent element" );
		}

		if ( !t.edgeList[2].hasElement( t ) )
		{
			Msg::warning( "edgeList[2] of triangle " + t.descr() + " has not got that triangle as an adjacent element" );
		}

		if ( t.edgeList[0].commonNode( t.edgeList[1] ) == null )
		{
			Msg::warning( "edgeList[0] and edgeList[1] of triangle " + t.descr() + " has no common Node" );
		}
		if ( t.edgeList[1].commonNode( t.edgeList[2] ) == null )
		{
			Msg::warning( "edgeList[1] and edgeList[2] of triangle " + t.descr() + " has no common Node" );
		}
		if ( t.edgeList[2].commonNode( t.edgeList[0] ) == null )
		{
			Msg::warning( "edgeList[2] and edgeList[0] of triangle " + t.descr() + " has no common Node" );
		}

		na = t.edgeList[0].leftNode;
		nb = t.edgeList[0].rightNode;
		nc = t.oppositeOfEdge( t.edgeList[0] );

		cross1 = cross( na, nc, nb, nc ); // The cross product nanc x nbnc

		if ( cross1 == 0 )
		{
			Msg::warning( "Degenerate triangle in triangleList, t= " + t.descr() );
		}
	}

	Element elem;
	Quad q;
	for ( Object element : elementList )
	{
		elem = (Element)element;
		if ( elem == null )
		{
			Msg::debug( "elementList has a null-entry." );
		}
		else if ( elem instanceof Quad )
		{
			q = (Quad)elem;

			if ( !q.edgeList[base].hasElement( q ) )
			{
				Msg::warning( "edgeList[base] of quad " + q.descr() + " has not got that quad as an adjacent element" );
			}

			if ( !q.edgeList[left].hasElement( q ) )
			{
				Msg::warning( "edgeList[left] of quad " + q.descr() + " has not got that quad as an adjacent element" );
			}

			if ( !q.edgeList[right].hasElement( q ) )
			{
				Msg::warning( "edgeList[right] of quad " + q.descr() + " has not got that quad as an adjacent element" );
			}

			if ( !q.isFake && !q.edgeList[top].hasElement( q ) )
			{
				Msg::warning( "edgeList[top] of quad " + q.descr() + " has not got that quad as an adjacent element" );
			}

			if ( q.edgeList[base].commonNode( q.edgeList[left] ) == null )
			{
				Msg::warning( "edgeList[base] and edgeList[left] of quad " + q.descr() + " has no common Node" );
			}
			if ( q.edgeList[base].commonNode( q.edgeList[right] ) == null )
			{
				Msg::warning( "edgeList[base] and edgeList[right] of quad " + q.descr() + " has no common Node" );
			}
			if ( !q.isFake && q.edgeList[left].commonNode( q.edgeList[top] ) == null )
			{
				Msg::warning( "edgeList[left] and edgeList[top] of quad " + q.descr() + " has no common Node" );
			}
			if ( !q.isFake && q.edgeList[right].commonNode( q.edgeList[top] ) == null )
			{
				Msg::warning( "edgeList[right] and edgeList[top] of quad " + q.descr() + " has no common Node" );
			}

			if ( q.isFake && q.edgeList[left].commonNode( q.edgeList[right] ) == null )
			{
				Msg::warning( "edgeList[left] and edgeList[right] of fake quad " + q.descr() + " has no common Node" );
			}
		}
	}

	Msg::debug( "Leaving consistencyCheck()" );

}