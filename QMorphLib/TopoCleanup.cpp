#include "pch.h"
#include "TopoCleanup.h"

#include "Quad.h"
#include "Edge.h"
#include "Node.h"
#include "Triangle.h"
#include "Msg.h"
#include "GlobalSmooth.h"

#include <array>

void
TopoCleanup::init()
{
	setCurMethod( this );

	elimChevsFinished = false;
	connCleanupFinished = false;
	boundaryCleanupFinished = false;
	shapeCleanupFinished = false;

	bcaseTriQFin = false;
	bcaseValPat1Fin = false;
	bcaseValPat2Fin = false;
	bcaseValPat3Fin = false;
	bcaseValPat4Fin = false;
	bcaseDiamondFin = false;

	shape1stTypeFin = false;
	passNum = 0;
	count = 0;

	// Ok... Remove fake quads and replace with triangles:
	for ( int i = 0; i < elementList.size(); i++ )
	{
		if ( elementList.at( i )->IsAQuad() )
		{
			auto q = static_cast<Quad*>(elementList.at( i ));
			if ( q->isFake )
			{
				auto tri = new Triangle( q->edgeList[base], q->edgeList[left], q->edgeList[right] );
				elementList[i] = tri;
				q->disconnectEdges();
				tri->connectEdges();
			}
		}
	}

	deleteList.clear();
	nodes = nodeList;
	d = new Dart();
}

void
TopoCleanup::run()
{
	Msg::debug( "Entering TopoCleanup.run()" );

	int j;

	if ( !m_step )
	{
		// Initial pass to eliminate chevrons
		while ( !elimChevsFinished )
		{
			elimChevsStep();
		}

		// Then the major cleanup processes:
		for ( j = 0; j < 3; j++ )
		{

			// Perform connectivity cleanup:
			// Parse the list of nodes looking for cases that match. Fix these.
			connCleanupFinished = false;
			while ( !connCleanupFinished )
			{
				connCleanupStep();
			}
			// Run some kind of global smooth
			globalSmooth();

			// Boundary cleanup:
			// Parse the list of elements looking for cases that match. Fix these.
			boundaryCleanupFinished = false;
			bcaseValPat1Fin = false;
			bcaseValPat2Fin = false;
			bcaseValPat3Fin = false;
			bcaseValPat4Fin = false;
			bcaseTriQFin = false;
			bcaseDiamondFin = false;
			while ( !boundaryCleanupFinished )
			{
				boundaryCleanupStep();
			}
			// Run some kind of global smooth
			globalSmooth();

			// Shape cleanup:
			// Parse the list of elements looking for cases that match. Fix these.
			// Run a local smooth after each action.
			shapeCleanupFinished = false;
			shape1stTypeFin = false;
			while ( !shapeCleanupFinished )
			{
				shapeCleanupStep();
			}

			// Run some kind of global smooth
			globalSmooth();
		}
	}

	if ( doSmooth )
	{
		setCurMethod( GeomBasics::globalSmooth );
	}
	else
	{
		setCurMethod( nullptr );
	}
	Msg::debug( "Leaving TopoCleanup.run()" );
}

void 
TopoCleanup::step()
{
	Msg::debug( "Entering TopoCleanup.step()" );

	if ( !elimChevsFinished )
	{
		elimChevsStep();
	}
	else if ( !connCleanupFinished )
	{
		connCleanupStep();
		if ( connCleanupFinished )
		{
			globalSmooth();
		}
	}
	else if ( !boundaryCleanupFinished )
	{
		boundaryCleanupStep();
		if ( boundaryCleanupFinished )
		{
			globalSmooth();
		}
	}
	else if ( !shapeCleanupFinished )
	{
		shapeCleanupStep();
		if ( shapeCleanupFinished )
		{
			globalSmooth();
		}
	}
	else
	{
		passNum++;
		if ( passNum == 3 )
		{
			passNum = 0;
			if ( doSmooth )
			{
				setCurMethod( GeomBasics::globalSmooth );
			}
			else
			{
				setCurMethod( nullptr );
			}
		}
		else
		{
			connCleanupFinished = false;
			boundaryCleanupFinished = false;
			bcaseValPat1Fin = false;
			bcaseValPat2Fin = false;
			bcaseValPat3Fin = false;
			bcaseValPat4Fin = false;
			bcaseTriQFin = false;
			bcaseDiamondFin = false;
			shapeCleanupFinished = false;
			shape1stTypeFin = false;
		}
	}
	Msg::debug( "Leaving TopoCleanup.step()" );
}

void 
TopoCleanup::elimChevsStep()
{
	Msg::debug( "Entering TopoCleanup.elimChevsStep()" );

	Element* elem;
	int i;

	while ( count < elementList.size() )
	{
		elem = elementList.at( count );

		if ( elem == nullptr || !(elem->IsAQuad()) )
		{
			count++;
		}
		else
		{
			Msg::debug( "...testing element " + elem->descr() );

			auto q = static_cast<Quad*>(elem);
			if ( q->isFake )
			{
				Msg::error( "...Fake quad encountered!!!" );
			}

			if ( q->isChevron() )
			{
				eliminateChevron( q );
				count++;
				Msg::debug( "Leaving TopoCleanup.elimChevsStep()" );
				return;
			}
			else
			{
				count++;
			}
		}
	}

	for ( i = 0; i < deleteList.size(); i++ )
	{
		elem = deleteList.at( i );
		elementList.erase( std::find( elementList.begin(), elementList.end(), elem ) );
	}
	deleteList.clear();

	elimChevsFinished = true;
	count = 0;
	Msg::debug( "Leaving TopoCleanup.elimChevsStep(), all elements ok." );
}

