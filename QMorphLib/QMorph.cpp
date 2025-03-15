#include "pch.h"
#include "QMorph.h"

#include "TopoCleanup.h"
#include "GlobalSmooth.h"
#include "Msg::h"
#include "Edge.h"
#include "Quad.h"
#include "Triangle.h"
#include "Node.h"

void 
QMorph::init()
{

	if ( leftmost == nullptr || lowermost == nullptr || rightmost == nullptr || uppermost == nullptr )
	{
		findExtremeNodes();
	}

	if ( doCleanUp )
	{
		topoCleanup = new TopoCleanup();
	}

	if ( doSmooth )
	{
		globalSmooth = new GlobalSmooth();
	}

	if ( doTri2QuadConversion )
	{
		setCurMethod( this );
		elementList.clear();
		finished = false;
		level = 0;

		repairZeroAreaTriangles();

		if ( !verifyTriangleMesh( triangleList ) )
		{
			Msg::error( "This is not an all-triangle mesh." );
		}

		Edge::clearStateList();
		frontList = defineInitFronts( edgeList );
		Msg::debug( "Initial front list (size==" + std::to_string( frontList.size() ) + "):" );
		printEdgeList( frontList );
		classifyStateOfAllFronts( frontList );

		nrOfFronts = countNOFrontsAtCurLowestLevel( frontList );
		if ( (nrOfFronts & 1) == 0 )
		{
			evenInitNrOfFronts = true;
		}
		else
		{
			evenInitNrOfFronts = false;
		}
	}
	else if ( doCleanUp )
	{
		setCurMethod( topoCleanup );
		topoCleanup->init();
	}
	else if ( doSmooth )
	{
		setCurMethod( globalSmooth );
		globalSmooth->init();
	}
}

uint8_t 
QMorph::oddNOFEdgesInLoopsWithFEdge( Edge* b, Edge* l, Edge* r, bool lLoop, bool rLoop )
{
	Msg::debug( "Entering oddNOFEdgesInLoopsWithFEdge(..)" );

	uint8_t ret = 0;
	int ln = 0, rn = 0;
	bothSidesInLoop = false;

	if ( lLoop )
	{
		ln = countFrontsInNewLoopAt( b, l, r );
	}
	if ( !bothSidesInLoop && rLoop )
	{
		rn = countFrontsInNewLoopAt( b, r, l );
	}

	if ( (ln & 1) == 1 )
	{
		ret += 1;
	}
	if ( (rn & 1) == 1 )
	{
		ret += 2;
	}
	if ( bothSidesInLoop )
	{
		ret += 4;
	}

	Msg::debug( "Leaving oddNOFEdgesInLoopsWithFEdge(..)..." );
	return ret;
}

