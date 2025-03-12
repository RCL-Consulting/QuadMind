#include "pch.h"
#include "Node.h"
#include "MyVector.h"
#include "Quad.h"
#include "Ray.h"
#include "Edge.h"
#include "Triangle.h"

#include <string>

Node Node::origin{ 0.0, 0.0 };

bool 
Node::equals( const Node* node ) const
{
	const double kZero = 1e-12;
	if ( abs( x - node->x ) < kZero && abs( y - node->y ) < kZero )
	{
		return true;
	}
	return false;
}

Node* 
Node::copy()
{
	auto n = new Node( x, y );
	n->edgeList = edgeList;
	return n;
}

Node*
Node::copyXY()
{
	return new Node( x, y );
}

void
Node::setXY( const Node& n )
{
	x = n.x;
	y = n.y;
}

void
Node::update()
{
	updateLRinEdgeList();
	updateEdgeLengths();
	updateAngles();
}

void
Node::moveToPos( double x, double y )
{
	this->x = x;
	this->y = y;

	updateLRinEdgeList();
}

void 
Node::moveTo( const Node& n )
{
	x = n.x;
	y = n.y;

	updateLRinEdgeList();
}

void 
Node::updateEdgeLengths()
{
	for ( auto e : edgeList )
	{
		e->len = e->computeLength();
	}
}

void 
Node::updateLRinEdgeList()
{
	bool btemp;
	Edge* temp;
	Node* node;
	Quad* q;
	for ( auto e : edgeList )
	{
		if ( (e->leftNode->x > e->rightNode->x) || (e->leftNode->x == e->rightNode->x && e->leftNode->y < e->rightNode->y) )
		{
			node = e->leftNode;
			e->leftNode = e->rightNode;
			e->rightNode = node;

			if ( e->frontEdge )
			{
				temp = e->leftFrontNeighbor;
				e->leftFrontNeighbor = e->rightFrontNeighbor;
				e->rightFrontNeighbor = temp;
				btemp = e->leftSide;
				e->leftSide = e->rightSide;
				e->rightSide = btemp;
			}

			if ( dynamic_cast<Quad*>(e->element1) )
			{
				q = static_cast<Quad*>(e->element1);
				if ( e == q->edgeList[base] )
				{
					q->updateLR();
				}
			}

			if ( dynamic_cast<Quad*>(e->element2) )
			{
				q = static_cast<Quad*>(e->element2);
				if ( e == q->edgeList[base] )
				{
					q->updateLR();
				}
			}
		}
	}
}

void 
Node::updateAngles()
{
	Msg::debug( "Entering Node.updateAngles()" );
	Edge* e, * ne;
	Node* other1, * other2;
	Quad* q;
	std::vector<Element*> list;

	for ( auto element : edgeList )
	{
		e = element;
		Msg::debug( "...e: " + e->descr() );
		if ( std::find( list.begin(), list.end(), e->element1 ) == list.end() )
		{
			Msg::debug( "...e.element1: " + e->element1->descr() );
			list.push_back( e->element1 );
			ne = e->element1->neighborEdge( this, e );
			Msg::debug( "...getting other1:  (elem1)" );
			other1 = e->otherNode( this );
			Msg::debug( "...getting other2:  (elem1)" );

			other2 = ne->otherNode( this );

			if ( dynamic_cast<Triangle*>(e->element1) )
			{
				e->element1->updateAngles();
			}
			else
			{
				q = static_cast<Quad*>(e->element1);
				q->updateAnglesExcept( q->oppositeNode( this ) );
			}
		}
		if ( e->element2 != nullptr && std::find( list.begin(), list.end(), e->element2 ) == list.end() )
		{
			list.push_back( e->element2 );
			ne = e->element2->neighborEdge( this, e );
			Msg::debug( "...getting other1:  (elem2)" );
			other1 = e->otherNode( this );
			Msg::debug( "...getting other2:  (elem2)" );

			other2 = ne->otherNode( this );

			if ( dynamic_cast<Triangle*>(e->element2) )
			{
				e->element2->updateAngles();
			}
			else
			{
				q = static_cast<Quad*>(e->element2);
				q->updateAnglesExcept( q->oppositeNode( this ) );
			}
		}
	}
	Msg::debug( "Leaving Node.updateAngles()" );
}

double 
Node::cross( const Node& n )
{
	return x * n.y - n.x * y;
}

void 
Node::connectToEdge( Edge* edge )
{
	auto Iter = std::find( edgeList.begin(), edgeList.end(), edge );
	if ( Iter == edgeList.end() )
	{
		edgeList.push_back( edge );
	}
}

std::vector<MyVector>
Node::ccwSortedVectorList()
{
	Element* elem = nullptr;
	Element* start = nullptr;
	MyVector v, v0, v1;
	std::vector<MyVector> boundaryVectors, vectors;

	for ( auto edge : edgeList )
	{
		auto e = edge;
		v = e->getVector( this );
		v.edge = e;

		if ( e->boundaryEdge() )
		{
			boundaryVectors.push_back( v );
		}
		else
		{
			vectors.push_back( v );
		}
	}

	// If the edgeList contains boundary edges, then select the vector of most CW
	// of these.
	// Else select the vector of an arbitrary edge.
	// The selected vector is put into v0.
	// Sets elem to the element that is ccw to v0 around this Node

	if ( boundaryVectors.size() > 0 )
	{ // this size is always 0 or 2
		Msg::debug( "...boundaryVectors yeah!" );
		v0 = boundaryVectors[0];
		v1 = boundaryVectors[1];

		elem = v0.edge->element1;
		auto e = elem->neighborEdge( this, v0.edge );
		v = e->getVector( this );
		v.edge = e;

		if ( v0.isCWto( v ) )
		{
			if ( elem->concavityAt( this ) )
			{
				v0 = v1;
				elem = v1.edge->element1;
			}
		}
		else if ( !elem->concavityAt( this ) )
		{
			v0 = v1;
			elem = v1.edge->element1;
		}
	}
	else
	{
		Msg::debug( "...boundaryVectors noooo!" );
		v0 = vectors[0];
		elem = v0.edge->element1;
		auto e = elem->neighborEdge( this, v0.edge );
		v1 = e->getVector( this );
		v1.edge = e;

		if ( v0.isCWto( v1 ) )
		{
			if ( elem->concavityAt( this ) )
			{
				v0 = v1;
			}
		}
		else if ( !elem->concavityAt( this ) )
		{
			v0 = v1;
		}
	}

	Msg::debug( "Node.ccwSortedVectorList(..): 0: " + v0.edge->descr() );

	// Sort vectors in ccw order starting with v0.
	// Uses the fact that elem initially is the element ccw to v0 around this Node.
	std::vector<MyVector> VS;
	auto e = v0.edge;

	start = elem;
	do
	{
		v = e->getVector( this );
		v.edge = e;
		Msg::debug( "... VS.add(" + v.descr() + ")" );
		VS.push_back( v );

		e = elem->neighborEdge( this, e );
		elem = elem->neighbor( e );
	} while ( elem != start && elem != nullptr );

	if ( elem == nullptr )
	{
		v = e->getVector( this );
		v.edge = e;
		Msg::debug( "... VS.add(" + v.descr() + ")" );
		VS.push_back( v );
	}

	return VS;
}