void 
TopoCleanup::eliminateChevron( Quad* q )
{
	Msg::debug( "Entering eliminateChevron(..)" );
	Msg::debug( "...q== " + q->descr() );

	Element* neighbor1, *neighbor2;
	Quad *q1, *q2;
	Edge *e1, *e2, *e3, *e4;
	Node *n, *n1, *n2, *n3;

	int j;
	std::array<int, 6> valenceAlt1;
	std::array<int, 6> valenceAlt2;
	int irrAlt1 = 0, irrAlt2 = 0, badAlt1 = 0, badAlt2 = 0;

	n = q->nodeAtLargestAngle();
	Msg::debug( "...n== " + n->descr() );
	Msg::debug( "...size of angle at node n is: " + std::to_string( toDegrees * q->ang[q->angleIndex( n )] ) + " degrees." );
	e3 = q->neighborEdge( n );
	n3 = e3->otherNode( n );
	e4 = q->neighborEdge( n, e3 );
	n1 = e4->otherNode( n );

	e2 = q->neighborEdge( n3, e3 );
	n2 = e2->otherNode( n3 );
	e1 = q->neighborEdge( n2, e2 );

	neighbor1 = q->neighbor( e1 );
	neighbor2 = q->neighbor( e2 );

	if ( !n->boundaryNode() )
	{
		// Then the node can be relocated so that the element is no longer a chevron
		Msg::debug( "...trying to resolve chevron by smoothing..." );
		Node* nOld = new Node( n->x, n->y ), *nNew = n->laplacianSmooth();

		if ( !n->equals( nNew ) )
		{
			n->moveTo( *nNew );
			inversionCheckAndRepair( n, nOld );
			if ( !q->isChevron() )
			{
				n->update();
				Msg::debug( "...success! Chevron resolved by smoothing!!!!" );
				return;
			}
			else
			{
				n->setXY( nOld->x, nOld->y );
				n->update();
				Msg::debug( "...unsuccessful! Chevron not resolved by smoothing!" );
			}
		}
	}

	q1 = dynamic_cast<Quad*>( neighbor1 );
	if ( q1 && q1->largestAngleGT180() )
	{
		q1 = nullptr;
	}

	q2 = dynamic_cast<Quad*>(neighbor2);
	if ( q2 && q2->largestAngleGT180() )
	{
		q2 = nullptr;
	}

	if ( !q1 && !q2 )
	{
		Msg::debug( "...n - node at largest angle of quad " + q->descr() + " is: " + n->descr() );

		if ( q->ang[q->angleIndex( n1 )] + q1->ang[q1->angleIndex( n1 )] < DEG_180 )
		{
			valenceAlt1[0] = n->valence() + 1;
			valenceAlt1[1] = n1->valence() - 1;
			valenceAlt1[2] = n2->valence();
			valenceAlt1[3] = n3->valence();
			valenceAlt1[4] = q1->oppositeNode( n2 )->valence() + 1;
			valenceAlt1[5] = q1->oppositeNode( n1 )->valence();

			irrAlt1 = 1; // the new central node will have valence 3
		}
		else
		{ // then we must consider a fill_4(q, e1, n1):
			valenceAlt1[0] = n->valence() + 1;
			valenceAlt1[1] = n1->valence();
			valenceAlt1[2] = n2->valence();
			valenceAlt1[3] = n3->valence();
			valenceAlt1[4] = q1->oppositeNode( n2 )->valence();
			valenceAlt1[5] = q1->oppositeNode( n1 )->valence() + 1;

			irrAlt1 = 2; // the two new central nodes will each have valence 3
		}

		if ( q->ang[q->angleIndex( n3 )] + q2->ang[q2->angleIndex( n3 )] < DEG_180 )
		{
			valenceAlt2[0] = n->valence() + 1;
			valenceAlt2[1] = n1->valence();
			valenceAlt2[2] = n2->valence();
			valenceAlt2[3] = n3->valence() - 1;
			valenceAlt2[4] = q2->oppositeNode( n2 )->valence() + 1;
			valenceAlt2[5] = q2->oppositeNode( n3 )->valence();

			irrAlt2 = 1; // the new central node will have valence 3
		}
		else
		{ // then we must consider a fill_4(q, e2, n3):
			valenceAlt2[0] = n->valence() + 1;
			valenceAlt2[1] = n1->valence();
			valenceAlt2[2] = n2->valence();
			valenceAlt2[3] = n3->valence();
			valenceAlt2[4] = q2->oppositeNode( n2 )->valence();
			valenceAlt2[5] = q2->oppositeNode( n3 )->valence() + 1;

			irrAlt2 = 2; // the two new central nodes will each have valence 3
		}

		for ( j = 0; j <= 5; j++ )
		{
			if ( valenceAlt1[j] != 4 )
			{
				irrAlt1++;
			}
			if ( valenceAlt1[j] < 3 || valenceAlt1[j] > 5 )
			{
				badAlt1++;
			}

			if ( valenceAlt2[j] != 4 )
			{
				irrAlt2++;
			}
			if ( valenceAlt2[j] < 3 || valenceAlt2[j] > 5 )
			{
				badAlt2++;
			}
		}
	}

	if ( (q1 != nullptr && q2 == nullptr) || (q1 != nullptr && q2 != nullptr && (badAlt1 < badAlt2 || (badAlt1 == badAlt2 && irrAlt1 <= irrAlt2))) )
	{

		Msg::debug( "...alt1 preferred, q1: " + q1->descr() );
		deleteList.push_back( nullptr );
		auto iter = std::find( elementList.begin(), elementList.end(), q );
		*iter = nullptr;
		deleteList.push_back( nullptr );
		iter = std::find( elementList.begin(), elementList.end(), q1 );
		*iter = nullptr;
		
		if ( q->ang[q->angleIndex( n1 )] + q1->ang[q1->angleIndex( n1 )] < DEG_180 )
		{
			fill3( q, e1, n2, true );
		}
		else
		{
			fill4( q, e1, n2 ); // n1
		}

	}
	else if ( q2 != nullptr )
	{
		Msg::debug( "...alt2 preferred, q2: " + q2->descr() );
		deleteList.push_back( nullptr ); // q don't need any special treatment;
		auto iter = std::find( elementList.begin(), elementList.end(), q );
		*iter = nullptr;
		deleteList.push_back( nullptr ); // but q2 does, because it migth be at a later pos
		iter = std::find( elementList.begin(), elementList.end(), q2 );
		*iter = nullptr;

		if ( q->ang[q->angleIndex( n3 )] + q2->ang[q2->angleIndex( n3 )] < DEG_180 )
		{
			fill3( q, e2, n2, true );
		}
		else
		{
			fill4( q, e2, n2 ); // n3
		}
	}
	else
	{
		Msg::debug( "...Both q1 and q2 are null" );
	}

	Msg::debug( "Leaving eliminateChevron(..)" );
}

Dart* 
TopoCleanup::fill3( Quad* q, Edge* e, Node* n, bool safe )
{
	Msg::debug( "Entering TopoCleanup.fill3(..)" );
	Msg::debug( "...q= " + q->descr() + ", e= " + e->descr() + ", n= " + n->descr() );

	auto qn = static_cast<Quad*>(q->neighbor( e ));
	Quad *qn1, *qn2, *qn3;
	Edge *b, *l, *r, *t, *ea, *eb, *ec;
	Node* nOpp = q->oppositeNode( n );
	Edge* e2 = q->neighborEdge( n, e );
	Node* eother = e->otherNode( n ), *e2other = e2->otherNode( n );
	Dart* d = new Dart();

	Node* newNode;
	if ( safe )
	{
		newNode = e->midPoint();
	}
	else
	{
		newNode = q->centroid();
	}

	newNode->color = 3;// java.awt.Color.red; // creation in tCleanup
	ea = new Edge( nOpp, newNode );
	eb = new Edge( n, newNode );
	ec = new Edge( newNode, qn->oppositeNode( n ) );

	// Some minor updating...
	q->disconnectEdges();
	qn->disconnectEdges();

	ea->connectNodes();
	eb->connectNodes();
	ec->connectNodes();

	b = q->neighborEdge( e2other, e2 );
	if ( nOpp == b->rightNode )
	{
		l = q->neighborEdge( b->leftNode, b );
		r = ea;
	}
	else
	{
		l = ea;
		r = q->neighborEdge( b->rightNode, b );
	}
	t = eb;
	qn1 = new Quad( b, l, r, t ); // 1st replacement quad

	b = eb;
	if ( ec->hasNode( b->rightNode ) )
	{ // b.leftNode== n
		l = qn->neighborEdge( n, e );
		r = ec;
	}
	else
	{
		r = qn->neighborEdge( n, e );
		l = ec;
	}
	t = qn->oppositeEdge( e );
	qn2 = new Quad( b, l, r, t ); // 2nd replacement quad

	b = q->neighborEdge( eother, e );
	if ( b->leftNode == nOpp )
	{
		l = ea;
		r = qn->neighborEdge( eother, e );
	}
	else
	{
		r = ea;
		l = qn->neighborEdge( eother, e );
	}
	t = ec;
	qn3 = new Quad( b, l, r, t ); // 3rd replacement quad

	// remember to update the lists (nodeList, edgeList,
	// elementList, the nodes' edgeLists, ...
	qn1->connectEdges();
	qn2->connectEdges();
	qn3->connectEdges();

	elementList.push_back( qn1 );
	elementList.push_back( qn2 );
	elementList.push_back( qn3 );

	e->disconnectNodes();
	edgeList.erase( std::find( edgeList.begin(), edgeList.end(), e ) );

	edgeList.push_back( ea );
	edgeList.push_back( eb );
	edgeList.push_back( ec );

	nodeList.push_back( newNode );
	nodes.push_back( newNode );

	Msg::debug( "...qn1: " + qn1->descr() );
	Msg::debug( "...qn2: " + qn2->descr() );
	Msg::debug( "...qn3: " + qn3->descr() );

	// Try smoothing the pos of newNode:
	Node* nOld = new Node( newNode->x, newNode->y ), *smoothed = newNode->laplacianSmooth();
	if ( !newNode->equals( smoothed ) )
	{
		newNode->moveTo( *smoothed );
		inversionCheckAndRepair( newNode, nOld );
		newNode->update();
	}

	d->elem = qn1;
	d->e = eb;
	d->n = n;

	Msg::debug( "Leaving TopoCleanup.fill3(..)" );
	return d;
}

