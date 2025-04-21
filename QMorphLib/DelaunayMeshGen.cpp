#include "pch.h"
#include "DelaunayMeshGen.h"

#include "MyVector.h"
#include "Quad.h"
#include "Triangle.h"

#include "Msg.h"
#include "Types.h"

//TODO: Tests
void
DelaunayMeshGen::init( bool delaunayCompliant )
{
	this->delaunayCompliant = delaunayCompliant;
	setCurMethod( shared_from_this() );

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
	auto edge2 = std::make_shared<Edge>( leftmost, uppermost );
	auto edge3 = std::make_shared<Edge>( leftmost, lowermost );
	auto edge4 = std::make_shared<Edge>( lowermost, rightmost );
	auto edge5 = std::make_shared<Edge>( uppermost, rightmost );

	// * Create the two initial triangles
	// * They are made Delaunay by selecting the correct interior edge.
	std::shared_ptr<Triangle> del1, del2;
	std::shared_ptr<Edge> edge1;
	if ( !rightmost->inCircle( uppermost, leftmost, lowermost ) )
	{
		edge1 = std::make_shared<Edge>( uppermost, lowermost );
		del1 = std::make_shared<Triangle>( edge1, edge2, edge3 );
		del2 = std::make_shared<Triangle>( edge1, edge4, edge5 );
	}
	else
	{
		edge1 = std::make_shared<Edge>( leftmost, rightmost );
		del1 = std::make_shared<Triangle>( edge1, edge2, edge5 );
		del2 = std::make_shared<Triangle>( edge1, edge3, edge4 );
	}

	edge1->connectNodes();
	edge2->connectNodes();
	edge3->connectNodes();
	edge4->connectNodes();
	edge5->connectNodes();

	del1->connectEdges();
	del2->connectEdges();

	// Update "global" edgeList, triangleList, and nodeList
	edgeList.add( edge1 );
	Msg::debug( "ADDING EDGE " + edge1->descr() + " to edgeList" );
	edgeList.add( edge2 );
	Msg::debug( "ADDING EDGE " + edge2->descr() + " to edgeList" );
	edgeList.add( edge3 );
	Msg::debug( "ADDING EDGE " + edge3->descr() + " to edgeList" );
	edgeList.add( edge4 );
	Msg::debug( "ADDING EDGE " + edge4->descr() + " to edgeList" );
	edgeList.add( edge5 );
	Msg::debug( "ADDING EDGE " + edge5->descr() + " to edgeList" );

	triangleList.add( del1 );
	triangleList.add( del2 );
}

//TODO: Tests
void
DelaunayMeshGen::run()
{
	Msg::debug( "Entering incrDelauney(..)" );
	// Point insertions

	for ( auto n: nodeList )
	{
		// The extreme nodes have been inserted already, so we skip them here
		if ( n != leftmost && n != rightmost && n != uppermost && n != lowermost )
		{
			insertNode( n, delaunayCompliant );
		}
	}

	Msg::debug( "Leaving incrDelauney(..)" );
}

//TODO: Tests
void
DelaunayMeshGen::step()
{
	if ( counter < nodeList.size() )
	{
		auto n = nodeList.get( counter );
		counter++;
		// The extreme nodes have been inserted already, so we skip them here
		while ( n == leftmost || n == rightmost || n == uppermost || n == lowermost )
		{
			if ( counter < nodeList.size() )
			{
				n = nodeList.get( counter );
				counter++;
			}
			else if ( counter == nodeList.size() )
			{
				counter++;
				return;
			}
			else
			{
				return;
			}
		}
		if ( n != leftmost && n != rightmost && n != uppermost && n != lowermost )
		{
			insertNode( n, delaunayCompliant );
		}
	}
	else if ( counter == nodeList.size() )
	{
		counter++;
	}
}