std::vector<Edge*> 
Node::calcCCWSortedEdgeList( Edge* b0, Edge* b1 )
{
	MyVector v, v0, v1;
	Edge* e;
	std::vector<MyVector> vectors;

	for ( auto element : edgeList )
	{
		e = element;
		if ( e != b0 && e != b1 )
		{
			v = e->getVector( this );
			v.edge = e;
			vectors.push_back( v );
		}
	}

	// Initially put the two vectors of b0 and b1 in list.
	// Select the most CW boundary edge to be first in list.

	std::vector<MyVector> VS;
	v0 = b0->getVector( this );
	v0.edge = b0;
	v1 = b1->getVector( this );
	v1.edge = b1;

	if ( vectors.size() > 0 )
	{
		v = vectors[0];
	}
	else
	{
		v = v1;
	}

	if ( v0.isCWto( v ) )
	{
		VS.push_back( v0 );
		VS.push_back( v1 );
	}
	else
	{
		VS.push_back( v1 );
		VS.push_back( v0 );
	}

	Msg::debug( "Node.calcCCWSortedEdgeList(..): 0: " + v0.edge->descr() );
	Msg::debug( "Node.calcCCWSortedEdgeList(..): 1: " + v1.edge->descr() );

	// Sort vectors in ccw order. I will not move the vector that lies first in VS.
	Msg::debug( "...vectors.size()= " + vectors.size() );
	for ( MyVector vector : vectors )
	{
		v = vector;

		for ( int j = 0; j < VS.size(); j++ )
		{
			v0 = VS[j];
			if ( j + 1 == VS.size() )
			{
				v1 = VS[0];
			}
			else
			{
				v1 = VS[j + 1];
			}

			if ( !v.isCWto( v0 ) && v.isCWto( v1 ) )
			{
				VS.insert( VS.begin() + j + 1, v );
				Msg::debug( "Node.calcCCWSortedEdgeList(..):" + std::to_string(j + 1) + ": " + v.edge->descr() );
				break;
			}
		}
	}

	std::vector<Edge*> edges( VS.size(), nullptr );
	for ( int i = 0; i < VS.size(); i++ )
	{
		edges[i] = v.edge;
	}

	return edges;
}

std::vector<Node*> 
Node::ccwSortedNeighbors()
{
	Msg::debug("Entering Node.ccwSortedNeighbors(..)");
	Element* elem;
	MyVector v, v0, v1;

	// First try to find two boundary edges
	int j = 0;
	std::array<MyVector, 2> b;
	for ( auto e : edgeList )
	{
		if ( e->boundaryEdge() )
		{
			b[j] = e->getVector( this );
			b[j++].edge = e;
			if ( j == 2 )
			{
				break;
			}
		}
	}

	Edge* e;
	// If these are found, then v0 is the vector of the most cw edge.
	if ( j == 2 )
	{
		elem = b[0].edge->element1;
		e = elem->neighborEdge( this, b[0].edge );
		v1 = e->getVector( this );
		v1.edge = e;

		if ( b[0].isCWto( v1 ) )
		{
			v0 = b[0];
		}
		else
		{
			v0 = b[1]; // that is, the other boundary vector
			elem = b[1].edge->element1;
		}
	}
	else
	{
		// Failing to find any boundary edges, we
		// select the vector of an arbitrary edge to be v0.
		// Sets elem to the element that is ccw to v0 around this Node
		e = edgeList[0];
		v0 = e->getVector( this );
		v0.edge = e;
		elem = e->element1;
		e = elem->neighborEdge( this, e );
		v1 = e->getVector( this );
		v1.edge = e;

		if ( v0.isCWto( v1 ) )
		{
			v0 = v0;
		}
		else
		{
			v0 = v1;
		}
	}

	// Sort nodes in ccw order starting with otherNode of v0 edge.
	// Uses the fact that elem initially is the element ccw to v0 around this Node.
	std::vector<Node*> ccwNodeList(edgeList.size() * 2,nullptr);
	Element* start = elem;
	Quad* q;
	e = v0.edge;
	Msg::debug( "... 1st node: " + e->otherNode( this )->descr() );

	int i = 0;
	do
	{
		ccwNodeList[i++] = e->otherNode( this );
		if ( !elem->IsAQuad() )
		{
			return std::vector<Node*>();
		}
		q = static_cast<Quad*>(elem);
		ccwNodeList[i++] = q->oppositeNode( this );
		e = elem->neighborEdge( this, e );
		elem = elem->neighbor( e );
	} while ( elem != start && elem != nullptr );

	if ( elem == nullptr )
	{
		ccwNodeList[i++] = e->otherNode( this );
	}

	Msg::debug( "Leaving Node.ccwSortedNeighbors(..): # nodes: " + std::to_string( i ) );
	return ccwNodeList;
}

double
Node::meanNeighborEdgeLength()
{
	double sumLengths = 0.0, len, j = 0;

	for ( auto e : edgeList )
	{

		len = e->length();
		if ( len != 0 )
		{
			j++;
			sumLengths += len;
		}
	}
	return sumLengths / j;
}

int
Node::nrOfAdjElements()
{
	return static_cast<int>(adjElements().size());
}

std::vector<Element*>
Node::adjElements()
{
	Edge* e;
	std::vector<Element*> list;

	for ( auto element : edgeList )
	{
		e = element;
		if ( std::find( list.begin(), list.end(), e->element1 ) == list.end() )
		{
			list.push_back( e->element1 );
		}

		if ( e->element2 != nullptr && std::find( list.begin(), list.end(), e->element2 ) == list.end() )
		{
			list.push_back( e->element2 );
		}
	}
	return list;
}

int
Node::nrOfAdjQuads()
{
	return static_cast<int>(adjQuads().size());
}

std::vector<Element*> 
Node::adjQuads()
{
	Edge* e;
	std::vector<Element*> list;

	for ( auto element : edgeList )
	{
		e = element;
		if ( e->element1->IsAQuad() && std::find( list.begin(), list.end(), e->element1 ) == list.end() )
		{
			list.push_back( e->element1 );
		}
		else if ( e->element2 != nullptr && e->element2->IsAQuad() && 
				  std::find( list.begin(), list.end(), e->element2 ) == list.end() )
		{
			list.push_back( e->element2 );
		}
	}
	return list;
}

int
Node::nrOfAdjTriangles()
{
	return static_cast<int>(adjTriangles().size());
}

std::vector<Triangle*>
Node::adjTriangles()
{
	std::vector<Triangle*> list;

	for ( auto e : edgeList )
	{
		if ( e->element1->IsATriangle() && std::find( list.begin(), list.end(), e->element1 ) == list.end() )
		{
			list.push_back( static_cast<Triangle*>(e->element1) );
		}
		else if ( e->element2 != nullptr && e->element2->IsATriangle() && 
				  std::find( list.begin(), list.end(), e->element2 ) == list.end() )
		{
			list.push_back( static_cast<Triangle*>(e->element2) );
		}
	}
	return list;
}

MyVector 
Node::laplacianMoveVector()
{
	MyVector c, cJSum(origin, origin);
	Node* nJ;

	for ( auto e : edgeList )
	{
		nJ = e->otherNode( this );
		c = MyVector( *this, *nJ );
		cJSum = cJSum.plus( c );
	}
	cJSum = cJSum.div( static_cast<double>(edgeList.size()) );
	return cJSum;
}

