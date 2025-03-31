#include "pch.h"
#include "Quad.h"

#include "Edge.h"
#include "Triangle.h"
#include "Node.h"
#include "MyVector.h"

Quad::Quad( Edge* baseEdge, Edge* leftEdge, Edge* rightEdge, Edge* topEdge )
{
	edgeList.resize( 4 );
	ang.resize( 4 );

	if ( rightEdge != topEdge )
	{
		isFake = false;
	}
	else
	{
		isFake = true;
	}

	edgeList[base] = baseEdge;
	edgeList[left] = leftEdge;
	edgeList[right] = rightEdge;
	edgeList[top] = topEdge;

	edgeList[base]->len = edgeList[base]->computeLength();
	edgeList[left]->len = edgeList[left]->computeLength();
	edgeList[right]->len = edgeList[right]->computeLength();
	if ( !isFake )
	{
		edgeList[top]->len = edgeList[top]->computeLength();
	}

	firstNode = baseEdge->leftNode;
	if ( inverted() )
	{
		firstNode = baseEdge->rightNode;
	}

	updateAngles();
}

Quad::Quad( Edge* e )
{
	isFake = false;
	edgeList.resize( 4 );
	ang.resize( 4 );

	Triangle* t1 = static_cast<Triangle*>(e->element1);
	Triangle* t2 = static_cast<Triangle*>(e->element2);

	Edge* c11 = t1->otherEdge( e );
	Edge* c12 = t1->otherEdge( e, c11 );
	Edge* c21, *c22, *temp1 = t2->otherEdge( e );
	if ( c11->commonNode( temp1 ) != nullptr )
	{
		c21 = temp1;
		c22 = t2->otherEdge( e, temp1 );
	}
	else
	{
		c21 = t2->otherEdge( e, temp1 );
		c22 = temp1;
	}

	// The following assignments to the array ang[] is based on a
	// geometric sketch. The correctness of this should be tested.
	if ( c11->leftNode->hasEdge( c12 ) )
	{
		edgeList[base] = c11;
		edgeList[left] = c12;
		ang[0] = t1->angle( c11, c12 );

		if ( c11->rightNode->hasEdge( c21 ) )
		{
			edgeList[right] = c21;
			edgeList[top] = c22;
			ang[1] = t1->angle( c11, e ) + t2->angle( e, c21 );
			ang[2] = t1->angle( c12, e ) + t2->angle( e, c22 );
			ang[3] = t2->angle( c21, c22 );
		}
		else
		{
			edgeList[right] = c22;
			edgeList[top] = c21;
			ang[1] = t1->angle( c11, e ) + t2->angle( e, c22 );
			ang[2] = t1->angle( c12, e ) + t2->angle( e, c21 );
			ang[3] = t2->angle( c21, c22 );
		}
	}
	else
	{ // c11.rightNode.hasEdge(c12)
		edgeList[base] = c11;
		edgeList[right] = c12;
		ang[1] = t1->angle( c11, c12 );

		if ( c11->leftNode->hasEdge( c21 ) )
		{
			edgeList[left] = c21;
			edgeList[top] = c22;
			ang[0] = t1->angle( c11, e ) + t2->angle( e, c21 );
			ang[2] = t2->angle( c21, c22 );
			ang[3] = t1->angle( c12, e ) + t2->angle( e, c22 );
		}
		else
		{ // c11.leftNode.hasEdge(c22)
			edgeList[left] = c22;
			edgeList[top] = c21;
			ang[0] = t1->angle( c11, e ) + t2->angle( e, c22 );
			ang[2] = t2->angle( c21, c22 );
			ang[3] = t1->angle( c12, e ) + t2->angle( e, c21 );
		}
	}

	firstNode = edgeList[base]->leftNode;
	if ( inverted() )
	{
		firstNode = edgeList[base]->rightNode;
	}
}

Quad::Quad( Triangle* t )
{
	isFake = true;
	edgeList.resize( 4 );
	ang.resize( 4 );

	edgeList[base] = t->edgeList[0];
	if ( t->edgeList[1]->hasNode( edgeList[base]->leftNode ) )
	{
		edgeList[left] = t->edgeList[1];
		edgeList[right] = t->edgeList[2];
	}
	else
	{
		edgeList[left] = t->edgeList[2];
		edgeList[right] = t->edgeList[1];
	}

	edgeList[top] = edgeList[right];

	firstNode = t->firstNode;
	ang[0] = t->ang[0];
	ang[1] = t->ang[1];
	ang[2] = t->ang[2];
}

Quad::Quad( Edge* e, Node* n1, Node* n2 )
{
	Msg::debug( "Entering Quad(Edge, Node, Node)" );
	Msg::debug( "e= " + e->descr() + ", n1= " + n1->descr() + ", n2= " + n2->descr() );
	isFake = false;
	edgeList.resize( 4 );

	edgeList[base] = new Edge( e->leftNode, n1 );
	edgeList[top] = new Edge( n2, e->rightNode );

	if ( edgeList[base]->leftNode == e->leftNode )
	{
		edgeList[right] = new Edge( edgeList[base]->rightNode, e->rightNode );
		edgeList[left] = new Edge( edgeList[base]->leftNode, edgeList[top]->otherNode( e->rightNode ) );
	}
	else
	{
		edgeList[right] = new Edge( edgeList[base]->rightNode, edgeList[top]->otherNode( e->rightNode ) );
		edgeList[left] = new Edge( edgeList[base]->leftNode, e->rightNode );
	}

	Msg::debug( "New quad is " + descr() );
	Msg::debug( "Leaving Quad(Edge, Node, Node)" );

	firstNode = edgeList[base]->leftNode;
	if ( inverted() )
	{
		firstNode = edgeList[base]->rightNode;
	}
}

Quad::Quad( Node* n1, Node* n2, Node* n3, Node* f )
{
	isFake = true;
	edgeList.resize( 4 );
	ang.resize( 4 );

	edgeList[base] = new Edge( n1, n2 );
	if ( edgeList[base]->leftNode == n1 )
	{
		edgeList[left] = new Edge( n1, n3 );
		edgeList[right] = new Edge( n2, n3 );
	}
	else
	{
		edgeList[left] = new Edge( n2, n3 );
		edgeList[right] = new Edge( n1, n3 );
	}
	edgeList[top] = edgeList[right];

	firstNode = f;
	updateAngles();
}

Quad::Quad( Node* n1, Node* n2, Node* n3, Node* n4, Node* f )
{
	isFake = false;
	edgeList.resize( 4 );
	ang.resize( 4 );

	edgeList[base] = new Edge( n1, n2 );
	edgeList[top] = new Edge( n3, n4 );
	if ( edgeList[base]->leftNode == n1 )
	{
		edgeList[left] = new Edge( n1, n3 );
		edgeList[right] = new Edge( n2, n4 );
	}
	else
	{
		edgeList[left] = new Edge( n2, n4 );
		edgeList[right] = new Edge( n1, n3 );
	}

	firstNode = f;
	updateAngles();
}

