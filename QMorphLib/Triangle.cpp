#include "pch.h"
#include "Triangle.h"

#include "Edge.h"
#include "MyVector.h"
#include "Node.h"

#include "Msg.h"

#include <iostream>

//TODO: Implement this method
//TODO: Tests
Triangle::Triangle( const std::shared_ptr<Edge>& edge1,
					const std::shared_ptr<Edge>& edge2,
					const std::shared_ptr<Edge>& edge3,
					double len1, double len2, double len3,
					double ang1, double ang2, double ang3,
					bool lengthsOpt, bool anglesOpt )
{
	/*edgeList.assign(3, nullptr);

	edgeList[0] = edge1;
	edgeList[1] = edge2;
	edgeList[2] = edge3;

	edgeList[0]->len = len1;
	edgeList[1]->len = len2;
	edgeList[2]->len = len3;

	// Make a pointer to the base node that is the origin of vector(edgeList[a])
	// so that the cross product vector(edgeList[a]) x vector(edgeList[b]) >= 0
	// where a,b in {1,2}
	firstNode = edgeList[0]->leftNode;
	if ( inverted() )
	{
		firstNode = edgeList[0]->rightNode;
	}

	if ( firstNode == edgeList[0]->commonNode( edgeList[2] ) )
	{
		std::swap( edgeList[1], edgeList[2] );
	}

	ang.assign( 3, 0.0 );
	ang[0] = ang1;
	ang[1] = ang2;
	ang[2] = ang3;

	if ( !lengthsOpt )
	{
		updateLengths();
	}
	if ( !anglesOpt )
	{
		updateAngles();
	}*/
}

//TODO: Tests
Triangle::Triangle( const Triangle& t )
{
	edgeList.assign( 3, nullptr );

	edgeList[0] = std::make_shared<Edge>( t.edgeList[0]->leftNode, t.edgeList[0]->rightNode );
	edgeList[1] = std::make_shared<Edge>( t.edgeList[1]->leftNode, t.edgeList[1]->rightNode );
	edgeList[2] = std::make_shared<Edge>( t.edgeList[2]->leftNode, t.edgeList[2]->rightNode );

	ang.assign( 3, 0.0 );
	ang[0] = t.ang[0];
	ang[1] = t.ang[1];
	ang[2] = t.ang[2];

	firstNode = t.firstNode;
}

//TODO: Implement this method
//TODO: Tests
Triangle::Triangle( const std::shared_ptr<Edge>& edge1,
					const std::shared_ptr<Edge>& edge2,
					const std::shared_ptr<Edge>& edge3 )
{
	/*edgeList = new Edge[3];

	if ( edge1 == null || edge2 == null || edge3 == null )
	{
		Msg.error( "Triangle: cannot create Triangle with null Edge." );
	}

	edgeList[0] = edge1;
	edgeList[1] = edge2;
	edgeList[2] = edge3;

	// Make a pointer to the base node that is the origin of vector(edgeList[a])
	// so that the cross product vector(edgeList[a]) x vector(edgeList[b]) >= 0
	// where a,b in {1,2}
	firstNode = edgeList[0].leftNode;
	if ( inverted() )
	{
		firstNode = edgeList[0].rightNode;
	}

	Edge temp;
	if ( firstNode == edgeList[0].commonNode( edgeList[2] ) )
	{
		temp = edgeList[1];
		edgeList[1] = edgeList[2];
		edgeList[2] = temp;
	}

	ang = new double[3];
	updateAngles();*/
}