int
QMorph::countFrontsInNewLoopAt( Edge* b, Edge* side, Edge* otherSide )
{
	Msg::debug( "Entering countFrontsInNewLoopAt(..)" );
	Msg::debug( "...b== " + b->descr() );
	Msg::debug( "...side== " + side->descr() );
	Msg::debug( "...otherSide== " + otherSide->descr() );

	Edge *cur = nullptr, *prev, *tmp;
	Node 
		*n1 = side->commonNode( b ), *n2 = otherSide->commonNode( b ), 
		*n3 = side->otherNode( n1 ), *n4 = otherSide->otherNode( n2 );
	int count1stLoop = 1, count2ndLoop = 1, count3rdLoop = 1, n3n4Edges = 0, count = 0;
	bool n4Inn3Loop = false;

	// First find the front edge where we want to start
	prev = b;
	cur = prev->frontNeighborAt( n1 );
	Msg::debug( "...cur= " + cur->descr() );

	// Parse the current loop of fronts:
	do
	{
		tmp = cur->nextFrontNeighbor( prev );
		prev = cur;
		cur = tmp;
		count1stLoop++;
		Msg::debug( "...cur= " + cur->descr() );

		if ( cur->hasNode( n1 ) )
		{ // Case 2,3,4,5 or 6
			bothSidesInLoop = true;
			Msg::debug( "...both sides in loop" );

			prev = n3->anotherFrontEdge( nullptr );
			cur = prev->frontNeighborAt( n3 );

			double alpha = cur->computeCCWAngle( side );
			double beta = prev->computeCCWAngle( side );

			// Should we go the other way?
			if ( beta < alpha )
			{
				tmp = cur;
				cur = prev;
				prev = tmp;
			}
			if ( cur->hasNode( n4 ) )
			{
				n4Inn3Loop = true;
				n3n4Edges = 1;
			}
			Msg::debug( "...cur= " + cur->descr() );

			// Parse the n3 loop and count number of edges here
			do
			{
				tmp = cur->nextFrontNeighbor( prev );
				prev = cur;
				cur = tmp;
				count2ndLoop++;
				Msg::debug( "...cur= " + cur->descr() );

				if ( !n4Inn3Loop && cur->hasNode( n4 ) )
				{
					n4Inn3Loop = true;
					n3n4Edges = count2ndLoop;
				}
			} while ( !cur->hasNode( n3 ) );

			if ( !n4Inn3Loop && !otherSide->isFrontEdge() && n4->frontNode() )
			{ // Case 5 only
				prev = n4->anotherFrontEdge( nullptr );
				cur = prev->frontNeighborAt( n4 );
				Msg::debug( "...counting edges in n4-loop (case 5)" );
				Msg::debug( "...cur= " + cur->descr() );

				// Parse the n4 loop and count number of edges here
				do
				{
					tmp = cur->nextFrontNeighbor( prev );
					prev = cur;
					cur = tmp;
					count3rdLoop++;
					Msg::debug( "...cur= " + cur->descr() );
				} while ( !cur->hasNode( n4 ) );
			}

			if ( n4Inn3Loop && !otherSide->isFrontEdge() )
			{// Case 2,3
				count = count1stLoop + count2ndLoop + 1 - n3n4Edges;
				Msg::debug( "...case 2 or 3" );
			}
			else if ( !n4Inn3Loop && otherSide->isFrontEdge() )
			{// Case 4
				count = count1stLoop + count2ndLoop;
				Msg::debug( "...case 4" );
			}
			else if ( !n4Inn3Loop && !otherSide->isFrontEdge() && !n4->frontNode() )
			{// Case 6
				count = count1stLoop + count2ndLoop + 2;
				Msg::debug( "...case 6" );
			}
			else if ( !n4Inn3Loop && !otherSide->isFrontEdge() && n4->frontNode() )
			{// Case 5
				count = count1stLoop + count2ndLoop + count3rdLoop + 2;
				Msg::debug( "...case 5" );
			}
			break;
		}
	} while ( !cur->hasNode( n1 ) && !cur->hasNode( n3 ) );

	if ( !bothSidesInLoop )
	{ // Case 1
		count = count1stLoop + 1; // Add the new edge between n1 and n3
	}

	Msg::debug( "Leaving int countFrontsInNewLoopAt(..), returns " + count );
	return count;
}

Quad* 
QMorph::makeQuad( Edge* e )
{
	Msg::debug( "Entering makeQuad(..)" );
	Msg::debug( "e= " + e.descr() );
	Quad* q;
	Triangle* t;
	Edge* top;
	int index;
	bool initL = false, initR = false;
	std::array<Edge*, 2> sideEdges;
	Edge* leftSide, *rightSide;
	std::vector<Triangle*> tris;

	if ( e->leftSide )
	{
		initL = true;
	}
	if ( e->rightSide )
	{
		initR = true;
	}

	q = handleSpecialCases( e );
	if ( q != nullptr )
	{
		Msg::debug( "Leaving makeQuad(..), returning specialCase quad" );
		return q;
	}
	sideEdges = makeSideEdges( e );
	leftSide = sideEdges[0];
	rightSide = sideEdges[1];
	Msg::debug( "Left side edge is " + leftSide->descr() );
	Msg::debug( "Right side edge is " + rightSide->descr() );

	if ( leftSide->commonNode( rightSide ) != nullptr )
	{
		Msg::warning( "Cannot create quad, so we settle with a triangle:" );

		Msg::debug( "base: " + e->descr() );
		Msg::debug( "left: " + leftSide->descr() );
		Msg::debug( "right :" + rightSide->descr() );

		t = new Triangle( e, leftSide, rightSide );
		auto index = std::find( triangleList.begin(), triangleList.end(), t );
		if ( index != triangleList.end() )
		{
			t = *index;
		}

		q = new Quad( t );
		clearQuad( q, t->edgeList[0]->getTriangleElement() );

		Msg::debug( "Leaving makeQuad(..)" );
		return q;
	}
	else
	{
		Node nC = leftSide->otherNode( e->leftNode );
		Node nD = rightSide->otherNode( e->rightNode );
		top = recoverEdge( nC, nD );

		// We need some way to handle errors from recoverEdge(..). This is solved by
		// a recursive approach, where makeQuad(e) is called with new parameter
		// values for e until it returns a valid Edge.
		if ( top == nullptr )
		{
			e->removeFromStateList();
			e->classifyStateOfFrontEdge();

			if ( leftSide->frontEdge )
			{
				leftSide->removeFromStateList();
				leftSide->classifyStateOfFrontEdge();
			}
			if ( rightSide->frontEdge )
			{
				rightSide->removeFromStateList();
				rightSide->classifyStateOfFrontEdge();
			}

			Msg::debug( "Leaving makeQuad(..), returning null" );
			return nullptr;
		}
		q = new Quad( e, leftSide, rightSide, top );

		// Quad.trianglesContained(..) and clearQuad(..) needs one of q's interior
		// triangles as parameter. The base edge borders to only one triangle, and
		// this lies inside of q, so it is safe to choose this:

		tris = q->trianglesContained( e->getTriangleElement() );
		if ( q->containsHole( tris ) )
		{
			e->removeFromStateList();
			e->classifyStateOfFrontEdge();

			if ( leftSide->frontEdge )
			{
				leftSide->removeFromStateList();
				leftSide->classifyStateOfFrontEdge();
			}
			if ( rightSide->frontEdge )
			{
				rightSide->removeFromStateList();
				rightSide->classifyStateOfFrontEdge();
			}

			Msg::debug( "Leaving makeQuad(..), contains hole, returning null" );
			return nullptr;
		}

		clearQuad( q, tris );
		// preSmoothUpdateFronts(q, frontList);
		Msg::debug( "Leaving makeQuad(..)" );
		return q;
	}
}