Dart* 
TopoCleanup::fill4( Quad* q, Edge* e, Node* n2 )
{
	Msg::debug( "Entering TopoCleanup.fill4(..)" );
	Msg::debug( "...q= " + q->descr() + ", e= " + e->descr() + ", n= " + n2->descr() );

	Dart* d = new Dart();
	auto qn = static_cast<Quad*>(q->neighbor( e ));
	// First get the nodes and edges in the two quads
	Node* n5 = e->otherNode( n2 );

	Edge* e1 = q->neighborEdge( n5, e );
	Node* n0 = e1->otherNode( n5 );
	Edge* e2 = q->neighborEdge( n0, e1 );
	Node* n1 = e2->otherNode( n0 );

	Edge* e3 = q->neighborEdge( n1, e2 );
	Edge* e4 = qn->neighborEdge( n2, e );
	Node* n3 = e4->otherNode( n2 );
	Edge* e5 = qn->neighborEdge( n3, e4 );
	Edge* e6 = qn->neighborEdge( n5, e );
	Node* n4 = e6->otherNode( n5 );

	// Create new nodes and edges
	Node* n6 = e->midPoint();
	// This is new... hope it works...
	Node* n7 = qn->centroid(); // new Node((n5.x + n3.x)*0.5, (n5.y + n3.y)*0.5);

	n6->color = 3;// java.awt.Color.red; // creation in tCleanup
	n7->color = 3;// java.awt.Color.red; // creation in tCleanup

	Edge* eNew1 = new Edge( n0, n6 );
	Edge* eNew2 = new Edge( n2, n6 );
	Edge* eNew3 = new Edge( n6, n7 );
	Edge* eNew4 = new Edge( n5, n7 );
	Edge* eNew5 = new Edge( n3, n7 );

	eNew1->connectNodes();
	eNew2->connectNodes();
	eNew3->connectNodes();
	eNew4->connectNodes();
	eNew5->connectNodes();

	// Create the new quads
	Edge *l, *r;
	if ( eNew1->leftNode == n0 )
	{
		l = e2;
		r = eNew2;
	}
	else
	{
		r = e2;
		l = eNew2;
	}
	auto qNew1 = new Quad( eNew1, l, r, e3 );

	if ( e1->leftNode == n5 )
	{
		l = eNew4;
		r = eNew1;
	}
	else
	{
		r = eNew4;
		l = eNew1;
	}
	auto qNew2 = new Quad( e1, l, r, eNew3 );

	if ( e4->leftNode == n3 )
	{
		l = eNew5;
		r = eNew2;
	}
	else
	{
		r = eNew5;
		l = eNew2;
	}
	auto qNew3 = new Quad( e4, l, r, eNew3 );

	if ( e6->leftNode == n4 )
	{
		l = e5;
		r = eNew4;
	}
	else
	{
		r = e5;
		l = eNew4;
	}
	auto qNew4 = new Quad( e6, l, r, eNew5 );

	// Update lists etc.
	e->disconnectNodes();
	q->disconnectEdges();
	qn->disconnectEdges();

	qNew1->connectEdges();
	qNew2->connectEdges();
	qNew3->connectEdges();
	qNew4->connectEdges();

	edgeList.erase( std::find( edgeList.begin(), edgeList.end(), e ) );
	edgeList.push_back( eNew1 );
	edgeList.push_back( eNew2 );
	edgeList.push_back( eNew3 );
	edgeList.push_back( eNew4 );
	edgeList.push_back( eNew5 );

	elementList.push_back( qNew1 );
	elementList.push_back( qNew2 );
	elementList.push_back( qNew3 );
	elementList.push_back( qNew4 );

	nodeList.push_back( n6 );
	nodeList.push_back( n7 );
	nodes.push_back( n6 );
	nodes.push_back( n7 );

	Node* nOld = new Node( n6->x, n6->y ), *nNew = n6->laplacianSmooth();
	if ( !n6->equals( nNew ) )
	{
		n6->moveTo( *nNew );
		inversionCheckAndRepair( n6, nOld );
		n6->update();
	}

	nOld = new Node( n7->x, n7->y );
	nNew = n7->laplacianSmooth();
	if ( !n7->equals( nNew ) )
	{
		n7->moveTo( *nNew );
		inversionCheckAndRepair( n7, nOld );
		n7->update();
	}
	d->elem = qNew1;
	d->e = eNew2;
	d->n = n2;

	Msg::debug( "Leaving TopoCleanup.fill4(..)" );
	return d;
}

void 
TopoCleanup::applyComposition( Dart* startDart, const std::vector<uint8_t>& fillPat )
{
	Msg::debug( "Entering applyComposition(..)" );
	uint8_t a;
	d = startDart;
	Msg::debug( "d== " + startDart->descr() );

	for ( int i = 1; i < fillPat[0]; i++ )
	{
		a = fillPat[i];

		Msg::debug( "a: " + a );

		// Alpha iterators:
		if ( a == 0 )
		{ // alpha 0
			d->n = d->e->otherNode( d->n );
		}
		else if ( a == 1 )
		{ // alpha 1
			d->e = d->elem->neighborEdge( d->n, d->e );
		}
		else if ( a == 2 )
		{ // alpha 2
			d->elem = d->elem->neighbor( d->e );
		}
		else if ( a == 3 )
		{
			d = closeQuad( static_cast<Quad*>(d->elem), d->e, d->n, false );
		}
		else if ( a == 4 )
		{
			d = closeQuad( static_cast<Quad*>(d->elem), d->e, d->n, true );
		}
		else if ( a == 5 )
		{
			auto qaIndex = std::find( elementList.begin(), elementList.end(), d->elem );
			elementList.erase( qaIndex );
			auto qbIndex = std::find( elementList.begin(), elementList.end(), d->elem->neighbor( d->e ) );
			elementList.erase( qbIndex );
			d = fill3( static_cast<Quad*>(d->elem), d->e, d->n, true );
		}
		else if ( a == 6 )
		{
			auto qaIndex = std::find( elementList.begin(), elementList.end(), d->elem );
			elementList.erase( qaIndex );
			auto qbIndex = std::find( elementList.begin(), elementList.end(), d->elem->neighbor( d->e ) );
			elementList.erase( qbIndex );
			d = fill4( static_cast<Quad*>(d->elem), d->e, d->n );
		}
		else if ( a == 7 )
		{
			d = openQuad( static_cast<Quad*>(d->elem), d->e, d->n );
		}
		else if ( a == 8 )
		{
			d = switchDiagonalCW( static_cast<Quad*>(d->elem), d->e, d->n );
		}
		else if ( a == 9 )
		{
			d = switchDiagonalCCW( static_cast<Quad*>(d->elem), d->e, d->n );
		}
		else
		{
			Msg::error( "Illegal mesh modification code: " + a );
		}

		if ( d == nullptr )
		{
			Msg::debug( "Leaving applyComposition(..), unsuccessful!" );
			return;
		}

		if ( d->elem == nullptr )
		{
			Msg::error( "d.elem== null" );
		}
		if ( d->e == nullptr )
		{
			Msg::error( "d.e== null" );
		}
		if ( d->n == nullptr )
		{
			Msg::error( "d.n== null" );
		}

		Msg::debug( "d== " + d->descr() );
	}
	Msg::debug( "Leaving applyComposition(..)" );
}