//TODO: Tests
std::shared_ptr<Constants>
DelaunayMeshGen::findTriangleContaining( const std::shared_ptr<Node>& newNode,
										 const std::shared_ptr<Triangle>& start )
{
	Msg::debug( "Entering findTriangleContaining(" + newNode->descr() + "..)" );
	// The initial "dart" must be ccw in the initial triangle:
	auto ts = start; // d_start
	std::shared_ptr<Edge> es; // d_start
	auto e1 = ts->edgeList[0];
	auto e2 = ts->edgeList[1];
	auto ns = e1->commonNode( e2 ); // d_start
	MyVector v1( ns, e1->otherNode( ns ) );
	MyVector v2( ns, e2->otherNode( ns ) );
	std::shared_ptr<Edge> online = nullptr;

	if ( v1.isCWto( v2 ) )
	{
		es = e2;
	}
	else
	{
		es = e1;
	}

	auto t = ts; // d_i
	auto e = es; // d_i
	auto n = ns; // d_i

	int count = 0;
	int hp;
	while ( true )
	{

		Msg::debug( "newNode= " + newNode->descr() );
		/*
		 * Msg.debug("Loop nr. "+count++); Msg.debug("newNode= "+newNode.descr());
		 * Msg.debug("Current dart has triangle= "+t.descr());
		 * Msg.debug("Current dart has edge= "+e.descr());
		 * Msg.debug("Current dart has node= "+n.descr());
		 * Msg.debug("Stop dart has triangle= "+ts.descr());
		 * Msg.debug("Stop dart has edge= "+es.descr());
		 * Msg.debug("Stop dart has node= "+ns.descr());
		 */

		hp = newNode->inHalfplane( t, e );
		Msg::debug( "hp: " + std::to_string( hp ) );
		if ( hp == 1 || hp == 0 )
		{ 
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
		/*
		 * else if (hp==0 && newNode.inBoundedPlane(e)) { // is newNode actually on Edge
		 * e? Msg.debug("Leaving findTriangleContaining(..), returning Edge"); inside=
		 * true; return (Object) e; }
		 */
		else
		{ // try to move to the adjacent triangle
			online = nullptr;
			Msg::debug( "*not* in halfplane t=" + t->descr() + " e=" + e->descr() );
			ts = std::dynamic_pointer_cast<Triangle>(t->neighbor( e ));
			if ( ts == nullptr )
			{
				/*
				 * if (hp== 0) { e= ; t= ; es= ; ts= ; } else {
				 */
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

//TODO: Tests
void
DelaunayMeshGen::swap( std::shared_ptr<Edge>& e )
{
	Msg::debug( "Entering swap(..)" );
	auto t1 = std::dynamic_pointer_cast<Triangle>(e->element1);
	auto t2 = std::dynamic_pointer_cast<Triangle>(e->element2);

	if ( t1 == nullptr || t2 == nullptr )
	{
		Msg::debug( "Leaving recSwapDelaunay(..), this is a boundary Edge" );
		return;
	}

	auto na = e->leftNode;
	auto nb = e->rightNode;
	auto nc = t1->oppositeOfEdge( e );
	auto nd = t2->oppositeOfEdge( e );

	double cross1 = cross( nc, na, nd, na ); // The cross product ca x da
	double cross2 = cross( nc, nb, nd, nb ); // The cross product cb x db

	if ( cross1 == 0 || cross2 == 0 )
	{
		Msg::debug( "Leaving swap(..), cannot create degenerate triangle" );
		return;
	}

	// Create the new Edge, do the swap
	auto ei = std::make_shared<Edge>( nc, nd );
	Msg::debug( "Swapping diagonal " + e->descr() );

	e->swapToAndSetElementsFor( ei );
	auto tNew1 = std::dynamic_pointer_cast<Triangle>(ei->element1);
	auto tNew2 = std::dynamic_pointer_cast<Triangle>(ei->element2);

	// Update "global" lists: remove old triangles and edge e, add new ones
	auto ind1 = triangleList.indexOf( t1 );
	if ( ind1 != -1 )
	{
		triangleList.remove( ind1 );
	}
	auto ind2 = triangleList.indexOf( t2 );
	if ( ind2 != -1 )
	{
		triangleList.remove( ind2 );
	}

	edgeList.remove( edgeList.indexOf( e ) );
	Msg::debug( "REMOVING EDGE " + e->descr() + " FROM edgeList" );
	edgeList.add( ei );
	Msg::debug( "ADDING EDGE " + ei->descr() + " to edgeList" );

	triangleList.add( tNew1 );
	triangleList.add( tNew2 );

	Msg::debug( "Leaving swap(..)" );
}

//TODO: Tests
void
DelaunayMeshGen::recSwapDelaunay( const std::shared_ptr<Edge>& e,
								  const std::shared_ptr<Node>& n )
{
	Msg::debug( "Entering recSwapDelaunay(..)" );
	auto t1 = std::dynamic_pointer_cast<Triangle>(e->element1);
	auto t2 = std::dynamic_pointer_cast<Triangle>(e->element2);

	if ( t1 == nullptr || t2 == nullptr )
	{// Make sure we're dealing with an interior edge
		Msg::debug( "Leaving recSwapDelaunay(..), this is a boundary Edge" );
		return;
	}

	std::shared_ptr<Triangle> t;
	if ( !e->element1->hasNode( n ) )
	{
		t = std::dynamic_pointer_cast<Triangle>(e->element1);
	}
	else
	{
		t = std::dynamic_pointer_cast<Triangle>(e->element2);
	}

	std::shared_ptr<Node> p1, p2, p3, opposite = t->oppositeOfEdge( e );
	auto q = std::make_shared<Quad>( e, opposite, n );

	auto nc = n;
	auto nd = e->oppositeNode( n );
	auto na = e->leftNode;
	auto nb = e->rightNode;

	double cross1 = cross( nc, na, nd, na ); // The cross product ca x da
	double cross2 = cross( nc, nb, nd, nb ); // The cross product cb x db

	if ( cross1 == 0 || cross2 == 0 )
	{
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
	auto ei = std::make_shared<Edge>( p2, n );
	Msg::debug( "Swapping diagonal " + e->descr() + " of quad " + q->descr() );

	e->swapToAndSetElementsFor( ei );
	auto tNew1 = std::dynamic_pointer_cast<Triangle>(ei->element1);
	auto tNew2 = std::dynamic_pointer_cast<Triangle>(ei->element2);

	// Update "global" lists: remove old triangles and edge e, add new ones
	auto ind1 = triangleList.indexOf( t1 );
	if ( ind1 != -1 )
	{
		triangleList.remove( ind1 );
	}
	auto ind2 = triangleList.indexOf( t2 );
	if ( ind2 != -1 )
	{
		triangleList.remove( ind2 );
	}

	edgeList.remove( edgeList.indexOf( e ) );
	Msg::debug( "REMOVING EDGE " + e->descr() + " FROM edgeList" );
	edgeList.add( ei );
	Msg::debug( "ADDING EDGE " + ei->descr() + " to edgeList" );

	triangleList.add( tNew1 );
	triangleList.add( tNew2 );

	// Proceed with recursive calls
	recSwapDelaunay( tNew1->oppositeOfNode( n ), n );
	recSwapDelaunay( tNew2->oppositeOfNode( n ), n );
	Msg::debug( "Leaving recSwapDelaunay(..)" );
}

//TODO: Tests
void
DelaunayMeshGen::makeDelaunayTriangle( const std::shared_ptr<Triangle>& t,
									   const std::shared_ptr<Edge>& e,
									   const std::shared_ptr<Node>& n )
{
	Msg::debug( "Entering makeDelaunayTriangle(..)" );
	Msg::debug( "checking triangle t= " + t->descr() );
	std::shared_ptr<Edge> e1, e2;
	std::shared_ptr<Triangle> t1, t2;
	std::shared_ptr<Node> p1, p2, p3, opposite = t->oppositeOfEdge( e );
	auto q = std::make_shared<Quad>( e, opposite, n );

	if ( !q->isStrictlyConvex() )
	{
		Msg::debug( "Leaving makeDelaunayTriangle(..): non-convex quad" );
		auto j = irNodes.indexOf( e->leftNode );
		if ( j == -1 )
		{
			irNodes.add( e->leftNode );
		}
		j = irNodes.indexOf( e->rightNode );
		if ( j == -1 )
		{
			irNodes.add( e->rightNode );
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
		auto j = irNodes.indexOf( p1 );
		if ( j == -1 )
		{
			irNodes.add( p1 );
		}
		j = irNodes.indexOf( p2 );
		if ( j == -1 )
		{
			irNodes.add( p2 );
		}
		j = irNodes.indexOf( p3 );
		if ( j == -1 )
		{
			irNodes.add( p3 );
		}

		e1 = t->otherEdge( e );
		t1 = std::dynamic_pointer_cast<Triangle>(t->neighbor( e1 ));
		e2 = t->otherEdge( e, e1 );
		t2 = std::dynamic_pointer_cast<Triangle>(t->neighbor( e2 ));

		e->disconnectNodes();
		j = edgeList.indexOf( e );
		if ( j != -1 )
		{
			edgeList.remove( j );
		}

		if ( t1 == nullptr )
		{
			e1->disconnectNodes();
			j = edgeList.indexOf( e1 );
			if ( j != -1 )
			{
				edgeList.remove( j );
			}
		}
		if ( t2 == nullptr )
		{
			e2->disconnectNodes();
			j = edgeList.indexOf( e2 );
			if ( j != -1 )
			{
				edgeList.remove( j );
			}
		}

		t->disconnectEdges();
		triangleList.remove( triangleList.indexOf( t ) );

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
		auto j = irNodes.indexOf( e->leftNode );
		if ( j == -1 )
		{
			irNodes.add( e->leftNode );
		}
		j = irNodes.indexOf( e->rightNode );
		if ( j == -1 )
		{
			irNodes.add( e->rightNode );
		}

		Msg::debug( "Leaving makeDelaunayTriangle(..), n lies outside circumcircle" );
	}
}

//TODO: Tests
void
DelaunayMeshGen::insertNode( const std::shared_ptr<Node>& n,
							 bool remainDelaunay )
{
	std::shared_ptr<Triangle> t1, t2 = nullptr, t3, t4 = nullptr, oldt1, oldt2;

	std::shared_ptr<Edge> e, e1 = nullptr, e2, e3, e4 = nullptr, e12, e22 = nullptr, e32, e42 = nullptr, eOld, b0 = nullptr, b1 = nullptr;
	std::shared_ptr<Node> node, nNode, pNode, other, other2, n0, n1;
	ArrayList<std::shared_ptr<Edge>> boundaryEdges;
	int i;
	bool loop = true;

	// Locate the triangle that contains the point
	auto o = findTriangleContaining( n, triangleList.get( 0 ) );
	if ( !inside )
	{
		// --- the Node is to be inserted outside the current triangulation --- //
		e = std::dynamic_pointer_cast<Edge>(o);
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
			auto j = nNode->inHalfplane( n, node, pNode );
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
		boundaryEdges.add( e );

		while ( loop )
		{
			Msg::debug( "insertNode: inside 2nd loop..." );
			e1 = node->anotherBoundaryEdge( e );

			nNode = e1->otherNode( node );
			auto j = nNode->inHalfplane( n, node, pNode );
			if ( j == -1 )
			{ // pNode and nNode must lie on different sides of (n, node)
				pNode = node;
				node = nNode;
				boundaryEdges.add( e1 );
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
			e = boundaryEdges.get( i );
			auto t = e->getTriangleElement();
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
			other = irNodes.get( i );
			e = std::make_shared<Edge>( n, other );
			e->connectNodes();
			edgeList.add( e );
			if ( other == n0 )
			{
				b0 = e;
			}
			else if ( other == n1 )
			{
				b1 = e;
			}
		}
		Msg::debug( "n.edgeList.size(): " + std::to_string( n->edgeList.size() ) );

		// Sort this list of edges in ccw order
		n->edgeList = n->calcCCWSortedEdgeList( b0, b1 );
		Msg::debug( "n.edgeList.size(): " + std::to_string( n->edgeList.size() ) );

		// Create each triangle
		printEdgeList( n->edgeList );
		for ( i = 0; i < n->edgeList.size() - 1; i++ )
		{
			e1 = n->edgeList.get( i );
			e2 = n->edgeList.get( i + 1 );
			other = e1->otherNode( n );
			other2 = e2->otherNode( n );
			e = std::make_shared<Edge>( other, other2 );
			auto j = other->edgeList.indexOf( e );
			if ( j != -1 )
			{
				e = other->edgeList.get( j );
			}
			else
			{
				e->connectNodes();
				edgeList.add( e );
			}

			auto t = std::make_shared<Triangle>( e, e1, e2 );
			triangleList.add( t );
			Msg::debug( "Creating new triangle: " + t->descr() );
			t->connectEdges();
		}
		irNodes.clear(); // NB! IMPORTANT!
	}
	else if ( auto t = std::dynamic_pointer_cast<Triangle>(o) )
	{
		// --- the Node is to be inserted inside the current triangulation --- //
		Msg::debug( "findTriangleCont... returns " + t->descr() );

		// Make some pointers for its edges
		auto te1 = t->edgeList[1], te2 = t->edgeList[2], te3 = t->edgeList[0];

		// Split the triangle into three new triangles with n as a common Node.
		e1 = std::make_shared<Edge>( n, te3->commonNode( te1 ) );
		e2 = std::make_shared<Edge>( n, te1->commonNode( te2 ) );
		e3 = std::make_shared<Edge>( n, te2->commonNode( te3 ) );

		e1->connectNodes();
		e2->connectNodes();
		e3->connectNodes();

		edgeList.add( e1 );
		Msg::debug( "ADDING EDGE " + e1->descr() + " to edgeList" );
		edgeList.add( e2 );
		Msg::debug( "ADDING EDGE " + e2->descr() + " to edgeList" );
		edgeList.add( e3 );
		Msg::debug( "ADDING EDGE " + e3->descr() + " to edgeList" );

		t1 = std::make_shared<Triangle>( e1, e2, te1 ); // This should be correct...
		t2 = std::make_shared<Triangle>( e2, e3, te2 ); //
		t3 = std::make_shared<Triangle>( e3, e1, te3 ); //

		// Disconnect Edges from old Triangle & connect Edges to new triangles.
		t->disconnectEdges();

		t1->connectEdges();
		t2->connectEdges();
		t3->connectEdges();

		// Update triangleList.
		triangleList.remove( triangleList.indexOf( t ) );
		triangleList.add( t1 );
		triangleList.add( t2 );
		triangleList.add( t3 );

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
	else if ( auto e = std::dynamic_pointer_cast<Edge>(o) )
	{ // n lies on Edge e:
// Split the (1 or) 2 Triangles adjacent Edge e into (2 or) 4 new Triangles.

		Msg::debug( "findTriangleCont... returns " + e->descr() );

		oldt1 = std::dynamic_pointer_cast<Triangle>(e->element1);
		oldt2 = std::dynamic_pointer_cast<Triangle>(e->element2);

		// Create the (2 or) 4 new Edges, get ptrs for the (2 or) 4 outer Edges,
		// remove the old Edge e.
		e1 = std::make_shared<Edge>( e->leftNode, n );
		e2 = std::make_shared<Edge>( e->rightNode, n );
		e12 = oldt1->neighborEdge( e->leftNode, e );
		e3 = std::make_shared<Edge>( n, e12->otherNode( e->leftNode ) );
		e32 = oldt1->neighborEdge( e->rightNode, e );

		e1->connectNodes();
		e2->connectNodes();
		e3->connectNodes();
		edgeList.add( e1 );

		Msg::debug( "ADDING EDGE " + e1->descr() + " to edgeList" );
		edgeList.add( e2 );
		Msg::debug( "ADDING EDGE " + e2->descr() + " to edgeList" );
		edgeList.add( e3 );
		Msg::debug( "ADDING EDGE " + e3->descr() + " to edgeList" );

		if ( oldt2 != nullptr )
		{
			e22 = oldt2->neighborEdge( e->leftNode, e );
			e4 = std::make_shared<Edge>( n, e22->otherNode( e->leftNode ) );
			e4->connectNodes();
			e42 = oldt2->neighborEdge( e->rightNode, e );

			edgeList.add( e4 );
			Msg::debug( "ADDING EDGE " + e4->descr() + " to edgeList" );
		}

		e->disconnectNodes();
		edgeList.remove( edgeList.indexOf( e ) );
		Msg::debug( "REMOVING EDGE " + e->descr() + " FROM edgeList" );

		// Create the (2 or) 4 new triangles
		t1 = std::make_shared<Triangle>( e1, e12, e3 ); // This should be correct...
		t3 = std::make_shared<Triangle>( e2, e3, e32 );
		if ( oldt2 != nullptr )
		{
			t2 = std::make_shared<Triangle>( e1, e22, e4 );
			t4 = std::make_shared<Triangle>( e2, e4, e42 );
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
		triangleList.remove( triangleList.indexOf( oldt1 ) );
		triangleList.add( t1 );
		triangleList.add( t3 );
		if ( oldt2 != nullptr )
		{
			triangleList.remove( triangleList.indexOf( oldt2 ) );
			triangleList.add( t2 );
			triangleList.add( t4 );
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