int 
QMorph::countNOFrontsAtCurLowestLevel( const std::vector<Edge*>& frontList2 )
{
	Msg::debug( "Entering countNOFrontsAtCurLowestLevel(..)" );

	Edge* cur;
	int lowestLevel, count = 0;

	// Get nr of fronts at the lowest level:
	if ( frontList2.size() > 0 )
	{
		cur = frontList2.at( 0 );
		lowestLevel = cur->level;
		count = 1;
		for ( int i = 1; i < frontList2.size(); i++ )
		{
			cur = frontList2.at( i );
			if ( cur->level < lowestLevel )
			{
				lowestLevel = cur->level;
				count = 1;
			}
			else if ( cur->level == lowestLevel )
			{
				count++;
			}
		}
	}
	Msg::debug( "Leaving countNOFrontsAtCurLowestLevel(..)" );
	return count;
}

void
QMorph::run()
{
	if ( doTri2QuadConversion )
	{
		if ( !m_step )
		{
			Msg::debug( "---------------------------------------------------" );
			Msg::debug( "Main loop goes here......." );
			Msg::debug( "---------------------------------------------------" );

			// The program's main loop from where all the real action originates
			while ( !finished )
			{
				step();
			}

		}
	}
	else if ( !m_step )
	{
		// Post-processing methods
		if ( doCleanUp )
		{
			topoCleanup->init();
			topoCleanup->run();
		}

		if ( doSmooth )
		{
			globalSmooth->init();
			globalSmooth->run();
		}

		Msg::debug( "The final elements are:" );
		printElements( elementList );
	}
}

void 
QMorph::step()
{
	Quad* q;
	Edge* e;
	int i, oldBaseState;

	e = Edge::getNextFront();
	if ( e != nullptr )
	{
		oldBaseState = e->getState();
		if ( nrOfFronts <= 0 )
		{
			level++;
			nrOfFronts = countNOFrontsAtCurLowestLevel( frontList );
		}

		Edge::printStateLists();
		Msg::debug( "# of fronts in current frontList: " + std::to_string( frontList.size() ) );
		Msg::debug( "Vi behandler kant: " + e->descr() );

		q = makeQuad( e );
		if ( q == nullptr )
		{
			while ( q == nullptr && e != nullptr )
			{
				e->selectable = false;
				e = Edge::getNextFront();
				if ( e == nullptr )
				{
					break;
				}
				Msg::debug( "Vi behandler kant: " + e->descr() );
				oldBaseState = e->getState();
				q = makeQuad( e );
			}
			Edge::markAllSelectable();
			Msg::warning( "Main loop: makeQuad(..) returned null, so I chose another edge. It's alright now." );
		}
		if ( q->firstNode != nullptr )
		{ // firstNode== null indicates a specialcase that
			elementList.push_back( q ); // is not really a quad, but an area that needs
			// local smoothing and update.
		}

		Msg::debug( "frontList:" );
		printEdgeList( frontList );
		Edge::printStateLists();

		if ( q->firstNode != nullptr )
		{ // for specialCases, smoothing is performed already
			localSmooth( q, frontList );
		}
		i = localUpdateFronts( q, level, frontList );
		Msg::debug( "O.K." );
		Msg::debug( "Smoothed & updated quad is " + q->descr() );

		Msg::debug( "frontList:" );
		printEdgeList( frontList );
		Edge::printStateLists();
		nrOfFronts -= i;
		Msg::debug( "nr of fronts removed from lowest level: " + i );
		Msg::debug( "nrOfFronts= " + nrOfFronts );
	}
	else if ( !finished )
	{
		// Post-processing methods
		if ( doCleanUp )
		{
			topoCleanup->init();
			if ( !step )
			{
				topoCleanup->run();
			}
			else
			{
				setCurMethod( topoCleanup );
				topoCleanup->step();
				return;
			}
		}
		if ( doSmooth )
		{
			globalSmooth->init();
			globalSmooth->run();
		}

		Msg::debug( "The final elements are:" );
		printElements( elementList );
		finished = true;
	}
}

