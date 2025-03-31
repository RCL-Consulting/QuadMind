#include "pch.h"
#include "Edge.h"

#include "Node.h"
#include "Triangle.h"
#include "Quad.h"
#include "MyVector.h"

#include <numbers>

std::array<std::vector<Edge*>, 3> Edge::stateList;

Edge::Edge( Node* node1, Node* node2 )
{
	if ( (node1->x < node2->x) || (node1->x == node2->x && node1->y > node2->y) )
	{
		leftNode = node1;
		rightNode = node2;
	}
	else
	{
		leftNode = node2;
		rightNode = node1;
	}

	len = computeLength();
}

Edge::Edge( Edge* e )
{
	leftNode = e->leftNode;
	rightNode = e->rightNode;
	len = e->len;
	e->element1 = element1;
	e->element2 = element2;
}

bool 
Edge::equals( const Edge* o ) const
{
	if ( !o ) return false;
	if ( leftNode->equals( o->leftNode ) && rightNode->equals( o->rightNode ) )
	{
		return true;
	}
	return false;
}

Edge* 
Edge::copy()
{
	return new Edge( *this );
}

void 
Edge::clearStateList()
{
	stateList[0].clear();
	stateList[1].clear();
	stateList[2].clear();
}

bool
Edge::removeFromStateList()
{
	int state = getState();
	auto& List = stateList[state];
	auto i = std::find( List.begin(), List.end(), this );
	if ( i == List.end() )
	{
		return false;
	}
	List.erase( i );
	return true;
}

bool 
Edge::removeFromStateList( int state )
{
	auto& List = stateList[state];
	auto i = std::find( List.begin(), List.end(), this );
	if ( i == List.end() )
	{
		return false;
	}
	List.erase( i );
	return true;
}

int 
Edge::getState()
{
	int ret = 0;
	if ( leftSide )
	{
		ret++;
	}
	if ( rightSide )
	{
		ret++;
	}
	return ret;
}

int 
Edge::getTrueState()
{
	return getState();
}

bool 
Edge::alterLeftState( bool newLeftState )
{
	int state = getState();
	auto& List = stateList[state];
	auto i = std::find( List.begin(), List.end(), this );
	if ( i == List.end() )
	{
		return false;
	}
	leftSide = newLeftState;
	int newState = getState();
	if ( state != newState )
	{
		List.erase( i );
		stateList[newState].push_back( this );
	}
	return true;
}

bool
Edge::alterRightState( bool newRightState )
{
	int state = getState();
	auto& List = stateList[state];
	auto i = std::find( List.begin(), List.end(), this );
	if ( i == List.end() )
	{
		return false;
	}
	rightSide = newRightState;
	int newState = getState();
	if ( state != newState )
	{
		List.erase( i );
		stateList[newState].push_back( this );
	}
	return true;
}

Edge* 
Edge::evalPotSideEdge( Edge* frontNeighbor, Node* n )
{
	Msg::debug("Entering Edge.evalPotSideEdge(..)");
	Element* tri = getTriangleElement(), *quad = getQuadElement();
	double ang;

	if ( tri != nullptr )
	{
		ang = sumAngle( tri, n, frontNeighbor );
	}
	else
	{
		ang = PIx2 - sumAngle( quad, n, frontNeighbor );
	}

	Msg::debug( "sumAngle(..) between " + descr() + " and " + frontNeighbor->descr() + ": " + std::to_string( toDegrees * ang ) );

	Msg::debug( "Leaving Edge.evalPotSideEdge(..)" );
	if ( ang < PIx3div4 )
	{ // if (ang< PIdiv2+EPSILON) // Could this be better?
		return frontNeighbor;
	}
	else
	{
		return nullptr;
	}
	return nullptr;
}

void 
Edge::classifyStateOfFrontEdge()
{
	Msg::debug("Entering Edge.classifyStateOfFrontEdge()");
	Msg::debug( "this: " + descr() );
	Edge* lfn = leftFrontNeighbor, *rfn = rightFrontNeighbor;

	Edge* l, *r;

	// Alter states and side Edges on left side:
	l = evalPotSideEdge( lfn, leftNode );
	if ( l != nullptr )
	{
		leftSide = true;
		if ( leftNode->equals( lfn->leftNode ) )
		{
			lfn->alterLeftState( true );
		}
		else
		{
			lfn->alterRightState( true );
		}
	}
	else
	{
		leftSide = false;
		if ( leftNode->equals( lfn->leftNode ) )
		{
			lfn->alterLeftState( false );
		}
		else
		{
			lfn->alterRightState( false );
		}
	}

	// Alter states and side Edges on right side:
	r = evalPotSideEdge( rfn, rightNode );
	if ( r != nullptr )
	{
		rightSide = true;
		if ( rightNode->equals( rfn->leftNode ) )
		{
			rfn->alterLeftState( true );
		}
		else
		{
			rfn->alterRightState( true );
		}
	}
	else
	{
		rightSide = false;
		if ( rightNode->equals( rfn->leftNode ) )
		{
			rfn->alterLeftState( false );
		}
		else
		{
			rfn->alterRightState( false );
		}
	}

	// Add this to a stateList:
	stateList[getState()].push_back( this );
	Msg::debug( "Leaving Edge.classifyStateOfFrontEdge()" );
}