Node* 
Node::laplacianSmooth()
{
	//RC - Should maybe not return a pointer?
	MyVector c, cJSum(origin, origin);
	Node* nJ;

	for ( auto e:edgeList )
	{
		nJ = e->otherNode( this );
		c = MyVector( *this,* nJ );
		cJSum = cJSum.plus( c );
	}
	cJSum = cJSum.div( static_cast<double>(edgeList.size()) );
	return new Node( x + cJSum.x, y + cJSum.y );
}

Node*
Node::laplacianSmoothExclude( Node* node )
{
	//RC - Should maybe not return a pointer?
	MyVector c, cJSum(origin, origin);
	Node* nJ;

	for ( auto e : edgeList )
	{
		nJ = e->otherNode( this );
		if ( nJ != node )
		{
			c = MyVector( *this, *nJ );
			cJSum = cJSum.plus( c );
		}
	}
	cJSum = cJSum.div( static_cast<double>(edgeList.size()) - 1 ); // -1 because node is excluded
	return new Node( x + cJSum.x, y + cJSum.y );
}

Node* 
Node::modifiedLWLaplacianSmooth()
{
	Msg::debug("Entering Node.modifiedLWLaplacianSmooth()...");
	Msg::debug( "this= " + descr() );

	Node* nJ;
	double cJLengthSum = 0, len;
	Edge* e, *bEdge1, *bEdge2;
	MyVector c, cJLengthMulcJSum( origin, origin ), deltaCj, deltaI;
	int n = static_cast<int>(edgeList.size());
	if ( n == 0 )
	{
		Msg::error( "...edgeList.size()== 0" );
	}
	for ( int i = 0; i < n; i++ )
	{
		e = edgeList[i];
		Msg::debug( "e= " + e->descr() );
		nJ = e->otherNode( this );
		c = MyVector( *this, *nJ );
		if ( nJ->boundaryNode() )
		{
			bEdge1 = nJ->anotherBoundaryEdge( nullptr );
			bEdge2 = nJ->anotherBoundaryEdge( bEdge1 );
			if ( bEdge1 == nullptr )
			{
				Msg::debug( "bEdge1==null" );
			}
			else
			{
				Msg::debug( "bEdge1: " + bEdge1->descr() );
			}
			if ( bEdge2 == nullptr )
			{
				Msg::debug( "bEdge2==null" );
			}
			else
			{
				Msg::debug( "bEdge2: " + bEdge2->descr() );
			}

			// This should be correct:
			deltaCj = nJ->angularSmoothnessAdjustment( this, bEdge1, bEdge2, e->length() );
			Msg::debug( "c= " + c.descr() );
			c = c.plus( deltaCj );
			Msg::debug( "c+deltaCj= " + c.descr() );
		}

		len = c.length();
		c = c.mul( len );
		cJLengthMulcJSum = cJLengthMulcJSum.plus( c );
		cJLengthSum += len;
	}
	Msg::debug( "...cJLengthSum: " + std::to_string(cJLengthSum) );
	Msg::debug( "...cJLengthMulcJSum: x: " + std::to_string(cJLengthMulcJSum.x) + ", y: " + std::to_string(cJLengthMulcJSum.y) );

	deltaI = cJLengthMulcJSum.div( cJLengthSum );

	auto node = new Node( x + deltaI.x, y + deltaI.y );
	Msg::debug( "Leaving Node.modifiedLWLaplacianSmooth()... returns node= " + node->descr() );
	return node;
}

int
Node::nrOfFrontEdges()
{
	int fronts = 0;
	for ( auto e : edgeList )
	{
		if ( e->frontEdge )
		{
			fronts++;
		}
	}
	return fronts;
}

Node* 
Node::blackerSmooth( Node* nJ, Edge* front1, Edge* front2, double ld )
{
	Msg::debug("Entering blackerSmooth(..)...");

	Node* nI = this;
	Node originNode( 0, 0 );
	auto adjQuads = this->adjQuads();

	// Step 1, the isoparametric smooth:
	Msg::debug( "...step 1..." );
	MyVector vI( origin, *nI );
	MyVector vMXsum( origin, origin );
	MyVector vMJ;
	MyVector vMK;
	MyVector vML;

	for ( auto adjQuad : adjQuads )
	{
		auto q = static_cast<Quad*>(adjQuad);

		auto n1 = q->edgeList[base]->leftNode;
		auto n2 = q->edgeList[base]->rightNode;
		auto n3 = q->edgeList[left]->otherNode( q->edgeList[base]->leftNode );
		auto n4 = q->edgeList[right]->otherNode( q->edgeList[base]->rightNode );

		// Sorting vMJ, vMK, and vML in ccw order:
		if ( nI == n1 )
		{
			vMJ = MyVector( origin, *n2 );
			vMK = MyVector( origin, *n4 );
			vML = MyVector( origin, *n3 );
		}
		else if ( nI == n2 )
		{
			vMJ = MyVector( origin, *n4 );
			vMK = MyVector( origin, *n3 );
			vML = MyVector( origin, *n1 );
		}
		else if ( nI == n3 )
		{
			vMJ = MyVector( origin, *n1 );
			vMK = MyVector( origin, *n2 );
			vML = MyVector( origin, *n4 );
		}
		else
		{ // if (nI==n4) {
			vMJ = MyVector( origin, *n3 );
			vMK = MyVector( origin, *n1 );
			vML = MyVector( origin, *n2 );
		}

		vMXsum = vMXsum.plus( vMJ );
		vMXsum = vMXsum.plus( vML );
		vMXsum = vMXsum.minus( vMK );
	}

	MyVector vImarked = vMXsum.div( static_cast<double>(adjQuads.size()) );
	MyVector deltaA = vImarked.minus( vI );

	if ( adjQuads.size() != 2 || nrOfFrontEdges() > 2 )
	{
		Msg::debug( "Leaving blackerSmooth(..)..." );
		return new Node( x + deltaA.x, y + deltaA.y );
	}
	// Step 2, length adjustment:
	else
	{
		Msg::debug( "...step 2..." );
		MyVector vJ = MyVector( origin, *nJ );
		MyVector vIJ = MyVector( *nJ, vImarked.x, vImarked.y );
		double la = vIJ.length();

		MyVector deltaB = deltaA.plus( vI );
		deltaB = deltaB.minus( vJ );
		deltaB = deltaB.mul( ld / la );
		deltaB = deltaB.plus( vJ );
		deltaB = deltaB.minus( vI );

		// Step 3, angular smoothness:
		Msg::debug( "...step 3..." );
		MyVector deltaC = angularSmoothnessAdjustment( nJ, front1, front2, ld );
		MyVector deltaI = deltaB.plus( deltaC );
		deltaI = deltaI.mul( 0.5 );
		Msg::debug( "Leaving blackerSmooth(..)..." );
		return new Node( x + deltaI.x, y + deltaI.y );
	}
	return nullptr;
}

