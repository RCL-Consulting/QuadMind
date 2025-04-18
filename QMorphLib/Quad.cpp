#include "pch.h"
#include "Quad.h"

#include "Triangle.h"
#include "Node.h"
#include "Edge.h"
#include "MyVector.h"

#include "Msg.h"

#include <iostream>

//TODO: Test
Quad::Quad( const std::shared_ptr<Edge>& baseEdge,
			const std::shared_ptr<Edge>& leftEdge,
			const std::shared_ptr<Edge>& rightEdge,
			const std::shared_ptr<Edge>& topEdge )
{
	edgeList.assign( 4, nullptr );
	ang.assign( 4, 0.0 );

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

//TODO: Test
Quad::Quad( const std::shared_ptr<Edge>& e )
{
	isFake = false;
	edgeList.assign( 4, nullptr );
	ang.assign( 4, 0.0 );

	auto t1 = std::dynamic_pointer_cast<Triangle>(e->element1);
	auto t2 = std::dynamic_pointer_cast<Triangle>(e->element2);

	auto c11 = t1->otherEdge( e );
	auto c12 = t1->otherEdge( e, c11 );
	auto temp1 = t2->otherEdge( e ), c21 = temp1, c22 = temp1;
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

//TODO: Test
Quad::Quad( const std::shared_ptr<Triangle>& t )
{
	isFake = true;
	edgeList.assign( 4, nullptr );
	ang.assign( 4, 0.0 );

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

//TODO: Test
Quad::Quad( const std::shared_ptr<Edge>& e,
			const std::shared_ptr<Node>& n1,
			const std::shared_ptr<Node>& n2 )
{
	Msg::debug( "Entering Quad(Edge, Node, Node)" );
	Msg::debug( "e= " + e->descr() + ", n1= " + n1->descr() + ", n2= " + n2->descr() );
	isFake = false;
	edgeList.assign( 4, nullptr );

	edgeList[base] = std::make_shared<Edge>( e->leftNode, n1 );
	edgeList[top] = std::make_shared<Edge>( n2, e->rightNode );

	if ( edgeList[base]->leftNode == e->leftNode )
	{
		edgeList[right] = std::make_shared<Edge>( edgeList[base]->rightNode, e->rightNode );
		edgeList[left] = std::make_shared<Edge>( edgeList[base]->leftNode, edgeList[top]->otherNode( e->rightNode ) );
	}
	else
	{
		edgeList[right] = std::make_shared<Edge>( edgeList[base]->rightNode, edgeList[top]->otherNode( e->rightNode ) );
		edgeList[left] = std::make_shared<Edge>( edgeList[base]->leftNode, e->rightNode );
	}

	Msg::debug( "New quad is " + descr() );
	Msg::debug( "Leaving Quad(Edge, Node, Node)" );

	firstNode = edgeList[base]->leftNode;
	if ( inverted() )
	{
		firstNode = edgeList[base]->rightNode;
	}
}

//TODO: Test
Quad::Quad( const std::shared_ptr<Node>& n1,
			const std::shared_ptr<Node>& n2,
			const std::shared_ptr<Node>& n3,
			const std::shared_ptr<Node>& f )
{
	isFake = true;
	edgeList.assign( 4, nullptr );
	ang.assign( 4, 0.0 );

	edgeList[base] = std::make_shared<Edge>( n1, n2 );
	if ( edgeList[base]->leftNode == n1 )
	{
		edgeList[left] = std::make_shared<Edge>( n1, n3 );
		edgeList[right] = std::make_shared<Edge>( n2, n3 );
	}
	else
	{
		edgeList[left] = std::make_shared<Edge>( n2, n3 );
		edgeList[right] = std::make_shared<Edge>( n1, n3 );
	}
	edgeList[top] = edgeList[right];

	firstNode = f;
	updateAngles();
}

//TODO: Test
Quad::Quad( const std::shared_ptr<Node>& n1,
			const std::shared_ptr<Node>& n2,
			const std::shared_ptr<Node>& n3,
			const std::shared_ptr<Node>& n4,
			const std::shared_ptr<Node>& f )
{
	isFake = false;
	edgeList.assign( 4, nullptr );
	ang.assign( 4, 0.0 );

	edgeList[base] = std::make_shared<Edge>( n1, n2 );
	edgeList[top] = std::make_shared<Edge>( n3, n4 );
	if ( edgeList[base]->leftNode == n1 )
	{
		edgeList[left] = std::make_shared<Edge>( n1, n3 );
		edgeList[right] = std::make_shared<Edge>( n2, n4 );
	}
	else
	{
		edgeList[left] = std::make_shared<Edge>( n2, n4 );
		edgeList[right] = std::make_shared<Edge>( n1, n3 );
	}

	firstNode = f;
	updateAngles();
}

//TODO: Test
std::shared_ptr<Element>
Quad::elementWithExchangedNodes( const std::shared_ptr<Node>& original,
								 const std::shared_ptr<Node>& replacement )
{
	auto node1 = edgeList[base]->leftNode;
	auto node2 = edgeList[base]->rightNode;
	auto node3 = edgeList[left]->otherNode( edgeList[base]->leftNode );

	if ( isFake )
	{
		if ( node1 == original )
		{
			if ( original == firstNode )
			{
				return std::make_shared<Quad>( replacement, node2, node3, replacement );
			}
			else
			{
				return std::make_shared<Quad>( replacement, node2, node3, firstNode );
			}
		}
		else if ( node2 == original )
		{
			if ( original == firstNode )
			{
				return std::make_shared<Quad>( node1, replacement, node3, replacement );
			}
			else
			{
				return std::make_shared<Quad>( node1, replacement, node3, firstNode );
			}
		}
		else if ( node3 == original )
		{
			if ( original == firstNode )
			{
				return std::make_shared<Quad>( node1, node2, replacement, replacement );
			}
			else
			{
				return std::make_shared<Quad>( node1, node2, replacement, firstNode );
			}
		}
		else
		{
			return nullptr;
		}
	}

	auto node4 = edgeList[right]->otherNode( edgeList[base]->rightNode );

	if ( node1 == original )
	{
		if ( original == firstNode )
		{
			return std::make_shared<Quad>( replacement, node2, node3, node4, replacement );
		}
		else
		{
			return std::make_shared<Quad>( replacement, node2, node3, node4, firstNode );
		}
	}
	else if ( node2 == original )
	{
		if ( original == firstNode )
		{
			return std::make_shared<Quad>( node1, replacement, node3, node4, replacement );
		}
		else
		{
			return std::make_shared<Quad>( node1, replacement, node3, node4, firstNode );
		}
	}
	else if ( node3 == original )
	{
		if ( original == firstNode )
		{
			return std::make_shared<Quad>( node1, node2, replacement, node4, replacement );
		}
		else
		{
			return std::make_shared<Quad>( node1, node2, replacement, node4, firstNode );
		}
	}
	else if ( node4 == original )
	{
		if ( original == firstNode )
		{
			return std::make_shared<Quad>( node1, node2, node3, replacement, replacement );
		}
		else
		{
			return std::make_shared<Quad>( node1, node2, node3, replacement, firstNode );
		}
	}
	else
	{
		return nullptr;
	}
}

//TODO: Test
bool 
Quad::invertedWhenNodeRelocated( const std::shared_ptr<Node>& n1,
								 const std::shared_ptr<Node>& n2 )
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

//TODO: Test
bool 
Quad::anyInvertedElementsWhenCollapsed( const std::shared_ptr<Node>& n,
										const std::shared_ptr<Node>& n1,
										const std::shared_ptr<Node>& n2,
										const ArrayList<std::shared_ptr<Element>>& lK,
										const ArrayList<std::shared_ptr<Element>>& lKOpp )
{
	Msg::debug( "Entering Quad.anyInvertedElementsWhenCollapsed(..)" );
	int i;

	for ( i = 0; i < lK.size(); i++ )
	{
		const auto& elem = lK.get( i );
		if ( elem != shared_from_this() && elem->invertedWhenNodeRelocated(n1, n) )
		{
			Msg::debug( "Leaving Quad.anyInvertedElementsWhenCollapsed(..) ret: true" );
			return true;
		}
	}

	for ( i = 0; i < lKOpp.size(); i++ )
	{
		const auto& elem = lKOpp.get( i );
		if ( elem != shared_from_this() && elem->invertedWhenNodeRelocated( n2, n ) )
		{
			Msg::debug( "Leaving Quad.anyInvertedElementsWhenCollapsed(..) ret: true" );
			return true;
		}
	}

	Msg::debug( "Leaving Quad.anyInvertedElementsWhenCollapsed(..) ret: false" );
	return false;
}

//TODO: Test
bool 
Quad::equals( const std::shared_ptr<Constants>& elem ) const
{
	auto q = std::dynamic_pointer_cast<Quad>(elem);
	if ( q != nullptr )
	{
		auto node1 = edgeList[base]->leftNode;
		auto node2 = edgeList[base]->rightNode;
		auto node3 = edgeList[left]->otherNode( edgeList[base]->leftNode );
		auto node4 = edgeList[right]->otherNode( edgeList[base]->rightNode );

		auto qnode1 = q->edgeList[base]->leftNode;
		auto qnode2 = q->edgeList[base]->rightNode;
		auto qnode3 = q->edgeList[left]->otherNode( q->edgeList[base]->leftNode );
		auto qnode4 = q->edgeList[right]->otherNode( q->edgeList[base]->rightNode );

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

//TODO: Test
int 
Quad::indexOf( const std::shared_ptr<Edge>& e )
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

//TODO: Test
//TODO: Implement 
void closeQuad( const std::shared_ptr<Edge>& e1,
				const std::shared_ptr<Edge>& e2 )
{
	/*Msg::debug("Entering Quad.closeQuad(..)");
	auto nK = e1->commonNode( e2 );
	auto nKp1 = e1->otherNode( nK ), nKm1 = e2->otherNode( nK ), other = nKp1;
	bool found = false;
	ArrayList<std::shared_ptr<Edge>> addList;
	ArrayList<std::shared_ptr<Quad>> quadList;

	Msg::debug( "...nKp1: " + nKp1->descr() );
	Msg::debug( "...nKm1: " + nKm1->descr() );

	for ( int i = 0; i < nKm1->edgeList.size(); i++ )
	{
		auto eI = nKm1->edgeList.get( i );
		other = eI->otherNode( nKm1 );
		Msg::debug( "...eI== " + eI->descr() );

		for ( int j = 0; j < nKp1->edgeList.size(); j++ )
		{
			auto eJ = nKp1->edgeList.get( j );

			if ( other == eJ->otherNode( nKp1 ) )
			{
				found = true;
				Msg::debug( "...eI is connected to eJ== " + eJ->descr() );

				other->edgeList.remove( other->edgeList.indexOf( eI ) );

				if ( eI->element1 != nullptr )
				{
					if ( eI->element1->firstNode == nKm1 )
					{
						eI->element1->firstNode = nKp1; // Don't forget firstNode!!
					}
					Msg::debug( "... replacing eI with eJ in eI.element1" );

					eI->element1->replaceEdge( eI, eJ );
					eJ->connectToElement( eI->element1 );
					if ( auto q = std::dynamic_pointer_cast<Quad>(eI->element1) )
					{ // Then LR might need updating
						quadList.add( q ); // but that must be done later
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

			nKm1->edgeList.set( i, nullptr );
			eI->replaceNode( nKm1, nKp1 );
			addList.add( eI ); // nKp1.edgeList.add(eI);
		}
		else
		{
			found = false;
		}
	}

	for ( int i = 0; i < addList.size(); i++ )
	{
		const auto& e = addList.get( i );
		nKp1->edgeList.add( e );
	}

	for ( auto& q : quadList )
	{
		q->updateLR();
	}

	nKm1->edgeList.clear();

	Msg::debug( "Leaving Quad.closeQuad(..)" );*/
}

//TODO: Test
std::shared_ptr<Quad>
Quad::combine( const std::shared_ptr<Quad>& q,
			   const std::shared_ptr<Edge>& e1,
			   const std::shared_ptr<Edge>& e2 )
{
	Msg::debug( "Entering Quad.combine(Quad, ..)" );
	std::vector<std::shared_ptr<Edge>> edges( 4, nullptr );
	int i;

	edges[0] = nullptr;
	for ( i = 0; i < 4; i++ )
	{
		auto e = edgeList[i];

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
			auto e = q->edgeList[i];

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
	auto quad = std::make_shared<Quad>( edges[0], edges[1], edges[2], edges[3] );
	Msg::debug( "Leaving Quad.combine(Quad, ..)" );
	return quad;
}

//TODO: Test
std::shared_ptr<Triangle>
Quad::combine( const std::shared_ptr<Triangle>& t,
			   const std::shared_ptr<Edge>& e1,
			   const std::shared_ptr<Edge>& e2 )
{
	Msg::debug( "Entering Quad.combine(Triangle, ..)" );
	std::vector<std::shared_ptr<Edge>> edges( 3, nullptr );
	int i;

	edges[0] = nullptr;
	for ( i = 0; i < 4; i++ )
	{
		auto e = edgeList[i];

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
		auto e = t->edgeList[i];

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

	auto tri = std::make_shared<Triangle>( edges[0], edges[1], edges[2] );
	Msg::debug( "Leaving Quad.combine(Triangle t, ..)" );
	return tri;
}

//TODO: Test
std::shared_ptr<Element> 
Quad::neighbor( const std::shared_ptr<Edge>& e )
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
		Msg::warning( "Quad.neighbor(Edge): returning null" );
		return nullptr;
	}
}

//TODO: Test
std::shared_ptr<Edge>
Quad::neighborEdge( const std::shared_ptr<Node>& n,
					const std::shared_ptr<Edge>& e )
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

//TODO: Test
std::shared_ptr<Edge> 
Quad::neighborEdge( const std::shared_ptr<Node>& n ) 
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

//TODO: Implement
//TODO: Test
double
Quad::angle( const std::shared_ptr<Edge>& e,
			 const std::shared_ptr<Node>& n )
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

//TODO: Test
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

//TODO: Test
int
Quad::angleIndex( const std::shared_ptr<Edge>& e1,
				  const std::shared_ptr<Edge>& e2 )
{
	return angleIndex( indexOf( e1 ), indexOf( e2 ) );
}

//TODO: Test
int 
Quad::angleIndex( const std::shared_ptr<Node>& n )
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

//TOD: Test
double 
Quad::angle( const std::shared_ptr<Edge>& e1,
			 const std::shared_ptr<Edge>& e2 )
{
	return ang[angleIndex( indexOf( e1 ), indexOf( e2 ) )];
}

//TODO: Test
bool 
Quad::hasNode( const std::shared_ptr<Node>& n )
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

//TODO: Test
bool
Quad::hasEdge( const std::shared_ptr<Edge>& e ) 
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

//TODO: Test
bool
Quad::boundaryQuad()
{
	if ( instanceOf<Quad>( neighbor( edgeList[base] ) ) ||
		 instanceOf<Quad>( neighbor( edgeList[left] ) ) ||
		 instanceOf<Quad>( neighbor( edgeList[right] ) ) ||
		 instanceOf<Quad>( neighbor( edgeList[top] ) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//TODO: Test
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

//TODO: Test
std::shared_ptr<Node>
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

//TODO: Test
bool
Quad::areaLargerThan0()
{
	auto node3 = edgeList[top]->leftNode;
	auto node4 = edgeList[top]->rightNode;

	if ( !node3->onLine( edgeList[base] ) || !node4->onLine( edgeList[base] ) )
	{
		return true;
	}
	return true;
}

//TODO: Test
bool
Quad::isConvex()
{
	auto n1 = edgeList[base]->leftNode;
	auto n2 = edgeList[base]->rightNode;
	auto n3 = edgeList[left]->otherNode( n1 );
	auto n4 = edgeList[right]->otherNode( n2 );

	MyVector d1( n1, n4 );
	MyVector d2( n2, n3 );
	if ( d1.pointIntersects( d2 ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//TODO: Test
bool
Quad::isStrictlyConvex()
{
	auto n1 = edgeList[base]->leftNode;
	auto n2 = edgeList[base]->rightNode;
	auto n3 = edgeList[left]->otherNode( n1 );
	auto n4 = edgeList[right]->otherNode( n2 );

	MyVector d1( n1, n4 );
	MyVector d2( n2, n3 );
	if ( d1.innerpointIntersects( d2 ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//TODO: Test
bool 
Quad::isBowtie()
{
	auto n1 = edgeList[base]->leftNode;
	auto n2 = edgeList[base]->rightNode;
	auto n3 = edgeList[left]->otherNode( n1 );
	auto n4 = edgeList[right]->otherNode( n2 );

	MyVector d1( n1, n2 );
	MyVector d2( n3, n4 );
	MyVector d3( n1, n3 );
	MyVector d4( n2, n4 );
	if ( d1.pointIntersects( d2 ) || d3.pointIntersects( d4 ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//TODO: Test
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

//TODO: Test
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

//TODO: Test
std::shared_ptr<Node>
Quad::nextCCWNodeOld( const std::shared_ptr<Node>& n )
{
	std::shared_ptr<Node> n0 = nullptr, n1 = nullptr;

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

	MyVector v0( n, n0 );
	MyVector v1( n, n1 );
	if ( v1.isCWto( v0 ) )
	{
		return n1;
	}
	else
	{
		return n0;
	}
}

//TODO: Test
std::shared_ptr<Node> 
Quad::nextCCWNode( const std::shared_ptr<Node>& n )
{
	if ( n == firstNode )
	{
		return edgeList[base]->otherNode( firstNode ); // n2
	}
	else
	{
		auto n2 = edgeList[base]->otherNode( firstNode );
		auto e2 = neighborEdge( n2, edgeList[base] );
		if ( n == n2 )
		{
			return e2->otherNode( n2 ); // n3
		}
		else
		{
			auto n3 = e2->otherNode( n2 );
			auto e3 = neighborEdge( n3, e2 );
			if ( n == n3 )
			{
				return e3->otherNode( n3 ); // n4
			}
			else
			{
				auto n4 = e3->otherNode( n3 );
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
	double dt0, dt1, dt2, dt3;
	if ( !edgeList[left]->hasNode( edgeList[base]->leftNode ) )
	{
		Msg::debug( "...updating" );
		auto temp = edgeList[left];
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

//TODO: Test
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

//TODO: Test
void
Quad::updateAnglesExcept( const std::shared_ptr<Node>& n )
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
Quad::updateAngle( const std::shared_ptr<Node>& n )
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

//TODO: Test
bool
Quad::inverted()
{
	Msg::debug( "Entering Quad.inverted()" );
	if ( isFake )
	{
		auto a = firstNode;
		auto b = edgeList[base]->otherNode( a );
		std::shared_ptr<Node> c = nullptr;
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

	auto a = edgeList[base]->leftNode, b = edgeList[base]->rightNode, c = edgeList[right]->otherNode( b ), d = edgeList[left]->otherNode( a );

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

	Msg::debug( "Leaving Quad.inverted(), okays: " + okays );
	if ( okays >= 3 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

//TODO: Test
bool 
Quad::invertedOrZeroArea()
{
	Msg::debug( "Entering Quad.invertedOrZeroArea()" );
	if ( isFake )
	{
		auto a = firstNode;
		auto b = edgeList[base]->otherNode( a );
		std::shared_ptr<Node> c = nullptr;
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

	auto a = edgeList[base]->leftNode, b = edgeList[base]->rightNode, c = edgeList[right]->otherNode( b ), d = edgeList[left]->otherNode( a );

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

	Msg::debug( "Leaving Quad.invertedOrZeroArea(), okays: " + okays );
	if ( okays >= 3 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

//TODO: Test
bool 
Quad::concavityAt( const std::shared_ptr<Node>& n )
{
	Msg::debug( "Entering Quad.concavityAt(..)" );
	if ( ang[angleIndex( n )] >= PI )
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

//TODO: Test
std::shared_ptr<Node>
Quad::centroid()
{
	double x = 0, y = 0;
	auto bL = edgeList[base]->leftNode, bR = edgeList[base]->rightNode;
	auto uL = edgeList[left]->otherNode( bL ), uR = edgeList[right]->otherNode( bR );

	if ( isFake )
	{
		x = (bL->x + uL->x + bR->x) / 3.0;
		y = (bL->y + uL->y + bR->y) / 3.0;
		return std::make_shared<Node>( x, y );
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
		return std::make_shared<Node>( x, y );
	}
}

//TODO: Test
std::shared_ptr<Node> 
Quad::oppositeNode( const std::shared_ptr<Node>& n )
{
	// 2 out of 4 edges has Node n, so at least 1 out of 3 edge must have it, too:
	std::shared_ptr<Edge> startEdge;
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

	auto n2 = startEdge->otherNode( n );
	auto e = neighborEdge( n2, startEdge );
	return e->otherNode( n2 );
}

//TODO: Test
std::shared_ptr<Edge> 
Quad::cwOppositeEdge( const std::shared_ptr<Node>& n )
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

//TODO: Test
std::shared_ptr<Edge> 
Quad::ccwOppositeEdge( const std::shared_ptr<Node>& n )
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

//TODO: Test
std::shared_ptr<Edge>
Quad::oppositeEdge( const std::shared_ptr<Edge>& e )
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

//TODO: Test
bool 
Quad::invertedNeighbors()
{
	auto uLNode = edgeList[left]->otherNode( edgeList[base]->leftNode );
	auto uRNode = edgeList[right]->otherNode( edgeList[base]->rightNode );

	// Parse all adjacent elements at upper left node from neigbor of left edge to,
	// but not including, neighbor of top edge.
	auto curElem = neighbor( edgeList[left] );
	auto curEdge = edgeList[left];
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

//TODO: Test
ArrayList<std::shared_ptr<Triangle>> 
Quad::getAdjTriangles()
{
	ArrayList<std::shared_ptr<Triangle>> triangleList;
	auto uLNode = edgeList[left]->otherNode( edgeList[base]->leftNode );
	auto uRNode = edgeList[right]->otherNode( edgeList[base]->rightNode );
	auto bLNode = edgeList[base]->leftNode;
	auto bRNode = edgeList[base]->rightNode;

	triangleList = bLNode->adjTriangles();

	// Parse all adjacent elements at upper left node from, but not not including,
	// neigbor of left edge to, but not including, neighbor of top edge.
	auto curElem = neighbor( edgeList[left] );
	auto curEdge = edgeList[left];
	if ( curElem != nullptr )
	{
		curEdge = curElem->neighborEdge( uLNode, curEdge );
		curElem = curElem->neighbor( curEdge );

		while ( curElem != nullptr && curEdge != edgeList[top] )
		{
			if ( Element::instanceOf<Triangle>( curElem ) )
			{
				triangleList.add( std::dynamic_pointer_cast<Triangle>(curElem) );
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
		auto Tri = std::dynamic_pointer_cast<Triangle>(curElem);
		if ( Tri && !triangleList.contains( Tri ) )
		{
			triangleList.add( Tri );
		}
		curEdge = curElem->neighborEdge( uRNode, curEdge );
		curElem = curElem->neighbor( curEdge );
	}

	triangleList.addAll( bRNode->adjTriangles() );

	return triangleList;
}

//TODO: Test
ArrayList<std::shared_ptr<Node>>
Quad::getAdjNodes()
{
	ArrayList<std::shared_ptr<Node>> nodeList;
	std::shared_ptr<Edge> e = nullptr;
	std::shared_ptr<Node> n = nullptr;
	auto bLNode = edgeList[base]->leftNode;
	auto bRNode = edgeList[base]->rightNode;
	auto uLNode = edgeList[left]->otherNode( bLNode );

	int i;

	for ( i = 0; i < bLNode->edgeList.size(); i++ )
	{
		e = bLNode->edgeList.get( i );
		if ( e != edgeList[base] && e != edgeList[left] && e != edgeList[right] && e != edgeList[top] )
		{
			nodeList.add( e->otherNode( bLNode ) );
		}
	}

	for ( i = 0; i < bRNode->edgeList.size(); i++ )
	{
		e = bRNode->edgeList.get( i );
		if ( e != edgeList[base] && e != edgeList[left] && e != edgeList[right] && e != edgeList[top] )
		{
			n = e->otherNode( bRNode );
			if ( !nodeList.contains( n ) )
			{
				nodeList.add( n );
			}
		}
	}

	for ( i = 0; i < uLNode->edgeList.size(); i++ )
	{
		e = uLNode->edgeList.get( i );
		if ( e != edgeList[base] && e != edgeList[left] && e != edgeList[right] && e != edgeList[top] )
		{
			n = e->otherNode( uLNode );
			if ( !nodeList.contains( n ) )
			{
				nodeList.add( n );
			}
		}
	}

	if ( !isFake )
	{
		auto uRNode = edgeList[right]->otherNode( bRNode );
		for ( i = 0; i < uRNode->edgeList.size(); i++ )
		{
			e = uRNode->edgeList.get( i );
			if ( e != edgeList[base] && e != edgeList[left] && e != edgeList[right] && e != edgeList[top] )
			{
				n = e->otherNode( uRNode );
				if ( !nodeList.contains( n ) )
				{
					nodeList.add( n );
				}
			}
		}
	}
	return nodeList;
}

//TODO: Test
void 
Quad::replaceEdge( const std::shared_ptr<Edge>& e,
				   const std::shared_ptr<Edge>& replacement )
{
	edgeList[indexOf( e )] = replacement;
}

//TODO: Test
void
Quad::connectEdges()
{
	auto pThis = shared_from_this();
	edgeList[base]->connectToQuad( pThis );
	edgeList[left]->connectToQuad( pThis );
	edgeList[right]->connectToQuad( pThis );
	if ( !isFake )
	{
		edgeList[top]->connectToQuad( pThis );
	}
}

//TODO: Test
void 
Quad::disconnectEdges()
{
	auto pThis = shared_from_this();
	edgeList[base]->disconnectFromElement( pThis );
	edgeList[left]->disconnectFromElement( pThis );
	edgeList[right]->disconnectFromElement( pThis );
	if ( !isFake )
	{
		edgeList[top]->disconnectFromElement( pThis );
	}
}

//TODO: Test
std::shared_ptr<Edge> 
Quad::commonEdgeAt( const std::shared_ptr<Node>& n,
					const std::shared_ptr<Quad>& q )
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

//TODO: Test
std::shared_ptr<Edge> 
Quad::commonEdge( const std::shared_ptr<Quad>& q )
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

//TODO: Test
bool
Quad::hasFrontEdgeAt( const std::shared_ptr<Node>& n )
{
	if ( edgeList[left]->hasNode(n) )
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

//TODO: Test
int 
Quad::nrOfQuadsSharingAnEdgeAt( const std::shared_ptr<Node>& n )
{
	int count = 0;

	if ( edgeList[left]->hasNode( n ) )
	{
		if ( Element::instanceOf<Quad>( neighbor( edgeList[left] ) ) )
		{
			count++;
		}
		if ( edgeList[base]->hasNode( n ) )
		{
			if ( Element::instanceOf<Quad>( neighbor( edgeList[base] ) ) )
			{
				count++;
			}
		}
		else if ( Element::instanceOf<Quad>( neighbor( edgeList[top] ) ) )
		{
			count++;
		}
		return count;
	}
	else if ( edgeList[right]->hasNode( n ) )
	{
		if ( Element::instanceOf<Quad>( neighbor( edgeList[right] ) ) )
		{
			count++;
		}
		if ( edgeList[base]->hasNode( n ) )
		{
			if ( Element::instanceOf<Quad>( neighbor( edgeList[base] ) ) )
			{
				count++;
			}
		}
		else if ( Element::instanceOf<Quad>( neighbor( edgeList[top] ) ) )
		{
			count++;
		}

		return count;
	}
	return count;
}

//TODO: Test
void 
Quad::updateDistortionMetric()
{
	Msg::debug("Entering Quad.updateDistortionMetric()");

	if ( isFake )
	{
		double AB = edgeList[base]->len, CB = edgeList[left]->len, CA = edgeList[right]->len;

		auto a = edgeList[base]->commonNode( edgeList[right] ), b = edgeList[base]->commonNode( edgeList[left] ), c = edgeList[left]->commonNode( edgeList[right] );
		MyVector vCA( c, a ), vCB( c, b );

		double temp = sqrt3x2 * std::abs( vCA.cross( vCB ) ) / (CA * CA + AB * AB + CB * CB);
		if ( inverted() )
		{
			distortionMetric = -temp;
		}
		else
		{
			distortionMetric = temp;
		}

		Msg::debug( "Leaving Quad.updateDistortionMetric(): " + std::to_string(distortionMetric) );
		return;
	}

	auto n1 = edgeList[base]->leftNode;
	auto n2 = edgeList[base]->rightNode;
	auto n3 = edgeList[left]->otherNode( n1 );
	auto n4 = edgeList[right]->otherNode( n2 );

	// The two diagonals
	auto e1 = std::make_shared<Edge>( n1, n4 );
	auto e2 = std::make_shared<Edge>( n2, n3 );

	// The four triangles
	auto t1 = std::make_shared<Triangle>( edgeList[base], edgeList[left], e2 );
	auto t2 = std::make_shared<Triangle>( edgeList[base], e1, edgeList[right] );
	auto t3 = std::make_shared<Triangle>( edgeList[top], edgeList[right], e2 );
	auto t4 = std::make_shared<Triangle>( edgeList[top], e1, edgeList[left] );

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

//TODO: Test
bool
Quad::coincidentNodes( const std::shared_ptr<Node>& n1,
					   const std::shared_ptr<Node>& n2,
					   const std::shared_ptr<Node>& n3,
					   const std::shared_ptr<Node>& n4 )
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
	double l12 = std::sqrt( x12diff * x12diff + y12diff * y12diff );
	double l13 = std::sqrt( x13diff * x13diff + y13diff * y13diff );
	double l14 = std::sqrt( x14diff * x14diff + y14diff * y14diff );
	double l23 = std::sqrt( x23diff * x23diff + y23diff * y23diff );
	double l24 = std::sqrt( x24diff * x24diff + y24diff * y24diff );
	double l34 = std::sqrt( x34diff * x34diff + y34diff * y34diff );

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

//TODO: Test
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

//TODO: Test
std::shared_ptr<Node> 
Quad::nodeAtLargestAngle()
{
	auto candNode = edgeList[base]->leftNode;
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

//TODO: Test
double 
Quad::longestEdgeLength()
{
	double t1 = std::max( edgeList[base]->len, edgeList[left]->len );
	double t2 = std::max( t1, edgeList[right]->len );
	return std::max( t2, edgeList[top]->len );
}

//TODO: Test
ArrayList<std::shared_ptr<Triangle>>
Quad::trianglesContained( const std::shared_ptr<Triangle>& first )
{
	Msg::debug( "Entering trianglesContained(..)" );
	ArrayList<std::shared_ptr<Triangle>> tris;

	tris.add( first );
	for ( int j = 0; j < tris.size(); j++ )
	{
		auto cur = std::dynamic_pointer_cast<Triangle>( tris.get( j ) );
		Msg::debug( "...parsing triangle " + cur->descr() );

		for ( int i = 0; i < 3; i++ )
		{
			auto e = cur->edgeList[i];
			if ( !hasEdge( e ) )
			{
				auto neighbor = std::dynamic_pointer_cast<Triangle>( cur->neighbor( e ) );
				if ( neighbor != nullptr && !tris.contains( neighbor ) )
				{
					tris.add( neighbor );
				}
			}
		}
	}
	Msg::debug( "Leaving trianglesContained(..)" );
	return tris;
}

//TODO: Test
bool
Quad::containsHole( const ArrayList<std::shared_ptr<Element>>& tris )
{
	if ( tris.size() == 0 )
	{
		return true; // Corresponds to a quad defined on a quad-shaped hole
	}

	for ( const auto& element : tris )
	{
		if ( auto t = std::dynamic_pointer_cast<Triangle>(element) )
		{
			if ( t->edgeList[0]->boundaryEdge() && !hasEdge( t->edgeList[0] ) || t->edgeList[1]->boundaryEdge() && !hasEdge( t->edgeList[1] )
				 || t->edgeList[2]->boundaryEdge() && !hasEdge( t->edgeList[2] ) )
			{
				return true;
			}
		}
	}
	return false;
}

//TODO: Test
void
Quad::markEdgesLegal() 
{
	edgeList[base]->color = Color::Green;
	edgeList[left]->color = Color::Green;
	edgeList[right]->color = Color::Green;
	edgeList[top]->color = Color::Green;
}

//TODO: Test
void
Quad::markEdgesIllegal()
{
	edgeList[base]->color = Color::Red;
	edgeList[left]->color = Color::Red;
	edgeList[right]->color = Color::Red;
	edgeList[top]->color = Color::Red;
}

std::string 
Quad::descr()
{
	auto node1 = edgeList[base]->leftNode;
	auto node2 = edgeList[base]->rightNode;
	auto node3 = edgeList[left]->otherNode( node1 );
	auto node4 = edgeList[right]->otherNode( node2 );

	return node1->descr() + ", " + node2->descr() + ", " + node3->descr() + ", " + node4->descr();
}

void
Quad::printMe()
{
	std::cout << 
		descr() << ", inverted(): " << inverted() << ", ang[0]: " << std::to_string( toDegrees * ang[0] ) <<
		", ang[1]: " << std::to_string( toDegrees * ang[1] ) << ", ang[2]: " << std::to_string( toDegrees * ang[2] ) <<
		", ang[3]: " << std::to_string( toDegrees * ang[3] ) << ", firstNode is " << firstNode->descr() << "/n";
}