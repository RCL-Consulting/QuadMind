#include "pch.h"
#include "Triangle.h"

#include "Edge.h"
#include "Msg.h"
#include "MyVector.h"

Triangle::Triangle( Edge* edge1, Edge* edge2, Edge* edge3,
					double len1, double len2, double len3,
					double ang1, double ang2, double ang3,
					bool lengthsOpt, bool anglesOpt )
{
	edgeList.resize(3);

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

	Edge* temp;
	if ( firstNode == edgeList[0]->commonNode( edgeList[2] ) )
	{
		temp = edgeList[1];
		edgeList[1] = edgeList[2];
		edgeList[2] = temp;
	}

	ang.resize( 3 );
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
	}
}

Triangle::Triangle( Edge* edge1, Edge* edge2, Edge* edge3 )
{
	edgeList.resize( 3 );

	if ( edge1 == nullptr || edge2 == nullptr || edge3 == nullptr )
	{
		Msg::error( "Triangle: cannot create Triangle with null Edge." );
	}

	edgeList[0] = edge1;
	edgeList[1] = edge2;
	edgeList[2] = edge3;

	// Make a pointer to the base node that is the origin of vector(edgeList[a])
	// so that the cross product vector(edgeList[a]) x vector(edgeList[b]) >= 0
	// where a,b in {1,2}
	firstNode = edgeList[0]->leftNode;
	if ( inverted() )
	{
		firstNode = edgeList[0]->rightNode;
	}

	Edge* temp;
	if ( firstNode == edgeList[0]->commonNode( edgeList[2] ) )
	{
		temp = edgeList[1];
		edgeList[1] = edgeList[2];
		edgeList[2] = temp;
	}

	ang.resize( 3 );
	updateAngles();
}

Triangle::Triangle( const Triangle& t )
{
	edgeList.resize( 3 );

	edgeList[0] = new Edge( t.edgeList[0]->leftNode, t.edgeList[0]->rightNode );
	edgeList[1] = new Edge( t.edgeList[1]->leftNode, t.edgeList[1]->rightNode );
	edgeList[2] = new Edge( t.edgeList[2]->leftNode, t.edgeList[2]->rightNode );

	ang.resize( 3 );
	ang[0] = t.ang[0];
	ang[1] = t.ang[1];
	ang[2] = t.ang[2];

	firstNode = t.firstNode;
}

Element* 
Triangle::elementWithExchangedNodes( Node* original, Node* replacement )
{
	Node* node1 = edgeList[0]->leftNode;
	Node* node2 = edgeList[0]->rightNode;
	Node* node3 = edgeList[1]->rightNode;
	if ( node3 == node1 || node3 == node2 )
	{
		node3 = edgeList[1]->leftNode;
	}
	Node* common1;

	// Make a copy of the original triangle...
	Triangle* t = new Triangle( *this );
	Edge* edge1 = t->edgeList[0], *edge2 = t->edgeList[1], *edge3 = t->edgeList[2];

	// ... and then replace the node
	if ( node1 == original )
	{
		common1 = edge1->commonNode( edge2 );
		if ( original == common1 )
		{
			edge2->replaceNode( original, replacement );
		}
		else
		{
			edge3->replaceNode( original, replacement );
		}
		edge1->replaceNode( original, replacement );
	}
	else if ( node2 == original )
	{
		common1 = edge1->commonNode( edge2 );
		if ( original == common1 )
		{
			edge2->replaceNode( original, replacement );
		}
		else
		{
			edge3->replaceNode( original, replacement );
		}
		edge1->replaceNode( original, replacement );
	}
	else if ( node3 == original )
	{
		common1 = edge2->commonNode( edge1 );
		if ( original == common1 )
		{
			edge1->replaceNode( original, replacement );
		}
		else
		{
			edge3->replaceNode( original, replacement );
		}
		edge2->replaceNode( original, replacement );
	}
	else
	{
		return nullptr;
	}

	if ( original == firstNode )
	{
		t->firstNode = replacement;
	}
	else
	{
		t->firstNode = firstNode;
	}
	return t;

	return nullptr;
}

