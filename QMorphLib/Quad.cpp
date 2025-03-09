#include "pch.h"
#include "Quad.h"

#include "Edge.h"
#include "Triangle.h"
#include "Node.h"

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
	/*Node* node1 = edgeList[base]->leftNode;
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
	}*/

	return nullptr;
}