void 
TopoCleanup::connCleanupStep()
{
	Msg::debug( "Entering TopoCleanup.connCleanupStep()" );
	int i, vInd;
	Node* c = nullptr;
	Dart* d;
	std::vector<uint8_t> pattern;
	std::vector<Node*> ccwNeighbors;
	std::vector<double> angles;

	// First check for the standard patterns:
	for ( i = 0; i < nodes.size(); i++ )
	{
		c = nodes.at( i );
		if ( c == nullptr || c->boundaryOrTriangleNode() )
		{
			continue;
		}
		ccwNeighbors = c->ccwSortedNeighbors();
		c->createValencePattern( ccwNeighbors );
		if ( c->irregNeighborNodes() <= 2 )
		{
			continue;
		}

		Msg::debug( "The valence of node " + c->descr() + " is " + c->valDescr() );
		angles = c->surroundingAngles( ccwNeighbors, c->pattern[0] - 2 );

		if ( (vInd = c->patternMatch( stdCase1, stdVertexCase1, angles )) != -1 )
		{
			Msg::debug( "connCleanupStep(): matching stdCase1" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			applyComposition( d, stdComp1 );

			auto iter = std::find( nodes.begin(), nodes.end(), c );
			if ( iter != nodes.end() )
			{
				nodes.erase( iter );
			}
			addNodes( ccwNeighbors, c->pattern[0] - 2 );
		}
		else if ( (vInd = c->patternMatch( stdCase2a, stdVertexCase2a, angles )) != -1 )
		{
			Msg::debug( "connCleanupStep(): matching stdCase2a" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			applyComposition( d, stdComp2a );

			auto iter = std::find( nodes.begin(), nodes.end(), c );
			if ( iter != nodes.end() )
			{
				nodes.erase( iter );
			}
			addNodes( ccwNeighbors, c->pattern[0] - 2 );
		}
		else if ( (vInd = c->patternMatch( stdCase2b, stdVertexCase2b, angles )) != -1 )
		{
			Msg::debug( "connCleanupStep(): matching stdCase2b" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			applyComposition( d, stdComp2b );

			auto iter = std::find( nodes.begin(), nodes.end(), c );
			if ( iter != nodes.end() )
			{
				nodes.erase( iter );
			}
			addNodes( ccwNeighbors, c->pattern[0] - 2 );
		}
		else if ( (vInd = c->patternMatch( stdCase3a, stdVertexCase3a, angles )) != -1 )
		{
			Msg::debug( "connCleanupStep(): matching stdCase3a" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			applyComposition( d, stdComp3a );

			auto iter = std::find( nodes.begin(), nodes.end(), c );
			if ( iter != nodes.end() )
			{
				nodes.erase( iter );
			}
			addNodes( ccwNeighbors, c->pattern[0] - 2 );
		}
		else if ( (vInd = c->patternMatch( stdCase3b, stdVertexCase3b, angles )) != -1 )
		{
			Msg::debug( "connCleanupStep(): matching stdCase3b" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			applyComposition( d, stdComp3b );

			auto iter = std::find( nodes.begin(), nodes.end(), c );
			if ( iter != nodes.end() )
			{
				nodes.erase( iter );
			}
			addNodes( ccwNeighbors, c->pattern[0] - 2 );
		}
	}

	// Then check for the other patterns:
	for ( i = 0; i < nodes.size(); i++ )
	{
		c = nodes.at( i );
		if ( c == nullptr || c->boundaryOrTriangleNode() )
		{
			continue;
		}
		ccwNeighbors = c->ccwSortedNeighbors();
		c->createValencePattern( ccwNeighbors );
		if ( c->irregNeighborNodes() <= 2 )
		{
			continue;
		}

		Msg::debug( "The valence of node " + c->descr() + " is " + c->valDescr() );
		angles = c->surroundingAngles( ccwNeighbors, c->pattern[0] - 2 );

		if ( (vInd = c->patternMatch( case1a )) != -1 )
		{
			Msg::debug( "connCleanupStep(): matching case1a" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			if ( d != nullptr )
			{
				applyComposition( d, comp1a );
				auto iter = std::find( nodes.begin(), nodes.end(), c );
				if ( iter != nodes.end() )
				{
					nodes.erase( iter );
				}
				addNodes( ccwNeighbors, c->pattern[0] - 2 );
			}
		}
		else if ( (vInd = c->patternMatch( case1b )) != -1 )
		{
			Msg::debug( "connCleanupStep(): matching case1b" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			if ( d != nullptr )
			{
				applyComposition( d, comp1b );
				auto iter = std::find( nodes.begin(), nodes.end(), c );
				if ( iter != nodes.end() )
				{
					nodes.erase( iter );
				}
				addNodes( ccwNeighbors, c->pattern[0] - 2 );
			}
		}
		else if ( (vInd = c->patternMatch( case2 )) != -1 )
		{
			Msg::debug( "connCleanupStep(): matching case2" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			if ( d != nullptr )
			{
				applyComposition( d, comp2 );
				auto iter = std::find( nodes.begin(), nodes.end(), c );
				if ( iter != nodes.end() )
				{
					nodes.erase( iter );
				}
				addNodes( ccwNeighbors, c->pattern[0] - 2 );
			}
		}
		else if ( (vInd = c->patternMatch( case3, vertexCase3, angles )) != -1 && internalNodes( ipat3, ccwNeighbors, vInd - 2, c->pattern[0] - 2 ) )
		{
			Msg::debug( "connCleanupStep(): matching case3" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			if ( d != nullptr )
			{
				applyComposition( d, comp3 );
				auto iter = std::find( nodes.begin(), nodes.end(), c );
				if ( iter != nodes.end() )
				{
					nodes.erase( iter );
				}
				addNodes( ccwNeighbors, c->pattern[0] - 2 );
			}
		}
		else if ( (vInd = c->patternMatch( case4 )) != -1 && internalNodes( ipat4, ccwNeighbors, vInd - 2, c->pattern[0] - 2 ) )
		{
			Msg::debug( "connCleanupStep(): matching case4" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			if ( d != nullptr )
			{
				applyComposition( d, comp4 );
				auto iter = std::find( nodes.begin(), nodes.end(), c );
				if ( iter != nodes.end() )
				{
					nodes.erase( iter );
				}
				addNodes( ccwNeighbors, c->pattern[0] - 2 );
			}
		}
		else if ( (vInd = c->patternMatch( case5 )) != -1 && internalNodes( ipat5, ccwNeighbors, vInd - 2, c->pattern[0] - 2 ) )
		{
			Msg::debug( "connCleanupStep(): matching case5" );
			d = getDartAt( c, ccwNeighbors, vInd - 2 );
			if ( d != nullptr )
			{
				applyComposition( d, comp5 );
				auto iter = std::find( nodes.begin(), nodes.end(), c );
				if ( iter != nodes.end() )
				{
					nodes.erase( iter );
				}
				addNodes( ccwNeighbors, c->pattern[0] - 2 );
			}
		}
	}

	nodes = nodeList;
	connCleanupFinished = true;
	count = 0;
	Msg::debug( "Leaving TopoCleanup.connCleanupStep(), all is well" );
}

bool
TopoCleanup::internalNodes( const std::vector<bool>& ipat,
							const std::vector<Node*>& ccwNodes,
							int index, int len )
{
	Msg::debug( "Entering internalNodes(..)" );
	int i = index, j = 0;
	while ( j < len )
	{
		if ( ipat[j] && ccwNodes[i]->boundaryNode() )
		{
			Msg::debug( "Leaving internalNodes(..): false" );
			return false;
		}
		j++;
		i++;
		if ( i == len )
		{
			i = 0;
		}
	}
	Msg::debug( "Leaving internalNodes(..): true" );
	return true;
}

void
TopoCleanup::addNodes( const std::vector<Node*>& arrayOfNodes, 
					   int len )
{
	Node* n;
	
	for ( int i = 0; i < len; i++ )
	{
		n = arrayOfNodes[i];
		if ( n != nullptr )
		{
			auto j = std::find( nodes.begin(), nodes.end(), n );
			if ( j == nodes.end() )
			{
				j = std::find( nodeList.begin(), nodeList.end(), n );
				if ( j != nodeList.end() )
				{
					nodes.push_back( n );
				}
			}
		}
	}
}

void
TopoCleanup::boundaryCleanupStep()
{
	Msg::debug( "Entering TopoCleanup.boundaryCleanupStep()" );
	int i, index;
	Element* elem;
	Triangle* tri;
	Quad* q = nullptr, *pq = nullptr;
	Node *n0, *n1 = nullptr, *n2, *n3;
	Edge *e1 = nullptr, *e2 = nullptr, *ep = nullptr, *e;
	std::vector<Node*> ccwNeighbors;
	Dart* d = nullptr;

	if ( !bcaseValPat1Fin )
	{

		for ( i = 0; i < nodes.size(); i++ )
		{
			n1 = nodes.at( i );
			if ( n1 == nullptr )
			{
				continue;
			}
			ccwNeighbors = n1->ccwSortedNeighbors();
			if ( ccwNeighbors.size() == 0 )
			{
				continue;
			}
			n1->createValencePattern( static_cast<uint8_t>(n1->edgeList.size() * 2 - 1), ccwNeighbors );

			if ( n1->boundaryPatternMatch( bcase1a, bpat1a, ccwNeighbors ) )
			{
				Msg::debug( "boundaryCleanupStep(): matching bcase1a, bpat1a" );
				d = getDartAt( n1, ccwNeighbors, 0 );
				applyComposition( d, bcomp1a );

				auto j = std::find( nodes.begin(), nodes.end(), n1 );
				if ( j != nodes.end() )
				{
					nodes.erase( j );
				}
				addNodes( ccwNeighbors, n1->pattern[0] - 2 );
			}
			else if ( n1->boundaryPatternMatch( bcase1b, bpat1b, ccwNeighbors ) )
			{
				Msg::debug( "boundaryCleanupStep(): matching bcase1b, bpat1b" );
				d = getDartAt( n1, ccwNeighbors, 0 );
				applyComposition( d, bcomp1b );

				auto j = std::find( nodes.begin(), nodes.end(), n1 );
				if ( j != nodes.end() )
				{
					nodes.erase( j );
				}
				addNodes( ccwNeighbors, n1->pattern[0] - 2 );
			}
		}
		// nodes= (ArrayList)nodeList.clone();
		bcaseValPat1Fin = true;
		Msg::debug( "Leaving boundaryCleanupStep(): Done looking for case 1s." );
		return;
	}
	else if ( !bcaseValPat2Fin )
	{

		for ( i = 0; i < nodes.size(); i++ )
		{
			n1 = nodes.at( i );
			if ( n1 == nullptr )
			{
				continue;
			}
			ccwNeighbors = n1->ccwSortedNeighbors();
			if ( ccwNeighbors.size() == 0 )
			{
				continue;
			}
			n1->createValencePattern( static_cast<uint8_t>(n1->edgeList.size() * 2 - 1), ccwNeighbors );

			if ( n1->boundaryPatternMatch( bcase2a, bpat2a, ccwNeighbors ) )
			{
				Msg::debug( "boundaryCleanupStep(): matching bcase2a, bpat2a" );
				d = getDartAt( n1, ccwNeighbors, 0 );
				applyComposition( d, bcomp2a );

				auto j = std::find( nodes.begin(), nodes.end(), n1 );
				if ( j != nodes.end() )
				{
					nodes.erase( j );
				}
				addNodes( ccwNeighbors, n1->pattern[0] - 2 );
			}
			else if ( n1->boundaryPatternMatch( bcase2b, bpat2b, ccwNeighbors ) )
			{
				Msg::debug( "boundaryCleanupStep(): matching bcase2b, bpat2b" );
				d = getDartAt( n1, ccwNeighbors, 0 );
				applyComposition( d, bcomp2b );

				auto j = std::find( nodes.begin(), nodes.end(), n1 );
				if ( j != nodes.end() )
				{
					nodes.erase( j );
				}
				addNodes( ccwNeighbors, n1->pattern[0] - 2 );
			}
		}
		// nodes= (ArrayList)nodeList.clone();
		bcaseValPat2Fin = true;
		Msg::debug( "Leaving boundaryCleanupStep(): Done looking for case 2s." );
		return;
	}
	else if ( !bcaseValPat3Fin )
	{

		for ( i = 0; i < nodes.size(); i++ )
		{
			n1 = nodes.at( i );
			if ( n1 == nullptr )
			{
				continue;
			}
			Msg::debug( "...testing node " + n1->descr() );
			if ( n1->boundaryNode() )
			{
				continue;
			}

			ccwNeighbors = n1->ccwSortedNeighbors();
			if ( ccwNeighbors.size() == 0 )
			{
				continue;
			}
			n1->createValencePattern( static_cast<uint8_t>(n1->edgeList.size() * 2), ccwNeighbors );

			if ( (index = n1->boundaryPatternMatchSpecial( bcase3, bpat3, ccwNeighbors )) != -1 )
			{
				Msg::debug( "boundaryCleanupStep(): matching bcase3, bpat3" );
				d = getDartAt( n1, ccwNeighbors, index - 2 );
				applyComposition( d, bcomp3 );

				auto j = std::find( nodes.begin(), nodes.end(), n1 );
				if ( j != nodes.end() )
				{
					nodes.erase( j );
				}
				addNodes( ccwNeighbors, n1->pattern[0] - 2 );
			}
		}
		// nodes= (ArrayList)nodeList.clone();
		bcaseValPat3Fin = true;
		Msg::debug( "Leaving boundaryCleanupStep(): Done looking for case 3s." );
		return;
	}
	else if ( !bcaseValPat4Fin )
	{
		for ( i = 0; i < nodes.size(); i++ )
		{
			n1 = nodes.at( i );
			if ( n1 == nullptr )
			{
				continue;
			}
			ccwNeighbors = n1->ccwSortedNeighbors();
			if ( ccwNeighbors.size() == 0 )
			{
				continue;
			}
			n1->createValencePattern( static_cast<uint8_t>(n1->edgeList.size() * 2 - 1), ccwNeighbors );

			if ( n1->boundaryPatternMatch( bcase4, bpat4, ccwNeighbors ) )
			{
				Msg::debug( "boundaryCleanupStep(): matching bcase4, bpat4" );
				d = getDartAt( n1, ccwNeighbors, 0 );
				applyComposition( d, bcomp4 );

				auto j = std::find( nodes.begin(), nodes.end(), n1 );
				if ( j != nodes.end() )
				{
					nodes.erase( j );
				}
				addNodes( ccwNeighbors, n1->pattern[0] - 2 );
			}
		}
		bcaseValPat4Fin = true;
		Msg::debug( "Leaving boundaryCleanupStep(): Done looking for case 4s." );
		return;
	}
	else if ( !bcaseTriQFin )
	{
		for ( i = 0; i < nodes.size(); i++ )
		{
			n1 = nodes.at( i );

			if ( n1 == nullptr )
			{
				continue;
			}

			if ( n1->boundaryNode() )
			{

				Msg::debug( "...testing node " + n1->descr() );
				e1 = n1->edgeList.at( 0 );
				elem = e1->element1;
				if ( !elem->IsAQuad() )
				{ // Must be quad
					continue;
				}
				q = static_cast<Quad*>(elem);

				if ( n1->edgeList.size() == 2 && q->ang[q->angleIndex( n1 )] > DEG_150 )
				{
					Msg::debug( "...inside if (n1.edgeList.size()==2 ..." );
					e2 = q->neighborEdge( n1, e1 );
					if ( e1->boundaryEdge() && e2->boundaryEdge() )
					{
						Msg::debug( "...inside if (e1.boundaryEdge() ..." );
						Quad *q3 = nullptr, *q4 = nullptr, *q33 = nullptr, *q44 = nullptr, *qNew, *qn = nullptr;
						Edge *e3, *e4, *e33, *e44;
						n0 = e1->otherNode( n1 );
						n2 = e2->otherNode( n1 );
						e3 = q->neighborEdge( n2, e2 );
						n3 = e3->otherNode( n2 );
						e4 = q->neighborEdge( n3, e3 );

						elem = q->neighbor( e3 );
						if ( elem->IsAQuad() )
						{
							q3 = static_cast<Quad*>(elem);
						}

						elem = q->neighbor( e4 );
						if ( elem->IsAQuad() )
						{
							q4 = static_cast<Quad*>(elem);
						}

						if ( q3 != nullptr && q4 != nullptr )
						{

							e33 = q3->neighborEdge( n3, e3 );
							e44 = q4->neighborEdge( n3, e4 );

							elem = q3->neighbor( e33 );
							if ( elem->IsAQuad() )
							{
								q33 = static_cast<Quad*>(elem);
							}

							elem = q4->neighbor( e44 );
							if ( elem->IsAQuad() )
							{
								q44 = static_cast<Quad*>(elem);
							}

							if ( q33 != nullptr )
							{
								e = q33->neighborEdge( n3, e33 );
								elem = q33->neighbor( e );
								if ( elem->IsAQuad() )
								{
									qn = static_cast<Quad*>(elem);
								}
							}

							if ( q33 == nullptr || q44 == nullptr || qn != q44 )
							{
								// One row transition
								elementList.erase( std::find( elementList.begin(), elementList.end(), q ) );
								elementList.erase( std::find( elementList.begin(), elementList.end(), q3 ) );
								elementList.erase( std::find( elementList.begin(), elementList.end(), q4 ) );

								fill4( q, e4, n3 );
								qNew = static_cast<Quad*>(q3->neighbor( e3 ));
								fill3( q3, e3, n2, true );
								elementList.erase( std::find( elementList.begin(), elementList.end(), qNew ) );

								auto j = std::find( nodes.begin(), nodes.end(), n1 );
								if ( j != nodes.end() )
								{
									nodes.erase( j );
								}
							}
							else if ( qn == q44 )
							{
								// Two row transition
								elementList.erase( std::find( elementList.begin(), elementList.end(), q ) );
								elementList.erase( std::find( elementList.begin(), elementList.end(), q4 ) );
								elementList.erase( std::find( elementList.begin(), elementList.end(), q44 ) );

								fill3( q4, e44, e44->otherNode( n3 ), true );
								qNew = static_cast<Quad*>(q->neighbor( e4 ));
								fill3( q, e4, n3, true );

								elementList.erase( std::find( elementList.begin(), elementList.end(), qNew ) );
								auto j = std::find( nodes.begin(), nodes.end(), n1 );
								if ( j != nodes.end() )
								{
									nodes.erase( j );
								}
							}
						}
					}
				}
			}
		}
		Msg::debug( "Leaving boundaryCleanupStep(): Done removing triangular quads." );
		bcaseTriQFin = true;
		return;
	}
	else if ( !bcaseDiamondFin )
	{

		for ( i = 0; i < nodes.size(); i++ )
		{
			n1 = nodes.at( i );

			if ( n1 == nullptr )
			{
				continue;
			}

			if ( n1->boundaryNode() && n1->valence() > 4 )
			{
				Msg::debug( "...testing node " + n1->descr() );

				// First find a quad having an edge at the boundary at node n1,
				// or if this does not exist, the find the first quad when looking
				// from the boundary an inwards around node n1:
				e1 = n1->anotherBoundaryEdge( nullptr );
				if ( e1->element1->IsAQuad() )
				{
					q = static_cast<Quad*>(e1->element1);
					pq = q;
					ep = e1;
				}
				else
				{
					tri = static_cast<Triangle*>(e1->element1);
					e1 = e1->nextQuadEdgeAt( n1, e1->element1 );
					if ( e1 != nullptr )
					{
						q = e1->getQuadElement();
						pq = q;
						ep = e1;
					}
				}

				if ( q != nullptr )
				{
					Msg::debug( "...first quad found is " + q->descr() );
				}

				// Then parse each quad from one boundary edge to the other until
				// a match is made:
				int val, n2val, n3val, maxdev;
				while ( q != nullptr )
				{
					if ( q->boundaryDiamond() )
					{

						Msg::debug( "...Considering to close quad" );

						n2 = e1->otherNode( n1 );
						e2 = q->neighborEdge( n1, e1 );
						n3 = e2->otherNode( n1 );

						n2val = n2->valence();
						n3val = n3->valence();
						maxdev = std::max( abs( 4 - n2val ), abs( 4 - n3val ) );
						val = n2val + n3val;

						if ( abs( 4 - (val - 2) ) <= maxdev )
						{
							Msg::debug( "...closing quad " + q->descr() );
							d = closeQuad( q, e1, e1->otherNode( n1 ), true );
							if ( d != nullptr )
							{
								q = pq;
								e1 = ep;
							}
						}
					}
					Msg::debug( "...q: " + q->descr() + ", e1: " + e1->descr() + ", n1: " + n1->descr() );
					pq = q;
					ep = e1;
					e1 = e1->nextQuadEdgeAt( n1, q );
					if ( e1 != nullptr )
					{
						q = static_cast<Quad*>(q->neighbor( e1 ));
					}
					else
					{
						break;
					}
				}
			}
		}
		bcaseDiamondFin = true;
		Msg::debug( "Leaving boundaryCleanupStep(): Done removing diamonds." );
		return;
	}
	else
	{
		nodes = nodeList;
		boundaryCleanupFinished = true;
		count = 0;
		Msg::debug( "Leaving TopoCleanup.boundaryCleanupStep(), all is well." );
		return;
	}
}

void 
TopoCleanup::shapeCleanupStep()
{
	Msg::debug( "Entering TopoCleanup.shapeCleanupStep()" );

	Element* elem;
	Quad *q = nullptr, *q2 = nullptr, *qo = nullptr, *qtemp;
	Edge *e1, *e2, *e3, *e4, *eo;
	Node *n, *n1, *n2, *n3, *n4, *nqOpp, *nq2Opp, *noOpp;
	int i;
	double ang, ang1 = 0, ang2 = 0, ango = 0, angtmp, q2angn3;

	if ( !shape1stTypeFin )
	{
		Msg::debug( "...hallo???" );
		for ( i = 0; i < nodes.size(); i++ )
		{
			n = nodes.at( i );

			if ( n == nullptr )
			{
				continue;
			}

			if ( n->boundaryNode() )
			{

				if ( n->edgeList.size() == 3 )
				{

					e1 = n->anotherBoundaryEdge( nullptr );
					elem = e1->element1;
					if ( !elem->IsAQuad() )
					{
						continue;
					}
					q = static_cast<Quad*>(elem);
					ang = q->ang[q->angleIndex( n )];

					e2 = q->neighborEdge( n, e1 );
					elem = q->neighbor( e2 );
					if ( elem->IsAQuad() )
					{
						q2 = static_cast<Quad*>(elem);

						ang2 = q2->ang[q2->angleIndex( n )];
						if ( ang2 > DEG_160 && ang2 > ang )
						{
							qtemp = q;
							q = q2;
							q2 = qtemp;
							angtmp = ang;
							ang = ang2;
							ang2 = angtmp;

							e1 = q->neighborEdge( n, e2 );
						}
					}

					if ( ang < DEG_160 )
					{
						continue;
					}

					n1 = e1->otherNode( n );
					n2 = e2->otherNode( n );
					nqOpp = q->oppositeNode( n );

					ang1 = q->ang[q->angleIndex( n1 )];

					if ( !n1->boundaryNode() || !n2->boundaryNode() || !nqOpp->boundaryNode() )
					{
						continue;
					}

					eo = q->neighborEdge( n1, e1 );

					elem = q->neighbor( eo );
					if ( elem->IsAQuad() )
					{
						qo = static_cast<Quad*>(elem);
						ango = qo->ang[qo->angleIndex( n1 )];
					}

					if ( q2 == nullptr || qo == nullptr )
					{ // Was && instead of ||
						continue;
					}

					e3 = q2->neighborEdge( n, e2 );
					n3 = e3->otherNode( n );
					q2angn3 = q2->ang[q2->angleIndex( n3 )];
					nq2Opp = q2->oppositeNode( n );

					e4 = qo->neighborEdge( n1, eo );
					n4 = e4->otherNode( n1 );
					noOpp = qo->oppositeNode( n1 );

					if ( ang2 != 0 && q2angn3 > ango && n3->boundaryNode() && nq2Opp->boundaryNode() )
					{
						elementList.erase( std::find( elementList.begin(), elementList.end(), q ) );
						elementList.erase( std::find( elementList.begin(), elementList.end(), q2 ) );
						fill4( q, e2, n );
					}
					else if ( ango != 0 && ango > q2angn3 && n4->boundaryNode() && nq2Opp->boundaryNode() )
					{
						elementList.erase( std::find( elementList.begin(), elementList.end(), q ) );
						elementList.erase( std::find( elementList.begin(), elementList.end(), qo ) );
						fill4( qo, eo, n1 );
					}
				}
				else if ( n->edgeList.size() == 4 )
				{
					e1 = n->anotherBoundaryEdge( nullptr );
					elem = e1->element1;
					if ( elem->IsAQuad() )
					{
						q2 = static_cast<Quad*>(elem);
					}
					else
					{
						continue;
					}

					e2 = q2->neighborEdge( n, e1 );
					elem = q2->neighbor( e2 );
					if ( elem->IsAQuad() )
					{
						q = static_cast<Quad*>(elem);
					}
					else
					{
						continue;
					}

					if ( q->ang[q->angleIndex( n )] < DEG_160 )
					{
						continue;
					}

					e3 = q->neighborEdge( n, e2 );
					elem = q->neighbor( e3 );
					if ( elem->IsAQuad() )
					{
						qo = static_cast<Quad*>(elem);
					}
					else
					{
						continue;
					}
					e4 = qo->neighborEdge( n, e3 );

					n1 = e1->otherNode( n );
					n2 = e2->otherNode( n );
					n3 = e3->otherNode( n );
					n4 = e4->otherNode( n );

					if ( !n1->boundaryNode() || !n2->boundaryNode() || !n3->boundaryNode() || !n4->boundaryNode() )
					{
						continue;
					}

					openQuad( q, e2, n );
					elementList.erase( std::find( elementList.begin(), elementList.end(), q2->neighbor( e2 ) ) );
					elementList.erase( std::find( elementList.begin(), elementList.end(), qo->neighbor( e3 ) ) );
					fill3( q2, e2, n2, true );
					fill3( qo, e3, n3, true );

					elementList.erase( std::find( elementList.begin(), elementList.end(), q2 ) );
					elementList.erase( std::find( elementList.begin(), elementList.end(), qo ) );
				}
			}
		}
		shape1stTypeFin = true;
		return;
	}

	while ( count < elementList.size() )
	{
		elem = elementList.at( count );

		if ( elem == nullptr || !(elem->IsAQuad()) )
		{
			count++;
		}
		else
		{
			Msg::debug( "...testing element " + elem->descr() );

			q = static_cast<Quad*>(elem);
			if ( q->isFake )
			{
				Msg::error( "...Fake quad encountered!!!" );
			}

			if ( q->isChevron() )
			{
				eliminateChevron( q );
				count++;
			}
			else
			{
				count++;
			}
		}
	}

	for ( i = 0; i < deleteList.size(); i++ )
	{
		elem = deleteList.at( i );
		elementList.erase( std::find( elementList.begin(), elementList.end(), elem ) );
	}
	deleteList.clear();

	nodes = nodeList;
	shapeCleanupFinished = true;
	count = 0;
	Msg::debug( "Leaving TopoCleanup.shapeCleanupStep(), all elements ok." );
}

Dart*
TopoCleanup::getDartAt( Node* c, const std::vector<Node*>& neighbors, int i )
{
	Msg::debug( "Entering getDartAt(..)" );
	Edge* e = c->commonEdge( neighbors[i] );
	if ( e == nullptr )
	{
		Msg::debug( "Leaving getDartAt(..), not found!" );
		return nullptr;
	}
	Msg::debug( "...1st matching node in neighbors: " + neighbors[i]->descr() );
	Quad* q1 = static_cast<Quad*>(e->element1);
	Quad* q2 = static_cast<Quad*>(e->element2);

	Msg::debug( "Leaving getDartAt(..)" );
	if ( q1->hasNode( neighbors[i + 1] ) )
	{
		return new Dart( c, e, q1 );
	}
	else if ( q2->hasNode( neighbors[i + 1] ) )
	{
		return new Dart( c, e, q2 );
	}
	else
	{
		return nullptr;
	}
}

Dart* 
TopoCleanup::closeQuad( Quad* q, Edge* e1, Node* nK, bool centroid )
{
	Msg::debug( "Entering closeQuad(..)" );
	Dart* d = new Dart();
	Element* nElem = q->neighbor( e1 ); // Save for later...
	Node* nKOpp = q->oppositeNode( nK );
	Node* nKp1 = e1->otherNode( nK ); // , nKm1= eKm1.otherNode(nK);
	Edge *e2 = q->neighborEdge( nKp1, e1 ), *e4 = q->neighborEdge( nK, e1 );

	std::vector<Element*> lK = nK->adjElements(), lKOpp = nKOpp->adjElements();
	Node* n = nullptr;

	if ( centroid )
	{
		n = safeNewPosWhenCollapsingQuad( q, nK, nKOpp );
		if ( n == nullptr )
		{
			Msg::debug( "Leaving closeQuad(..), returning null!" );
			return nullptr;
		}
	}
	else
	{
		if ( q->anyInvertedElementsWhenCollapsed( nKOpp, nK, nKOpp, lK, lKOpp ) )
		{
			Msg::debug( "Leaving closeQuad(..), returning null!" );
			return nullptr;
		}
		else
		{
			n = nKOpp;
		}
	}
	elementList.erase( std::find( elementList.begin(), elementList.end(), q ) );

	edgeList.erase( std::find( edgeList.begin(), edgeList.end(), e1 ) );
	edgeList.erase( std::find( edgeList.begin(), edgeList.end(), q->neighborEdge( nK, e1 ) ) );
	q->disconnectEdges();
	q->closeQuad( e2, e1 ); // (e1,e2)

	nKOpp->setXY( *n ); // nK.setXY(n);
	nodeList.erase( std::find( nodeList.begin(), nodeList.end(), nK ) );

	auto iter = std::find( nodes.begin(), nodes.end(), nK );
	if ( iter != nodes.end() )
	{
		*iter = nullptr;
	}
	nKOpp->update(); // nK.update();

	d->elem = nElem;
	d->e = e2; // e1;
	d->n = nKOpp; // nK;

	Msg::debug( "Leaving closeQuad(..)" );
	return d;
}

Dart* 
TopoCleanup::openQuad( Quad* q, Edge* e, Node* n1 )
{
	Msg::debug( "Entering openQuad(..)" );
	Dart* d = new Dart();
	Node* c = q->centroid();
	Edge* e1 = q->neighborEdge( n1 );
	Edge* e2 = q->neighborEdge( n1, e1 );

	Node* n2 = e2->otherNode( n1 );
	Edge* e3 = q->neighborEdge( n2, e2 );
	Node* n3 = e3->otherNode( n2 );
	Edge* e4 = q->neighborEdge( n3, e3 );
	Node* n4 = e4->otherNode( n3 );

	Edge* e1New = new Edge( c, n1 );
	Edge* e4New = new Edge( c, n3 );

	e1New->connectNodes();
	e4New->connectNodes();

	q->replaceEdge( e1, e1New );
	q->replaceEdge( e4, e4New );
	e1->disconnectFromElement( q );
	e4->disconnectFromElement( q );

	e1New->connectToQuad( q );
	e4New->connectToQuad( q );

	Quad* qNew;
	if ( e1->leftNode == n1 )
	{
		qNew = new Quad( e1, e1New, e4, e4New );
	}
	else
	{
		qNew = new Quad( e1, e4, e1New, e4New );
	}

	qNew->connectEdges();

	edgeList.push_back( e1New );
	edgeList.push_back( e4New );

	elementList.push_back( qNew );
	c->color = 3;// java.awt.Color.red; // Indicate it was created during clean-up
	nodeList.push_back( c );
	nodes.push_back( c );

	q->updateLR();

	d->elem = q;
	d->e = e;
	d->n = n1;

	Msg::debug( "Leaving openQuad(..)" );
	return d;
}

Dart* 
TopoCleanup::switchDiagonalCCW( Quad* qa, Edge* e1a, Node* n )
{
	Msg::debug( "Entering switchDiagonalCCW(..)" );
	Dart* d = new Dart();
	Node* n1a, *n2a, *n3a, *n4a, *n1b, *n2b, *n3b, *n4b;
	Edge* e2a, *e3a, *e4a, *e1b, *e2b, *e3b, *e4b;
	Edge* eNew, *l, *r;
	Quad* q1, *q2;

	Quad* qb = static_cast<Quad*>(qa->neighbor( e1a ));
	auto qaIndex = std::find( elementList.begin(), elementList.end(), qa );
	auto qbIndex = std::find( elementList.begin(), elementList.end(), qb );

	// First get the edges of qa in ccw order:
	n2a = qa->nextCCWNode( e1a->leftNode );
	if ( n2a == e1a->rightNode )
	{
		n1a = e1a->leftNode;
	}
	else
	{
		n1a = e1a->rightNode;
		n2a = e1a->leftNode;
	}

	e2a = qa->neighborEdge( n2a, e1a );
	n3a = e2a->otherNode( n2a );
	e3a = qa->neighborEdge( n3a, e2a );
	n4a = e3a->otherNode( n3a );
	e4a = qa->neighborEdge( n4a, e3a );

	// Now get the edges of qb in ccw order:
	e1b = e1a;
	n2b = qb->nextCCWNode( e1b->leftNode );
	if ( n2b == e1b->rightNode )
	{
		n1b = e1b->leftNode;
	}
	else
	{
		n1b = e1b->rightNode;
		n2b = e1b->leftNode;
	}
	e2b = qb->neighborEdge( n2b, e1b );
	n3b = e2b->otherNode( n2b );
	e3b = qb->neighborEdge( n3b, e2b );
	n4b = e3b->otherNode( n3b );
	e4b = qb->neighborEdge( n4b, e3b );

	Node *nOld, *smoothed;
	// Check to see if the switch will violate the mesh topology:
	if ( e4a->sumAngle( qa, n1a, e2b ) >= std::numbers::pi)
	{ // if angle >= 180 degrees...
		if ( n1a->boundaryNode() )
		{ // exit if node on boundary
			Msg::debug( "Leaving switchDiagonalCCW(..): returning null" );
			return nullptr;
		}

		// ...then try smoothing the pos of the node:
		nOld = new Node( n1a->x, n1a->y );
		smoothed = n1a->laplacianSmoothExclude( n2a );
		if ( !n1a->equals( smoothed ) )
		{
			n1a->setXY( smoothed->x, smoothed->y );
			inversionCheckAndRepair( n1a, nOld );
			n1a->update();
		}

		if ( e4a->sumAngle( qa, n1a, e2b ) >= std::numbers::pi )
		{ // Still angle >= 180 degrees?
			Msg::debug( "Leaving switchDiagonalCCW(..): returning null" );
			return nullptr;
		}
	}

	if ( e2a->sumAngle( qa, n2a, e4b ) >= std::numbers::pi )
	{ // if angle >= 180 degrees...
		if ( n2a->boundaryNode() )
		{ // exit if node on boundary
			Msg::debug( "Leaving switchDiagonalCCW(..): returning null" );
			return nullptr;
		}

		// ...then try smoothing the pos of the node:
		nOld = new Node( n2a->x, n2a->y );
		smoothed = n2a->laplacianSmoothExclude( n1a );
		if ( !n2a->equals( smoothed ) )
		{
			n2a->setXY( smoothed->x, smoothed->y );
			inversionCheckAndRepair( n2a, nOld );
			n2a->update();
		}

		if ( e2a->sumAngle( qa, n2a, e4b ) >= std::numbers::pi )
		{ // Still angle >= 180 degrees?
			Msg::debug( "Leaving switchDiagonalCCW(..): returning null" );
			return nullptr;
		}
	}
	// The new diagonal:
	eNew = new Edge( n3a, n3b );

	// Create the new quads:
	l = qa->neighborEdge( e4a->leftNode, e4a );
	r = qa->neighborEdge( e4a->rightNode, e4a );
	if ( l == e1a )
	{
		l = e2b;
	}
	else
	{
		r = e2b;
	}
	q1 = new Quad( e4a, l, r, eNew );

	l = qb->neighborEdge( e4b->leftNode, e4b );
	r = qb->neighborEdge( e4b->rightNode, e4b );
	if ( l == e1b )
	{
		l = e2a;
	}
	else
	{
		r = e2a;
	}
	q2 = new Quad( e4b, l, r, eNew );

	qa->disconnectEdges();
	qb->disconnectEdges();
	e1a->disconnectNodes();
	q1->connectEdges();
	q2->connectEdges();
	eNew->connectNodes();

	// Update lists:
	auto iter = std::find( edgeList.begin(), edgeList.end(), e1a );
	*iter = eNew;

	*qaIndex = q1;
	*qbIndex = q2;

	d->elem = q1;
	d->e = eNew;
	if ( n == n1a )
	{
		d->n = n3b;
	}
	else
	{
		d->n = n3a;
	}

	Msg::debug( "Leaving switchDiagonalCCW(..)" );
	return d;
}

Dart*
TopoCleanup::switchDiagonalCW( Quad* qa, Edge* e1a, Node* n )
{
	Msg::debug( "Entering switchDiagonalCW(..)" );
	Dart *d = new Dart();
	Node *n1a, *n2a, *n3a, *n4a, *n1b, *n2b, *n3b, *n4b;
	Edge *e2a, *e3a, *e4a, *e1b, *e2b, *e3b, *e4b;
	Edge *eNew, *l, *r;
	Quad *q1, *q2;

	Quad* qb = static_cast<Quad*>(qa->neighbor( e1a ));
	auto qaIndex = std::find( elementList.begin(), elementList.end(), qa );
	auto qbIndex = std::find( elementList.begin(), elementList.end(), qb );

	// First get the edges of qa in ccw order:
	n2a = qa->nextCCWNode( e1a->leftNode );
	if ( n2a == e1a->rightNode )
	{
		n1a = e1a->leftNode;
	}
	else
	{
		n1a = e1a->rightNode;
		n2a = e1a->leftNode;
	}
	e2a = qa->neighborEdge( n2a, e1a );
	n3a = e2a->otherNode( n2a );
	e3a = qa->neighborEdge( n3a, e2a );
	n4a = e3a->otherNode( n3a );
	e4a = qa->neighborEdge( n4a, e3a );

	// Now get the edges of qb in ccw order:
	e1b = e1a;
	n2b = qb->nextCCWNode( e1b->leftNode );
	if ( n2b == e1b->rightNode )
	{
		n1b = e1b->leftNode;
	}
	else
	{
		n1b = e1b->rightNode;
		n2b = e1b->leftNode;
	}
	e2b = qb->neighborEdge( n2b, e1b );
	n3b = e2b->otherNode( n2b );
	e3b = qb->neighborEdge( n3b, e2b );
	n4b = e3b->otherNode( n3b );
	e4b = qb->neighborEdge( n4b, e3b );

	Node *nOld, *smoothed;
	// Check to see if the switch will violate the mesh topology:
	if ( e4a->sumAngle( qa, n1a, e2b ) >= std::numbers::pi )
	{ // if angle >= 180 degrees...
// ...then try smoothing the pos of the node:
		nOld = new Node( n1a->x, n1a->y );
		smoothed = n1a->laplacianSmooth();
		if ( !n1a->equals( smoothed ) )
		{
			n1a->moveTo( *smoothed );
			inversionCheckAndRepair( n1a, nOld );
			n1a->update();
		}

		if ( e4a->sumAngle( qa, n1a, e2b ) >= std::numbers::pi )
		{ // Still angle >= 180 degrees?
			Msg::debug( "Leaving switchDiagonalCW(..): returning null" );
			return nullptr;
		}
	}

	// Check to see if the switch will violate the mesh topology:
	if ( e2a->sumAngle( qa, n2a, e4b ) >= std::numbers::pi )
	{ // if angle >= 180 degrees...
// ...then try smoothing the pos of the node:
		nOld = new Node( n2a->x, n2a->y );
		smoothed = n2a->laplacianSmooth();
		if ( !n2a->equals( smoothed ) )
		{
			n2a->moveTo( *smoothed );
			inversionCheckAndRepair( n2a, nOld );
			n2a->update();
		}

		if ( e2a->sumAngle( qa, n2a, e4b ) >= std::numbers::pi )
		{ // Still angle >= 180 degrees?
			Msg::debug( "Leaving switchDiagonalCW(..): returning null" );
			return nullptr;
		}
	}

	// The new diagonal:
	eNew = new Edge( n4a, n4b );

	// Create the new quads:
	l = qa->neighborEdge( e2a->leftNode, e2a );
	r = qa->neighborEdge( e2a->rightNode, e2a );
	if ( l == e1a )
	{
		l = e4b;
	}
	else
	{
		r = e4b;
	}
	q1 = new Quad( e2a, l, r, eNew );

	l = qb->neighborEdge( e2b->leftNode, e2b );
	r = qb->neighborEdge( e2b->rightNode, e2b );
	if ( l == e1b )
	{
		l = e4a;
	}
	else
	{
		r = e4a;
	}
	q2 = new Quad( e2b, l, r, eNew );

	qa->disconnectEdges();
	qb->disconnectEdges();
	e1a->disconnectNodes();
	q1->connectEdges();
	q2->connectEdges();
	eNew->connectNodes();

	// Update lists:
	auto iter = std::find( edgeList.begin(), edgeList.end(), eNew );
	*iter = eNew;

	*qaIndex = q1;
	*qbIndex = q2;

	d->elem = q1;
	d->e = eNew;
	if ( n == n1a )
	{
		d->n = n4a;
	}
	else
	{
		d->n = n4b;
	}

	Msg::debug( "Leaving switchDiagonalCW(..)" );
	return d;
}

void 
TopoCleanup::globalSmooth()
{
	Msg::debug( "Entering TopoCleanup.globalSmoth()" );
	Node *nn, *nOld;

	for ( auto n : nodeList )
	{
		if ( !n->boundaryNode() )
		{

			// Try smoothing the pos of the node:
			nOld = new Node( n->x, n->y );
			nn = n->laplacianSmooth();
			if ( !n->equals( nn ) )
			{
				n->setXY( nn->x, nn->y );
				inversionCheckAndRepair( n, nOld );
				n->update();
			}
		}
	}
	Msg::debug( "Leaving TopoCleanup.globalSmoth()" );
}