bool
Triangle::invertedWhenNodeRelocated( Node* n1, Node* n2 )
{
	Msg::debug( "Entering Triangle.invertedWhenNodeRelocated(..)" );
	Node* a, *b, *c;
	MyVector ac, bc;
	a = firstNode;
	b = edgeList[0]->otherNode( a );
	c = edgeList[1]->rightNode;
	if ( c == a || c == b )
	{
		c = edgeList[1]->leftNode;
	}

	if ( a == n1 )
	{
		a = n2;
	}
	else if ( b == n1 )
	{
		b = n2;
	}
	else if ( c == n1 )
	{
		c = n2;
	}

	ac = MyVector( *a, *c );
	bc = MyVector( *b, *c );

	Msg::debug( "Leaving Triangle.invertedWhenNodeRelocated(..)" );
	if ( ac.cross( bc ) <= 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool 
Triangle::equals( const Triangle& t )
{
	Node* node1, *node2, *node3;
	Node *tnode1, *tnode2, *tnode3;
	node1 = edgeList[0]->leftNode;
	node2 = edgeList[0]->rightNode;
	node3 = edgeList[1]->rightNode;
	if ( node3 == node1 || node3 == node2 )
	{
		node3 = edgeList[1]->leftNode;
	}

	tnode1 = t.edgeList[0]->leftNode;
	tnode2 = t.edgeList[0]->rightNode;
	tnode3 = t.edgeList[1]->rightNode;
	if ( tnode3 == tnode1 || tnode3 == tnode2 )
	{
		tnode3 = t.edgeList[1]->leftNode;
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

double 
Triangle::angle( Edge* e, Node* n )
{
	return ang[angleIndex( e, neighborEdge( n, e ) )];
}

double
Triangle::angle( Edge* e1, Edge* e2 )
{
	return ang[angleIndex( e1, e2 )];
}

void
Triangle::updateAttributes()
{
	updateLengths();
	updateAngles();
}

void 
Triangle::updateLengths()
{
	edgeList[0]->len = edgeList[0]->computeLength();
	edgeList[1]->len = edgeList[1]->computeLength();
	edgeList[2]->len = edgeList[2]->computeLength();
}

void
Triangle::updateAngle( Node* n )
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

bool 
Triangle::hasEdge( Edge* e )
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

bool
Triangle::hasNode( Node* n )
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

Edge* 
Triangle::otherEdge( Edge* e )
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

Edge*
Triangle::otherEdge( Edge* e1, Edge* e2 )
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

Element*
Triangle::neighbor( Edge* e )
{
	if ( e->element1 == this )
	{
		return e->element2;
	}
	else if ( e->element2 == this )
	{
		return e->element1;
	}
	else
	{
		Msg::warning( "Triangle.neighbor(Edge): returning null" );
		return nullptr;
	}
}

Edge*
Triangle::neighborEdge( Node* n, Edge* e )
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
		return nullptr;
	}
}

int
Triangle::indexOf( Edge* e )
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

int
Triangle::angleIndex( Node* n )
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

int
Triangle::angleIndex( Edge* e1, Edge* e2 )
{
	return angleIndex( indexOf( e1 ), indexOf( e2 ) );
}

Edge* 
Triangle::oppositeOfNode( Node* n )
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

Node*
Triangle::oppositeOfEdge( Edge* e )
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

void 
Triangle::connectEdges()
{
	edgeList[0]->connectToTriangle( this );
	edgeList[1]->connectToTriangle( this );
	edgeList[2]->connectToTriangle( this );
}

void 
Triangle::disconnectEdges()
{
	edgeList[0]->disconnectFromElement( this );
	edgeList[1]->disconnectFromElement( this );
	edgeList[2]->disconnectFromElement( this );
}

Edge* 
Triangle::nextCCWEdge( Edge* e1 )
{
	Node* e1commone2, *e1commone3;
	Edge* e2, *e3;
	MyVector v1Forv2, v1Forv3, v2, v3;
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

	v2 = MyVector( *e1commone2, *e2->otherNode( e1commone2 ) );
	v3 = MyVector( *e1commone3, *e3->otherNode( e1commone3 ) );
	v1Forv2 = MyVector( *e1commone2, *e1->otherNode( e1commone2 ) );
	v1Forv3 = MyVector( *e1commone3, *e1->otherNode( e1commone3 ) );

	// Positive angles between e1 and each of the other two edges:
	double ang1, ang2;
	ang1 = ang[angleIndex( e1, e2 )];
	ang2 = ang[angleIndex( e1, e3 )];

	if ( v2.isCWto( v1Forv2 ) )
	{
		ang1 += std::numbers::pi;
	}
	if ( v3.isCWto( v1Forv3 ) )
	{
		ang2 += std::numbers::pi;
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

Edge* 
Triangle::nextCWEdge( Edge* e1 )
{
	Node* e1commone2,* e1commone3;
	Edge *e2, *e3;
	MyVector v1Forv2, v1Forv3, v2, v3;
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

	v2 = MyVector( *e1commone2, *e2->otherNode( e1commone2 ) );
	v3 = MyVector( *e1commone3, *e3->otherNode( e1commone3 ) );
	v1Forv2 = MyVector( *e1commone2, *e1->otherNode( e1commone2 ) );
	v1Forv3 = MyVector( *e1commone3, *e1->otherNode( e1commone3 ) );

	// Positive angles between e1 and each of the other two edges:
	double ang1, ang2;
	ang1 = ang[angleIndex( e1, e2 )];
	ang2 = ang[angleIndex( e1, e3 )];

	// Transform into true angles:
	e1commone2 = e1->commonNode( e2 );
	e1commone3 = e1->commonNode( e3 );

	v2 = MyVector( *e1commone2, *e2->otherNode( e1commone2 ) );
	v3 = MyVector( *e1commone3, *e3->otherNode( e1commone3 ) );
	v1Forv2 = MyVector( *e1commone2, *e1->otherNode( e1commone2 ) );
	v1Forv3 = MyVector( *e1commone3, *e1->otherNode( e1commone3 ) );

	if ( v2.isCWto( v1Forv2 ) )
	{
		ang1 += std::numbers::pi;
	}
	if ( v3.isCWto( v1Forv3 ) )
	{
		ang2 += std::numbers::pi;
	}

	if ( ang2 > ang1 )
	{
		return e2;
	}
	else
	{
		return e3;
	}
}

bool 
Triangle::areaLargerThan0()
{
	Node* na = edgeList[0]->leftNode;
	Node* nb = edgeList[0]->rightNode;
	Node* nc = oppositeOfEdge( edgeList[0] );

	if ( cross( na, nc, nb, nc ) != 0 )
	{ // The cross product nanc x nbnc
		return true;
	}
	else
	{ // The cross product nanc x nbnc
		return false;
	}
}

bool
Triangle::inverted( Node* oldN, Node* newN )
{
	Edge* e = oppositeOfNode( newN );
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

bool 
Triangle::inverted()
{
	Node* a, *b, *c;
	a = firstNode;
	b = edgeList[0]->otherNode( a );
	c = edgeList[1]->rightNode;
	if ( c == a || c == b )
	{
		c = edgeList[1]->leftNode;
	}

	if ( cross( a, c, b, c ) < 0 )
	{
		// if (!ac.newcross(bc))
		return true;
	}
	else
	{
		return false;
	}
}

bool
Triangle::invertedOrZeroArea()
{
	Node* a, *b, *c;
	a = firstNode;
	b = edgeList[0]->otherNode( a );
	c = edgeList[1]->rightNode;
	if ( c == a || c == b )
	{
		c = edgeList[1]->leftNode;
	}

	if ( cross( a, c, b, c ) <= 0 )
	{
		// if (!ac.newcross(bc))
		return true;
	}
	else
	{
		return false;
	}
}

bool
Triangle::zeroArea()
{
	Node* a, *b, *c;
	a = firstNode;
	b = edgeList[0]->otherNode( a );
	c = edgeList[1]->rightNode;
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

void 
Triangle::replaceEdge( Edge* e, Edge* replacement )
{
	edgeList[indexOf( e )] = replacement;
}

Edge*
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

void 
Triangle::updateDistortionMetric()
{
	updateDistortionMetric( sqrt3x2 );
}

void 
Triangle::updateDistortionMetric( double factor )
{
	Msg::debug( "Entering Triangle.updateDistortionMetric(..)" );
	double AB = edgeList[0]->len, CB = edgeList[1]->len, CA = edgeList[2]->len;
	Node* a = edgeList[2]->commonNode( edgeList[0] ), *b = edgeList[0]->commonNode( edgeList[1] ), *c = edgeList[2]->commonNode( edgeList[1] );
	MyVector vCA = MyVector( *c, *a ), vCB = MyVector( *c, *b );

	double temp = factor * abs( vCA.cross( vCB ) ) / (CA * CA + AB * AB + CB * CB);
	if ( inverted() )
	{
		distortionMetric = -temp;
	}
	else
	{
		distortionMetric = temp;
	}
	Msg::debug( "Leaving Triangle.updateDistortionMetric(..): " + std::to_string( distortionMetric ) );
}

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

Node*
Triangle::nodeAtLargestAngle()
{
	Node* candNode = edgeList[0]->leftNode;
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

double
Triangle::longestEdgeLength()
{
	double temp = std::max( edgeList[0]->len, edgeList[1]->len );
	return std::max( temp, edgeList[2]->len );
}

Edge*
Triangle::longestEdge()
{
	Edge* temp;
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

void 
Triangle::markEdgesLegal()
{
	edgeList[0]->color = 1;//java.awt.Color.green;
	edgeList[1]->color = 1;//java.awt.Color.green;
	edgeList[2]->color = 1;//java.awt.Color.green;
}

void
Triangle::markEdgesIllegal()
{
	edgeList[0]->color = 1;//java.awt.Color.red;
	edgeList[1]->color = 1;//java.awt.Color.red;
	edgeList[2]->color = 1;//java.awt.Color.red;
}

std::string 

Triangle::descr()
{
	Node* node1, *node2, *node3;
	node1 = edgeList[0]->leftNode;
	node2 = edgeList[0]->rightNode;
	node3 = edgeList[1]->rightNode;
	if ( node3 == node1 || node3 == node2 )
	{
		node3 = edgeList[1]->leftNode;
	}

	return node1->descr() + ", " + node2->descr() + ", " + node3->descr();
}

void 
Triangle::printMe()
{
	if ( inverted() )
	{
		std::cout << descr() << ", inverted" << std::endl;
	}
	else
	{
		std::cout << descr() << ", not inverted" << std::endl;
	}

}

std::string
Triangle::toString()
{
	return descr();
}