MyVector 
Node::angularSmoothnessAdjustment( Node* nJ, Edge* f1, Edge* f2, double ld )
{
	const double kZero = 1e-12;
	Msg::debug( "Entering angularSmoothnessAdjustment(..) ..." );
	Node* nI = this;
	Msg::debug( "nI= " + nI->descr() );
	Msg::debug( "nJ= " + nJ->descr() );

	if ( std::isnan( ld ) )
	{
		Msg::error( "ld is NaN!!!" );
	}

	if ( abs( f2->length() ) < kZero )
	{
		Msg::error( "f2.length()== 0" );
	}

	Msg::debug( "f1= " + f1->descr() );
	Msg::debug( "f2= " + f2->descr() );

	auto nIm1 = f1->otherNode( nI );
	auto nIp1 = f2->otherNode( nI );

	Msg::debug( "nIp1= " + nIp1->descr() );

	if ( nIm1->equals( nI ) )
	{
		Msg::error( "nIm1.equals(nI)" );
	}

	if ( nIp1->equals( nI ) )
	{
		Msg::error( "nIp1.equals(nI)" );
	}

	// if (nIm1.equals(nIp1))
	// this should be okay, in fact...
	// Msg::error("nIm1.equals(nIp1)");

	MyVector pI1 = MyVector( *nJ, *nIm1 );
	MyVector pI = MyVector( *nJ, *nI );
	MyVector pI2 = MyVector( *nJ, *nIp1 );

	double pI1Angle = pI1.posAngle();
	double pI2Angle = pI2.posAngle();
	double pIp1Angle = std::max( pI1Angle, pI2Angle );
	double pIm1Angle = std::min( pI1Angle, pI2Angle );
	double pIAngle = pI.posAngle();
	double pIm1p1Angle = 0;
	if ( pIAngle < pIm1Angle || pIAngle > pIp1Angle )
	{
		pIm1p1Angle = PIx2 - pIp1Angle + pIm1Angle;
	}
	else
	{
		pIm1p1Angle = pIp1Angle - pIm1Angle;
	}

	Msg::debug( "pIAngle= " + std::to_string( toDegrees * pIAngle ) );
	Msg::debug( "pIp1Angle= " + std::to_string( toDegrees * pIp1Angle ) );
	Msg::debug( "pIm1Angle= " + std::to_string( toDegrees * pIm1Angle ) );

	// Check if the sum of angles between pIp1 and pI and the angle between pIm1 and
	// PI is greater or equal to 180 degrees. If so, I choose ld as the length of
	// pB2.
	if ( pIm1p1Angle > std::numbers::pi )
	{
		Msg::debug( "okei, we're in there.." );
		double pB1Angle = pIm1p1Angle * 0.5 + pIp1Angle;
		if ( pB1Angle >= PIx2 )
		{
			pB1Angle = std::fmod( pB1Angle, PIx2 );
			//pB1Angle = Math.IEEEremainder( pB1Angle, PIx2 );
		}
		Msg::debug( "pB1Angle= " + std::to_string( toDegrees * pB1Angle ) );
		double pB1pIMax = std::max( pB1Angle, pIAngle );
		double pB1pIMin = std::min( pB1Angle, pIAngle );
		Msg::debug( "pB1pIMax= " + std::to_string( toDegrees * pB1pIMax ) );
		Msg::debug( "pB1pIMin= " + std::to_string( toDegrees * pB1pIMin ) );
		double pB2Angle = pB1pIMin + 0.5 * (pB1pIMax - pB1pIMin);
		if ( pB1pIMax - pB1pIMin > std::numbers::pi )
		{
			pB2Angle += std::numbers::pi;
		}

		MyVector pB2 = MyVector( pB2Angle, ld, *nJ );
		MyVector deltaC = pB2.minus( pI );
		Msg::debug( "Leaving angularSmoothnessAdjustment(..) returns " + deltaC.descr() );
		return deltaC;
	}

	Msg::debug( "pI1= " + pI1.descr() );
	Msg::debug( "pI2= " + pI2.descr() );

	MyVector line( *nIp1, *nIm1 );
	Msg::debug( "line= " + line.descr() );

	// pB1 should be the halved angle between pIp1 and pIm1, in the direction of pI:
	double pB1Angle = pIm1Angle + 0.5 * (pIp1Angle - pIm1Angle);
	if ( pIp1Angle - pIm1Angle > std::numbers::pi )
	{
		pB1Angle += std::numbers::pi;
	}

	if ( std::isnan( pB1Angle ) )
	{
		Msg::error( "pB1Angle is NaN!!!" );
	}
	Msg::debug( "pB1Angle= " + std::to_string( toDegrees * pB1Angle ) );

	double pB1pIMax = std::max( pB1Angle, pIAngle );
	double pB1pIMin = std::min( pB1Angle, pIAngle );
	Msg::debug( "pB1pIMax= " + std::to_string( toDegrees * pB1pIMax ) );
	Msg::debug( "pB1pIMin= " + std::to_string( toDegrees * pB1pIMin ) );

	double pB2Angle = pB1pIMin + 0.5 * (pB1pIMax - pB1pIMin);
	if ( pB1pIMax - pB1pIMin > std::numbers::pi )
	{
		pB2Angle += std::numbers::pi;
	}

	if ( std::isnan( pB2Angle ) )
	{
		Msg::error( "pB2Angle is NaN!!!" );
	}
	Msg::debug( "pB2Angle= " + std::to_string( toDegrees * pB2Angle ) );

	Ray pB2Ray( *nJ, pB2Angle );

	Msg::debug( "pB2Ray= " + pB2Ray.descr() );
	Msg::debug( "pB2Ray= " + pB2Ray.values() );
	auto q = pB2Ray.pointIntersectsAt( line );
	double lq = q->length( nJ );
	if ( std::isnan( lq ) )
	{
		Msg::error( "lq is NaN!!!" );
	}

	MyVector pB2;
	if ( ld > lq )
	{
		pB2 = MyVector( pB2Ray, (lq + ld) * 0.5 );
	}
	else
	{
		pB2 = MyVector( pB2Ray, ld );
	}

	MyVector deltaC = pB2.minus( pI );
	Msg::debug( "Leaving angularSmoothnessAdjustment(..) returns " + deltaC.descr() );
	return deltaC;

	return MyVector();
}

bool
Node::invertedOrZeroAreaElements( const std::vector<Element*>& elements )
{
	for ( auto elem : elements )
	{
		if ( elem->invertedOrZeroArea() )
		{
			Msg::debug( "Node.invertedOrZeroAreaElements(..): invertedOrZeroArea: " + elem->descr() );
			return true;
		}
	}
	return false;
}