Node* 
QMorph::smoothFrontNode( Node* nK, Node* nJ, Quad* myQ, Edge* front1, Edge* front2 )
{
	Msg::debug( "Entering smoothFrontNode(..)..." );
	//		List<Element> adjQuads = nK.adjQuads();
	double tr, ld = 0;
	Quad* q;
	Node* newNode;
	int n = 0, seqQuads = myQ->nrOfQuadsSharingAnEdgeAt( nK ) + 1;

	Msg::debug( "nK= " + nK->descr() );
	Msg::debug( "nJ= " + nJ->descr() );

	if ( front1 == nullptr )
	{
		Msg::error( "front1 is null" );
	}
	if ( front2 == nullptr )
	{
		Msg::error( "front2 is null" );
	}

	Edge* eD = new Edge( nK, nJ );
	if ( contains( nK->edgeList, eD ) )
	{
		auto iter = std::find( nK->edgeList.begin(), nK->edgeList.end(), eD );
		eD = *iter;
	}

	if ( seqQuads == 2 )
	{
		if ( front1->length() > front2->length() )
		{
			tr = front1->length() / front2->length();
		}
		else
		{
			tr = front2->length() / front1->length();
		}

		if ( tr > 20 )
		{
			// Msg::debug("******************* tr > 20");
			ld = nK->meanNeighborEdgeLength();
			newNode = eD->otherNodeGivenNewLength( ld, nJ );
		}
		else if ( tr <= 20 && tr > 2.5 )
		{
			Msg::debug( "******************* tr<= 20 && tr> 2.5" );
			// The mean of (some of) the other edges in all the adjacent elements
			ld = 0;
			// First add the lengths of edges from Triangles ahead of the front
			for ( auto e : nK->edgeList )
			{
				if ( e != eD && e != front1 && e != front2 )
				{
					ld += e->length();
					Msg::debug( "from edge ahead of the front adding " + e->length() );
					n++;
				}
			}
			// Then add the lengths of edges from the two Quads behind the front
			q = front1->getQuadElement();
			ld += q->edgeList[base]->length();
			Msg::debug( "adding " + std::to_string( q->edgeList[base]->length() ) );
			if ( q->edgeList[left] != eD )
			{
				ld += q->edgeList[left]->length();
				Msg::debug( "adding " + std::to_string( q->edgeList[left]->length() ) );
			}
			else
			{
				ld += q->edgeList[right]->length();
				Msg::debug( "adding " + std::to_string( q->edgeList[right]->length() ) );
			}

			q = front2->getQuadElement();
			ld += q->edgeList[base]->length();
			Msg::debug( "adding " + std::to_string( q->edgeList[base]->length() ) );
			if ( q->edgeList[left] != eD )
			{
				ld += q->edgeList[left]->length();
				Msg::debug( "adding " + std::to_string( q->edgeList[left]->length() ) );
			}
			else
			{
				ld += q->edgeList[right]->length();
				Msg::debug( "adding " + std::to_string( q->edgeList[right]->length() ) );
			}

			ld = ld / (4.0 + n);
			Msg::debug( "ld= " + std::to_string( ld ) );
			newNode = eD->otherNodeGivenNewLength( ld, nJ );
		}
		else
		{
			// Msg::debug("******************* tr<= 2.5");
			newNode = nK->blackerSmooth( nJ, front1, front2, eD->length() );
		}
	}
	else
	{ // || nK.nrOfFrontEdges()> 2) {
		newNode = nK->blackerSmooth( nJ, front1, front2, eD->length() );
	}

	Msg::debug( "Leaving smoothFrontNode(..)...returning " + newNode->descr() );
	return newNode;
}