//TODO: Tests
void
Triangle::updateLengths()
{
	edgeList[0]->len = edgeList[0]->computeLength();
	edgeList[1]->len = edgeList[1]->computeLength();
	edgeList[2]->len = edgeList[2]->computeLength();
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Element> 
Triangle::elementWithExchangedNodes( const std::shared_ptr<Node>& original,
									 const std::shared_ptr<Node>& replacement )
{
	return nullptr;
	/*Node node1 = edgeList[0].leftNode;
	Node node2 = edgeList[0].rightNode;
	Node node3 = edgeList[1].rightNode;
	if ( node3 == node1 || node3 == node2 )
	{
		node3 = edgeList[1].leftNode;
	}
	Node common1;

	// Make a copy of the original triangle...
	Triangle t = new Triangle( this );
	Edge edge1 = t.edgeList[0], edge2 = t.edgeList[1], edge3 = t.edgeList[2];

	// ... and then replace the node
	if ( node1 == original )
	{
		common1 = edge1.commonNode( edge2 );
		if ( original == common1 )
		{
			edge2.replaceNode( original, replacement );
		}
		else
		{
			edge3.replaceNode( original, replacement );
		}
		edge1.replaceNode( original, replacement );
	}
	else if ( node2 == original )
	{
		common1 = edge1.commonNode( edge2 );
		if ( original == common1 )
		{
			edge2.replaceNode( original, replacement );
		}
		else
		{
			edge3.replaceNode( original, replacement );
		}
		edge1.replaceNode( original, replacement );
	}
	else if ( node3 == original )
	{
		common1 = edge2.commonNode( edge1 );
		if ( original == common1 )
		{
			edge1.replaceNode( original, replacement );
		}
		else
		{
			edge3.replaceNode( original, replacement );
		}
		edge2.replaceNode( original, replacement );
	}
	else
	{
		return null;
	}

	if ( original == firstNode )
	{
		t.firstNode = replacement;
	}
	else
	{
		t.firstNode = firstNode;
	}
	return t;*/
}

//TODO: Implement this method
//TODO: Tests
bool 
Triangle::equals( const std::shared_ptr<Constants>& elem ) const 
{
	return false;
	/*if ( o instanceof Triangle )
	{
		Triangle t = (Triangle)o;
		Node node1, node2, node3;
		Node tnode1, tnode2, tnode3;
		node1 = edgeList[0].leftNode;
		node2 = edgeList[0].rightNode;
		node3 = edgeList[1].rightNode;
		if ( node3 == node1 || node3 == node2 )
		{
			node3 = edgeList[1].leftNode;
		}

		tnode1 = t.edgeList[0].leftNode;
		tnode2 = t.edgeList[0].rightNode;
		tnode3 = t.edgeList[1].rightNode;
		if ( tnode3 == tnode1 || tnode3 == tnode2 )
		{
			tnode3 = t.edgeList[1].leftNode;
		}

		if ( node1 == tnode1 && node2 == tnode2 && node3 == tnode3 )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}*/
}

//TODO: Tests
double 
Triangle::angle( const std::shared_ptr<Edge>& e,
				 const std::shared_ptr<Node>& n )
{
	return ang[angleIndex( e, neighborEdge( n, e ) )];
}

//TODO: Tests
double
Triangle::angle( const std::shared_ptr<Edge>& e1,
				 const std::shared_ptr<Edge>& e2 )
{
	return ang[angleIndex( e1, e2 )];
}

//TODO: Tests
void 
Triangle::updateAttributes()
{
	updateLengths();
	updateAngles();
}

//TODO: Tests
void 
Triangle::updateAngle( const std::shared_ptr<Node>& n )
{
	int j = angleIndex( n ), i;

	i = angleIndex( edgeList[0], edgeList[1] );
	if ( j == i )
	{
		ang[i] = edgeList[0]->computeCCWAngle( edgeList[1] );
	}
	else
	{
		i = angleIndex( edgeList[1], edgeList[2] );
		if ( j == i )
		{
			ang[i] = edgeList[1]->computeCCWAngle( edgeList[2] );
		}
		else
		{
			i = angleIndex( edgeList[0], edgeList[2] );
			if ( j == i )
			{
				ang[i] = edgeList[2]->computeCCWAngle( edgeList[0] );
			}
			else
			{
				Msg::error( "Quad.updateAngle(Node): Node not found." );
			}
		}
	}
}

//TODO: Tests
void 
Triangle::updateAngles()
{
	int i;
	i = angleIndex( edgeList[0], edgeList[1] );
	ang[i] = edgeList[0]->computeCCWAngle( edgeList[1] );

	i = angleIndex( edgeList[1], edgeList[2] );
	ang[i] = edgeList[1]->computeCCWAngle( edgeList[2] );

	i = angleIndex( edgeList[0], edgeList[2] );
	ang[i] = edgeList[2]->computeCCWAngle( edgeList[0] );
}

//TODO: Tests
bool
Triangle::hasEdge( const std::shared_ptr<Edge>& e )
{
	if ( edgeList[0] == e || edgeList[1] == e || edgeList[2] == e )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//TODO: Tests
bool
Triangle::hasNode( const std::shared_ptr<Node>& n )
{
	if ( edgeList[0]->hasNode( n ) || edgeList[1]->hasNode( n ) || edgeList[2]->hasNode( n ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//TODO: Tests
std::shared_ptr<Edge> 
Triangle::otherEdge( const std::shared_ptr<Edge>& e )
{
	if ( edgeList[0] != e )
	{
		return edgeList[0];
	}
	else if ( edgeList[1] != e )
	{
		return edgeList[1];
	}
	if ( edgeList[2] != e )
	{
		return edgeList[2];
	}
	else
	{
		Msg::error( "Cannot find an other edge in triangle " + descr() + "." );
		return nullptr;
	}
}

//TODO: Tests
std::shared_ptr<Edge>
Triangle::otherEdge( const std::shared_ptr<Edge>& e1,
					 const std::shared_ptr<Edge>& e2 )
{
	if ( (edgeList[0] == e1 && edgeList[1] == e2) || (edgeList[1] == e1 && edgeList[0] == e2) )
	{
		return edgeList[2];
	}
	else if ( (edgeList[1] == e1 && edgeList[2] == e2) || (edgeList[2] == e1 && edgeList[1] == e2) )
	{
		return edgeList[0];
	}
	else if ( (edgeList[0] == e1 && edgeList[2] == e2) || (edgeList[2] == e1 && edgeList[0] == e2) )
	{
		return edgeList[1];
	}
	else
	{
		Msg::error( "Cannot find the other edge in triangle " + descr() + "." );
		return nullptr;
	}
}

//TODO: Tests
std::shared_ptr<Element> 
Triangle::neighbor( const std::shared_ptr<Edge>& e )
{
	if ( e->element1 == shared_from_this() )
	{
		return e->element2;
	}
	else if ( e->element2 == shared_from_this() )
	{
		return e->element1;
	}
	else
	{
		Msg::warning( "Triangle.neighbor(Edge): returning null" );
		return nullptr;
	}
}

//TODO: Tests
std::shared_ptr<Edge>
Triangle::neighborEdge( const std::shared_ptr<Node>& n,
						const std::shared_ptr<Edge>& e )
{
	if ( edgeList[0] != e && edgeList[0]->hasNode( n ) )
	{
		return edgeList[0];
	}
	else if ( edgeList[1] != e && edgeList[1]->hasNode( n ) )
	{
		return edgeList[1];
	}
	else if ( edgeList[2] != e && edgeList[2]->hasNode( n ) )
	{
		return edgeList[2];
	}
	else
	{
		Msg::error( "Triangle.neighborEdge(Node, Edge): Neighbor not found." );
		return nullptr; // e
	}
}

//TODO: Tests
int
Triangle::indexOf( const std::shared_ptr<Edge>& e )
{
	if ( edgeList[0] == e )
	{
		return 0;
	}
	else if ( edgeList[1] == e )
	{
		return 1;
	}
	else if ( edgeList[2] == e )
	{
		return 2;
	}
	else
	{
		Msg::warning( "Triangle.indexOf(Edge): Edge not part of triangle." );
		return -1;
	}
}

//TODO: Tests
int 
Triangle::angleIndex( int e1Index, int e2Index )
{
	// angle betw. edges 0 && 1
	if ( (e1Index == 0 && e2Index == 1) || (e1Index == 1 && e2Index == 0) )
	{
		return 2;
	}
	else if ( (e1Index == 1 && e2Index == 2) || (e1Index == 2 && e2Index == 1) )
	{
		return 0;
	}
	else if ( (e1Index == 0 && e2Index == 2) || (e1Index == 2 && e2Index == 0) )
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

//TODO: Tests
int 
Triangle::angleIndex( const std::shared_ptr<Node>& n )
{
	// angle betw. edges 0 && 1
	if ( edgeList[0]->commonNode( edgeList[1] ) == n )
	{
		return 2;
	}
	else if ( edgeList[1]->commonNode( edgeList[2] ) == n )
	{
		return 0;
	}
	else if ( edgeList[2]->commonNode( edgeList[0] ) == n )
	{
		return 1;
	}
	else
	{
		Msg::error( "Quad.angleIndex(Node): Node not found" );
		return -1;
	}
}

//TODO: Tests
int 
Triangle::angleIndex( const std::shared_ptr<Edge>& e1,
					  const std::shared_ptr<Edge>& e2 )
{
	return angleIndex( indexOf( e1 ), indexOf( e2 ) );
}

//TODO: Tests
std::shared_ptr<Edge>
Triangle::oppositeOfNode( const std::shared_ptr<Node>& n )
{
	if ( edgeList[0]->hasNode( n ) && edgeList[1]->hasNode( n ) )
	{
		return edgeList[2];
	}
	else if ( edgeList[0]->hasNode( n ) && edgeList[2]->hasNode( n ) )
	{
		return edgeList[1];
	}
	else if ( edgeList[1]->hasNode( n ) && edgeList[2]->hasNode( n ) )
	{
		return edgeList[0];
	}
	else
	{
		Msg::error( "Cannot find opposide edge of node." );
		return nullptr;
	}
}

//TODO: Tests
std::shared_ptr<Node>
Triangle::oppositeOfEdge( const std::shared_ptr<Edge>& e )
{
	if ( edgeList[0] == e )
	{
		if ( !e->hasNode( edgeList[1]->leftNode ) )
		{
			return edgeList[1]->leftNode;
		}
		else
		{
			return edgeList[1]->rightNode;
		}
	}
	else if ( edgeList[1] == e )
	{
		if ( !e->hasNode( edgeList[2]->leftNode ) )
		{
			return edgeList[2]->leftNode;
		}
		else
		{
			return edgeList[2]->rightNode;
		}
	}
	else if ( edgeList[2] == e )
	{
		if ( !e->hasNode( edgeList[0]->leftNode ) )
		{
			return edgeList[0]->leftNode;
		}
		else
		{
			return edgeList[0]->rightNode;
		}
	}
	else
	{
		Msg::error( "oppositeOfEdge: Cannot find node opposite of the supplied edge." );
		return nullptr;
	}
}

//TODO: Tests
void 
Triangle::connectEdges()
{
	edgeList[0]->connectToTriangle( shared_from_this() );
	edgeList[1]->connectToTriangle( shared_from_this() );
	edgeList[2]->connectToTriangle( shared_from_this() );
}

//TODO: Tests
void 
Triangle::disconnectEdges()
{
	edgeList[0]->disconnectFromElement( shared_from_this() );
	edgeList[1]->disconnectFromElement( shared_from_this() );
	edgeList[2]->disconnectFromElement( shared_from_this() );
}

//TODO: Tests
std::shared_ptr<Edge> 
Triangle::nextCCWEdge( const std::shared_ptr<Edge>& e1 )
{
	std::shared_ptr<Node> e1commone2 = nullptr, e1commone3 = nullptr;
	std::shared_ptr<Edge> e2 = nullptr, e3 = nullptr;
	if ( e1 == edgeList[0] )
	{
		e2 = edgeList[1];
		e3 = edgeList[2];
	}
	else if ( e1 == edgeList[1] )
	{
		e2 = edgeList[0];
		e3 = edgeList[2];
	}
	else if ( e1 == edgeList[2] )
	{
		e2 = edgeList[0];
		e3 = edgeList[1];
	}
	else
	{
		Msg::error( "Edge " + e1->descr() + " is not part of triangle" );
		return nullptr;
	}

	e1commone2 = e1->commonNode( e2 );
	e1commone3 = e1->commonNode( e3 );

	MyVector v2( e1commone2, e2->otherNode( e1commone2 ) );
	MyVector v3( e1commone3, e3->otherNode( e1commone3 ) );
	MyVector v1Forv2( e1commone2, e1->otherNode( e1commone2 ) );
	MyVector v1Forv3( e1commone3, e1->otherNode( e1commone3 ) );

	// Positive angles between e1 and each of the other two edges:
	double ang1, ang2;
	ang1 = ang[angleIndex( e1, e2 )];
	ang2 = ang[angleIndex( e1, e3 )];

	if ( v2.isCWto( v1Forv2 ) )
	{
		ang1 += PI;
	}
	if ( v3.isCWto( v1Forv3 ) )
	{
		ang2 += PI;
	}

	if ( ang2 > ang1 )
	{
		return e3;
	}
	else
	{
		return e2;
	}
}

//TODO: Tests
bool 
Triangle::areaLargerThan0() 
{
	auto na = edgeList[0]->leftNode;
	auto nb = edgeList[0]->rightNode;
	auto nc = oppositeOfEdge( edgeList[0] );

	if ( cross( na, nc, nb, nc ) != 0 )
	{ // The cross product nanc x nbnc
		return true;
	}
	else
	{ // The cross product nanc x nbnc
		return false;
	}

}

//TOD:Tests
bool 
Triangle::inverted( const std::shared_ptr<Node>& oldN,
					const std::shared_ptr<Node>& newN )
{
	auto e = oppositeOfNode( newN );
	// Check with edge e:
	double oldN_e_det = (e->leftNode->x - oldN->x) * (e->rightNode->y - oldN->y) - (e->leftNode->y - oldN->y) * (e->rightNode->x - oldN->x);
	double newN_e_det = (e->leftNode->x - newN->x) * (e->rightNode->y - newN->y) - (e->leftNode->y - newN->y) * (e->rightNode->x - newN->x);

	// If different sign, or 0, they are inverted:
	if ( oldN_e_det >= 0 )
	{
		if ( newN_e_det <= 0 || oldN_e_det == 0 )
		{
			Msg::debug( "Triangle.inverted(..): Triangle " + descr() + " is inverted" );
			return true;
		}
	}
	else if ( newN_e_det >= 0 )
	{
		Msg::debug( "Triangle.inverted(..): Triangle " + descr() + " is inverted" );
		return true;
	}
	return false;
}

//TODO: Tests
bool
Triangle::inverted() 
{
	auto a = firstNode;
	auto b = edgeList[0]->otherNode( a );
	auto c = edgeList[1]->rightNode;
	if ( c == a || c == b )
	{
		c = edgeList[1]->leftNode;
	}

	if ( cross( a, c, b, c ) < 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//TODO: Tests
bool 
Triangle::invertedOrZeroArea()
{
	auto a = firstNode;
	auto b = edgeList[0]->otherNode( a );
	auto c = edgeList[1]->rightNode;
	if ( c == a || c == b )
	{
		c = edgeList[1]->leftNode;
	}

	if ( cross( a, c, b, c ) <= 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//TODO: Tests
bool
Triangle::zeroArea()
{
	auto a = firstNode;
	auto b = edgeList[0]->otherNode( a );
	auto c = edgeList[1]->rightNode;
	if ( c == a || c == b )
	{
		c = edgeList[1]->leftNode;
	}

	if ( cross( a, c, b, c ) == 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//TODO: Tests
void 
Triangle::replaceEdge( const std::shared_ptr<Edge>& e,
					   const std::shared_ptr<Edge>& replacement )
{
	edgeList[indexOf( e )] = replacement;
}

//TODO: Tests
std::shared_ptr<Edge>
Triangle::getBoundaryEdge()
{
	if ( edgeList[0]->element2 == nullptr )
	{
		return edgeList[0];
	}
	else if ( edgeList[1]->element2 == nullptr )
	{
		return edgeList[1];
	}
	else if ( edgeList[2]->element2 == nullptr )
	{
		return edgeList[2];
	}
	else
	{
		return nullptr;
	}
}

//TODO: Tests
void 
Triangle::updateDistortionMetric() 
{
	updateDistortionMetric( sqrt3x2 );
}

//TODO: Tests
void
Triangle::updateDistortionMetric( double factor )
{
	Msg::debug( "Entering Triangle.updateDistortionMetric(..)" );
	double AB = edgeList[0]->len, CB = edgeList[1]->len, CA = edgeList[2]->len;
	auto a = edgeList[2]->commonNode( edgeList[0] ), b = edgeList[0]->commonNode( edgeList[1] ), c = edgeList[2]->commonNode( edgeList[1] );
	MyVector vCA( c, a ), vCB( c, b );

	double temp = factor * std::abs( vCA.cross( vCB ) ) / (CA * CA + AB * AB + CB * CB);
	if ( inverted() )
	{
		distortionMetric = -temp;
	}
	else
	{
		distortionMetric = temp;
	}
	Msg::debug( "Leaving Triangle.updateDistortionMetric(..): " + std::to_string(distortionMetric) );
}

//TODO: Tests
double 
Triangle::largestAngle() 
{
	double cand = ang[0];
	if ( ang[1] > cand )
	{
		cand = ang[1];
	}
	if ( ang[2] > cand )
	{
		cand = ang[2];
	}
	return cand;
}

//TODO: Tests
std::shared_ptr<Node>
Triangle::nodeAtLargestAngle()
{
	auto candNode = edgeList[0]->leftNode;
	double cand = ang[angleIndex( candNode )], temp;

	temp = ang[angleIndex( edgeList[0]->rightNode )];
	if ( temp > cand )
	{
		candNode = edgeList[0]->rightNode;
		cand = temp;
	}
	temp = ang[angleIndex( oppositeOfEdge( edgeList[0] ) )];
	if ( temp > cand )
	{
		candNode = oppositeOfEdge( edgeList[0] );
	}
	return candNode;
}

//TODO: Tests
double 
Triangle::longestEdgeLength()
{
	double temp = std::max( edgeList[0]->len, edgeList[1]->len );
	return std::max( temp, edgeList[2]->len );
}

//TODO: Tests
std::shared_ptr<Edge>
Triangle::longestEdge() 
{
	std::shared_ptr<Edge> temp;
	if ( edgeList[0]->len > edgeList[1]->len )
	{
		temp = edgeList[0];
	}
	else
	{
		temp = edgeList[1];
	}

	if ( edgeList[2]->len > temp->len )
	{
		return edgeList[2];
	}
	else
	{
		return temp;
	}
}

//TODO: Tests
void 
Triangle::markEdgesLegal() 

{
	edgeList[0]->color = Color::Green;
	edgeList[1]->color = Color::Green;
	edgeList[2]->color = Color::Green;
}

//TODO: Tests
void
Triangle::markEdgesIllegal() 
{
	edgeList[0]->color = Color::Red;
	edgeList[1]->color = Color::Red;
	edgeList[2]->color = Color::Red;
}

//TODO: Tests
std::string 
Triangle::descr()
{
	auto node1 = edgeList[0]->leftNode;
	auto node2 = edgeList[0]->rightNode;
	auto node3 = edgeList[1]->rightNode;
	if ( node3 == node1 || node3 == node2 )
	{
		node3 = edgeList[1]->leftNode;
	}

	return node1->descr() + ", " + node2->descr() + ", " + node3->descr();
}

//TODO: Tests
void 
Triangle::printMe()
{
	if ( inverted() )
	{
		std::cout << descr() + ", inverted";
	}
	else
	{
		std::cout << descr() + ", not inverted";
	}

}

//TODO: Tests
std::string 
Triangle::toString()
{
	return descr();
}