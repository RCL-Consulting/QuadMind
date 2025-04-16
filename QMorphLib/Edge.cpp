#include "pch.h"
#include "Edge.h"

#include "Node.h"
#include "MyVector.h"

#include "Numbers.h"
#include "Msg.h"
#include "Element.h"

#include <iostream>

std::array<ArrayList<std::shared_ptr<Edge>>, 3> Edge::stateList;

Edge::Edge( const std::shared_ptr<Node>& node1,
			const std::shared_ptr<Node>& node2 )
{
	if ( (node1->x < node2->x) || (rcl::equal(node1->x, node2->x) && node1->y > node2->y) )
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

Edge::Edge( const Edge& e )
{
	leftNode = e.leftNode;
	rightNode = e.rightNode;
	len = e.len;
	element1 = e.element1;
	element2 = e.element2;
}

std::shared_ptr<Edge>
Edge::copy()
{
	const auto& e = *this;
	return std::make_shared<Edge>( e );
}

void 
Edge::clearStateList()
{
	stateList[0].clear();
	stateList[1].clear();
	stateList[2].clear();
}

bool 
Edge::operator==( const Edge& e ) const
{
	return  leftNode->equals( e.leftNode ) && rightNode->equals( e.rightNode );
}

bool
Edge::equals( const std::shared_ptr<Constants>& elem ) const
{
	auto edge = std::dynamic_pointer_cast<Edge>(elem);
	if ( edge )
	{
		return *this == *edge;
	}
	return false;
}

double 
Edge::computeLength()
{
	double xdiff = rightNode->x - leftNode->x;
	double ydiff = rightNode->y - leftNode->y;
	return std::sqrt( xdiff * xdiff + ydiff * ydiff );
}

bool
Edge::removeFromStateList()
{
	int state = getState();
	auto i = stateList[state].indexOf( shared_from_this() );
	if ( i == -1 )
	{
		return false;
	}
	stateList[state].remove( i );
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

bool 
Edge::removeFromStateList( int state )
{
	auto i = stateList[state].indexOf( shared_from_this() );
	if ( i == -1 )
	{
		return false;
	}
	stateList[state].remove( i );
	return true;
}

bool
Edge::alterLeftState( bool newLeftState )
{
	int state = getState();
	auto i = stateList[state].indexOf( shared_from_this() );
	if ( i == -1 )
	{
		return false;
	}
	leftSide = newLeftState;
	int newState = getState();
	if ( state != newState )
	{
		stateList[state].remove( i );
		stateList[newState].add( shared_from_this() );
	}
	return true;
}

bool
Edge::alterRightState( bool newRightState )
{
	int state = getState();
	auto i = stateList[state].indexOf( shared_from_this() );
	if ( i == -1 )
	{
		return false;
	}
	rightSide = newRightState;
	int newState = getState();
	if ( state != newState )
	{
		stateList[state].remove( i );
		stateList[newState].add( shared_from_this() );
	}
	return true;
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Edge>
Edge::evalPotSideEdge( const std::shared_ptr<Edge>& frontNeighbor,
					   const std::shared_ptr<Node>& n )
{
	return nullptr;
	/*Msg::debug("Entering Edge.evalPotSideEdge(..)");
	Element tri = getTriangleElement(), quad = getQuadElement();
	double ang;

	if ( tri != null )
	{
		ang = sumAngle( tri, n, frontNeighbor );
	}
	else
	{
		ang = PIx2 - sumAngle( quad, n, frontNeighbor );
	}

	Msg.debug( "sumAngle(..) between " + descr() + " and " + frontNeighbor.descr() + ": " + Math.toDegrees( ang ) );

	Msg.debug( "Leaving Edge.evalPotSideEdge(..)" );
	if ( ang < PIx3div4 )
	{ // if (ang< PIdiv2+EPSILON) // Could this be better?
		return frontNeighbor;
	}
	else
	{
		return null;
	}*/
}

//TODO: Implement this method
//TODO: Tests
void 
Edge::classifyStateOfFrontEdge()
{
	/*Msg::debug("Entering Edge.classifyStateOfFrontEdge()");
	Msg::debug( "this: " + descr() );
	final Edge lfn = leftFrontNeighbor, rfn = rightFrontNeighbor;

	Edge l, r;

	// Alter states and side Edges on left side:
	l = evalPotSideEdge( lfn, leftNode );
	if ( l != null )
	{
		leftSide = true;
		if ( leftNode == lfn.leftNode )
		{
			lfn.alterLeftState( true );
		}
		else
		{
			lfn.alterRightState( true );
		}
	}
	else
	{
		leftSide = false;
		if ( leftNode == lfn.leftNode )
		{
			lfn.alterLeftState( false );
		}
		else
		{
			lfn.alterRightState( false );
		}
	}

	// Alter states and side Edges on right side:
	r = evalPotSideEdge( rfn, rightNode );
	if ( r != null )
	{
		rightSide = true;
		if ( rightNode == rfn.leftNode )
		{
			rfn.alterLeftState( true );
		}
		else
		{
			rfn.alterRightState( true );
		}
	}
	else
	{
		rightSide = false;
		if ( rightNode == rfn.leftNode )
		{
			rfn.alterLeftState( false );
		}
		else
		{
			rfn.alterRightState( false );
		}
	}

	// Add this to a stateList:
	stateList.get( getState() ).add( this );
	Msg.debug( "Leaving Edge.classifyStateOfFrontEdge()" );*/
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Edge>
Edge::getNextFront()
{
	return nullptr;
	/*std::shared_ptr<Edge> current, selected = nullptr;
	int selState, curState = 2, i;

	// Select a front preferrably in stateList[2]

	while ( curState >= 0 && selected == nullptr )
	{
		for ( i = 0; i < stateList[curState].size(); i++ )
		{
			current = stateList[curState].get( i );
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
		current = stateList[selState].get( i );

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
			if ( selected->length() > selected->rightFrontNeighbor.length() && selected->rightFrontNeighbor->selectable )
			{
				return selected->rightFrontNeighbor;
			}
		}
	}
	return selected;*/
}

bool 
Edge::replaceNode( const std::shared_ptr<Node>& n1,
				   const std::shared_ptr<Node>& n2 )
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

//TODO: Implement this method
//TODO: Tests
void
Edge::seamWith( const std::shared_ptr<Edge>& e )
{
	/*Node nK = commonNode(e);
	Node nKp1 = otherNode( nK ), nKm1 = e.otherNode( nK ), other;
	boolean found = false;
	Edge eI, eJ;

	for ( int i = 0; i < nKm1.edgeList.size(); i++ )
	{
		eI = nKm1.edgeList.get( i );
		other = eI.otherNode( nKm1 );

		if ( other != nKp1 )
		{
			for ( int j = 0; j < nKp1.edgeList.size(); j++ )
			{
				eJ = nKp1.edgeList.get( j );

				if ( other == eJ.otherNode( nKp1 ) )
				{
					found = true;

					other.edgeList.remove( other.edgeList.indexOf( eI ) );

					if ( eI.element1.firstNode == nKm1 )
					{ // Don't forget firstNode!!
						eI.element1.firstNode = nKp1;
					}
					eI.element1.replaceEdge( eI, eJ );
					eJ.connectToElement( eI.element1 );
					break;
				}
			}
			if ( !found )
			{
				if ( eI.element1.firstNode == nKm1 )
				{ // Don't forget firstNode!!
					eI.element1.firstNode = nKp1;
				}
				if ( eI.element2.firstNode == nKm1 )
				{ // Don't forget firstNode!!
					eI.element2.firstNode = nKp1;
				}

				eI.replaceNode( nKm1, nKp1 );
				nKp1.edgeList.add( eI );
			}
			else
			{
				found = false;
			}
		}
		else
		{
			// Remove the edge between eKp1 and eKm1 (from the edgeList of eKp1)
			nKp1.edgeList.remove( nKp1.edgeList.indexOf( eI ) );
		}
	}*/
}

std::shared_ptr<Node> 
Edge::midPoint()
{
	double xDiff = rightNode->x - leftNode->x;
	double yDiff = rightNode->y - leftNode->y;

	return std::make_shared<Node>( leftNode->x + xDiff * 0.5, leftNode->y + yDiff * 0.5 );
}

double
Edge::length( double x1, double y1, double x2, double y2 )
{
	double xdiff = x2 - x1;
	double ydiff = y2 - y1;
	return std::sqrt( xdiff * xdiff + ydiff * ydiff );
}

double
Edge::length( const std::shared_ptr<Node>& node1,
			  const std::shared_ptr<Node>& node2 )
{
	return length( node1->x, node1->y, node2->x, node2->y );
}

//TODO: Implement this method
//TODO: Tests
double
Edge::angleAt( const std::shared_ptr<Node>& n )
{
	return 0.0;
	/*// Math.acos returns values in the range 0.0 through pi, avoiding neg numbers.
	// If this is CW to x-axis, then return pos acos, else return neg acos
	// double x= leftNode.x-rightNode.x;
	// double y= leftNode.y-rightNode.y;
	Node other = otherNode( n );

	double x = n.x - other.x;
	double y = n.y - other.y;

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
		double hyp = Math.sqrt( x * x + y * y );

		if ( x > 0 )
		{
			if ( y > 0 )
			{
				return Math.PI + Math.acos( x / hyp );
			}
			else
			{
				return Math.PI - Math.acos( x / hyp );
			}
		}
		else
		{
			if ( y > 0 )
			{
				return Math.PI + Math.PI - Math.acos( -x / hyp );
			}
			else
			{
				return Math.acos( -x / hyp );
			}
		}
	}*/
}

std::string 
Edge::descr()
{
	return "(" + std::to_string( leftNode->x ) + ", " + std::to_string( leftNode->y ) + "), (" +
		std::to_string( rightNode->x ) + ", " + std::to_string( rightNode->y ) + ")";
}

//TODO: Implement this method
//TODO: Tests
double 
Edge::sumAngle( const std::shared_ptr<Element>& sElem,
				const std::shared_ptr<Node>& n,
				const std::shared_ptr<Edge>& eEdge )
{
	return 0.0;
	/*Msg::debug("Entering sumAngle(..)");
	Msg::debug( "this: " + descr() );
	if ( sElem != nullptr )
	{
		Msg::debug( "sElem: " + sElem->descr() );
	}
	if ( n != null )
	{
		Msg.debug( "n: " + n.descr() );
	}
	if ( eEdge != null )
	{
		Msg.debug( "eEdge: " + eEdge.descr() );
	}

	Element curElem = sElem;
	Edge curEdge = this;
	double ang = 0, iang = 0;
	double d;

	while ( curEdge != eEdge && curElem != null )
	{
		d = curElem.angle( curEdge, n );
		ang += d;
		curEdge = curElem.neighborEdge( n, curEdge );
		curElem = curElem.neighbor( curEdge );
	}

	// If n is a boundaryNode, the situation gets more complicated:
	if ( curEdge != eEdge )
	{
		// Sum the "internal" angles:
		curEdge = this;
		curElem = sElem.neighbor( curEdge );

		while ( curEdge != eEdge && curElem != null )
		{
			d = curElem.angle( curEdge, n );
			iang += d;
			curEdge = curElem.neighborEdge( n, curEdge );
			curElem = curElem.neighbor( curEdge );
		}
		ang = PIx2 - iang;
	}
	Msg.debug( "Leaving sumAngle(..), returning " + Math.toDegrees( ang ) );
	return ang;*/
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Edge>
Edge::firstFrontEdgeAt( const std::shared_ptr<Element>& sElem,
						const std::shared_ptr<Node>& n )
{
	return nullptr;
	/*auto curElem = sElem;
	auto curEdge = shared_from_this();

	while ( !curEdge->frontEdge )
	{
		curEdge = curElem->neighborEdge( n, curEdge );
		curElem = curElem->neighbor( curEdge );
	}
	return curEdge;*/
}

double 
Edge::computePosAngle( const std::shared_ptr<Edge>& edge,
					   const std::shared_ptr<Node>& n )
{
	double a, b, c;
	if ( edge == shared_from_this() )
	{
		Msg::warning( "Edge.computePosAngle(..): The parameter Edge is the same as this Edge." );
		return 2 * PI;
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
	b = edge->computeLength(); // edge.len; // try this later...!

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
		return PI;
	}
	else
	{
		return std::acos( itemp );
	}
}

//TODO: Implement this method
//TODO: Tests
double
Edge::computeCCWAngle( const std::shared_ptr<Edge>& edge )
{
	return 0.0;
	/*Node n = commonNode(edge);
	double temp = computePosAngle( edge, n );

	MyVector thisVector = getVector( n );
	MyVector edgeVector = edge.getVector( n );

	if ( thisVector.isCWto( edgeVector ) )
	{
		return temp;
	}
	else
	{
		return PIx2 - temp;
	}*/
}

std::shared_ptr<Node>
Edge::commonNode( const std::shared_ptr<Edge> e )
{
	if ( hasNode(e->leftNode) )
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

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Element> 
Edge::commonElement( const std::shared_ptr<Edge>& e )
{
	return nullptr;
	/*if ( hasElement(e.element1) )
	{
		return e.element1;
	}
	else if ( e.element2 != null && hasElement( e.element2 ) )
	{
		return e.element2;
	}
	else
	{
		return null;
	}*/
}

void 
Edge::connectNodes()
{
	leftNode->edgeList.add( shared_from_this() );
	rightNode->edgeList.add( shared_from_this() );
}

void
Edge::disconnectNodes()
{
	leftNode->edgeList.remove( leftNode->edgeList.indexOf( shared_from_this() ) );
	rightNode->edgeList.remove( rightNode->edgeList.indexOf( shared_from_this() ) );
}

void
Edge::tryToDisconnectNodes()
{
	auto i = leftNode->edgeList.indexOf( shared_from_this() );
	if ( i != -1 )
	{
		leftNode->edgeList.remove( i );
	}
	i = rightNode->edgeList.indexOf( shared_from_this() );
	if ( i != -1 )
	{
		rightNode->edgeList.remove( i );
	}
}

//TODO: Implement this method
//TODO: Tests
void 
Edge::connectToTriangle( const std::shared_ptr<Triangle>& triangle )
{
	/*if ( hasElement(triangle) )
	{
		return;
	}
	if ( element1 == null )
	{
		element1 = triangle;
	}
	else if ( element2 == null )
	{
		element2 = triangle;
	}
	else
	{
		Msg.error( "Edge.connectToTriangle(..): An edge cannot be connected to more than two elements. edge= " + descr() );
	}*/
}

//TODO: Implement this method
//TODO: Tests
void 
Edge::connectToQuad( const std::shared_ptr<Quad>& q )
{
	/*if ( hasElement(q) )
	{
		return;
	}
	if ( element1 == null )
	{
		element1 = q;
	}
	else if ( element2 == null )
	{
		element2 = q;
	}
	else
	{
		Msg.error( "Edge.connectToQuad(..):An edge cannot be connected to more than two elements." );
	}*/
}

//TODO: Implement this method
//TODO: Tests
void
Edge::connectToElement( const std::shared_ptr<Element>& elem )
{
	/*if ( hasElement(elem) )
	{
		return;
	}
	if ( element1 == null )
	{
		element1 = elem;
	}
	else if ( element2 == null )
	{
		element2 = elem;
	}
	else
	{
		Msg.error( "Edge.connectToElement(..):An edge cannot be connected to more than two elements." );
	}*/
}

//TODO: Implement this method
//TODO: Tests
void 
Edge::disconnectFromElement( const std::shared_ptr<Element>& elem )
{
	/*if ( element1 == elem )
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
	}*/
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Node> 
Edge::oppositeNode( const std::shared_ptr<Node>& wrongNode )
{
	return nullptr;
	/*std::shared_ptr<Node> candidate = nullptr;
	std::shared_ptr<Edge> otherEdge = nullptr;

	// Pick one of the other edges in element1
	int ind = element1->indexOf( this );
	if ( ind == 0 || ind == 1 )
	{
		otherEdge = element1.edgeList[2];
	}
	else
	{
		otherEdge = element1.edgeList[1];
	}

	// This edge contains an opposite node... get this node....
	if ( otherEdge.leftNode != leftNode && otherEdge.leftNode != rightNode )
	{
		candidate = otherEdge.leftNode;
	}
	else
	{
		candidate = otherEdge.rightNode;
	}

	// Damn, it's the wrong node! Then we must go look in element2.
	if ( candidate == wrongNode )
	{

		// Pick one of the other edges in element2
		ind = element2.indexOf( this );
		if ( ind == 0 || ind == 1 )
		{
			otherEdge = element2.edgeList[2];
		}
		else if ( ind == 2 )
		{
			otherEdge = element2.edgeList[1];
		}

		// This edge contains an opposite node
		// get this node....
		if ( otherEdge.leftNode != leftNode && otherEdge.leftNode != rightNode )
		{
			candidate = otherEdge.leftNode;
		}
		else if ( otherEdge.rightNode != leftNode && otherEdge.rightNode != rightNode )
		{
			candidate = otherEdge.rightNode;
		}
	}
	return candidate;*/
}

std::shared_ptr<Edge>
Edge::unitNormalAt( const std::shared_ptr<Node>& n )
{
	Msg::debug( "Entering Edge.unitNormalAt(..)" );

	double xdiff = rightNode->x - leftNode->x;
	double ydiff = rightNode->y - leftNode->y;

	Msg::debug( "this: " + descr() + ", n: " + n->descr() );

	double c = std::sqrt( xdiff * xdiff + ydiff * ydiff );

	double xn = n->x - ydiff / c;
	double yn = n->y + xdiff / c;

	auto newNode = std::make_shared<Node>( xn, yn );

	Msg::debug( "Leaving Edge.unitNormalAt(..)" );
	return std::make_shared<Edge>( n, newNode );
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Edge> 
Edge::getSwappedEdge()
{
	return nullptr;
	/*if ( element2 == nullptr )
	{
		Msg::warning( "getSwappedEdge: Cannot swap a boundary edge." );
		return nullptr;
	}
	if ( element1 instanceof Quad || element2 instanceof Quad )
	{
		Msg.warning( "getSwappedEdge: Edge must lie between two triangles." );
		return null;
	}

	Node n = this.oppositeNode( null );
	Node m = this.oppositeNode( n );
	Edge swappedEdge = new Edge( n, m );

	return swappedEdge;*/
}

//TODO: Implement this method
//TODO: Tests
void 
Edge::swapToAndSetElementsFor( const std::shared_ptr<Edge>& e )
{
	/*Msg.debug("Entering Edge.swapToAndSetElementsFor(..)");
	if ( element1 == null || element2 == null )
	{
		Msg.error( "Edge.swapToAndSetElementsFor(..): both elements not set" );
	}

	Msg.debug( "element1: " + element1.descr() );
	Msg.debug( "element2: " + element2.descr() );

	Msg.debug( "...this: " + descr() );

	Edge e1 = element1.neighborEdge( leftNode, this );
	Edge e2 = element1.neighborEdge( e1.otherNode( leftNode ), e1 );
	Edge e3 = element2.neighborEdge( rightNode, this );
	Edge e4 = element2.neighborEdge( e3.otherNode( rightNode ), e3 );

	element2.disconnectEdges(); // important: element2 *first*, then element1
	element1.disconnectEdges();

	Triangle t1 = new Triangle( e, e2, e3 );
	Triangle t2 = new Triangle( e, e4, e1 );

	t1.connectEdges();
	t2.connectEdges();

	// Update edgeLists at this.leftNode and this.rightNode
	// and at e.leftNode and e.rightNode:
	disconnectNodes();
	e.connectNodes();

	Msg.debug( "Leaving Edge.swapToAndSetElementsFor(..)" );*/
}

MyVector 
Edge::getVector()
{
	return MyVector( leftNode, rightNode );
}

MyVector
Edge::getVector( std::shared_ptr<Node>& origin )
{
	if ( origin->equals( leftNode ) )
	{
		return MyVector( leftNode, rightNode );
	}
	else if ( origin->equals( rightNode ) )
	{
		return MyVector( rightNode, leftNode );
	}
	else
	{
		Msg::error( "Edge::getVector(Node): Node not an endpoint in this edge." );
		throw std::runtime_error( "Node not an endpoint in this edge." );
	}
}

//TODO: Implement this method
//TODO: Tests
bool
Edge::bordersToTriangle()
{
	return false;
	/*if ( element1 instanceof Triangle )
	{
		return true;
	}
	else if ( element2 != null && element2 instanceof Triangle )
	{
		return true;
	}
	else
	{
		return false;
	}*/
}

//TODO: Tests
bool 
Edge::boundaryEdge()
{
	if ( element1 == nullptr || element2 == nullptr )
	{
		return true;
	}

	return false;
}

//TODO: Implement this method
//TODO: Tests
bool 
Edge::boundaryOrTriangleEdge()
{
	return false;
	/*if ( element1 == nullptr || element2 == nullptr || element1 instanceof Triangle || element2 instanceof Triangle )
	{
		return true;
	}
	else
	{
		return false;
	}*/
}

bool 
Edge::hasNode( const std::shared_ptr<Node>& n )
{
	if ( leftNode == n || rightNode == n )
	{
		return true;
	}
	return false;
}

//TODO: Tests
bool 
Edge::hasElement( const std::shared_ptr<Element>& elem )
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
Edge::hasFrontNeighbor( const std::shared_ptr<Edge>& e )
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

std::shared_ptr<Node> 
Edge::otherNode( const std::shared_ptr<Node>& n )
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

std::shared_ptr<Node>
Edge::otherNodeGivenNewLength( double length,
							   const std::shared_ptr<Node>& nJ )
{
	// First find the angle between the existing edge and the x-axis:
	// Use a triangle containing one 90 degrees angle:
	MyVector v( nJ, otherNode( nJ ) );
	v.setLengthAndAngle( length, v.angle() );

	// Use this to create the new node:
	return std::make_shared<Node>( v.origin->x + v.x, v.origin->y + v.y );
}

std::shared_ptr<Node>
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

std::shared_ptr<Node> 
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

//TODO: Implement this function
//TODO: Tests
bool noTrianglesInOrbit( const std::shared_ptr<Edge>& e,
						 const std::shared_ptr<Quad>& startQ )
{
	return false;
	/*Msg.debug("Entering Edge.noTrianglesInOrbit(..)");
	Edge curEdge = this;
	Element curElem = startQ;
	Node n = commonNode( e );
	if ( n == null )
	{
		Msg.debug( "Leaving Edge.noTrianglesInOrbit(..), returns false" );
		return false;
	}
	if ( curEdge.boundaryEdge() )
	{
		curEdge = n.anotherBoundaryEdge( curEdge );
		curElem = curEdge.element1;
	}
	do
	{
		if ( curElem instanceof Triangle )
		{
			Msg.debug( "Leaving Edge.noTrianglesInOrbit(..), returns false" );
			return false;
		}
		curEdge = curElem.neighborEdge( n, curEdge );
		if ( curEdge.boundaryEdge() )
		{
			curEdge = n.anotherBoundaryEdge( curEdge );
			curElem = curEdge.element1;
		}
		else
		{
			curElem = curElem.neighbor( curEdge );
		}
	} while ( curEdge != e );

	Msg.debug( "Leaving Edge.noTrianglesInOrbit(..), returns true" );
	return true;*/
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Edge>
Edge::findLeftFrontNeighbor( const ArrayList<std::shared_ptr<Edge>>& frontList2 )
{
	return nullptr;
	/*List<Edge> list = new ArrayList<>();
	Edge candidate = null;
	double candAng = Double.POSITIVE_INFINITY, curAng;
	Triangle t;

	for ( int j = 0; j < leftNode.edgeList.size(); j++ )
	{
		Edge leftEdge = leftNode.edgeList.get( j );
		if ( leftEdge != this && leftEdge.isFrontEdge() )
		{
			list.add( leftEdge );
		}
	}
	if ( list.size() == 1 )
	{
		return list.get( 0 );
	}
	else if ( list.size() > 0 )
	{

		// Choose the front edge with the smallest angle
		t = getTriangleElement();

		for ( Edge leftEdge : list )
		{
			curAng = sumAngle( t, leftNode, leftEdge );
			if ( curAng < candAng )
			{
				candAng = curAng;
				candidate = leftEdge;
			}

			// if (noTrianglesInOrbit(leftEdge, q))
			// return leftEdge;
		}
		return candidate;
	}
	Msg.warning( "findLeftFrontNeighbor(..): Returning null" );
	return null;*/
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Edge> 
Edge::findRightFrontNeighbor( const ArrayList<std::shared_ptr<Edge>>& frontList2 )
{
	return nullptr;
	/*List<Edge> list = new ArrayList<>();
	Edge candidate = null;
	double candAng = Double.POSITIVE_INFINITY, curAng;
	Triangle t;

	for ( int j = 0; j < rightNode.edgeList.size(); j++ )
	{
		Edge rightEdge = rightNode.edgeList.get( j );
		if ( rightEdge != this && rightEdge.isFrontEdge() )
		{
			list.add( rightEdge );
		}
	}
	if ( list.size() == 1 )
	{
		return list.get( 0 );
	}
	else if ( list.size() > 0 )
	{
		t = getTriangleElement();
		for ( Edge rightEdge : list )
		{
			curAng = sumAngle( t, rightNode, rightEdge );

			Msg.debug( "findRightFrontNeighbor(): Angle between edge this: " + descr() + " and edge " + rightEdge.descr() + ": " + curAng );
			if ( curAng < candAng )
			{
				candAng = curAng;
				candidate = rightEdge;
			}

			// if (noTrianglesInOrbit(rightEdge, q))
			// return rightEdge;
		}
		Msg.debug( "findRightFrontNeighbor(): Returning candidate " + candidate.descr() );
		return candidate;
	}
	Msg.warning( "findRightFrontNeighbor(..): List.size== " + list.size() + ". Returning null" );
	return null;*/
}

void
Edge::setFrontNeighbor( const std::shared_ptr<Edge>& e )
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

//TODO: Implement this method
//TODO: Tests
bool 
Edge::setFrontNeighbors( const ArrayList<std::shared_ptr<Edge>>& frontList2 )
{
	return false;
	/*auto lFront = findLeftFrontNeighbor(frontList2);
	auto rFront = findRightFrontNeighbor( frontList2 );
	boolean res = false;
	if ( lFront != leftFrontNeighbor || rFront != rightFrontNeighbor )
	{
		res = true;
	}
	leftFrontNeighbor = lFront;
	rightFrontNeighbor = rFront;

	if ( lFront != null && !lFront.hasFrontNeighbor( this ) )
	{
		// res= true;
		lFront.setFrontNeighbor( this );
	}
	if ( rFront != null && !rFront.hasFrontNeighbor( this ) )
	{
		// res= true;
		rFront.setFrontNeighbor( this );
	}
	return res;*/
}

void 
Edge::promoteToFront( int level,
					  ArrayList<std::shared_ptr<Edge>>& frontList )
{
	if ( !frontEdge )
	{
		frontList.add( shared_from_this() );
		this->level = level;
		frontEdge = true;
	}
}

bool
Edge::removeFromFront( ArrayList<std::shared_ptr<Edge>>& frontList2 )
{
	auto i = frontList2.indexOf( shared_from_this() );
	frontEdge = false;
	if ( i != -1 )
	{
		frontList2.remove( i );
		return true;
	}
	else
	{
		return false;
	}
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Edge>
Edge::splitTrianglesAt( const std::shared_ptr<Node>& nN,
						const std::shared_ptr<Node>& ben,
						const ArrayList<std::shared_ptr<Triangle>>& triangleList,
						const ArrayList<std::shared_ptr<Edge>>& edgeList,
						const ArrayList<std::shared_ptr<Node>>& nodeList )
{
	return nullptr;
	/*Msg.debug("Entering Edge.splitTrianglesAt(..)");
	Edge eK1 = new Edge( leftNode, nN );
	Edge eK2 = new Edge( rightNode, nN );

	Triangle tri1 = (Triangle)element1;
	Triangle tri2 = (Triangle)element2;

	Node n1 = tri1.oppositeOfEdge( this );
	Node n2 = tri2.oppositeOfEdge( this );
	Edge diagonal1 = new Edge( nN, n1 );
	Edge diagonal2 = new Edge( nN, n2 );

	Edge e12 = tri1.neighborEdge( leftNode, this );
	Edge e13 = tri1.neighborEdge( rightNode, this );
	Edge e22 = tri2.neighborEdge( leftNode, this );
	Edge e23 = tri2.neighborEdge( rightNode, this );

	Triangle t11 = new Triangle( diagonal1, e12, eK1 );
	Triangle t12 = new Triangle( diagonal1, e13, eK2 );
	Triangle t21 = new Triangle( diagonal2, e22, eK1 );
	Triangle t22 = new Triangle( diagonal2, e23, eK2 );

	// Update the nodes' edgeLists
	disconnectNodes();
	eK1.connectNodes();
	eK2.connectNodes();
	diagonal1.connectNodes();
	diagonal2.connectNodes();

	// Disconnect old Triangles
	tri1.disconnectEdges();
	tri2.disconnectEdges();

	// Connect Edges to new Triangles
	t11.connectEdges();
	t12.connectEdges();
	t21.connectEdges();
	t22.connectEdges();

	// Update "global" lists
	edgeList.remove( edgeList.indexOf( this ) );
	edgeList.add( eK1 );
	edgeList.add( eK2 );
	edgeList.add( diagonal1 );
	edgeList.add( diagonal2 );

	triangleList.remove( triangleList.indexOf( tri1 ) );
	triangleList.remove( triangleList.indexOf( tri2 ) );
	triangleList.add( t11 );
	triangleList.add( t12 );
	triangleList.add( t21 );
	triangleList.add( t22 );

	Msg.debug( "...Created triangle " + t11.descr() );
	Msg.debug( "...Created triangle " + t12.descr() );
	Msg.debug( "...Created triangle " + t21.descr() );
	Msg.debug( "...Created triangle " + t22.descr() );

	Msg.debug( "Leaving Edge.splitTrianglesAt(..)" );
	if ( eK1.hasNode( ben ) )
	{
		return eK1;
	}
	else if ( eK2.hasNode( ben ) )
	{
		return eK2;
	}
	else
	{
		Msg.error( "" );
		return null;
	}*/
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Edge>
Edge::splitTrianglesAtMyMidPoint( const ArrayList<std::shared_ptr<Triangle>>& triangleList,
								  const ArrayList<std::shared_ptr<Edge>>& edgeList,
								  const ArrayList<std::shared_ptr<Node>>& nodeList,
								  const std::shared_ptr<Edge>& baseEdge )
{
	return nullptr;
	/*Msg.debug("Entering Edge.splitTrianglesAtMyMidPoint(..).");

	Edge lowerEdge;
	Node ben = baseEdge.commonNode( this );
	Node mid = this.midPoint();
	nodeList.add( mid );
	mid.color = java.awt.Color.blue;

	Msg.debug( "Splitting edge " + descr() );
	Msg.debug( "Creating new Node: " + mid.descr() );

	lowerEdge = splitTrianglesAt( mid, ben, triangleList, edgeList, nodeList );

	Msg.debug( "Leaving Edge.splitTrianglesAtMyMidPoint(..)." );
	return lowerEdge;*/
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Edge> 
Edge::nextQuadEdgeAt( const std::shared_ptr<Node>& n,
					  const std::shared_ptr<Element>& startElem )
{
	return nullptr;
	/*Msg.debug("Entering Edge.nextQuadEdgeAt(..)");
	Element elem;
	Edge e;
	int i = 3;

	e = startElem.neighborEdge( n, this );
	elem = startElem.neighbor( e );

	while ( elem != null && !(elem instanceof Quad) && elem != startElem )
	{
		e = elem.neighborEdge( n, e );
		Msg.debug( "..." + i );
		i++;
		elem = elem.neighbor( e );
	}
	Msg.debug( "Leaving Edge.nextQuadEdgeAt(..)" );
	if ( elem != null && elem instanceof Quad && elem != startElem )
	{
		return e;
	}
	else
	{
		return null;
	}*/
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Quad> 
Edge::getQuadElement()
{
	return nullptr;
	/*if ( element1 instanceof Quad )
	{
		return (Quad)element1;
	}
	else if ( element2 instanceof Quad )
	{
		return (Quad)element2;
	}
	else
	{
		return null;
	}*/
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Triangle> 
Edge::getTriangleElement()
{
	return nullptr;
	/*if ( element1 instanceof Triangle )
	{
		return (Triangle)element1;
	}
	else if ( element2 instanceof Triangle )
	{
		return (Triangle)element2;
	}
	else
	{
		return null;
	}*/
}

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Quad>
Edge::getQuadWithEdge( const std::shared_ptr<Edge>& e )
{
	return nullptr;
	/*if ( element1 instanceof Quad && element1.hasEdge(e) )
	{
		return (Quad)element1;
	}
	else if ( element2 instanceof Quad && element2.hasEdge( e ) )
	{
		return (Quad)element2;
	}
	else
	{
		return null;
	}*/
}

std::shared_ptr<Edge> 
Edge::frontNeighborAt( const std::shared_ptr<Node>& n )
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

std::shared_ptr<Edge> 
Edge::nextFrontNeighbor( const std::shared_ptr<Edge>& prev )
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

//TODO: Implement this method
//TODO: Tests
std::shared_ptr<Edge> 
Edge::trueFrontNeighborAt( const std::shared_ptr<Node>& n )
{
	return nullptr;
	/*auto curElem = getTriangleElement();
	Edge curEdge = this;

	if ( !hasNode( n ) )
	{
		Msg.error( "trueFrontNeighborAt(..): this Edge hasn't got Node " + n.descr() );
	}

	do
	{
		curEdge = curElem.neighborEdge( n, curEdge );
		curElem = curElem.neighbor( curEdge );
	} while ( !curEdge.isFrontEdge() );

	return curEdge;*/
}

//TODO: Test
std::string
Edge::toString()
{
	return descr();
}

//TODO: Test
void 
Edge::markAllSelectable()
{
	std::for_each( stateList.begin(), stateList.end(), []( std::vector<std::shared_ptr<Edge>>& l )
				   {
					   std::for_each( l.begin(), l.end(), []( std::shared_ptr<Edge>& e )
									  {
										  e->selectable = true;
									  } );
				   } );
}

//TODO: Test
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

//TODO: Test
bool 
Edge::leftTo( const std::shared_ptr<Edge>& e )
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

//TODO: Test
bool 
Edge::isFrontEdge()
{
	if ( (Element::instanceOf<Triangle>( element1 ) && !(Element::instanceOf<Triangle>( element2 ))) || (Element::instanceOf<Triangle>( element2 ) && !(Element::instanceOf<Triangle>( element1 ))) )
	{
		return true;
	}
	else
	{
		return false;
	}

	/*
	 * if ((element1 instanceof Quad && element2 instanceof Quad) || (element1
	 * instanceof Triangle && element2 instanceof Triangle)) return false; else
	 * return true;
	 */
}

//TODO: Test
std::string
Edge::descr()
{
	return "(" + std::to_string( leftNode->x ) + ", " + std::to_string( leftNode->y ) + "), (" + 
		std::to_string( rightNode->x ) + ", " + std::to_string( rightNode->y ) + ")";
}

//TODO: Test
void 
Edge::printMe()
{
	std::cout << descr() << "\n";
}