Node*
QMorph::getSmoothedPos( Node* n, Quad* q )
{
	Msg::debug( "Entering getSmoothedPos(..)" );
	Node *newN, *behind;
	Edge *front1, *front2, *e;
	Quad *q2, *qn;
	Element *neighbor;

	Msg::debug( "...q: " + q->descr() );

	if ( q->hasFrontEdgeAt( n ) && !n->boundaryNode() )
	{
		Msg::debug( "...n:" + n->descr() );
		if ( n == q->edgeList[left]->otherNode( q->edgeList[base]->leftNode ) )
		{
			Msg::debug( "...n is left, top" );
			Msg::debug( "...q.edgeList[top]:" + q->edgeList[top]->descr() );

			if ( q->edgeList[top]->isFrontEdge() )
			{
				front1 = q->edgeList[top];
			}
			else
			{
				front1 = q->edgeList[left];
			}
			front2 = front1->trueFrontNeighborAt( n );
			q2 = front2->getQuadElement();
			e = q->commonEdgeAt( n, q2 );
			if ( e == nullptr )
			{
				if ( q->edgeList[top] != front1 && q->edgeList[top] != front2 )
				{
					e = q->edgeList[top];
				}
				else
				{
					e = q->edgeList[left];
				}
			}
		}
		else if ( n == q->edgeList[right]->otherNode( q->edgeList[base]->rightNode ) )
		{
			Msg::debug( "...n is right, top" );
			Msg::debug( "...q.edgeList[top]:" + q->edgeList[top]->descr() );

			if ( q->edgeList[top]->isFrontEdge() )
			{
				front1 = q->edgeList[top];
			}
			else
			{
				front1 = q->edgeList[right];
			}
			front2 = front1->trueFrontNeighborAt( n );
			q2 = front2->getQuadElement();
			e = q->commonEdgeAt( n, q2 );
			if ( e == nullptr )
			{
				if ( q->edgeList[top] != front1 && q->edgeList[top] != front2 )
				{
					e = q->edgeList[top];
				}
				else
				{
					e = q->edgeList[right];
				}
			}
		}
		else if ( n == q->edgeList[base]->leftNode )
		{
			Msg::debug( "...n is left, base" );
			if ( q->edgeList[left]->isFrontEdge() )
			{
				front1 = q->edgeList[left];
			}
			else
			{
				front1 = q->edgeList[base];
			}
			front2 = front1->trueFrontNeighborAt( n );
			q2 = front2->getQuadElement();
			e = q->commonEdgeAt( n, q2 );
			if ( e == nullptr )
			{
				if ( q->edgeList[left] != front1 && q->edgeList[left] != front2 )
				{
					e = q->edgeList[left];
				}
				else
				{
					e = q->edgeList[base];
				}
			}
		}
		else if ( n == q->edgeList[base]->rightNode )
		{
			Msg::debug( "...n is right, base" );
			if ( q->edgeList[right]->isFrontEdge() )
			{
				front1 = q->edgeList[right];
			}
			else
			{
				front1 = q->edgeList[base];
			}
			front2 = front1->trueFrontNeighborAt( n );
			q2 = front2->getQuadElement();
			e = q->commonEdgeAt( n, q2 );
			if ( e == nullptr )
			{
				if ( q->edgeList[right] != front1 && q->edgeList[right] != front2 )
				{
					e = q->edgeList[right];
				}
				else
				{
					e = q->edgeList[base];
				}
			}
		}
		else
		{
			Msg::error( "getSmoothedPos(..): Node n is not in Quad q" );
			newN = nullptr;
			front1 = nullptr;
			front2 = nullptr;
			e = nullptr;
		}
		behind = e->otherNode( n );
		newN = smoothFrontNode( n, behind, q, front1, front2 );
	}
	else if ( !n->boundaryNode() )
	{
		newN = n->modifiedLWLaplacianSmooth();
	}
	else
	{
		Msg::debug( "...n: " + n->descr() + " is a boundaryNode" );
		newN = n;
	}
	Msg::debug( "Leaving getSmoothedPos(..)" );
	return newN;
}