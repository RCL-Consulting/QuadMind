#include "pch.h"
#include "DelaunayMeshGen.h"

#include "Msg.h"
#include "Node.h"
#include "Edge.h"
#include "Triangle.h"
#include "Quad.h"
#include "MyVector.h"

void 
DelaunayMeshGen::init( bool delaunayCompliant )
{
	this->delaunayCompliant = delaunayCompliant;
	setCurMethod( this );

	// Perform the steps necessary before inserting the Nodes in incrDelaunay():
	// Create the two initial Delaunay triangles from the four most extreme Nodes.

	triangleList.clear();
	edgeList.clear();
	findExtremeNodes();

	Msg::debug( "uppermost= " + uppermost->descr() );
	Msg::debug( "lowermost= " + lowermost->descr() );
	Msg::debug( "leftmost= " + leftmost->descr() );
	Msg::debug( "rightmost= " + rightmost->descr() );

	// The boundary edges
	auto edge2 = new Edge( leftmost, uppermost );
	auto edge3 = new Edge( leftmost, lowermost );
	auto edge4 = new Edge( lowermost, rightmost );
	auto edge5 = new Edge( uppermost, rightmost );

	// * Create the two initial triangles
	// * They are made Delaunay by selecting the correct interior edge.
	Triangle* del1, *del2;
	Edge* edge1;
	if ( !rightmost->inCircle( uppermost, leftmost, lowermost ) )
	{
		edge1 = new Edge( uppermost, lowermost );
		del1 = new Triangle( edge1, edge2, edge3 );
		del2 = new Triangle( edge1, edge4, edge5 );
	}
	else
	{
		edge1 = new Edge( leftmost, rightmost );
		del1 = new Triangle( edge1, edge2, edge5 );
		del2 = new Triangle( edge1, edge3, edge4 );
	}

	edge1->connectNodes();
	edge2->connectNodes();
	edge3->connectNodes();
	edge4->connectNodes();
	edge5->connectNodes();

	del1->connectEdges();
	del2->connectEdges();

	// Update "global" edgeList, triangleList, and nodeList
	edgeList.push_back( edge1 );
	Msg::debug( "ADDING EDGE " + edge1->descr() + " to edgeList" );
	edgeList.push_back( edge2 );
	Msg::debug( "ADDING EDGE " + edge2->descr() + " to edgeList" );
	edgeList.push_back( edge3 );
	Msg::debug( "ADDING EDGE " + edge3->descr() + " to edgeList" );
	edgeList.push_back( edge4 );
	Msg::debug( "ADDING EDGE " + edge4->descr() + " to edgeList" );
	edgeList.push_back( edge5 );
	Msg::debug( "ADDING EDGE " + edge5->descr() + " to edgeList" );

	triangleList.push_back( del1 );
	triangleList.push_back( del2 );
}

void
DelaunayMeshGen::run()
{
	Msg::debug( "Entering incrDelauney(..)" );
	// Point insertions
	for ( auto n : nodeList )
	{
		// The extreme nodes have been inserted already, so we skip them here
		if ( n != leftmost && n != rightmost && n != uppermost && n != lowermost )
		{
			insertNode( n, delaunayCompliant );
		}
	}

	Msg::debug( "Leaving incrDelauney(..)" );
}