Element* 
Quad::elementWithExchangedNodes( Node* original, Node* replacement )
{
	Node* node1 = edgeList[base]->leftNode;
	Node* node2 = edgeList[base]->rightNode;
	Node* node3 = edgeList[left]->otherNode( edgeList[base]->leftNode );

	if ( isFake )
	{
		if ( node1 == original )
		{
			if ( original == firstNode )
			{
				return new Quad( replacement, node2, node3, replacement );
			}
			else
			{
				return new Quad( replacement, node2, node3, firstNode );
			}
		}
		else if ( node2 == original )
		{
			if ( original == firstNode )
			{
				return new Quad( node1, replacement, node3, replacement );
			}
			else
			{
				return new Quad( node1, replacement, node3, firstNode );
			}
		}
		else if ( node3 == original )
		{
			if ( original == firstNode )
			{
				return new Quad( node1, node2, replacement, replacement );
			}
			else
			{
				return new Quad( node1, node2, replacement, firstNode );
			}
		}
		else
		{
			return nullptr;
		}
	}

	Node* node4 = edgeList[right]->otherNode( edgeList[base]->rightNode );

	if ( node1 == original )
	{
		if ( original == firstNode )
		{
			return new Quad( replacement, node2, node3, node4, replacement );
		}
		else
		{
			return new Quad( replacement, node2, node3, node4, firstNode );
		}
	}
	else if ( node2 == original )
	{
		if ( original == firstNode )
		{
			return new Quad( node1, replacement, node3, node4, replacement );
		}
		else
		{
			return new Quad( node1, replacement, node3, node4, firstNode );
		}
	}
	else if ( node3 == original )
	{
		if ( original == firstNode )
		{
			return new Quad( node1, node2, replacement, node4, replacement );
		}
		else
		{
			return new Quad( node1, node2, replacement, node4, firstNode );
		}
	}
	else if ( node4 == original )
	{
		if ( original == firstNode )
		{
			return new Quad( node1, node2, node3, replacement, replacement );
		}
		else
		{
			return new Quad( node1, node2, node3, replacement, firstNode );
		}
	}
	else
	{
		return nullptr;
	}

	return nullptr;
}

bool 
Quad::invertedWhenNodeRelocated( Node* n1, Node* n2 )
{
	Msg::debug( "Entering Quad.invertedWhenNodeRelocated(..)" );
	auto thisFirstNode = firstNode;
	auto a = edgeList[base]->leftNode, b = edgeList[base]->rightNode, c = edgeList[right]->otherNode( b ), d = edgeList[left]->otherNode( a );

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
	else if ( d == n1 )
	{
		d = n2;
	}

	if ( n1 == firstNode )
	{
		thisFirstNode = n2;
	}

	// We need at least 3 okays to be certain that this quad is not inverted
	int okays = 0;
	if ( cross( a, d, b, d ) > 0 )
	{ // Was: >=
		okays++;
	}
	if ( cross( a, c, b, c ) > 0 )
	{ // Was: >=
		okays++;
	}
	if ( cross( c, a, d, a ) > 0 )
	{ // Was: >=
		okays++;
	}
	if ( cross( c, b, d, b ) > 0 )
	{ // Was: >=
		okays++;
	}

	if ( b == thisFirstNode )
	{
		okays = 4 - okays;
	}

	Msg::debug( "Leaving Quad.invertedWhenNodeRelocated(..), okays: " + okays );
	if ( okays >= 3 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool
Quad::anyInvertedElementsWhenCollapsed( Node* n, Node* n1, Node* n2,
										const std::vector<Element*>& lK,
										const std::vector<Element*>& lKOpp )
{
	Msg::debug( "Entering Quad.anyInvertedElementsWhenCollapsed(..)" );
	Element* elem;
	int i;

	for ( i = 0; i < lK.size(); i++ )
	{
		elem = lK[i];
		if ( elem != this && elem->invertedWhenNodeRelocated( n1, n ) )
		{
			Msg::debug( "Leaving Quad.anyInvertedElementsWhenCollapsed(..) ret: true" );
			return true;
		}
	}

	for ( i = 0; i < lKOpp.size(); i++ )
	{
		elem = lKOpp[i];
		if ( elem != this && elem->invertedWhenNodeRelocated( n2, n ) )
		{
			Msg::debug( "Leaving Quad.anyInvertedElementsWhenCollapsed(..) ret: true" );
			return true;
		}
	}

	Msg::debug( "Leaving Quad.anyInvertedElementsWhenCollapsed(..) ret: false" );
	return false;
}

bool 
Quad::equals( const Element* o ) const
{
	if ( Element::IsAQuad( o ) )
	{
		const auto q = static_cast<const Quad*>(o);
		Node* node1, *node2, *node3, *node4;
		Node* qnode1, *qnode2, *qnode3, *qnode4;
		node1 = edgeList[base]->leftNode;
		node2 = edgeList[base]->rightNode;
		node3 = edgeList[left]->otherNode( edgeList[base]->leftNode );
		node4 = edgeList[right]->otherNode( edgeList[base]->rightNode );

		qnode1 = q->edgeList[base]->leftNode;
		qnode2 = q->edgeList[base]->rightNode;
		qnode3 = q->edgeList[left]->otherNode( q->edgeList[base]->leftNode );
		qnode4 = q->edgeList[right]->otherNode( q->edgeList[base]->rightNode );

		if ( node1 == qnode1 && node2 == qnode2 && node3 == qnode3 && node4 == qnode4 )
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
	}
}

int 
Quad::indexOf( Edge* e )
{
	if ( edgeList[base] == e )
	{
		return base;
	}
	else if ( edgeList[left] == e )
	{
		return left;
	}
	else if ( edgeList[right] == e )
	{
		return right;
	}
	else if ( edgeList[top] == e )
	{
		return top;
	}
	else
	{
		return -1;
	}
}

void 
Quad::closeQuad( Edge* e1, Edge* e2 )
{
	Msg::debug("Entering Quad.closeQuad(..)");
	Node* nK = e1->commonNode( e2 );
	Node* nKp1 = e1->otherNode( nK ), *nKm1 = e2->otherNode( nK ), *other;
	bool found = false;
	Edge* e, *eI, *eJ;
	std::vector<Edge*> addList;
	std::vector<Quad*> quadList;

	Msg::debug( "...nKp1: " + nKp1->descr() );
	Msg::debug( "...nKm1: " + nKm1->descr() );

	for ( int i = 0; i < nKm1->edgeList.size(); i++ )
	{
		eI = nKm1->edgeList[i];
		other = eI->otherNode( nKm1 );
		Msg::debug( "...eI== " + eI->descr() );

		for ( int j = 0; j < nKp1->edgeList.size(); j++ )
		{
			eJ = nKp1->edgeList[j];

			if ( other == eJ->otherNode( nKp1 ) )
			{
				found = true;
				Msg::debug( "...eI is connected to eJ== " + eJ->descr() );

				auto Iter = std::find( other->edgeList.begin(), other->edgeList.end(), eI );
				if ( Iter != other->edgeList.end() )
				{
					other->edgeList.erase( Iter );
				}

				if ( eI->element1 != nullptr )
				{
					if ( eI->element1->firstNode == nKm1 )
					{
						eI->element1->firstNode = nKp1; // Don't forget firstNode!!
					}
					Msg::debug( "... replacing eI with eJ in eI.element1" );

					eI->element1->replaceEdge( eI, eJ );
					eJ->connectToElement( eI->element1 );
					if ( Element::IsAQuad( eI->element1 ) )
					{ // Then LR might need updating
						quadList.push_back( static_cast<Quad*>(eI->element1) ); // but that must be done later
					}
				}
				break;
			}
		}

		if ( !found )
		{
			Msg::debug( "...none of nKp1's edges connected to eI" );
			if ( eI->element1 != nullptr && eI->element1->firstNode == nKm1 )
			{
				eI->element1->firstNode = nKp1; // Don't forget firstNode!!
			}
			if ( eI->element2 != nullptr && eI->element2->firstNode == nKm1 )
			{
				eI->element2->firstNode = nKp1; // Don't forget firstNode!!
			}

			Msg::debug( "...replacing " + nKm1->descr() + " with " + nKp1->descr() + " on edge " + eI->descr() );

			nKm1->edgeList[i] = nullptr;
			eI->replaceNode( nKm1, nKp1 );
			addList.push_back( eI ); // nKp1.edgeList.add(eI);
		}
		else
		{
			found = false;
		}
	}

	for ( int i = 0; i < addList.size(); i++ )
	{
		e = addList[i];
		nKp1->edgeList.push_back( e );
	}

	for ( auto q : quadList )
	{
		q->updateLR();
	}

	nKm1->edgeList.clear();

	Msg::debug( "Leaving Quad.closeQuad(..)" );
}

Quad*
Quad::combine( Quad* q, Edge* e1, Edge* e2 )
{
	Msg::debug("Entering Quad.combine(Quad, ..)");
	Quad* quad;
	Edge* e;
	std::array<Edge*, 4> edges;
	int i;

	edges[0] = nullptr;
	for ( i = 0; i < 4; i++ )
	{
		e = edgeList[i];

		if ( e != e1 && e != e2 )
		{
			if ( edges[0] == nullptr )
			{
				edges[0] = e;
			}
			else if ( e->hasNode( edges[0]->leftNode ) )
			{
				edges[1] = e;
			}
			else if ( e->hasNode( edges[0]->rightNode ) )
			{
				edges[2] = e;
			}
		}
	}

	for ( i = 0; i < 4; i++ )
	{
		if ( q->isFake && i == 3 )
		{
			edges[3] = edges[2];
		}
		else
		{
			e = q->edgeList[i];

			if ( e != e1 && e != e2 )
			{
				if ( e->hasNode( edges[0]->leftNode ) )
				{
					edges[1] = e;
				}
				else if ( e->hasNode( edges[0]->rightNode ) )
				{
					edges[2] = e;
				}
				else
				{
					edges[3] = e;
				}
			}
		}
	}

	// A triangle (fake quad) will have edges[2]== edges[3].
	quad = new Quad( edges[0], edges[1], edges[2], edges[3] );
	Msg::debug( "Leaving Quad.combine(Quad, ..)" );
	return quad;
	return nullptr;
}

Triangle* 
Quad::combine( Triangle* t, Edge* e1, Edge* e2 )
{
	Msg::debug("Entering Quad.combine(Triangle, ..)");
	Triangle* tri;
	Edge* e;
	std::array<Edge*, 3> edges;
	int i;

	edges[0] = nullptr;
	for ( i = 0; i < 4; i++ )
	{
		e = edgeList[i];

		if ( e != e1 && e != e2 )
		{
			if ( edges[0] == nullptr )
			{
				edges[0] = e;
			}
			else if ( e->hasNode( edges[0]->leftNode ) )
			{
				edges[1] = e;
			}
			else
			{
				edges[2] = e;
			}
		}
	}

	for ( i = 0; i < 3; i++ )
	{
		e = t->edgeList[i];

		if ( e != e1 && e != e2 )
		{
			if ( e->hasNode( edges[0]->leftNode ) )
			{
				edges[1] = e;
			}
			else
			{
				edges[2] = e;
			}
		}
	}

	tri = new Triangle( edges[0], edges[1], edges[2] );
	Msg::debug( "Leaving Quad.combine(Triangle t, ..)" );
	return tri;
	return nullptr;
}

Element* 
Quad::neighbor( Edge* e )
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
		Msg::warning( "Quad.neighbor(Edge): returning null" );
		return nullptr;
	}
}