bool
Node::incrAdjustUntilNotInvertedOrZeroArea( Node* old, const std::vector<Element*>& elements )
{
	Msg::debug("Entering incrAdjustUntilNotInvertedOrZeroArea(..)");
	Msg::debug( "..this: " + descr() );
	Msg::debug( "..old: " + old->descr() );

	MyVector back( *this, *old );
	double startX = x, startY = y;
	double xstep = back.x / 50.0, ystep = back.y / 50.0;
	double xinc, yinc;
	int steps, i;

	if ( abs( xstep ) < COINCTOL || abs( ystep ) < COINCTOL )
	{

		if ( COINCTOL < abs( back.x ) && COINCTOL < abs( back.y ) )
		{// && or || ?
			Msg::debug( "...ok, resorting to use of minimum increment" );
			if ( abs( back.x ) < abs( back.y ) )
			{
				if ( back.x < 0 )
				{
					xinc = -COINCTOL;
				}
				else
				{
					xinc = COINCTOL;
				}

				yinc = abs( old->y ) * COINCTOL / abs( old->x );
				if ( back.y < 0 )
				{
					yinc = -yinc;
				}

				steps = (int)(back.x / xinc);
			}
			else
			{
				if ( back.y < 0 )
				{
					yinc = -COINCTOL;
				}
				else
				{
					yinc = COINCTOL;
				}

				xinc = abs( old->x ) * COINCTOL / abs( old->y );
				if ( back.x < 0 )
				{
					xinc = -xinc;
				}

				steps = (int)(back.y / yinc);
			}

			Msg::debug( "...back.x is: " + std::to_string( back.x ));
			Msg::debug( "...back.y is: " + std::to_string( back.y ));

			Msg::debug( "...xinc is: " + std::to_string( xinc ));
			Msg::debug( "...yinc is: " + std::to_string( yinc ));

			for ( i = 1; i <= steps; i++ )
			{
				x = startX + xinc * i;
				y = startY + yinc * i;

				if ( !invertedOrZeroAreaElements( elements ) )
				{
					Msg::debug( "Leaving incrAdjustUntilNotInvertedOrZeroArea(..)" );
					return true;
				}
			}
		}
	}
	else
	{
		for ( i = 1; i <= 49; i++ )
		{
			x = startX + back.x * i / 50.0;
			y = startY + back.y * i / 50.0;

			if ( !invertedOrZeroAreaElements( elements ) )
			{
				Msg::debug( "Leaving incrAdjustUntilNotInvertedOrZeroArea(..)" );
				return true;
			}
		}
	}

	x = old->x;
	y = old->y;
	if ( !invertedOrZeroAreaElements( elements ) )
	{
		Msg::debug( "Leaving incrAdjustUntilNotInvertedOrZeroArea(..)" );
		return true;
	}

	Msg::debug( "Leaving incrAdjustUntilNotInvertedOrZeroArea(..)" );
	return false;
}

bool 
Node::boundaryNode()
{
	for ( auto e : edgeList )
	{
		if ( e->boundaryEdge() )
		{
			return true;
		}
	}
	return false;
}

bool
Node::boundaryOrTriangleNode()
{
	for ( auto e : edgeList )
	{
		if ( e->boundaryOrTriangleEdge() )
		{
			return true;
		}
	}
	return false;
}

bool
Node::frontNode()
{
	for ( auto e : edgeList )
	{
		if ( e->isFrontEdge() )
		{
			return true;
		}
	}
	return false;
}

Edge* 
Node::anotherFrontEdge( Edge* known )
{
	for ( auto e : edgeList )
	{
		if ( e != known && e->isFrontEdge() )
		{
			return e;
		}
	}
	return nullptr;
}

Edge* 
Node::anotherBoundaryEdge( Edge* known )
{
	for ( auto e : edgeList )
	{
		if ( e != known && e->boundaryEdge() )
		{
			return e;
		}
	}
	return nullptr;
}

double 
Node::length( Node* n ) const
{
	double xDiff = x - n->x;
	double yDiff = y - n->y;
	return sqrt( xDiff * xDiff + yDiff * yDiff );
}

double
Node::length( double x, double y ) const
{
	double xDiff = this->x - x;
	double yDiff = this->y - y;
	return sqrt( xDiff * xDiff + yDiff * yDiff );
}

bool 
Node::onLine( Edge* e )
{
	//RC - all double should be BigDecimal
	double x1 = e->leftNode->x;
	double y1 = e->leftNode->y;
	double x2 = e->rightNode->x;
	double y2 = e->rightNode->y;
	double x3 = x;
	double y3 = y;

	double zero = 0.0;
	double l_cross_r = (x1 * y2) - (x2 * y1);
	double xdiff = x1 - x2;
	double ydiff = y1 - y2;
	double det1 = l_cross_r - (xdiff * y3 + ydiff * x3);

	const double kZero = 1e-12;
	if ( abs( det1 ) < kZero )
	{
		return true;
	}
	else
	{
		return false;
	}
}

int 
Node::inHalfplane( Triangle* t, Edge* e )
{
	return inHalfplane( e->leftNode, e->rightNode, t->oppositeOfEdge( e ) );
}

int
Node::inHalfplane( Edge* e, Node* n )
{
	return inHalfplane( e->leftNode, e->rightNode, n );
}

int
Node::inHalfplane( Node* l1, Node* l2, Node* n )
{
	auto compare = [](double Value)
		{
			const double kPZero = 1e-12;
			const double kNZero = -1e-12;

			if ( Value < kNZero )
			{
				return -1;
			}
			else if ( Value > kPZero )
			{
				return 1;
			}
			else
			{
				return 0;
			}
		};

	//RC - all double should be BigDecimal
	Msg::debug( "Entering Node.inHalfplane(..)" );
	Msg::debug( "l1: " + l1->descr() + ", l2: " + l2->descr() + ", n:" + n->descr() );
	double x1 = l1->x;
	double y1 = l1->y;
	double x2 = l2->x;
	double y2 = l2->y;
	double x3 = x;
	double y3 = y;
	double x4 = n->x;
	double y4 = n->y;

	double l_cross_r = (x1 * y2) - (x2 * y1);
	double xdiff = x1 - x2;
	double ydiff = y1 - y2;
	double det1 = l_cross_r - (xdiff * y3 + ydiff * x3);

	int eval1 = compare( det1 );
	if ( eval1 == 0 )
	{
		Msg::debug( "Leaving Node.inHalfplane(..)" );
		return 0;
	}

	double det2 = l_cross_r - (xdiff * y4 + ydiff * x4);
	int eval2 = compare( det2 );
	Msg::debug( "Leaving Node.inHalfplane(..)" );
	if ( (eval1 < 0 && eval2 < 0) || (eval1 > 0 && eval2 > 0) )
	{
		return 1;
	}
	else
	{
		return -1;
	}

	return 0;
}

bool 
Node::inBoundedPlane( Edge* e )
{
	auto normal1 = e->unitNormalAt( e->leftNode );
	auto normal2 = e->unitNormalAt( e->rightNode );

	int a = inHalfplane( normal1, e->rightNode );
	int b = inHalfplane( normal2, e->leftNode );

	Msg::debug( "Node.inBoundedPlane(..): a: " + std::to_string( a ) + ", b: " + std::to_string( b ) );

	if ( (a == 1 || a == 0) && (b == 1 || b == 0) )
	{
		Msg::debug( "Node.inBoundedPlane(..): returns true" );
		return true;
	}
	else
	{
		Msg::debug( "Node.inBoundedPlane(..): returns false" );
		return false;
	}
	return false;
}

bool
Node::inCircle( Node* p1, Node* p2, Node* p3 )
{
	Msg::debug("Entering inCircle(..)");

	double cosAlpha = (p3->x - p2->x) * (p1->x - p2->x) + (p3->y - p2->y) * (p1->y - p2->y);
	double cosBeta = (p1->x - x) * (p3->x - x) + (p1->y - y) * (p3->y - y);

	if ( cosAlpha < 0 && cosBeta < 0 )
	{ // (if both angles > than 90 degrees)
		Msg::debug( "Leaving inCircle(..), cosAlpha && cosBeta <0, return true" );
		return true;
	}
	else if ( cosAlpha > 0 && cosBeta > 0 )
	{ // (if both angles < than 90 degrees)
		Msg::debug( "Leaving inCircle(..), cosAlpha && cosBeta >0, return false" );
		return false;
	}
	else
	{
		double sinAlpha = p3->x * p1->y - p3->x * p2->y - p2->x * p1->y - p3->y * p1->x + p3->y * p2->x + p2->y * p1->x;
		double sinBeta = p3->y * p1->x - p1->x * y - x * p3->y - p3->x * p1->y + p1->y * x + y * p3->x;
		if ( cosAlpha * sinBeta + sinAlpha * cosBeta < 0 )
		{
			Msg::debug( "Leaving inCircle(..), passed last check, returns true" );
			return true;
		}
		else
		{
			Msg::debug( "Leaving inCircle(..), failed last check, returns false" );
			return false;
		}
	}
	return false;
}