Constants*
DelaunayMeshGen::findTriangleContaining( Node* newNode, Triangle* start )
{
	Msg::debug( "Entering findTriangleContaining(" + newNode->descr() + "..)" );
	// The initial "dart" must be ccw in the initial triangle:
	Triangle* ts = start; // d_start
	Edge* es; // d_start
	Edge* e1 = ts->edgeList[0];
	Edge* e2 = ts->edgeList[1];
	Node* ns = e1->commonNode( e2 ); // d_start
	MyVector v1( *ns, *e1->otherNode( ns ) );
	MyVector v2( *ns, *e2->otherNode( ns ) );
	Edge* online = nullptr;

	if ( v1.isCWto( v2 ) )
	{
		es = e2;
	}
	else
	{
		es = e1;
	}

	Triangle* t = ts; // d_i
	Edge* e = es; // d_i
	Node* n = ns; // d_i

	int count = 0;
	int hp;
	while ( true )
	{

		Msg::debug( "newNode= " + newNode->descr() );
		//
		// Msg::debug("Loop nr. "+count++); Msg::debug("newNode= "+newNode.descr());
		// Msg::debug("Current dart has triangle= "+t.descr());
		// Msg::debug("Current dart has edge= "+e.descr());
		// Msg::debug("Current dart has node= "+n.descr());
		// Msg::debug("Stop dart has triangle= "+ts.descr());
		// Msg::debug("Stop dart has edge= "+es.descr());
		// Msg::debug("Stop dart has node= "+ns.descr());
		//

		hp = newNode->inHalfplane( t, e );
		Msg::debug( "hp: " + hp );
		if ( hp == 1 || hp == 0 )
		{ // || (hp== 0 && !newNode.inBoundedPlane(e))) {
			if ( hp == 0 )
			{
				online = e;
			}
			// is newNode in halfplane defined by (t, e)?
			Msg::debug( "in halfplane t=" + t->descr() + " e=" + e->descr() );
			n = e->otherNode( n );
			e = t->neighborEdge( n, e );
			if ( ts == t && es == e && ns == n )
			{
				inside = true;

				if ( online != nullptr )
				{
					Msg::debug( "Leaving findTriangleContaining(..), returning Edge" );
					return online;
				}
				else
				{
					Msg::debug( "Leaving findTriangleContaining(..), returning Triangle" );
					return t;
				}
			}
		}
		//
		// else if (hp==0 && newNode.inBoundedPlane(e)) { // is newNode actually on Edge
		// e? Msg::debug("Leaving findTriangleContaining(..), returning Edge"); inside=
		// true; return (Object) e; }
		//
		else
		{ // try to move to the adjacent triangle
			online = nullptr;
			Msg::debug( "*not* in halfplane t=" + t->descr() + " e=" + e->descr() );
			ts = static_cast<Triangle*>(t->neighbor( e ));
			if ( ts == nullptr )
			{
				Msg::debug( "Leaving findTriangleContaining(..), not found!" );
				inside = false;
				return e; // outside triangulation
				// }
			}
			else
			{
				t = ts; // d_start= alpha_0 o alpha_2(d_i)
				es = e;
				ns = e->otherNode( n );

				// d_i= alpha_1 o alpha_2(d_i)
				e = t->neighborEdge( n, e );
			}
		}
	}
}

void
DelaunayMeshGen::swap( Edge* e )
{
	Msg::debug( "Entering swap(..)" );
	Triangle* t1 = static_cast<Triangle*>(e->element1);
	Triangle* t2 = static_cast<Triangle*>(e->element2);

	if ( t1 == nullptr || t2 == nullptr )
	{
		Msg::debug( "Leaving recSwapDelaunay(..), this is a boundary Edge" );
		return;
	}

	Node* na, *nb, *nc, *nd;
	na = e->leftNode;
	nb = e->rightNode;
	nc = t1->oppositeOfEdge( e );
	nd = t2->oppositeOfEdge( e );

	double cross1 = cross( *nc, *na, *nd, *na ); // The cross product ca x da
	double cross2 = cross( *nc, *nb, *nd, *nb ); // The cross product cb x db

	if ( cross1 == 0 || cross2 == 0 )
	{
		// if (!q.isStrictlyConvex()) {
		Msg::debug( "Leaving swap(..), cannot create degenerate triangle" );
		return;
	}

	// Create the new Edge, do the swap
	Edge* ei = new Edge( nc, nd );
	Msg::debug( "Swapping diagonal " + e->descr() );

	e->swapToAndSetElementsFor( ei );
	auto tNew1 = static_cast<Triangle*>(ei->element1);
	auto tNew2 = static_cast<Triangle*>(ei->element2);

	// Update "global" lists: remove old triangles and edge e, add new ones
	auto ind1 = std::find( triangleList.begin(), triangleList.end(), t1 );
	if ( ind1 != triangleList.end() )
	{
		triangleList.erase( ind1 );
	}
	
	auto ind2 = std::find( triangleList.begin(), triangleList.end(), t2 );
	if ( ind2 != triangleList.end() )
	{
		triangleList.erase( ind2 );
	}

	edgeList.erase( std::find( edgeList.begin(), edgeList.end(), e ) );
	Msg::debug( "REMOVING EDGE " + e->descr() + " FROM edgeList" );
	edgeList.push_back( ei );
	Msg::debug( "ADDING EDGE " + ei->descr() + " to edgeList" );

	triangleList.push_back( tNew1 );
	triangleList.push_back( tNew2 );

	Msg::debug( "Leaving swap(..)" );
}