Edge* 
Quad::neighborEdge( Node* n, Edge* e )
{
	if ( edgeList[base]->leftNode == n )
	{
		if ( edgeList[base] == e )
		{
			return edgeList[left];
		}
		else if ( edgeList[left] == e )
		{
			return edgeList[base];
		}
		else
		{
			Msg::warning( "Quad.neighborEdge(Node n, Edge e): neighbor not found:" );
			Msg::error( "quad: " + descr() + ", n: " + n->descr() + ", e: " + e->descr() );
			return nullptr;
		}
	}
	else if ( edgeList[base]->rightNode == n )
	{
		if ( edgeList[base] == e )
		{
			return edgeList[right];
		}
		else if ( edgeList[right] == e )
		{
			return edgeList[base];
		}
		else
		{
			Msg::warning( "Quad.neighborEdge(Node n, Edge e): neighbor not found:" );
			Msg::error( "quad: " + descr() + ", n: " + n->descr() + ", e: " + e->descr() );
			return nullptr;
		}
	}
	else if ( edgeList[left]->otherNode( edgeList[base]->leftNode ) == n )
	{
		if ( isFake )
		{
			if ( edgeList[right] == e )
			{
				return edgeList[left];
			}
			else if ( edgeList[left] == e )
			{
				return edgeList[right];
			}
			else
			{
				Msg::warning( "Quad.neighborEdge(Node n, Edge e): neighbor not found:" );
				Msg::error( "quad: " + descr() + ", n: " + n->descr() + ", e: " + e->descr() );
				return nullptr;
			}
		}
		else
		{
			if ( edgeList[top] == e )
			{
				return edgeList[left];
			}
			else if ( edgeList[left] == e )
			{
				return edgeList[top];
			}
			else
			{
				Msg::warning( "Quad.neighborEdge(Node n, Edge e): neighbor not found:" );
				Msg::error( "quad: " + descr() + ", n: " + n->descr() + ", e: " + e->descr() );
				return nullptr;
			}
		}
	}
	else if ( edgeList[right]->otherNode( edgeList[base]->rightNode ) == n )
	{
		if ( edgeList[top] == e )
		{
			return edgeList[right];
		}
		else if ( edgeList[right] == e )
		{
			return edgeList[top];
		}
		else
		{
			Msg::warning( "Quad.neighborEdge(Node n, Edge e): neighbor not found:" );
			Msg::error( "quad: " + descr() + ", n: " + n->descr() + ", e: " + e->descr() );
			return nullptr;
		}
	}
	else
	{
		Msg::warning( "Quad.neighborEdge(Node n, Edge e): neighbor not found:" );
		Msg::error( "quad: " + descr() + ", n: " + n->descr() + ", e: " + e->descr() );
		return nullptr;
	}
}

Edge* 
Quad::neighborEdge( Node* n )
{
	if ( edgeList[base]->leftNode == n )
	{
		return edgeList[base];
	}
	else if ( edgeList[base]->rightNode == n )
	{
		return edgeList[right];
	}
	else if ( edgeList[left]->otherNode( edgeList[base]->leftNode ) == n )
	{
		return edgeList[left];
	}
	else if ( edgeList[right]->otherNode( edgeList[base]->rightNode ) == n )
	{
		return edgeList[top];
	}
	else
	{
		Msg::error( "Quad.neighborEdge(Node n): neighbor not found." );
		return nullptr;
	}
}

double
Quad::angle( Edge* e, Node* n )
{
	// Find this edge's index
	int thisEdgeIndex = indexOf( e );

	// Find other edge's index
	int otherEdgeIndex;
	if ( edgeList[base] != e && edgeList[base]->hasNode( n ) )
	{
		otherEdgeIndex = base;
	}
	else if ( edgeList[left] != e && edgeList[left]->hasNode( n ) )
	{
		otherEdgeIndex = left;
	}
	else if ( edgeList[right] != e && edgeList[right]->hasNode( n ) )
	{
		otherEdgeIndex = right;
	}
	else
	{
		otherEdgeIndex = top;
	}

	// Return correct angle
	return ang[angleIndex( thisEdgeIndex, otherEdgeIndex )];
}