void
Node::merge( Node* n )
{
	auto oldN = n->copyXY();

	n->setXY( *this );
	for (auto e:edgeList )
	{
		auto ind = std::find( edgeList.begin(), edgeList.end(), e );
		if ( ind == edgeList.end() )
		{
			e->replaceNode( n, this );
			edgeList.push_back( e );
		}
		else
		{ // collapsed edges must be removed
			if ( e->leftNode == e->rightNode )
			{
				edgeList.erase( ind );
			}
		}
	}
	n->setXY( *oldN );
}

std::vector<Edge*> 
Node::frontEdgeList()
{
	std::vector<Edge*> list;
	for ( auto e : edgeList )
	{
		if ( e->frontEdge )
		{
			list.push_back( e );
		}
	}
	return list;
}

bool 
Node::hasEdge( Edge* e )
{
	for ( auto curEdge : edgeList )
	{
		if ( e == curEdge )
		{
			return true;
		}
	}
	return false;
}

uint8_t 
Node::valence()
{
	uint8_t temp = static_cast<uint8_t>(edgeList.size());
	if ( !boundaryNode() )
	{
		return temp;
	}
	else
	{
		auto b1 = anotherBoundaryEdge( nullptr );
		auto b2 = anotherBoundaryEdge( b1 );
		double ang = b1->sumAngle( b1->element1, this, b2 );

		// Determine which kind of boundary node we're dealing with
		if ( ang <= PIx3div4 )
		{ // PIdiv2
			return temp + 2;
		}
		else if ( ang < PIx5div4 )
		{ // PIx3div2
			return (temp + 1);
		}
		else
		{ // PIx3div2
			return (temp);
		}

	}
	return 0;
}

void
Node::createValencePattern( const std::vector<Node*>& ccwNodes )
{
	Msg::debug("Entering Node.createValencePattern(..)");
	int j = static_cast<int>(edgeList.size() * 2);
	if ( j >= 128 )
	{
		Msg::error( "Number of edges adjacent node " + descr() + " was greater than expected (" + std::to_string(edgeList.size()) + "-2 >= 64)" );
	}
	uint8_t ccwNodesSize = static_cast<uint8_t>(j);
	pattern.resize(ccwNodesSize + 2); // +2 for size and c.valence()
	pattern[0] = ccwNodesSize + 2;
	pattern[1] = valence();

	for ( int i = 0; i < ccwNodesSize; i++ )
	{
		pattern[i + 2] = ccwNodes[i]->valence();
	}
	Msg::debug( "Leaving Node.createValencePattern(..)" );
}

void 
Node::createValencePattern( uint8_t ccwNodesSize, const std::vector<Node*>& ccwNodes )
{
	Msg::debug("Entering Node.createValencePattern(" + std::to_string(ccwNodesSize) + ", Node [])");
	pattern.resize(ccwNodesSize + 2); // +2 for size and c.valence()
	pattern[0] = ccwNodesSize + 2;
	pattern[1] = valence();

	for ( int i = 0; i < ccwNodesSize; i++ )
	{
		Msg::debug( "...i== " + std::to_string( i ) );
		pattern[i + 2] = ccwNodes[i]->valence();
	}
	Msg::debug( "Leaving Node.createValencePattern(byte, Node [])" );
}

int
Node::irregNeighborNodes()
{
	int count = 0;
	for ( int i = 1; i < pattern[0]; i++ )
	{
		if ( pattern[i] != 4 )
		{
			count++;
		}
	}
	return count;
}