void
DelaunayMeshGen::recSwapDelaunay( Edge* e, Node* n )
{
	Msg::debug( "Entering recSwapDelaunay(..)" );
	Triangle* t1 = static_cast<Triangle*>(e->element1);
	Triangle* t2 = static_cast<Triangle*>(e->element2);
	Node *na, *nb, *nc, *nd;

	if ( t1 == nullptr || t2 == nullptr )
	{// Make sure we're dealing with an interior edge
		Msg::debug( "Leaving recSwapDelaunay(..), this is a boundary Edge" );
		return;
	}

	Triangle* t;
	if ( !e->element1->hasNode( n ) )
	{
		t = static_cast<Triangle*>(e->element1);
	}
	else
	{
		t = static_cast<Triangle*>(e->element2);
	}

	Node* p1, *p2, *p3, *opposite = t->oppositeOfEdge( e );
	Quad* q = new Quad( e, opposite, n );

	nc = n;
	nd = e->oppositeNode( n );
	na = e->leftNode;
	nb = e->rightNode;

	double cross1 = cross( *nc, *na, *nd, *na ); // The cross product ca x da
	double cross2 = cross( *nc, *nb, *nd, *nb ); // The cross product cb x db

	if ( cross1 == 0 || cross2 == 0 )
	{
		// if (!q.isStrictlyConvex()) {
		Msg::debug( "Leaving recSwapDelaunay(..), cannot create degenerate triangle" );
		return;
	}

	p1 = q->nextCCWNode( n );
	p2 = q->nextCCWNode( p1 );
	p3 = q->nextCCWNode( p2 );

	if ( !n->inCircle( p1, p2, p3 ) )
	{ // If n lies outside the cicrumcircle..
		Msg::debug( "Leaving recSwapDelaunay(..), n lies outside circumcircle" );
		return;
	}

	// Create the new Edge, do the swap
	Edge* ei = new Edge( p2, n );
	Msg::debug( "Swapping diagonal " + e->descr() + " of quad " + q->descr() );

	e->swapToAndSetElementsFor( ei );
	auto tNew1 = static_cast<Triangle*>(ei->element1);
	auto tNew2 = static_cast<Triangle*>(ei->element2);

	// Update "global" lists: remove old triangles and edge e, add new ones
	auto ind1 = std::find( triangleList.begin(), triangleList.end(), t1 );
	if ( ind1 != triangleList.end() )
	{
		triangleList.erase( ind1 );
	}
	
	auto ind2 = std::find( triangleList.begin(), triangleList.end(), t2 );
	if ( ind2 != triangleList.end() )
	{
		triangleList.erase( ind2 );
	}

	edgeList.erase( std::find( edgeList.begin(), edgeList.end(), e ) );
	Msg::debug( "REMOVING EDGE " + e->descr() + " FROM edgeList" );
	edgeList.push_back( ei );
	Msg::debug( "ADDING EDGE " + ei->descr() + " to edgeList" );

	triangleList.push_back( tNew1 );
	triangleList.push_back( tNew2 );

	// Proceed with recursive calls
	recSwapDelaunay( tNew1->oppositeOfNode( n ), n );
	recSwapDelaunay( tNew2->oppositeOfNode( n ), n );
	Msg::debug( "Leaving recSwapDelaunay(..)" );
}