int
Quad::angleIndex( int e1Index, int e2Index )
{
	if ( (e1Index == base && e2Index == left) || // angle at base, left
		 (e1Index == left && e2Index == base) )
	{
		return 0;
	}
	else if ( (e1Index == base && e2Index == right) || // angle at base,right
			  (e1Index == right && e2Index == base) )
	{
		return 1;
	}
	else if ( (e1Index == top && e2Index == left) || // angle at top, left
			  (e1Index == left && e2Index == top) )
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

int 
Quad::angleIndex( Edge* e1, Edge* e2 )
{
	return angleIndex( indexOf( e1 ), indexOf( e2 ) );
}

int
Quad::angleIndex( Node* n )
{
	if ( edgeList[base]->leftNode == n )
	{
		return 0;
	}
	else if ( edgeList[base]->rightNode == n )
	{
		return 1;
	}
	else if ( edgeList[left]->otherNode( edgeList[base]->leftNode ) == n )
	{
		return 2;
	}
	else if ( edgeList[right]->otherNode( edgeList[base]->rightNode ) == n )
	{
		return 3;
	}
	else
	{
		Msg::error( "Quad.angleIndex(Node): Node not found" );
		return -1;
	}
}

double
Quad::angle( Edge* e1, Edge* e2 )
{
	return ang[angleIndex( indexOf( e1 ), indexOf( e2 ) )];
}

bool 
Quad::hasNode( Node* n )
{
	if ( edgeList[base]->leftNode->equals( n ) || edgeList[base]->rightNode->equals( n ) || edgeList[top]->leftNode->equals( n ) || edgeList[top]->rightNode->equals( n ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
Quad::hasEdge( Edge* e )
{
	if ( edgeList[base] == e || edgeList[left] == e || edgeList[right] == e || edgeList[top] == e )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
Quad::boundaryQuad()
{
	if ( Element::IsAQuad( neighbor( edgeList[base] ) ) ||
		 Element::IsAQuad( neighbor( edgeList[left] ) ) || 
		 Element::IsAQuad( neighbor( edgeList[right] ) ) ||
		 Element::IsAQuad( neighbor( edgeList[top] ) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool 
Quad::boundaryDiamond()
{
	int bnodes = 0;
	if ( edgeList[base]->leftNode->boundaryNode() )
	{
		bnodes++;
	}
	if ( edgeList[base]->rightNode->boundaryNode() )
	{
		bnodes++;
	}
	if ( edgeList[top]->leftNode->boundaryNode() )
	{
		bnodes++;
	}
	if ( edgeList[top]->rightNode->boundaryNode() )
	{
		bnodes++;
	}

	if ( bnodes == 1 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

Node* 
Quad::getBoundaryNode()
{
	if ( edgeList[base]->leftNode->boundaryNode() )
	{
		return edgeList[base]->leftNode;
	}
	else if ( edgeList[base]->rightNode->boundaryNode() )
	{
		return edgeList[base]->rightNode;
	}
	else if ( edgeList[top]->leftNode->boundaryNode() )
	{
		return edgeList[top]->leftNode;
	}
	else if ( edgeList[top]->rightNode->boundaryNode() )
	{
		return edgeList[top]->rightNode;
	}
	else
	{
		return nullptr;
	}
}

bool 
Quad::areaLargerThan0()
{
	Node* node3 = edgeList[top]->leftNode;
	Node* node4 = edgeList[top]->rightNode;

	if ( !node3->onLine( edgeList[base] ) || !node4->onLine( edgeList[base] ) )
	{
		return true;
	}
	return true;
}

bool
Quad::isConvex()
{
	Node* n1 = edgeList[base]->leftNode;
	Node* n2 = edgeList[base]->rightNode;
	Node* n3 = edgeList[left]->otherNode( n1 );
	Node* n4 = edgeList[right]->otherNode( n2 );

	MyVector d1( *n1, *n4 );
	MyVector d2( *n2, *n3 );
	if ( d1.pointIntersects( d2 ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool 
Quad::isStrictlyConvex()
{
	Node* n1 = edgeList[base]->leftNode;
	Node* n2 = edgeList[base]->rightNode;
	Node* n3 = edgeList[left]->otherNode( n1 );
	Node* n4 = edgeList[right]->otherNode( n2 );

	MyVector d1( *n1, *n4 );
	MyVector d2( *n2, *n3 );
	if ( d1.innerpointIntersects( d2 ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool 
Quad::isBowtie()
{
	Node* n1 = edgeList[base]->leftNode;
	Node* n2 = edgeList[base]->rightNode;
	Node* n3 = edgeList[left]->otherNode( n1 );
	Node* n4 = edgeList[right]->otherNode( n2 );

	MyVector d1( *n1, *n2 );
	MyVector d2( *n3, *n4 );
	MyVector d3( *n1, *n3 );
	MyVector d4( *n2, *n4 );
	if ( d1.pointIntersects( d2 ) || d3.pointIntersects( d4 ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool 
Quad::isChevron()
{
	if ( largestAngle() >= CHEVRONMIN )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool 
Quad::largestAngleGT180()
{
	if ( largestAngle() > DEG_180 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

Node* 
Quad::nextCCWNodeOld( Node* n )
{
	MyVector v0, v1;
	Node* n0, *n1;

	if ( n == edgeList[base]->leftNode )
	{
		n0 = edgeList[base]->rightNode;
		n1 = edgeList[left]->otherNode( n );
	}
	else if ( n == edgeList[base]->rightNode )
	{
		n0 = edgeList[right]->otherNode( n );
		n1 = edgeList[base]->leftNode;
	}
	else if ( n == edgeList[left]->otherNode( edgeList[base]->leftNode ) )
	{
		n0 = edgeList[base]->leftNode;
		n1 = edgeList[right]->otherNode( edgeList[base]->rightNode );
	}
	else if ( n == edgeList[right]->otherNode( edgeList[base]->rightNode ) )
	{
		n0 = edgeList[base]->rightNode;
		n1 = edgeList[left]->otherNode( edgeList[base]->leftNode );
	}
	else
	{
		return nullptr;
	}

	v0 = MyVector( *n,* n0 );
	v1 = MyVector( *n, *n1 );
	if ( v1.isCWto( v0 ) )
	{
		return n1;
	}
	else
	{
		return n0;
	}
}

Node* 
Quad::nextCCWNode( Node* n )
{
	Node* n2, *n3, *n4;
	Edge* e2, *e3;
	if ( n == firstNode )
	{
		return edgeList[base]->otherNode( firstNode ); // n2
	}
	else
	{
		n2 = edgeList[base]->otherNode( firstNode );
		e2 = neighborEdge( n2, edgeList[base] );
		if ( n == n2 )
		{
			return e2->otherNode( n2 ); // n3
		}
		else
		{
			n3 = e2->otherNode( n2 );
			e3 = neighborEdge( n3, e2 );
			if ( n == n3 )
			{
				return e3->otherNode( n3 ); // n4
			}
			else
			{
				n4 = e3->otherNode( n3 );
				if ( n == n4 )
				{
					return firstNode;
				}
				else
				{
					return nullptr;
				}
			}
		}
	}
}

void 
Quad::updateLR()
{
	Msg::debug( "Entering Quad.updateLR()" );
	Edge* temp;
	double dt0, dt1, dt2, dt3;
	if ( !edgeList[left]->hasNode( edgeList[base]->leftNode ) )
	{
		Msg::debug( "...updating" );
		temp = edgeList[left];
		edgeList[left] = edgeList[right];
		edgeList[right] = temp;

		dt0 = ang[0];
		dt1 = ang[1];
		dt2 = ang[2];
		dt3 = ang[3];

		ang[0] = dt1;
		ang[1] = dt0;
		ang[2] = dt3;
		ang[3] = dt2;
	}
	Msg::debug( "Leaving Quad.updateLR()" );
}

void 
Quad::updateAngles()
{
	if ( isFake )
	{
		if ( firstNode == edgeList[base]->leftNode )
		{
			ang[0] = edgeList[base]->computeCCWAngle( edgeList[left] );
			ang[1] = edgeList[right]->computeCCWAngle( edgeList[base] );
			ang[2] = edgeList[left]->computeCCWAngle( edgeList[right] );
		}
		else
		{
			ang[0] = edgeList[left]->computeCCWAngle( edgeList[base] );
			ang[1] = edgeList[base]->computeCCWAngle( edgeList[right] );
			ang[2] = edgeList[right]->computeCCWAngle( edgeList[left] );
		}
	}
	else if ( firstNode == edgeList[base]->leftNode )
	{
		ang[0] = edgeList[base]->computeCCWAngle( edgeList[left] );
		ang[1] = edgeList[right]->computeCCWAngle( edgeList[base] );
		ang[2] = edgeList[left]->computeCCWAngle( edgeList[top] );
		ang[3] = edgeList[top]->computeCCWAngle( edgeList[right] );
	}
	else
	{
		ang[0] = edgeList[left]->computeCCWAngle( edgeList[base] );
		ang[1] = edgeList[base]->computeCCWAngle( edgeList[right] );
		ang[2] = edgeList[top]->computeCCWAngle( edgeList[left] );
		ang[3] = edgeList[right]->computeCCWAngle( edgeList[top] );
	}
}

void
Quad::updateAnglesExcept( Node* n )
{
	int i = angleIndex( n );
	if ( isFake )
	{
		if ( firstNode == edgeList[base]->leftNode )
		{
			if ( i != 0 )
			{
				ang[0] = edgeList[base]->computeCCWAngle( edgeList[left] );
			}
			if ( i != 1 )
			{
				ang[1] = edgeList[right]->computeCCWAngle( edgeList[base] );
			}
			if ( i != 2 )
			{
				ang[2] = edgeList[left]->computeCCWAngle( edgeList[right] );
			}
		}
		else
		{
			if ( i != 0 )
			{
				ang[0] = edgeList[left]->computeCCWAngle( edgeList[base] );
			}
			if ( i != 1 )
			{
				ang[1] = edgeList[base]->computeCCWAngle( edgeList[right] );
			}
			if ( i != 2 )
			{
				ang[2] = edgeList[right]->computeCCWAngle( edgeList[left] );
			}
		}
	}
	else if ( firstNode == edgeList[base]->leftNode )
	{
		if ( i != 0 )
		{
			ang[0] = edgeList[base]->computeCCWAngle( edgeList[left] );
		}
		if ( i != 1 )
		{
			ang[1] = edgeList[right]->computeCCWAngle( edgeList[base] );
		}
		if ( i != 2 )
		{
			ang[2] = edgeList[left]->computeCCWAngle( edgeList[top] );
		}
		if ( i != 3 )
		{
			ang[3] = edgeList[top]->computeCCWAngle( edgeList[right] );
		}
	}
	else
	{
		if ( i != 0 )
		{
			ang[0] = edgeList[left]->computeCCWAngle( edgeList[base] );
		}
		if ( i != 1 )
		{
			ang[1] = edgeList[base]->computeCCWAngle( edgeList[right] );
		}
		if ( i != 2 )
		{
			ang[2] = edgeList[top]->computeCCWAngle( edgeList[left] );
		}
		if ( i != 3 )
		{
			ang[3] = edgeList[right]->computeCCWAngle( edgeList[top] );
		}
	}
}

void 
Quad::updateAngle( Node* n )
{
	int i = angleIndex( n );

	if ( isFake )
	{
		if ( firstNode == edgeList[base]->leftNode )
		{
			if ( i == 0 )
			{
				ang[0] = edgeList[base]->computeCCWAngle( edgeList[left] );
			}
			else if ( i == 1 )
			{
				ang[1] = edgeList[right]->computeCCWAngle( edgeList[base] );
			}
			else if ( i == 2 )
			{
				ang[2] = edgeList[left]->computeCCWAngle( edgeList[right] );
			}
		}
		else
		{
			if ( i == 0 )
			{
				ang[0] = edgeList[left]->computeCCWAngle( edgeList[base] );
			}
			else if ( i == 1 )
			{
				ang[1] = edgeList[base]->computeCCWAngle( edgeList[right] );
			}
			else if ( i == 2 )
			{
				ang[2] = edgeList[right]->computeCCWAngle( edgeList[left] );
			}
		}
	}
	else if ( firstNode == edgeList[base]->leftNode )
	{
		if ( i == 0 )
		{
			ang[0] = edgeList[base]->computeCCWAngle( edgeList[left] );
		}
		else if ( i == 1 )
		{
			ang[1] = edgeList[right]->computeCCWAngle( edgeList[base] );
		}
		else if ( i == 2 )
		{
			ang[2] = edgeList[left]->computeCCWAngle( edgeList[top] );
		}
		else if ( i == 3 )
		{
			ang[3] = edgeList[top]->computeCCWAngle( edgeList[right] );
		}
	}
	else
	{
		if ( i == 0 )
		{
			ang[0] = edgeList[left]->computeCCWAngle( edgeList[base] );
		}
		else if ( i == 1 )
		{
			ang[1] = edgeList[base]->computeCCWAngle( edgeList[right] );
		}
		else if ( i == 2 )
		{
			ang[2] = edgeList[top]->computeCCWAngle( edgeList[left] );
		}
		else if ( i == 3 )
		{
			ang[3] = edgeList[right]->computeCCWAngle( edgeList[top] );
		}
	}
}

bool 
Quad::inverted()
{
	Msg::debug( "Entering Quad.inverted()" );
	if ( isFake )
	{

		Node* a, *b, *c;
		a = firstNode;
		b = edgeList[base]->otherNode( a );
		if ( a == edgeList[base]->leftNode )
		{
			c = edgeList[left]->otherNode( a );
		}
		else
		{
			c = edgeList[right]->otherNode( a );
		}

		Msg::debug( "Leaving Quad.inverted() (fake)" );
		if ( cross( a, c, b, c ) < 0 )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	Node* a = edgeList[base]->leftNode, *b = edgeList[base]->rightNode, *c = edgeList[right]->otherNode( b ), *d = edgeList[left]->otherNode( a );

	// We need at least 3 okays to be certain that this quad is not inverted
	int okays = 0;
	if ( cross( a, d, b, d ) > 0 )
	{
		okays++;
	}
	if ( cross( a, c, b, c ) > 0 )
	{
		okays++;
	}
	if ( cross( c, a, d, a ) > 0 )
	{
		okays++;
	}
	if ( cross( c, b, d, b ) > 0 )
	{
		okays++;
	}

	if ( b == firstNode )
	{
		okays = 4 - okays;
	}

	Msg::debug( "Leaving Quad.inverted(), okays: " + std::to_string( okays ) );
	if ( okays >= 3 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool 
Quad::invertedOrZeroArea()
{
	Msg::debug( "Entering Quad.invertedOrZeroArea()" );
	if ( isFake )
	{

		Node* a, *b, *c;
		a = firstNode;
		b = edgeList[base]->otherNode( a );
		if ( a == edgeList[base]->leftNode )
		{
			c = edgeList[left]->otherNode( a );
		}
		else
		{
			c = edgeList[right]->otherNode( a );
		}

		Msg::debug( "Leaving Quad.invertedOrZeroArea() (fake)" );
		if ( cross( a, c, b, c ) <= 0 )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	Node* a = edgeList[base]->leftNode, *b = edgeList[base]->rightNode, *c = edgeList[right]->otherNode( b ), *d = edgeList[left]->otherNode( a );

	// We need at least 3 okays to be certain that this quad is not inverted
	int okays = 0;
	if ( cross( a, d, b, d ) >= 0 )
	{
		okays++;
	}
	if ( cross( a, c, b, c ) >= 0 )
	{
		okays++;
	}
	if ( cross( c, a, d, a ) >= 0 )
	{
		okays++;
	}
	if ( cross( c, b, d, b ) >= 0 )
	{
		okays++;
	}

	if ( b == firstNode )
	{
		okays = 4 - okays;
	}

	Msg::debug( "Leaving Quad.invertedOrZeroArea(), okays: " + std::to_string( okays ) );
	if ( okays >= 3 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool 
Quad::concavityAt( Node* n )
{
	Msg::debug( "Entering Quad.concavityAt(..)" );
	if ( ang[angleIndex( n )] >= std::numbers::pi )
	{
		Msg::debug( "Leaving Quad.concavityAt(..), returning true" );
		return true;
	}
	else
	{
		Msg::debug( "Leaving Quad.concavityAt(..), returning false" );
		return false;
	}
}

Node* 
Quad::centroid()
{
	double x = 0, y = 0;
	Node* bL = edgeList[base]->leftNode, *bR = edgeList[base]->rightNode;
	Node* uL = edgeList[left]->otherNode( bL ), *uR = edgeList[right]->otherNode( bR );

	if ( isFake )
	{
		x = (bL->x + uL->x + bR->x) / 3.0;
		y = (bL->y + uL->y + bR->y) / 3.0;
		return new Node( x, y );
	}
	else
	{
		if ( concavityAt( bL ) )
		{
			x = (bL->x + uR->x) / 2.0;
			y = (bL->y + uR->y) / 2.0;
		}
		else if ( concavityAt( bR ) )
		{
			x = (bR->x + uL->x) / 2.0;
			y = (bR->y + uL->y) / 2.0;
		}
		else if ( concavityAt( uL ) )
		{
			x = (uL->x + bR->x) / 2.0;
			y = (uL->y + bR->y) / 2.0;
		}
		else if ( concavityAt( uR ) )
		{
			x = (uR->x + bL->x) / 2.0;
			y = (uR->y + bL->y) / 2.0;
		}
		else
		{
			x = (bL->x + uL->x + uR->x + bR->x) / 4.0;
			y = (bL->y + uL->y + uR->y + bR->y) / 4.0;

		}
		return new Node( x, y );
	}
}

Node* 
Quad::oppositeNode( Node* n )
{
	// 2 out of 4 edges has Node n, so at least 1 out of 3 edge must have it, too:
	Edge* startEdge;
	if ( edgeList[0]->hasNode( n ) )
	{
		startEdge = edgeList[0];
	}
	else if ( edgeList[1]->hasNode( n ) )
	{
		startEdge = edgeList[1];
	}
	else if ( edgeList[2]->hasNode( n ) )
	{
		startEdge = edgeList[2];
	}
	else
	{
		return nullptr; // Most likely, Node n is not part of this Quad.
	}

	Node* n2 = startEdge->otherNode( n );
	Edge* e = neighborEdge( n2, startEdge );
	return e->otherNode( n2 );
}

Edge* 
Quad::cwOppositeEdge( Node* n )
{
	if ( n == edgeList[base]->leftNode )
	{
		return edgeList[right];
	}
	else if ( n == edgeList[base]->rightNode )
	{
		return edgeList[top];
	}
	else if ( n == edgeList[left]->otherNode( edgeList[base]->leftNode ) )
	{
		return edgeList[base];
	}
	else if ( n == edgeList[right]->otherNode( edgeList[base]->rightNode ) )
	{
		return edgeList[left];
	}
	else
	{
		return nullptr;
	}
}

Edge* 
Quad::ccwOppositeEdge( Node* n )
{
	if ( n == edgeList[base]->leftNode )
	{
		return edgeList[top];
	}
	else if ( n == edgeList[base]->rightNode )
	{
		return edgeList[left];
	}
	else if ( n == edgeList[left]->otherNode( edgeList[base]->leftNode ) )
	{
		return edgeList[right];
	}
	else if ( n == edgeList[right]->otherNode( edgeList[base]->rightNode ) )
	{
		return edgeList[base];
	}
	else
	{
		return nullptr;
	}
}

Edge* 
Quad::oppositeEdge( Edge* e )
{
	if ( e == edgeList[base] )
	{
		return edgeList[top];
	}
	else if ( e == edgeList[left] )
	{
		return edgeList[right];
	}
	else if ( e == edgeList[right] )
	{
		return edgeList[left];
	}
	else if ( e == edgeList[top] )
	{
		return edgeList[base];
	}
	else
	{
		return nullptr;
	}
}

bool
Quad::invertedNeighbors()
{
	Node* uLNode = edgeList[left]->otherNode( edgeList[base]->leftNode );
	Node* uRNode = edgeList[right]->otherNode( edgeList[base]->rightNode );
	Element* curElem;
	Edge* curEdge;

	// Parse all adjacent elements at upper left node from neigbor of left edge to,
	// but not including, neighbor of top edge.
	curElem = neighbor( edgeList[left] );
	curEdge = edgeList[left];
	while ( curElem != nullptr && curEdge != edgeList[top] )
	{
		if ( curElem->inverted() )
		{
			return true;
		}
		curEdge = curElem->neighborEdge( uLNode, curEdge );
		curElem = curElem->neighbor( curEdge );
	}

	// Parse all adjacent elements at upper right node from neigbor of top edge to,
	// but not including, neighbor of right edge.
	curElem = neighbor( edgeList[top] );
	curEdge = edgeList[top];
	while ( curElem != nullptr && curEdge != edgeList[right] )
	{
		if ( curElem->inverted() )
		{
			return true;
		}
		curEdge = curElem->neighborEdge( uRNode, curEdge );
		curElem = curElem->neighbor( curEdge );
	}

	// Parse all adjacent elements at lower right node from neigbor of right edge
	// to,
	// but not including, neighbor of base edge.
	curElem = neighbor( edgeList[right] );
	curEdge = edgeList[right];
	while ( curElem != nullptr && curEdge != edgeList[base] )
	{
		if ( curElem->inverted() )
		{
			return true;
		}
		curEdge = curElem->neighborEdge( edgeList[base]->rightNode, curEdge );
		curElem = curElem->neighbor( curEdge );
	}

	// Parse all adjacent elements at lower left node from neigbor of base edge to,
	// but not including, neighbor of left edge.
	curElem = neighbor( edgeList[base] );
	curEdge = edgeList[base];
	while ( curElem != nullptr && curEdge != edgeList[left] )
	{
		if ( curElem->inverted() )
		{
			return true;
		}
		curEdge = curElem->neighborEdge( edgeList[base]->leftNode, curEdge );
		curElem = curElem->neighbor( curEdge );
	}
	return false;
}

std::vector<Triangle*> 
Quad::getAdjTriangles()
{
	std::vector<Triangle*>  triangleList;
	Node* uLNode = edgeList[left]->otherNode( edgeList[base]->leftNode );
	Node* uRNode = edgeList[right]->otherNode( edgeList[base]->rightNode );
	Node* bLNode = edgeList[base]->leftNode;
	Node* bRNode = edgeList[base]->rightNode;

	triangleList = bLNode->adjTriangles();

	// Parse all adjacent elements at upper left node from, but not not including,
	// neigbor of left edge to, but not including, neighbor of top edge.
	Element* curElem = neighbor( edgeList[left] );
	Edge* curEdge = edgeList[left];
	if ( curElem != nullptr )
	{
		curEdge = curElem->neighborEdge( uLNode, curEdge );
		curElem = curElem->neighbor( curEdge );

		while ( curElem != nullptr && curEdge != edgeList[top] )
		{
			if ( Element::IsATriangle( curElem ) )
			{
				triangleList.push_back( static_cast<Triangle*>(curElem) );
			}
			curEdge = curElem->neighborEdge( uLNode, curEdge );
			curElem = curElem->neighbor( curEdge );
		}
	}
	// Parse all adjacent elements at upper right node from neigbor of top edge to,
	// but not including, neighbor of right edge.
	curElem = neighbor( edgeList[top] );
	curEdge = edgeList[top];
	while ( curElem != nullptr && curEdge != edgeList[right] )
	{
		auto Iter = std::find( triangleList.begin(), triangleList.end(), curElem );
		if ( Element::IsATriangle( curElem ) && Iter == triangleList.end() )
		{
			triangleList.push_back( static_cast<Triangle*>(curElem) );
		}
		curEdge = curElem->neighborEdge( uRNode, curEdge );
		curElem = curElem->neighbor( curEdge );
	}

	auto AdjacentList = bRNode->adjTriangles();
	triangleList.insert( triangleList.begin(), AdjacentList.begin(), AdjacentList.end() );

	return triangleList;
}

std::vector<Node*> 
Quad::getAdjNodes()
{
	std::vector<Node*> nodeList;
	Edge* e;
	Node* n;
	Node* bLNode = edgeList[base]->leftNode;
	Node* bRNode = edgeList[base]->rightNode;
	Node* uLNode = edgeList[left]->otherNode( bLNode );

	int i;

	for ( i = 0; i < bLNode->edgeList.size(); i++ )
	{
		e = bLNode->edgeList[i];
		if ( e != edgeList[base] && e != edgeList[left] && e != edgeList[right] && e != edgeList[top] )
		{
			nodeList.push_back( e->otherNode( bLNode ) );
		}
	}

	for ( i = 0; i < bRNode->edgeList.size(); i++ )
	{
		e = bRNode->edgeList[i];
		if ( e != edgeList[base] && e != edgeList[left] && e != edgeList[right] && e != edgeList[top] )
		{
			n = e->otherNode( bRNode );
			auto Iter = std::find( nodeList.begin(), nodeList.end(), n );
			if ( Iter == nodeList.end() )
			{
				nodeList.push_back( n );
			}
		}
	}

	for ( i = 0; i < uLNode->edgeList.size(); i++ )
	{
		e = uLNode->edgeList[i];
		if ( e != edgeList[base] && e != edgeList[left] && e != edgeList[right] && e != edgeList[top] )
		{
			n = e->otherNode( uLNode );
			auto Iter = std::find( nodeList.begin(), nodeList.end(), n );
			if ( Iter == nodeList.end() )
			{
				nodeList.push_back( n );
			}
		}
	}

	if ( !isFake )
	{
		Node* uRNode = edgeList[right]->otherNode( bRNode );
		for ( i = 0; i < uRNode->edgeList.size(); i++ )
		{
			e = uRNode->edgeList[i];
			if ( e != edgeList[base] && e != edgeList[left] && e != edgeList[right] && e != edgeList[top] )
			{
				n = e->otherNode( uRNode );
				auto Iter = std::find( nodeList.begin(), nodeList.end(), n );
				if ( Iter == nodeList.end() )
				{
					nodeList.push_back( n );
				}
			}
		}
	}
	return nodeList;
}

void 
Quad::replaceEdge( Edge* e, Edge* replacement )
{
	edgeList[indexOf( e )] = replacement;
}

void
Quad::connectEdges()
{
	edgeList[base]->connectToQuad( this );
	edgeList[left]->connectToQuad( this );
	edgeList[right]->connectToQuad( this );
	if ( !isFake )
	{
		edgeList[top]->connectToQuad( this );
	}
}

void
Quad::disconnectEdges()
{
	edgeList[base]->disconnectFromElement( this );
	edgeList[left]->disconnectFromElement( this );
	edgeList[right]->disconnectFromElement( this );
	if ( !isFake )
	{
		edgeList[top]->disconnectFromElement( this );
	}
}

Edge* 
Quad::commonEdgeAt( Node* n, Quad* q )
{
	for ( auto e : n->edgeList )
	{
		if ( hasEdge( e ) && q->hasEdge( e ) )
		{
			return e;
		}
	}
	return nullptr;
}

Edge* 
Quad::commonEdge( Quad* q )
{
	if ( q == neighbor( edgeList[base] ) )
	{
		return edgeList[base];
	}
	else if ( q == neighbor( edgeList[left] ) )
	{
		return edgeList[left];
	}
	else if ( q == neighbor( edgeList[right] ) )
	{
		return edgeList[right];
	}
	else if ( q == neighbor( edgeList[top] ) )
	{
		return edgeList[top];
	}
	else
	{
		return nullptr;
	}
}

bool 
Quad::hasFrontEdgeAt( Node* n )
{
	if ( edgeList[left]->hasNode( n ) )
	{
		if ( edgeList[base]->hasNode( n ) )
		{
			if ( edgeList[left]->isFrontEdge() || edgeList[base]->isFrontEdge() )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if ( edgeList[top]->hasNode( n ) )
		{
			if ( edgeList[left]->isFrontEdge() || edgeList[top]->isFrontEdge() )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else if ( edgeList[right]->hasNode( n ) )
	{
		if ( edgeList[base]->hasNode( n ) )
		{
			if ( edgeList[right]->isFrontEdge() || edgeList[base]->isFrontEdge() )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if ( edgeList[top]->hasNode( n ) )
		{
			if ( edgeList[right]->isFrontEdge() || edgeList[top]->isFrontEdge() )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

int 
Quad::nrOfQuadsSharingAnEdgeAt( Node* n )
{
	int count = 0;

	if ( edgeList[left]->hasNode( n ) )
	{
		if ( Element::IsAQuad( neighbor( edgeList[left] ) ) )
		{
			count++;
		}
		if ( edgeList[base]->hasNode( n ) )
		{
			if ( Element::IsAQuad( neighbor( edgeList[base] ) ) )
			{
				count++;
			}
		}
		else if ( Element::IsAQuad( neighbor( edgeList[top] ) ) )
		{
			count++;
		}
		return count;
	}
	else if ( edgeList[right]->hasNode( n ) )
	{
		if ( Element::IsAQuad( neighbor( edgeList[right] ) ) )
		{
			count++;
		}
		if ( edgeList[base]->hasNode( n ) )
		{
			if ( Element::IsAQuad( neighbor( edgeList[base] ) ) )
			{
				count++;
			}
		}
		else if ( Element::IsAQuad( neighbor( edgeList[top] ) ) )
		{
			count++;
		}

		return count;
	}
	return count;
}

void
Quad::updateDistortionMetric()
{
	Msg::debug("Entering Quad.updateDistortionMetric()");

	if ( isFake )
	{
		double AB = edgeList[base]->len, CB = edgeList[left]->len, CA = edgeList[right]->len;

		Node* a = edgeList[base]->commonNode( edgeList[right] ), *b = edgeList[base]->commonNode( edgeList[left] ), *c = edgeList[left]->commonNode( edgeList[right] );
		MyVector vCA( *c, *a ), vCB( *c, *b );

		double temp = sqrt3x2 * abs( vCA.cross( vCB ) ) / (CA * CA + AB * AB + CB * CB);
		if ( inverted() )
		{
			distortionMetric = -temp;
		}
		else
		{
			distortionMetric = temp;
		}

		Msg::debug( "Leaving Quad.updateDistortionMetric(): " + std::to_string( distortionMetric ) );
		return;
	}

	Node* n1 = edgeList[base]->leftNode;
	Node* n2 = edgeList[base]->rightNode;
	Node* n3 = edgeList[left]->otherNode( n1 );
	Node* n4 = edgeList[right]->otherNode( n2 );

	// The two diagonals
	Edge* e1 = new Edge( n1, n4 );
	Edge* e2 = new Edge( n2, n3 );

	// The four triangles
	Triangle* t1 = new Triangle( edgeList[base], edgeList[left], e2 );
	Triangle* t2 = new Triangle( edgeList[base], e1, edgeList[right] );
	Triangle* t3 = new Triangle( edgeList[top], edgeList[right], e2 );
	Triangle* t4 = new Triangle( edgeList[top], e1, edgeList[left] );

	// Place the firstNodes correctly
	t1->firstNode = firstNode;
	if ( firstNode == n1 )
	{
		t2->firstNode = n1;
		t3->firstNode = n4;
		t4->firstNode = n4;
	}
	else
	{
		t2->firstNode = n2;
		t3->firstNode = n3;
		t4->firstNode = n3;
	}

	// Compute and get alpha values for each triangle
	t1->updateDistortionMetric( 4.0 );
	t2->updateDistortionMetric( 4.0 );
	t3->updateDistortionMetric( 4.0 );
	t4->updateDistortionMetric( 4.0 );

	double alpha1 = t1->distortionMetric, alpha2 = t2->distortionMetric, alpha3 = t3->distortionMetric, alpha4 = t4->distortionMetric;

	int invCount = 0;
	if ( alpha1 < 0 )
	{
		invCount++;
	}
	if ( alpha2 < 0 )
	{
		invCount++;
	}
	if ( alpha3 < 0 )
	{
		invCount++;
	}
	if ( alpha4 < 0 )
	{
		invCount++;
	}

	double temp12 = std::min( alpha1, alpha2 );
	double temp34 = std::min( alpha3, alpha4 );
	double alphaMin = std::min( temp12, temp34 );
	double negval = 0;

	if ( invCount >= 3 )
	{
		if ( invCount == 3 )
		{
			negval = 2.0;
		}
		else
		{
			negval = 3.0;
		}
	}
	else if ( ang[0] < DEG_6 || ang[1] < DEG_6 || ang[2] < DEG_6 || ang[3] < DEG_6 || coincidentNodes( n1, n2, n3, n4 ) || invCount == 2 )
	{
		negval = 1.0;
	}

	distortionMetric = alphaMin - negval;
	Msg::debug( "Leaving Quad.updateDistortionMetric(): " + std::to_string( distortionMetric ) );
}

bool 
Quad::coincidentNodes( Node* n1, Node* n2, Node* n3, Node* n4 )
{
	Msg::debug( "Entering Quad.coincidentNodes(..)" );
	double x12diff = n2->x - n1->x;
	double y12diff = n2->y - n1->y;
	double x13diff = n3->x - n1->x;
	double y13diff = n3->y - n1->y;
	double x14diff = n4->x - n1->x;
	double y14diff = n4->y - n1->y;

	double x23diff = n3->x - n2->x;
	double y23diff = n3->y - n2->y;
	double x24diff = n4->x - n2->x;
	double y24diff = n4->y - n2->y;

	double x34diff = n4->x - n3->x;
	double y34diff = n4->y - n3->y;

	// Using Pythagoras: hyp^2= kat1^2 + kat2^2
	double l12 = sqrt( x12diff * x12diff + y12diff * y12diff );
	double l13 = sqrt( x13diff * x13diff + y13diff * y13diff );
	double l14 = sqrt( x14diff * x14diff + y14diff * y14diff );
	double l23 = sqrt( x23diff * x23diff + y23diff * y23diff );
	double l24 = sqrt( x24diff * x24diff + y24diff * y24diff );
	double l34 = sqrt( x34diff * x34diff + y34diff * y34diff );

	if ( l12 < COINCTOL || l13 < COINCTOL || l14 < COINCTOL || l23 < COINCTOL || l24 < COINCTOL || l34 < COINCTOL )
	{
		Msg::debug( "Leaving Quad.coincidentNodes(..), returning true" );
		return true;
	}
	else
	{
		Msg::debug( "Leaving Quad.coincidentNodes(..), returning false" );
		return false;
	}
}

double
Quad::largestAngle()
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
	if ( ang[3] > cand )
	{
		cand = ang[3];
	}
	return cand;
}

Node*
Quad::nodeAtLargestAngle()
{
	Node* candNode = edgeList[base]->leftNode;
	double cand = ang[0];

	if ( ang[1] > cand )
	{
		candNode = edgeList[base]->rightNode;
		cand = ang[1];
	}
	if ( ang[2] > cand )
	{
		candNode = edgeList[left]->otherNode( edgeList[base]->leftNode );
		cand = ang[2];
	}
	if ( ang[3] > cand )
	{
		candNode = edgeList[right]->otherNode( edgeList[base]->rightNode );
	}
	return candNode;
}

double 
Quad::longestEdgeLength()
{
	double t1 = std::max( edgeList[base]->len, edgeList[left]->len );
	double t2 = std::max( t1, edgeList[right]->len );
	return std::max( t2, edgeList[top]->len );
}

std::vector<Triangle*> 
Quad::trianglesContained( Triangle* first )
{
	Msg::debug( "Entering trianglesContained(..)" );
	std::vector<Triangle*> tris;
	Element* neighbor;
	Triangle* cur;
	Edge* e;

	tris.push_back( first );
	for ( int j = 0; j < tris.size(); j++ )
	{
		cur = static_cast<Triangle*>( tris[j] );
		Msg::debug( "...parsing triangle " + cur->descr() );

		for ( int i = 0; i < 3; i++ )
		{
			e = cur->edgeList[i];
			if ( !hasEdge( e ) )
			{
				neighbor = cur->neighbor( e );
				auto Iter = std::find( tris.begin(), tris.end(), neighbor );
				if ( neighbor != nullptr && Iter == tris.end() && Element::IsATriangle( neighbor ) )
				{
					tris.push_back( static_cast<Triangle*>( neighbor ) );
				}
			}
		}
	}
	Msg::debug( "Leaving trianglesContained(..)" );
	return tris;
}

bool 
Quad::containsHole( const std::vector<Triangle*>& tris )
{

	if ( tris.size() == 0 )
	{
		return true; // Corresponds to a quad defined on a quad-shaped hole
	}

	for ( auto t : tris )
	{
		if ( t->edgeList[0]->boundaryEdge() && !hasEdge( t->edgeList[0] ) || t->edgeList[1]->boundaryEdge() && !hasEdge( t->edgeList[1] )
			 || t->edgeList[2]->boundaryEdge() && !hasEdge( t->edgeList[2] ) )
		{
			return true;
		}
	}
	return false;
}

void 
Quad::markEdgesLegal()
{
	edgeList[base]->color = 1;// java.awt.Color.green;
	edgeList[left]->color = 1;//java.awt.Color.green;
	edgeList[right]->color = 1;//java.awt.Color.green;
	edgeList[top]->color = 1;//java.awt.Color.green;
}

void
Quad::markEdgesIllegal()
{
	edgeList[base]->color = 2;//java.awt.Color.red;
	edgeList[left]->color = 2;//java.awt.Color.red;
	edgeList[right]->color = 2;//java.awt.Color.red;
	edgeList[top]->color = 2;// java.awt.Color.red;
}

std::string 
Quad::descr()
{
	Node* node1, *node2, *node3, *node4;
	node1 = edgeList[base]->leftNode;
	node2 = edgeList[base]->rightNode;
	node3 = edgeList[left]->otherNode( node1 );
	node4 = edgeList[right]->otherNode( node2 );

	return node1->descr() + ", " + node2->descr() + ", " + node3->descr() + ", " + node4->descr();
}

void 
Quad::printMe()
{
	std::cout << 
		descr() + ", inverted(): " + (inverted() ? "true":"false") + 
		", ang[0]: " + std::to_string( toDegrees * ang[0] ) + ", ang[1]: " +
		std::to_string( toDegrees * ang[1] ) + ", ang[2]: " + 
		std::to_string( toDegrees * ang[2] ) + ", ang[3]: " + 
		std::to_string( toDegrees * ang[3] ) + ", firstNode is " + 
		firstNode->descr();
}