int 
Node::patternMatch( const std::vector<uint8_t>& pattern2 )
{
	Msg::debug("Entering patternMatch(..)");
	if ( pattern[0] != pattern2[0] || pattern[1] != pattern2[1] )
	{
		Msg::debug( "Leaving patternMatch(..): mismatch" );
		return -1; // Different length or different valence of central node
	}

	int i, j, jstart = 2, matches = 0;

	Msg::debug( "pattern[0]==" + std::to_string( pattern[0] ) );
	Msg::debug( "pattern2[0]==" + std::to_string( pattern2[0] ) );
	Msg::debug( "pattern[1]==" + std::to_string( pattern[1] ) );
	Msg::debug( "pattern2[1]==" + std::to_string( pattern2[1] ) );

	while ( jstart < pattern[0] )
	{
		// Find index of next valence in pattern2 that matches valence of pattern[2]
		for ( j = jstart; j < pattern[0]; j += 2 )
		{
			if ( pattern[j] == 2 && (pattern2[2] == 2 || pattern2[2] == 14 || pattern2[2] == 0) )
			{
				matches = 1;
				jstart = j;
				Msg::debug( "... rolling pattern..." );
				Msg::debug( "...pattern2[2]: " + std::to_string( pattern2[2] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ));
				break;
			}
			else if ( pattern[j] == 3 && (pattern2[2] == 3 || pattern2[2] == 14 || pattern2[2] == 0) )
			{

				matches = 1;
				jstart = j;
				Msg::debug( "... rolling pattern..." );
				Msg::debug( "...pattern2[2]: " + std::to_string( pattern2[2] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
				break;
			}
			else if ( pattern[j] == 4 && (pattern2[2] == 4 || pattern2[2] == 14 || pattern2[2] == 24 || pattern2[2] == 0) )
			{

				matches = 1;
				jstart = j;
				Msg::debug( "... rolling pattern..." );
				Msg::debug( "...pattern2[2]: " + std::to_string( pattern2[2] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
				break;
			}
			else if ( pattern[j] >= 5 && (pattern2[2] == 5 || pattern2[2] == 24 || pattern2[2] == 0) )
			{

				matches = 1;
				jstart = j;
				Msg::debug( "... rolling pattern..." );
				Msg::debug( "...pattern2[2]: " + std::to_string( pattern2[2] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
				break;
			}
		}

		if ( matches == 0 )
		{
			Msg::debug( "Leaving patternMatch(..): mismatch" );
			return -1; // Search completed, patterns don't match
		}

		if ( jstart == pattern[0] - 1 )
		{
			j = 1;
		}
		else
		{
			j = jstart + 1;
		}
		// Count nr of sequential matches starting at this index
		for ( i = 3; matches < pattern2[0] - 2; i++, j++ )
		{
			if ( pattern[j] == 2 && (pattern2[i] == 2 || pattern2[i] == 14 || pattern2[i] == 0) )
			{
				matches++;
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else if ( pattern[j] == 3 && (pattern2[i] == 3 || pattern2[i] == 14 || pattern2[i] == 0) )
			{
				matches++;
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else if ( pattern[j] == 4 && (pattern2[i] == 4 || pattern2[i] == 14 || pattern2[i] == 24 || pattern2[i] == 0) )
			{
				matches++;
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else if ( pattern[j] >= 5 && (pattern2[i] == 5 || pattern2[i] == 24 || pattern2[i] == 0) )
			{
				matches++;
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string(pattern2[i]) + ", pattern[" + std::to_string(j) + "]: " + std::to_string(pattern[j]));
			}
			else
			{
				matches = 0;
				break;
			}

			if ( j == pattern[0] - 1 )
			{
				j = 1;
			}
		}
		if ( matches == pattern2[0] - 2 )
		{
			Msg::debug( "Leaving patternMatch(..): match, returns: " + std::to_string( jstart ) );
			return jstart; // Search completed, patterns match
		}
		jstart += 2;
	}
	Msg::debug( "Leaving patternMatch(..): mismatch" );
	return -1;
}

int 
Node::patternMatch( const std::vector<uint8_t>& pattern2,
					const std::vector<bool>& vertexPat,
					const std::vector<double>& angles )
{
	Msg::debug("Entering patternMatch(byte [], boolean [], double [])");
	if ( pattern[0] != pattern2[0] || pattern[1] != pattern2[1] )
	{
		Msg::debug( "Leaving patternMatch(byte [], boolean [], double []): mismatch" );
		return -1; // Different length or different valence of central node
	}

	int i, j, jstart = 2, matches = 0;

	Msg::debug( "pattern[0]==" + std::to_string( pattern[0] ) );
	Msg::debug( "pattern2[0]==" + std::to_string( pattern2[0] ) );
	Msg::debug( "pattern[1]==" + std::to_string( pattern[1] ) );
	Msg::debug( "pattern2[1]==" + std::to_string( pattern2[1] ) );

	while ( jstart < pattern[0] )
	{
		// Find index of next valence in pattern2 that matches valence of pattern[2]
		for ( j = jstart; j < pattern[0]; j += 2 )
		{
			if ( pattern[j] == 2 && (pattern2[2] == 2 || pattern2[2] == 14 || pattern2[2] == 0) )
			{
				if ( fitsVertexPat( static_cast<uint8_t>(j - 2), angles, vertexPat, pattern[0] - 2 ) )
				{
					matches = 1;
					jstart = j;
					Msg::debug( "... rolling pattern..." );
					Msg::debug( "...pattern2[2]: " + std::to_string( pattern2[2] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
					break;
				}
			}
			else if ( pattern[j] == 3 && (pattern2[2] == 3 || pattern2[2] == 14 || pattern2[2] == 0) )
			{
				if ( fitsVertexPat( static_cast<uint8_t>(j - 2), angles, vertexPat, pattern[0] - 2 ) )
				{
					matches = 1;
					jstart = j;
					Msg::debug( "... rolling pattern..." );
					Msg::debug( "...pattern2[2]: " + std::to_string( pattern2[2] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
					break;
				}
			}
			else if ( pattern[j] == 4 && (pattern2[2] == 4 || pattern2[2] == 14 || pattern2[2] == 24 || pattern2[2] == 0) )
			{
				if ( fitsVertexPat( static_cast<uint8_t>(j - 2), angles, vertexPat, pattern[0] - 2 ) )
				{
					matches = 1;
					jstart = j;
					Msg::debug( "... rolling pattern..." );
					Msg::debug( "...pattern2[2]: " + std::to_string( pattern2[2] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
					break;
				}
			}
			else if ( pattern[j] >= 5 && (pattern2[2] == 5 || pattern2[2] == 24 || pattern2[2] == 0) )
			{
				if ( fitsVertexPat( static_cast<uint8_t>(j - 2), angles, vertexPat, pattern[0] - 2 ) )
				{
					matches = 1;
					jstart = j;
					Msg::debug( "... rolling pattern..." );
					Msg::debug( "...pattern2[2]: " + std::to_string( pattern2[2] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
					break;
				}
			}
		}

		if ( matches == 0 )
		{
			Msg::debug( "Leaving patternMatch(byte [], boolean [], double []): mismatch" );
			return -1; // Search completed, patterns don't match
		}
		Msg::debug( "...broken out of loop!!" );
		if ( jstart == pattern[0] - 1 )
		{
			j = 1; // Shouldn't it be 2???
		}
		else
		{
			j = jstart + 1;
		}
		// Count nr of sequential matches starting at this index
		for ( i = 3; matches < pattern2[0] - 2; i++, j++ )
		{
			Msg::debug( "i== " + i );
			if ( pattern[j] == 2 && (pattern2[i] == 2 || pattern2[i] == 14 || pattern2[i] == 0) )
			{
				matches++;
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else if ( pattern[j] == 3 && (pattern2[i] == 3 || pattern2[i] == 14 || pattern2[i] == 0) )
			{
				matches++;
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else if ( pattern[j] == 4 && (pattern2[i] == 4 || pattern2[i] == 14 || pattern2[i] == 24 || pattern2[i] == 0) )
			{
				matches++;
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else if ( pattern[j] >= 5 && (pattern2[i] == 5 || pattern2[i] == 24 || pattern2[i] == 0) )
			{
				matches++;
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else
			{
				matches = 0;
				break;
			}

			if ( j == pattern[0] - 1 )
			{
				j = 1;
			}
		}
		if ( matches == pattern2[0] - 2 )
		{
			Msg::debug( "Leaving patternMatch(byte [], boolean [], double []): match, returns: " + std::to_string( jstart ) );
			return jstart; // Search completed, patterns match
		}
		jstart += 2;
	}
	Msg::debug( "Leaving patternMatch(byte [], boolean [], double []): mismatch" );
	return -1;
}

bool
Node::fitsVertexPat( uint8_t start,
					 const std::vector<double>& ang,
					 const std::vector<bool>& vertexPat,
					 int len )
{
	Msg::debug("Entering Node.fitsVertexPat(..)");
	int i, j = start, k = 0, l;
	do
	{
		// Check the corresponding boolean in vertexPat
		if ( vertexPat[k] )
		{
			// Compare ang[j] to all other angles at non-vertex nodes
			i = j + 1;
			if ( i == len )
			{
				i = 0;
			}
			l = k + 1;
			if ( l == len )
			{
				l = 0;
			}

			while ( i != j )
			{
				if ( !vertexPat[l] && ang[i] < ang[j] )
				{
					Msg::debug( "ang[" + std::to_string( i ) + "] < ang[" + std::to_string( j ) + "]" );
					Msg::debug( "ang[" + std::to_string( i ) + "]== " + std::to_string( toDegrees * ang[i] ) );
					Msg::debug( "ang[" + std::to_string( j ) + "]== " + std::to_string( toDegrees * ang[j] ) );
					Msg::debug( "Leaving Node.fitsVertexPat(..): false" );
					return false;
				}
				i++;
				if ( i == len )
				{
					i = 0;
				}
				l++;
				if ( l == len )
				{
					l = 0;
				}
			}
		}

		j++;
		if ( j == len )
		{
			j = 0;
		}
		k++;
	} while ( j != start );

	Msg::debug( "Leaving Node.fitsVertexPat(..): true" );
	return true;
}

std::vector<double>
Node::surroundingAngles( std::vector<Node*>& ccwNeighbors,
						 int len )
{
	Msg::debug( "Entering Node.surroundingAngles(..)" );
	Quad* q, *qa, *qb;
	Edge* e, *ep, *en;
	Node *n, *np, *nn, *no;
	std::vector<double> angles( len, 0.0 );
	for ( int i = 0; i < len; i++ )
	{
		n = ccwNeighbors[i];
		e = commonEdge( n );
		if ( e == nullptr )
		{
			if ( i - 1 >= 0 )
			{
				np = ccwNeighbors[i - 1];
			}
			else
			{
				np = ccwNeighbors[len - 1];
			}

			if ( i + 1 < len )
			{
				nn = ccwNeighbors[i + 1];
			}
			else
			{
				nn = ccwNeighbors[0];
			}

			ep = commonEdge( np );
			en = commonEdge( nn );
			q = static_cast<Quad*>(ep->commonElement( en ));

			no = q->oppositeNode( this );
			angles[i] = q->ang[q->angleIndex( no )];
		}
		else
		{
			no = e->otherNode( this );
			qa = static_cast<Quad*>(e->element1);
			qb = static_cast<Quad*>(e->element2);

			angles[i] = qa->ang[qa->angleIndex( no )] + qb->ang[qb->angleIndex( no )];
		}
	}
	Msg::debug( "Leaving Node.surroundingAngles(..)" );
	return angles;

	return std::vector<double>();
}

bool 
Node::boundaryPatternMatch( const std::vector<uint8_t>& pattern2, 
							const std::vector<bool>& bpat,
							const std::vector<Node*>& ccwNeighbors )
{
	Msg::debug( "Entering boundaryPatternMatch(..)" );

	if ( pattern[0] != pattern2[0] || pattern[1] != pattern2[1] || bpat[0] != boundaryNode() )
	{
		Msg::debug( "Leaving boundaryPatternMatch(..): mismatch" );
		return false;
	}
	int i;

	for ( i = 2; i < pattern[0]; i++ )
	{
		if ( pattern[i] == 2 && (pattern2[i] == 2 || pattern2[i] == 14 || pattern2[i] == 0) )
		{
			if ( bpat[i - 1] && !ccwNeighbors[i - 2]->boundaryNode() )
			{
				return false;
			}

			Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( i ) + "]: " + std::to_string( pattern[i] ) );
		}
		else if ( pattern[i] == 3 && (pattern2[i] == 3 || pattern2[i] == 14 || pattern2[i] == 0) )
		{
			if ( bpat[i - 1] && !ccwNeighbors[i - 2]->boundaryNode() )
			{
				return false;
			}
			Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( i ) + "]: " + std::to_string( pattern[i] ) );
		}
		else if ( pattern[i] == 4 && (pattern2[i] == 4 || pattern2[i] == 14 || pattern2[i] == 24 || pattern2[i] == 0) )
		{
			if ( bpat[i - 1] && !ccwNeighbors[i - 2]->boundaryNode() )
			{
				return false;
			}
			Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( i ) + "]: " + std::to_string( pattern[i] ) );
		}
		else if ( pattern[i] >= 5 && (pattern2[i] == 5 || pattern2[i] == 24 || pattern2[i] == 0) )
		{
			if ( bpat[i - 1] && !ccwNeighbors[i - 2]->boundaryNode() )
			{
				return false;
			}
			Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( i ) + "]: " + std::to_string( pattern[i] ) );
		}
		else
		{
			Msg::debug( "Leaving boundaryPatternMatch(..): mismatch" );
			return false;
		}
	}
	Msg::debug( "Leaving boundaryPatternMatch(..): match" );
	return true;
}

int
Node::boundaryPatternMatchSpecial( const std::vector<uint8_t>& pattern2,
								   const std::vector<bool>& bpat,
								   const std::vector<Node*>& ccwNeighbors )
{
	Msg::debug( "Entering boundaryPatternMatchSpecial(..)" );

	if ( pattern[0] != pattern2[0] || pattern[1] != pattern2[1] || bpat[0] != boundaryNode() )
	{
		Msg::debug( "Leaving boundaryPatternMatchSpecial(..): mismatch" );
		return -1;
	}
	int i, j, k;
	bool match;

	Msg::debug( "...entering the for loop" );

	for ( k = 2; k < pattern[0]; k++ )
	{

		Msg::debug( "...k== " + k );
		j = k;
		match = true;

		for ( i = 2; i < pattern[0]; i++ )
		{
			Msg::debug( "...i== " + i );

			Msg::debug( "...pattern[" + std::to_string( j ) + "]== " + std::to_string( pattern[j] ) );
			Msg::debug( "...pattern2[" + std::to_string( i ) + "]== " + std::to_string( pattern2[i] ) );

			if ( pattern[j] == 2 && (pattern2[i] == 2 || pattern2[i] == 14 || pattern2[i] == 0) )
			{
				if ( bpat[i - 1] && !ccwNeighbors[j - 2]->boundaryNode() )
				{
					match = false;
					break;
				}
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else if ( pattern[j] == 3 && (pattern2[i] == 3 || pattern2[i] == 14 || pattern2[i] == 0) )
			{
				if ( bpat[i - 1] && !ccwNeighbors[j - 2]->boundaryNode() )
				{
					match = false;
					break;
				}
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else if ( pattern[j] == 4 && (pattern2[i] == 4 || pattern2[i] == 14 || pattern2[i] == 24 || pattern2[i] == 0) )
			{
				if ( bpat[i - 1] && !ccwNeighbors[j - 2]->boundaryNode() )
				{
					match = false;
					break;
				}
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else if ( pattern[j] >= 5 && (pattern2[i] == 5 || pattern2[i] == 24 || pattern2[i] == 0) )
			{
				if ( bpat[i - 1] )
				{
					Msg::debug( "bpat[" + std::to_string( i - 1) + "] is true" );
				}
				else
				{
					Msg::debug( "bpat[" + std::to_string( i - 1) + "] is false" );
				}

				if ( ccwNeighbors[j - 2]->boundaryNode() )
				{
					Msg::debug( "ccwNeighbors[" + std::to_string( j - 2) + "].boundaryNode()]) is true" );
				}
				else
				{
					Msg::debug( "ccwNeighbors[" + std::to_string( j - 2) + "].boundaryNode()]) is false" );
				}

				if ( bpat[i - 1] && !ccwNeighbors[j - 2]->boundaryNode() )
				{
					match = false;
					break;
				}
				Msg::debug( "...pattern2[" + std::to_string( i ) + "]: " + std::to_string( pattern2[i] ) + ", pattern[" + std::to_string( j ) + "]: " + std::to_string( pattern[j] ) );
			}
			else
			{
				match = false;
				break;
			}

			j++;
			if ( j == pattern[0] )
			{
				j = 2;
			}
		}
		if ( match )
		{
			Msg::debug( "Leaving boundaryPatternMatchSpecial(..): match" );
			return k;
		}
	}
	Msg::debug( "Leaving boundaryPatternMatchSpecial(..): mismatch" );
	return -1;
}

Edge*
Node::commonEdge( Node* n )
{
	Node* other;
	for ( auto e : edgeList )
	{
		other = e->otherNode( this );
		if ( other == n )
		{
			return e;
		}
	}
	return nullptr;
}

bool 
Node::replaceWithStdMesh()
{
	Msg::debug( "Entering replaceWithStdMesh(..)" );
	Msg::debug( "Leaving replaceWithStdMesh(..)" );
	return true;
}

std::string
Node::descr()
{
	return "(" + std::to_string( x ) + ", " + std::to_string( y ) + ")";
}

std::string 
Node::valDescr()
{
	std::string s = "" + std::to_string( pattern[1] ) + "-";
	for ( int i = 2; i < pattern[0]; i++ )
	{
		s = s + std::to_string( pattern[i] );
	}

	return s;
}

void 
Node::printMe()
{
	std::cout << descr();
}

std::string
Node::toString()
{
	return descr();
}