void
DelaunayMeshGen::insertNode( Node* n, bool remainDelaunay )
{
	Triangle* t, *t1, *t2 = nullptr, *t3, *t4 = nullptr, *oldt1, *oldt2;

	Edge* e, *e1 = nullptr, *e2, *e3, *e4 = nullptr, *e12, *e22 = nullptr, *e32, *e42 = nullptr,
		*b0 = nullptr, *b1 = nullptr;
	Node* node, *nNode, *pNode, *other, *other2, *n0, *n1;
	std::vector<Edge*> boundaryEdges;
	int i, j;
	bool loop = true;

	// Locate the triangle that contains the point
	auto o = findTriangleContaining( n, triangleList.at( 0 ) );
	if ( !inside )
	{
		// --- the Node is to be inserted outside the current triangulation --- //
		e = static_cast<Edge*>(o);
		Msg::debug( "Node " + n->descr() + " is outside of the current triangulation" );
		Msg::debug( "findTriangleCont... returns " + e->descr() );

		// Compile an ordered list of boundary edges that is part of the i. polygon.
		// First find the leftmost edge of these boundary edges...
		pNode = e->rightNode;
		node = e->leftNode;

		while ( loop )
		{
			Msg::debug( "insertNode: inside 1st loop..." );
			e1 = node->anotherBoundaryEdge( e );
			Msg::debug( "e1: " + e1->descr() );
			Msg::debug( "node: " + node->descr() );

			nNode = e1->otherNode( node );
			j = nNode->inHalfplane( n, node, pNode );
			if ( j == -1 )
			{ // pNode and nNode must lie on different sides of (n, node)
				pNode = node;
				node = nNode;
				e = e1;
			}
			else
			{ // pNode and nNode must lie on different sides of (n, node)
				loop = false;
			}
		}
		Msg::debug( "insertNode: 1st loop done" );

		// ... then traverse the boundary edges towards the right, adding one
		// edge at a time until the rightmost edge is encountered
		loop = true;
		pNode = node;
		n0 = node; // Most distant node to the left
		node = e->otherNode( node );
		boundaryEdges.push_back( e );

		while ( loop )
		{
			Msg::debug( "insertNode: inside 2nd loop..." );
			e1 = node->anotherBoundaryEdge( e );

			nNode = e1->otherNode( node );
			j = nNode->inHalfplane( n, node, pNode );
			if ( j == -1 )
			{ // pNode and nNode must lie on different sides of (n, node)
				pNode = node;
				node = nNode;
				boundaryEdges.push_back( e1 );
				e = e1;
			}
			else
			{ // pNode and nNode must lie on different sides of (n, node)
				loop = false;
			}
		}
		n1 = node; // Most distant node to the right
		Msg::debug( "n0: " + n0->descr() + ", n1: " + n1->descr() );
		Msg::debug( "insertNode: 2nd loop done" );
		Msg::debug( "Nr of affected edges on the boundary is " + std::to_string( boundaryEdges.size() ) );

		// From this list, find each triangle in the influence region and delete it.
		// Also create the list of nodes in the influence region
		// makeDelaunayTriangle does the job.
		for ( i = 0; i < boundaryEdges.size(); i++ )
		{
			e = boundaryEdges.at( i );
			t = e->getTriangleElement();
			if ( t != nullptr )
			{ // (The triangle may have been deleted already)
				makeDelaunayTriangle( t, e, n );
			}
		}
		Msg::debug( "Done finding and deleting triangles in the influence region." );
		Msg::debug( "Nr of nodes in influence region is " + std::to_string( irNodes.size() ) );

		// Create new triangles by connecting the nodes in the
		// influence region to node n:

		// Build initial edgeList for node n
		for ( i = 0; i < irNodes.size(); i++ )
		{
			other = irNodes.at( i );
			e = new Edge( n, other );
			e->connectNodes();
			edgeList.push_back( e );
			if ( other == n0 )
			{
				b0 = e;
			}
			else if ( other == n1 )
			{
				b1 = e;
			}
		}
		Msg::debug( "n.edgeList.size(): " + n->edgeList.size() );

		// Sort this list of edges in ccw order
		n->edgeList = n->calcCCWSortedEdgeList( b0, b1 );
		Msg::debug( "n.edgeList.size(): " + std::to_string( n->edgeList.size() ) );

		// Create each triangle
		printEdgeList( n->edgeList );
		for ( i = 0; i < n->edgeList.size() - 1; i++ )
		{
			e1 = n->edgeList.at( i );
			e2 = n->edgeList.at( i + 1 );
			other = e1->otherNode( n );
			other2 = e2->otherNode( n );
			e = new Edge( other, other2 );
			auto j = std::find( edgeList.begin(), edgeList.end(), e );
			if ( j != edgeList.end() )
			{
				e = *j;
			}
			else
			{
				e->connectNodes();
				edgeList.push_back( e );
			}

			t = new Triangle( e, e1, e2 );
			triangleList.push_back( t );
			Msg::debug( "Creating new triangle: " + t->descr() );
			t->connectEdges();
		}
		irNodes.clear(); // NB! IMPORTANT!
	}
	else if ( t = dynamic_cast<Triangle*>(o) )
	{
		// --- the Node is to be inserted inside the current triangulation --- //
		Msg::debug( "findTriangleCont... returns " + t->descr() );

		// Make some pointers for its edges
		Edge* te1 = t->edgeList[1], *te2 = t->edgeList[2], *te3 = t->edgeList[0];

		// Split the triangle into three new triangles with n as a common Node.
		e1 = new Edge( n, te3->commonNode( te1 ) );
		e2 = new Edge( n, te1->commonNode( te2 ) );
		e3 = new Edge( n, te2->commonNode( te3 ) );

		e1->connectNodes();
		e2->connectNodes();
		e3->connectNodes();

		edgeList.push_back( e1 );
		Msg::debug( "ADDING EDGE " + e1->descr() + " to edgeList" );
		edgeList.push_back( e2 );
		Msg::debug( "ADDING EDGE " + e2->descr() + " to edgeList" );
		edgeList.push_back( e3 );
		Msg::debug( "ADDING EDGE " + e3->descr() + " to edgeList" );

		t1 = new Triangle( e1, e2, te1 ); // This should be correct...
		t2 = new Triangle( e2, e3, te2 ); //
		t3 = new Triangle( e3, e1, te3 ); //

		// Disconnect Edges from old Triangle & connect Edges to new triangles.
		t->disconnectEdges();

		t1->connectEdges();
		t2->connectEdges();
		t3->connectEdges();

		// Update triangleList.
		triangleList.erase( std::find( triangleList.begin(), triangleList.end(), t ) );
		triangleList.push_back( t1 );
		triangleList.push_back( t2 );
		triangleList.push_back( t3 );

		// Swap edges so that the new triangulation becomes Delauney
		if ( remainDelaunay )
		{
			recSwapDelaunay( te1, n );
			recSwapDelaunay( te2, n );
			recSwapDelaunay( te3, n );
		}
		else
		{
			if ( !t1->areaLargerThan0() )
			{
				swap( te1 );
			}
			if ( !t2->areaLargerThan0() )
			{
				swap( te2 );
			}
			if ( !t3->areaLargerThan0() )
			{
				swap( te3 );
			}
		}
	}
	else if ( e = static_cast<Edge*>(o) )
	{ 
		// n lies on Edge e:
		// Split the (1 or) 2 Triangles adjacent Edge e into (2 or) 4 new Triangles.

		Msg::debug( "findTriangleCont... returns " + e->descr() );

		oldt1 = static_cast<Triangle*>(e->element1);
		oldt2 = static_cast<Triangle*>(e->element2);

		// Create the (2 or) 4 new Edges, get ptrs for the (2 or) 4 outer Edges,
		// remove the old Edge e.
		e1 = new Edge( e->leftNode, n );
		e2 = new Edge( e->rightNode, n );
		e12 = oldt1->neighborEdge( e->leftNode, e );
		e3 = new Edge( n, e12->otherNode( e->leftNode ) );
		e32 = oldt1->neighborEdge( e->rightNode, e );

		e1->connectNodes();
		e2->connectNodes();
		e3->connectNodes();
		edgeList.push_back( e1 );

		Msg::debug( "ADDING EDGE " + e1->descr() + " to edgeList" );
		edgeList.push_back( e2 );
		Msg::debug( "ADDING EDGE " + e2->descr() + " to edgeList" );
		edgeList.push_back( e3 );
		Msg::debug( "ADDING EDGE " + e3->descr() + " to edgeList" );

		if ( oldt2 != nullptr )
		{
			e22 = oldt2->neighborEdge( e->leftNode, e );
			e4 = new Edge( n, e22->otherNode( e->leftNode ) );
			e4->connectNodes();
			e42 = oldt2->neighborEdge( e->rightNode, e );

			edgeList.push_back( e4 );
			Msg::debug( "ADDING EDGE " + e4->descr() + " to edgeList" );
		}

		e->disconnectNodes();
		edgeList.erase( std::find( edgeList.begin(), edgeList.end(), e ) );
		Msg::debug( "REMOVING EDGE " + e->descr() + " FROM edgeList" );

		// Create the (2 or) 4 new triangles
		t1 = new Triangle( e1, e12, e3 ); // This should be correct...
		t3 = new Triangle( e2, e3, e32 );
		if ( oldt2 != nullptr )
		{
			t2 = new Triangle( e1, e22, e4 );
			t4 = new Triangle( e2, e4, e42 );
		}

		// Disconnect Edges from old Triangles & connect Edges to new triangles.
		oldt1->disconnectEdges();
		if ( oldt2 != nullptr )
		{
			oldt2->disconnectEdges();
		}

		t1->connectEdges();

		t3->connectEdges();
		if ( oldt2 != nullptr )
		{
			t2->connectEdges();
			t4->connectEdges();
		}

		// Update triangleList.
		triangleList.erase( std::find( triangleList.begin(), triangleList.end(), oldt1 ) );
		triangleList.push_back( t1 );
		triangleList.push_back( t3 );
		if ( oldt2 != nullptr )
		{
			triangleList.erase( std::find( triangleList.begin(), triangleList.end(), oldt2 ) );
			triangleList.push_back( t2 );
			triangleList.push_back( t4 );
		}

		// Swap edges so that the new triangulation becomes Delauney
		if ( remainDelaunay )
		{
			recSwapDelaunay( e12, n );
			recSwapDelaunay( e32, n );
			if ( oldt2 != nullptr )
			{
				recSwapDelaunay( e22, n );
				recSwapDelaunay( e42, n );
			}
		}
		else
		{
			if ( !t1->areaLargerThan0() )
			{
				swap( e12 );
			}
			if ( !t3->areaLargerThan0() )
			{
				swap( e32 );
			}
			if ( oldt2 != nullptr )
			{
				if ( !t2->areaLargerThan0() )
				{
					swap( e22 );
				}
				if ( !t4->areaLargerThan0() )
				{
					swap( e42 );
				}
			}
		}
	}
}