bool
Edge::isLargeTransition( Edge* e )
{
	double ratio;
	double e1Len = length();
	double e2Len = e->length();

	if ( e1Len > e2Len )
	{
		ratio = e1Len / e2Len;
	}
	else
	{
		ratio = e2Len / e1Len;
	}

	if ( ratio > 2.5 )
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

Edge*
Edge::getNextFront()
{
	Edge* current, * selected = nullptr;
	int selState, curState = 2, i;

	// Select a front preferrably in stateList[2]

	while ( curState >= 0 && selected == nullptr )
	{
		for ( i = 0; i < stateList[curState].size(); i++ )
		{
			current = stateList[curState][i];
			if ( current->selectable )
			{
				selected = current;
				break;
			}
		}
		curState--;
	}
	if ( selected == nullptr )
	{
		Msg::warning( "getNextFront(): no selectable fronts found in stateLists." );
		return nullptr;
	}

	selState = selected->getState();

	for ( i = 0; i < stateList[selState].size(); i++ )
	{
		current = stateList[selState][i];

		if ( current->selectable && (current->level < selected->level || (current->level == selected->level && current->length() < selected->length())) )
		{
			selected = current;
		}
	}

	if ( selState != 2 )
	{
		if ( selected->isLargeTransition( selected->leftFrontNeighbor ) )
		{
			if ( selected->length() > selected->leftFrontNeighbor->length() && selected->leftFrontNeighbor->selectable )
			{
				return selected->leftFrontNeighbor;
			}
		}

		if ( selected->isLargeTransition( selected->rightFrontNeighbor ) )
		{
			if ( selected->length() > selected->rightFrontNeighbor->length() && selected->rightFrontNeighbor->selectable )
			{
				return selected->rightFrontNeighbor;
			}
		}
	}
	return selected;
}

void 
Edge::markAllSelectable()
{
	std::for_each( stateList.begin(), stateList.end(), []( std::vector<Edge*>& l )
				   {
					   std::for_each( l.begin(), l.end(), []( Edge* e )
									  {
										  e->selectable = true;
									  } );
				   } );
}

void 
Edge::printStateLists()
{
	if ( Msg::debugMode )
	{
		std::cout << "frontsInState 1-1:\n";
		for ( auto edge : stateList[2] )
		{
			std::cout << "" << edge->descr() << ", (" << edge->getState() << ")\n";
		}
		std::cout << "frontsInState 0-1 and 1-0:\n";
		for ( auto edge : stateList[1] )
		{
			std::cout << "" << edge->descr() << ", (" << edge->getState() << ")\n";
		}
		std::cout << "frontsInState 0-0:\n";
		for ( auto edge : stateList[0] )
		{
			std::cout << "" << edge->descr() << ", (" << edge->getState() << ")\n";
		}
	}
}

bool
Edge::leftTo( Edge* e )
{
	if ( (leftNode->x < e->leftNode->x) || (leftNode->x == e->leftNode->x && leftNode->y < e->leftNode->y) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool 
Edge::isFrontEdge()
{
	if ( Element::IsATriangle( element1 ) && !Element::IsATriangle( element2 ) )
	{
		return true;
	}

	return false;
}

std::string
Edge::descr() const
{
	return "(" + std::to_string( leftNode->x ) + ", " + std::to_string( leftNode->y ) + "), (" + std::to_string( rightNode->x ) + ", " + std::to_string( rightNode->y ) + ")";
}

void 
Edge::printMe()
{
	std::cout << descr() << "\n";
}

double 
Edge::length()
{
	return len;
}

bool
Edge::replaceNode( Node* n1, Node* n2 )
{
	if ( leftNode->equals( n1 ) )
	{
		leftNode = n2;
	}
	else if ( rightNode->equals( n1 ) )
	{
		rightNode = n2;
	}
	else
	{
		return false;
	}
	len = computeLength();
	return true;
}

void 
Edge::seamWith( Edge* e )
{
	Node* nK = commonNode(e);
	Node* nKp1 = otherNode( nK ), *nKm1 = e->otherNode( nK ), *other;
	bool found = false;
	Edge* eI, *eJ;

	for ( int i = 0; i < nKm1->edgeList.size(); i++ )
	{
		eI = nKm1->edgeList[i];
		other = eI->otherNode( nKm1 );

		if ( other != nKp1 )
		{
			for ( int j = 0; j < nKp1->edgeList.size(); j++ )
			{
				eJ = nKp1->edgeList[j];

				if ( other->equals( eJ->otherNode( nKp1 ) ) )
				{
					found = true;

					auto Iter = std::find( other->edgeList.begin(), other->edgeList.end(), eI );
					if ( Iter != other->edgeList.end() )
					{
						other->edgeList.erase( Iter );
					}

					if ( eI->element1->firstNode->equals( nKm1 ) )
					{ // Don't forget firstNode!!
						eI->element1->firstNode = nKp1;
					}
					eI->element1->replaceEdge( eI, eJ );
					eJ->connectToElement( eI->element1 );
					break;
				}
			}
			if ( !found )
			{
				if ( eI->element1->firstNode->equals( nKm1 ) )
				{ // Don't forget firstNode!!
					eI->element1->firstNode = nKp1;
				}
				if ( eI->element2->firstNode->equals( nKm1 ) )
				{ // Don't forget firstNode!!
					eI->element2->firstNode = nKp1;
				}

				eI->replaceNode( nKm1, nKp1 );
				nKp1->edgeList.push_back( eI );
			}
			else
			{
				found = false;
			}
		}
		else
		{
			// Remove the edge between eKp1 and eKm1 (from the edgeList of eKp1)
			auto Iter = std::find( nKp1->edgeList.begin(), nKp1->edgeList.end(), eI );
			if ( Iter != nKp1->edgeList.end() )
			{
				nKp1->edgeList.erase( Iter );
			}
		}
	}
}

Node*
Edge::midPoint()
{
	double xDiff = rightNode->x - leftNode->x;
	double yDiff = rightNode->y - leftNode->y;

	return new Node( leftNode->x + xDiff * 0.5, leftNode->y + yDiff * 0.5 );
}

double
Edge::computeLength()
{
	double xdiff = rightNode->x - leftNode->x;
	double ydiff = rightNode->y - leftNode->y;
	return sqrt( xdiff * xdiff + ydiff * ydiff );
}

double
Edge::length( double x1, double y1, double x2, double y2 )
{
	double xdiff = x2 - x1;
	double ydiff = y2 - y1;
	return sqrt( xdiff * xdiff + ydiff * ydiff );
}

double 
Edge::length( Node* node1, Node* node2 )
{
	double xdiff = node2->x - node1->x;
	double ydiff = node2->y - node1->y;
	return sqrt( xdiff * xdiff + ydiff * ydiff );
}

double
Edge::angleAt( const Node* n ) const
{
	// Math.acos returns values in the range 0.0 through pi, avoiding neg numbers.
	// If this is CW to x-axis, then return pos acos, else return neg acos
	// double x= leftNode->x-rightNode.x;
	// double y= leftNode->y-rightNode.y;
	auto other = otherNode( n );

	double x = n->x - other->x;
	double y = n->y - other->y;

	if ( x == 0 && y > 0 )
	{
		return -PIdiv2;
	}
	else if ( x == 0 && y < 0 )
	{
		return PIdiv2;
	}
	else
	{
		double hyp = sqrt( x * x + y * y );

		if ( x > 0 )
		{
			if ( y > 0 )
			{
				return std::numbers::pi + acos( x / hyp );
			}
			else
			{
				return std::numbers::pi - acos( x / hyp );
			}
		}
		else
		{
			if ( y > 0 )
			{
				return std::numbers::pi + std::numbers::pi - acos( -x / hyp );
			}
			else
			{
				return acos( -x / hyp );
			}
		}
	}
	return 0.0;
}

double 
Edge::sumAngle( Element* sElem, Node* n, Edge* eEdge )
{
	Msg::debug("Entering sumAngle(..)");
	Msg::debug( "this: " + descr() );
	if ( sElem != nullptr )
	{
		Msg::debug( "sElem: " + sElem->descr() );
	}
	if ( n != nullptr )
	{
		Msg::debug( "n: " + n->descr() );
	}
	if ( eEdge != nullptr )
	{
		Msg::debug( "eEdge: " + eEdge->descr() );
	}

	Element* curElem = sElem;
	Edge* curEdge = this;
	double ang = 0, iang = 0;
	double d;

	while ( curEdge != eEdge && curElem != nullptr )
	{
		d = curElem->angle( curEdge, n );
		// Msg::debug("curEdge= "+curEdge.descr());
		// Msg::debug("curElem->angle(..) returns "+Math.toDegrees(d));
		ang += d;
		curEdge = curElem->neighborEdge( n, curEdge );
		curElem = curElem->neighbor( curEdge );
	}

	// If n is a boundaryNode, the situation gets more complicated:
	if ( curEdge != eEdge )
	{
		// Sum the "internal" angles:
		curEdge = this;
		curElem = sElem->neighbor( curEdge );

		while ( curEdge != eEdge && curElem != nullptr )
		{
			d = curElem->angle( curEdge, n );
			// Msg::debug("curEdge= "+curEdge.descr());
			// Msg::debug("curElem->angle(..) returns "+Math.toDegrees(d));
			iang += d;
			curEdge = curElem->neighborEdge( n, curEdge );
			curElem = curElem->neighbor( curEdge );
		}
		ang = PIx2 - iang;
	}
	Msg::debug( "Leaving sumAngle(..), returning " + std::to_string( toDegrees * ang ) );
	return ang;
}

Edge* 
Edge::firstFrontEdgeAt( Element* sElem, Node* n )
{
	Element* curElem = sElem;
	Edge* curEdge = this;

	while ( !curEdge->frontEdge )
	{
		curEdge = curElem->neighborEdge( n, curEdge );
		curElem = curElem->neighbor( curEdge );
	}
	return curEdge;
}

double
Edge::computePosAngle( Edge* edge, Node* n )
{
	double a, b, c;
	if ( edge == this )
	{
		Msg::warning( "Edge.computePosAngle(..): The parameter Edge is the same as this Edge." );
		return 2 * std::numbers::pi;
	}

	if ( leftNode->equals( n ) )
	{
		if ( n->equals( edge->leftNode ) )
		{
			c = length( rightNode, edge->rightNode );
		}
		else if ( n->equals( edge->rightNode ) )
		{
			c = length( rightNode, edge->leftNode );
		}
		else
		{
			Msg::error( "Edge::computePosAngle(..): These edges are not connected." );
			return 0;
		}
	}
	else if ( rightNode->equals( n ) )
	{
		if ( n->equals( edge->leftNode ) )
		{
			c = length( leftNode, edge->rightNode );
		}
		else if ( n->equals( edge->rightNode ) )
		{
			c = length( leftNode, edge->leftNode );
		}
		else
		{
			Msg::error( "Edge::computePosAngle(..): These edges are not connected." );
			return 0;
		}
	}
	else
	{
		Msg::error( "Edge::computePosAngle(..): These edges are not connected." );
		return 0;
	}
	a = computeLength(); // len; // try this later...!
	b = edge->computeLength(); // edge->len; // try this later...!

	// Math.acos returns a value in the range [0, PI],
	// and input *MUST BE STRICTLY* in the range [-1, 1] !!!!!!!!
	// ^^^^^^^^
	double itemp = (a * a + b * b - c * c) / (2 * a * b);
	if ( itemp > 1.0 )
	{
		return 0;
	}
	else if ( itemp < -1.0 )
	{
		return  std::numbers::pi;
	}
	else
	{
		return acos( itemp );
	}
	
}

double 
Edge::computeCCWAngle( Edge* edge )
{
	auto n = commonNode( edge );
	double temp = computePosAngle( edge, n );

	MyVector thisVector = getVector( n );
	MyVector edgeVector = edge->getVector( n );

	if ( thisVector.isCWto( edgeVector ) )
	{
		return temp;
	}
	else
	{
		return PIx2 - temp;
	}
}

Node* 
Edge::commonNode( Edge* e )
{
	if ( hasNode( e->leftNode ) )
	{
		return e->leftNode;
	}
	else if ( hasNode( e->rightNode ) )
	{
		return e->rightNode;
	}
	else
	{
		return nullptr;
	}
}

Element*
Edge::commonElement( Edge* e )
{
	if ( hasElement( e->element1 ) )
	{
		return e->element1;
	}
	else if ( e->element2 != nullptr && hasElement( e->element2 ) )
	{
		return e->element2;
	}
	else
	{
		return nullptr;
	}
}

void
Edge::connectNodes()
{
	leftNode->edgeList.push_back( this );
	rightNode->edgeList.push_back( this );
}

void
Edge::disconnectNodes()
{
	auto Iter = std::find( leftNode->edgeList.begin(), leftNode->edgeList.end(), this );
	leftNode->edgeList.erase( Iter );
	
	Iter = std::find( rightNode->edgeList.begin(), rightNode->edgeList.end(), this );
	rightNode->edgeList.erase( Iter );
}

void
Edge::tryToDisconnectNodes()
{
	auto Iter = std::find( leftNode->edgeList.begin(), leftNode->edgeList.end(), this );
	if ( Iter != leftNode->edgeList.end() )
	{
		leftNode->edgeList.erase( Iter );
	}

	Iter = std::find( rightNode->edgeList.begin(), rightNode->edgeList.end(), this );
	if ( Iter != rightNode->edgeList.end() )
	{
		rightNode->edgeList.erase( Iter );
	}
}

void 
Edge::connectToTriangle( Triangle* triangle )
{
	if ( hasElement( triangle ) )
	{
		return;
	}
	if ( element1 == nullptr )
	{
		element1 = triangle;
	}
	else if ( element2 == nullptr )
	{
		element2 = triangle;
	}
	else
	{
		Msg::error( "Edge.connectToTriangle(..): An edge cannot be connected to more than two elements. edge= " + descr() );
	}
}

void
Edge::connectToQuad( Quad* q )
{
	if ( hasElement( q ) )
	{
		return;
	}
	if ( element1 == nullptr )
	{
		element1 = q;
	}
	else if ( element2 == nullptr )
	{
		element2 = q;
	}
	else
	{
		Msg::error( "Edge.connectToQuad(..):An edge cannot be connected to more than two elements." );
	}
}

void 
Edge::connectToElement( Element* elem )
{
	if ( hasElement( elem ) )
	{
		return;
	}
	if ( element1 == nullptr )
	{
		element1 = elem;
	}
	else if ( element2 == nullptr )
	{
		element2 = elem;
	}
	else
	{
		Msg::error( "Edge.connectToElement(..):An edge cannot be connected to more than two elements." );
	}
}

void
Edge::disconnectFromElement( Element* elem )
{
	if ( element1 == elem )
	{
		element1 = element2;
		element2 = nullptr;
	}
	else if ( element2 == elem )
	{
		element2 = nullptr;
	}
	else
	{
		Msg::error( "Edge " + descr() + " is not connected to element " + elem->descr() + "." );
	}
}

Node*
Edge::oppositeNode( Node* wrongNode )
{
	Node* candidate;
	Edge* otherEdge;

	// Pick one of the other edges in element1
	int ind = element1->indexOf( this );
	if ( ind == 0 || ind == 1 )
	{
		otherEdge = element1->edgeList[2];
	}
	else
	{
		otherEdge = element1->edgeList[1];
	}

	// This edge contains an opposite node... get this node....
	if ( otherEdge->leftNode != leftNode && otherEdge->leftNode != rightNode )
	{
		candidate = otherEdge->leftNode;
	}
	else
	{
		candidate = otherEdge->rightNode;
	}

	// Damn, it's the wrong node! Then we must go look in element2.
	if ( candidate->equals( wrongNode ) )
	{

		// Pick one of the other edges in element2
		ind = element2->indexOf( this );
		if ( ind == 0 || ind == 1 )
		{
			otherEdge = element2->edgeList[2];
		}
		else if ( ind == 2 )
		{
			otherEdge = element2->edgeList[1];
		}

		// This edge contains an opposite node
		// get this node....
		if ( otherEdge->leftNode != leftNode && otherEdge->leftNode != rightNode )
		{
			candidate = otherEdge->leftNode;
		}
		else if ( otherEdge->rightNode != leftNode && otherEdge->rightNode != rightNode )
		{
			candidate = otherEdge->rightNode;
		}
	}
	return candidate;
}

Edge* 
Edge::unitNormalAt( Node* n )
{
	Msg::debug( "Entering Edge.unitNormalAt(..)" );

	double xdiff = rightNode->x - leftNode->x;
	double ydiff = rightNode->y - leftNode->y;

	Msg::debug( "this: " + descr() + ", n: " + n->descr() );

	double c = sqrt( xdiff * xdiff + ydiff * ydiff );

	double xn = n->x - ydiff / c;
	double yn = n->y + xdiff / c;

	Node* newNode = new Node( xn, yn );

	Msg::debug( "Leaving Edge.unitNormalAt(..)" );
	return new Edge( n, newNode );
}

Edge*
Edge::getSwappedEdge()
{
	if ( element2 == nullptr )
	{
		Msg::warning( "getSwappedEdge: Cannot swap a boundary edge." );
		return nullptr;
	}
	if ( Element::IsAQuad( element1 ) || Element::IsAQuad( element2 ) )
	{
		Msg::warning( "getSwappedEdge: Edge must lie between two triangles." );
		return nullptr;
	}

	Node* n = this->oppositeNode( nullptr );
	Node* m = this->oppositeNode( n );
	Edge* swappedEdge = new Edge( n, m );

	return swappedEdge;
}

void
Edge::swapToAndSetElementsFor( Edge* e )
{
	Msg::debug("Entering Edge.swapToAndSetElementsFor(..)");
	if ( element1 == nullptr || element2 == nullptr )
	{
		Msg::error( "Edge.swapToAndSetElementsFor(..): both elements not set" );
	}

	Msg::debug( "element1: " + element1->descr() );
	Msg::debug( "element2: " + element2->descr() );

	Msg::debug( "...this: " + descr() );

	Edge* e1 = element1->neighborEdge( leftNode, this );
	Edge* e2 = element1->neighborEdge( e1->otherNode( leftNode ), e1 );
	Edge* e3 = element2->neighborEdge( rightNode, this );
	Edge* e4 = element2->neighborEdge( e3->otherNode( rightNode ), e3 );

	element2->disconnectEdges(); // important: element2 *first*, then element1
	element1->disconnectEdges();

	auto t1 = new Triangle( e, e2, e3 );
	auto t2 = new Triangle( e, e4, e1 );

	t1->connectEdges();
	t2->connectEdges();

	// Update edgeLists at this.leftNode and this.rightNode
	// and at e.leftNode and e.rightNode:
	disconnectNodes();
	e->connectNodes();

	Msg::debug( "Leaving Edge.swapToAndSetElementsFor(..)" );
}

MyVector
Edge::getVector()
{
	return MyVector{ *leftNode, *rightNode };
}

MyVector 
Edge::getVector( Node* origin )
{
	if ( origin->equals(leftNode) )
	{
		return MyVector( *leftNode, *rightNode );
	}
	else if ( origin->equals( rightNode ) )
	{
		return MyVector( *rightNode, *leftNode );
	}
	else
	{
		Msg::error( "Edge::getVector(Node): Node not an endpoint in this edge." );
		return MyVector();
	}
}

bool
Edge::bordersToTriangle()
{
	if ( Element::IsATriangle( element1 ) )
	{
		return true;
	}
	else if ( Element::IsATriangle( element2 ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool 
Edge::boundaryEdge()
{
	if ( element1 == nullptr || element2 == nullptr )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
Edge::boundaryOrTriangleEdge()
{
	if ( element1 == nullptr || element2 == nullptr || Element::IsATriangle( element1 ) || Element::IsATriangle( element2 ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
Edge::hasNode( Node* n )
{
	if ( leftNode->equals( n ) || rightNode->equals( n ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
Edge::hasElement( Element* elem )
{
	if ( element1 == elem || element2 == elem )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
Edge::hasFalseFrontNeighbor()
{
	if ( leftFrontNeighbor == nullptr || !leftFrontNeighbor->frontEdge || rightFrontNeighbor == nullptr || !rightFrontNeighbor->frontEdge )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
Edge::hasFrontNeighbor( Edge* e )
{
	if ( leftFrontNeighbor == e || rightFrontNeighbor == e )
	{
		return true;
	}
	else
	{
		return false;
	}
}

Node*
Edge::otherNode( const Node* n ) const
{
	if ( n->equals( leftNode ) )
	{
		return rightNode;
	}
	else if ( n->equals( rightNode ) )
	{
		return leftNode;
	}
	else
	{
		Msg::debug( "this: " + descr() );
		Msg::debug( "n: " + n->descr() );
		Msg::error( "Edge.otherNode(Node): n is not on this edge" );
		return nullptr;
	}
}

Node*
Edge::otherNodeGivenNewLength( double length, Node* nJ )
{
	// First find the angle between the existing edge and the x-axis:
	// Use a triangle containing one 90 degrees angle:
	MyVector v( *nJ, *otherNode( nJ ) );
	v.setLengthAndAngle( length, v.angle() );

	// Use this to create the new node:
	return new Node( v.origin->x + v.x, v.origin->y + v.y );
}

Node* 
Edge::upperNode()
{
	if ( leftNode->y >= rightNode->y )
	{
		return leftNode;
	}
	else
	{
		return rightNode;
	}
}

Node* 
Edge::lowerNode()
{
	if ( rightNode->y <= leftNode->y )
	{
		return rightNode;
	}
	else
	{
		return leftNode;
	}
}

bool
Edge::noTrianglesInOrbit( Edge* e, Quad* startQ )
{
	Msg::debug( "Entering Edge.noTrianglesInOrbit(..)" );
	Edge* curEdge = this;
	Element* curElem = startQ;
	Node* n = commonNode( e );
	if ( n == nullptr )
	{
		Msg::debug( "Leaving Edge.noTrianglesInOrbit(..), returns false" );
		return false;
	}
	if ( curEdge->boundaryEdge() )
	{
		curEdge = n->anotherBoundaryEdge( curEdge );
		curElem = curEdge->element1;
	}
	do
	{
		if ( Element::IsATriangle( curElem ) )
		{
			Msg::debug( "Leaving Edge.noTrianglesInOrbit(..), returns false" );
			return false;
		}
		curEdge = curElem->neighborEdge( n, curEdge );
		if ( curEdge->boundaryEdge() )
		{
			curEdge = n->anotherBoundaryEdge( curEdge );
			curElem = curEdge->element1;
		}
		else
		{
			curElem = curElem->neighbor( curEdge );
		}
	} while ( curEdge != e );

	Msg::debug( "Leaving Edge.noTrianglesInOrbit(..), returns true" );
	return true;
}

Edge*
Edge::findLeftFrontNeighbor( const std::vector<Edge*>& frontList2 )
{
	std::vector<Edge*> list;
	Edge* candidate = nullptr;
	double candAng = std::numeric_limits<double>::infinity(), curAng;
	Triangle* t = nullptr;

	for ( int j = 0; j < leftNode->edgeList.size(); j++ )
	{
		auto leftEdge = leftNode->edgeList[j];
		if ( leftEdge != this && leftEdge->isFrontEdge() )
		{
			list.push_back( leftEdge );
		}
	}
	if ( list.size() == 1 )
	{
		return list[0];
	}
	else if ( list.size() > 0 )
	{

		// Choose the front edge with the smallest angle
		t = getTriangleElement();

		for ( auto leftEdge : list )
		{
			curAng = sumAngle( t, leftNode, leftEdge );
			if ( curAng < candAng )
			{
				candAng = curAng;
				candidate = leftEdge;
			}
		}
		return candidate;
	}
	Msg::warning( "findLeftFrontNeighbor(..): Returning null" );
	return nullptr;
}

Edge*
Edge::findRightFrontNeighbor( const std::vector<Edge*>& frontList2 )
{
	std::vector<Edge*> list;
	Edge* candidate = nullptr;
	double candAng = std::numeric_limits<double>::infinity(), curAng;
	Triangle* t = nullptr;

	for ( int j = 0; j < rightNode->edgeList.size(); j++ )
	{
		auto rightEdge = rightNode->edgeList[j];
		if ( rightEdge != this && rightEdge->isFrontEdge() )
		{
			list.push_back( rightEdge );
		}
	}
	if ( list.size() == 1 )
	{
		return list[0];
	}
	else if ( list.size() > 0 )
	{
		t = getTriangleElement();
		for ( auto rightEdge : list )
		{
			curAng = sumAngle( t, rightNode, rightEdge );

			Msg::debug( "findRightFrontNeighbor(): Angle between edge this: " + descr() + " and edge " + rightEdge->descr() + ": " + std::to_string( curAng ) );
			if ( curAng < candAng )
			{
				candAng = curAng;
				candidate = rightEdge;
			}

			// if (noTrianglesInOrbit(rightEdge, q))
			// return rightEdge;
		}
		Msg::debug( "findRightFrontNeighbor(): Returning candidate " + candidate->descr() );
		return candidate;
	}
	Msg::warning( "findRightFrontNeighbor(..): List.size== " + std::to_string( list.size() ) + ". Returning null" );
	return nullptr;
}

void
Edge::setFrontNeighbor( Edge* e )
{
	if ( e->hasNode( leftNode ) )
	{
		leftFrontNeighbor = e;
	}
	else if ( e->hasNode( rightNode ) )
	{
		rightFrontNeighbor = e;
	}
	else
	{
		Msg::warning( "Edge.setFrontNeighbor(..): Could not set." );
	}
}

bool
Edge::setFrontNeighbors( const std::vector<Edge*>& frontList2 )
{
	Edge* lFront = findLeftFrontNeighbor( frontList2 );
	Edge* rFront = findRightFrontNeighbor( frontList2 );
	bool res = false;
	if ( lFront != leftFrontNeighbor || rFront != rightFrontNeighbor )
	{
		res = true;
	}
	leftFrontNeighbor = lFront;
	rightFrontNeighbor = rFront;

	if ( lFront != nullptr && !lFront->hasFrontNeighbor( this ) )
	{
		// res= true;
		lFront->setFrontNeighbor( this );
	}
	if ( rFront != nullptr && !rFront->hasFrontNeighbor( this ) )
	{
		// res= true;
		rFront->setFrontNeighbor( this );
	}
	return res;
}

void 
Edge::promoteToFront( int level, std::vector<Edge*>& frontList )
{
	if ( !frontEdge )
	{
		frontList.push_back( this );
		this->level = level;
		frontEdge = true;
	}
}

bool
Edge::removeFromFront( std::vector<Edge*>& frontList )
{
	auto i = std::find( frontList.begin(), frontList.end(), this );
	frontEdge = false;
	if ( i != frontList.end() )
	{
		frontList.erase( i );
		return true;
	}
	else
	{
		return false;
	}
}

Edge* 
Edge::splitTrianglesAt( Node* nN, Node* ben,
						std::vector<Triangle*>& triangleList,
						std::vector<Edge*>& edgeList )
{
	Msg::debug("Entering Edge.splitTrianglesAt(..)");
	Edge* eK1 = new Edge( leftNode, nN );
	Edge* eK2 = new Edge( rightNode, nN );

	Triangle* tri1 = static_cast<Triangle*>(element1);
	Triangle* tri2 = static_cast<Triangle*>(element2);

	Node* n1 = tri1->oppositeOfEdge( this );
	Node* n2 = tri2->oppositeOfEdge( this );
	Edge* diagonal1 = new Edge( nN, n1 );
	Edge* diagonal2 = new Edge( nN, n2 );

	Edge* e12 = tri1->neighborEdge( leftNode, this );
	Edge* e13 = tri1->neighborEdge( rightNode, this );
	Edge* e22 = tri2->neighborEdge( leftNode, this );
	Edge* e23 = tri2->neighborEdge( rightNode, this );

	Triangle* t11 = new Triangle( diagonal1, e12, eK1 );
	Triangle* t12 = new Triangle( diagonal1, e13, eK2 );
	Triangle* t21 = new Triangle( diagonal2, e22, eK1 );
	Triangle* t22 = new Triangle( diagonal2, e23, eK2 );

	// Update the nodes' edgeLists
	disconnectNodes();
	eK1->connectNodes();
	eK2->connectNodes();
	diagonal1->connectNodes();
	diagonal2->connectNodes();

	// Disconnect old Triangles
	tri1->disconnectEdges();
	tri2->disconnectEdges();

	// Connect Edges to new Triangles
	t11->connectEdges();
	t12->connectEdges();
	t21->connectEdges();
	t22->connectEdges();

	// Update "global" lists
	edgeList.erase( std::find( edgeList.begin(), edgeList.end(), this ) );
	edgeList.push_back( eK1 );
	edgeList.push_back( eK2 );
	edgeList.push_back( diagonal1 );
	edgeList.push_back( diagonal2 );

	triangleList.erase( std::find( triangleList.begin(), triangleList.end(), tri1 ) );
	triangleList.erase( std::find( triangleList.begin(), triangleList.end(), tri2 ) );
	triangleList.push_back( t11 );
	triangleList.push_back( t12 );
	triangleList.push_back( t21 );
	triangleList.push_back( t22 );

	Msg::debug( "...Created triangle " + t11->descr() );
	Msg::debug( "...Created triangle " + t12->descr() );
	Msg::debug( "...Created triangle " + t21->descr() );
	Msg::debug( "...Created triangle " + t22->descr() );

	Msg::debug( "Leaving Edge.splitTrianglesAt(..)" );
	if ( eK1->hasNode( ben ) )
	{
		return eK1;
	}
	else if ( eK2->hasNode( ben ) )
	{
		return eK2;
	}
	else
	{
		Msg::error( "" );
		return nullptr;
	}
}

Edge* 
Edge::splitTrianglesAtMyMidPoint( std::vector<Triangle*>& triangleList,
								  std::vector<Edge*>& edgeList,
								  std::vector<Node*>& nodeList,
								  Edge* baseEdge )
{
	Msg::debug("Entering Edge.splitTrianglesAtMyMidPoint(..).");

	Edge* lowerEdge;
	Node* ben = baseEdge->commonNode( this );
	Node* mid = midPoint();
	nodeList.push_back( mid );
	mid->color = 3;//java.awt.Color.blue;

	Msg::debug( "Splitting edge " + descr() );
	Msg::debug( "Creating new Node: " + mid->descr() );

	lowerEdge = splitTrianglesAt( mid, ben, triangleList, edgeList );

	Msg::debug( "Leaving Edge.splitTrianglesAtMyMidPoint(..)." );
	return lowerEdge;
}

Edge*
Edge::nextQuadEdgeAt( Node* n, Element* startElem )
{
	Msg::debug( "Entering Edge.nextQuadEdgeAt(..)" );
	Element* elem;
	Edge* e;
	int i = 3;

	e = startElem->neighborEdge( n, this );
	elem = startElem->neighbor( e );

	while ( elem && !Element::IsAQuad( elem ) && elem != startElem )
	{
		e = elem->neighborEdge( n, e );
		Msg::debug( "..." + std::to_string( i ) );
		i++;
		elem = elem->neighbor( e );
	}
	Msg::debug( "Leaving Edge.nextQuadEdgeAt(..)" );
	if ( Element::IsAQuad( elem ) && elem != startElem )
	{
		return e;
	}
	else
	{
		return nullptr;
	}
}

Quad* 
Edge::getQuadElement()
{
	if ( Element::IsAQuad( element1 ) )
	{
		return static_cast<Quad*>(element1);
	}
	else if ( Element::IsAQuad( element2 ) )
	{
		return static_cast<Quad*>(element2);
	}
	else
	{
		return nullptr;
	}
}

Triangle* 
Edge::getTriangleElement()
{
	if ( Element::IsATriangle( element1 ) )
	{
		return static_cast<Triangle*>(element1);
	}
	else if ( Element::IsATriangle( element2 ) )
	{
		return static_cast<Triangle*>(element2);
	}
	else
	{
		return nullptr;
	}
}

Quad* 
Edge::getQuadWithEdge( Edge* e )
{
	if ( Element::IsAQuad( element1 ) && element1->hasEdge( e ) )
	{
		return static_cast<Quad*>(element1);
	}
	else if ( Element::IsAQuad( element2 ) && element2->hasEdge( e ) )
	{
		return static_cast<Quad*>(element2);
	}
	else
	{
		return nullptr;
	}
}

Edge* 
Edge::frontNeighborAt( Node* n )
{
	if ( leftFrontNeighbor != nullptr && commonNode( leftFrontNeighbor ) == n )
	{
		return leftFrontNeighbor;
	}
	else if ( rightFrontNeighbor != nullptr && commonNode( rightFrontNeighbor ) == n )
	{
		return rightFrontNeighbor;
	}
	else
	{
		return nullptr;
	}
}

Edge*
Edge::nextFrontNeighbor( Edge* prev )
{
	if ( leftFrontNeighbor != prev )
	{
		return leftFrontNeighbor;
	}
	else if ( rightFrontNeighbor != prev )
	{
		return rightFrontNeighbor;
	}
	else
	{
		Msg::error( "Edge.nextFrontNeighbor(Edge): Cannot find a suitable next edge." );
		return nullptr;
	}
}

Edge* 
Edge::trueFrontNeighborAt( Node* n )
{
	Element* curElem = getTriangleElement();
	Edge* curEdge = this;

	if ( !hasNode( n ) )
	{
		Msg::error( "trueFrontNeighborAt(..): this Edge hasn't got Node " + n->descr() );
	}

	do
	{
		curEdge = curElem->neighborEdge( n, curEdge );
		curElem = curElem->neighbor( curEdge );
	} while ( !curEdge->isFrontEdge() );

	return curEdge;
}

std::string 
Edge::toString()
{
	return descr();
}