void 
DelaunayMeshGen::makeDelaunayTriangle( Triangle* t, Edge* e, Node* n )
{
	Msg::debug( "Entering makeDelaunayTriangle(..)" );
	Msg::debug( "checking triangle t= " + t->descr() );
	
	Edge* e1, *e2;
	Triangle* t1, *t2;
	Node* p1, *p2, *p3, *opposite = t->oppositeOfEdge( e );
	auto q = new Quad( e, opposite, n );

	if ( !q->isStrictlyConvex() )
	{
		Msg::debug( "Leaving makeDelaunayTriangle(..): non-convex quad" );

		auto j = std::find( irNodes.begin(), irNodes.end(), e->leftNode );
		if ( j == irNodes.end() )
		{
			irNodes.push_back( e->leftNode );
		}

		j = std::find( irNodes.begin(), irNodes.end(), e->rightNode );
		if ( j == irNodes.end() )
		{
			irNodes.push_back( e->rightNode );
		}

		return;
	}

	p1 = q->nextCCWNode( n );
	p2 = q->nextCCWNode( p1 );
	p3 = q->nextCCWNode( p2 );

	Msg::debug( "n: " + n->descr() );
	Msg::debug( "p1: " + p1->descr() );
	Msg::debug( "p2: " + p2->descr() );
	Msg::debug( "p3: " + p3->descr() );

	if ( n->inCircle( p1, p2, p3 ) )
	{
		{
			auto j = std::find( irNodes.begin(), irNodes.end(), p1 );
			if ( j == irNodes.end() )
			{
				irNodes.push_back( p1 );
			}

			j = std::find( irNodes.begin(), irNodes.end(), p2 );
			if ( j == irNodes.end() )
			{
				irNodes.push_back( p2 );
			}

			j = std::find( irNodes.begin(), irNodes.end(), p3 );
			if ( j == irNodes.end() )
			{
				irNodes.push_back( p3 );
			}
		}

		e1 = t->otherEdge( e );
		t1 = static_cast<Triangle*>(t->neighbor( e1 ));
		e2 = t->otherEdge( e, e1 );
		t2 = static_cast<Triangle*>(t->neighbor( e2 ));

		e->disconnectNodes();
		auto j = std::find( edgeList.begin(), edgeList.end(), e );
		if ( j != edgeList.end() )
		{
			edgeList.erase( j );
		}

		if ( t1 == nullptr )
		{
			e1->disconnectNodes();
			j = std::find( edgeList.begin(), edgeList.end(), e1 );
			if ( j != edgeList.end() )
			{
				edgeList.erase( j );
			}
		}
		if ( t2 == nullptr )
		{
			e2->disconnectNodes();
			j = std::find( edgeList.begin(), edgeList.end(), e2 );
			if ( j != edgeList.end() )
			{
				edgeList.erase( j );
			}
		}

		t->disconnectEdges();
		triangleList.erase( std::find( triangleList.begin(), triangleList.end(), t ) );

		if ( t1 != nullptr )
		{
			makeDelaunayTriangle( t1, e1, n );
		}
		if ( t2 != nullptr )
		{
			makeDelaunayTriangle( t2, e2, n );
		}

		Msg::debug( "Leaving makeDelaunayTriangle(..)... done!" );
	}
	else
	{
		auto j = std::find( irNodes.begin(), irNodes.end(), e->leftNode );
		if ( j == irNodes.end() )
		{
			irNodes.push_back( e->leftNode );
		}

		j = std::find( irNodes.begin(), irNodes.end(), e->rightNode );
		if ( j == irNodes.end() )
		{
			irNodes.push_back( e->rightNode );
		}

		Msg::debug( "Leaving makeDelaunayTriangle(..), n lies outside circumcircle" );
	}
}