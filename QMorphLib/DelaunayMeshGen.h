#pragma once

/**
 * This class offers methods for incrementally constructing Delaunay triangle
 * meshes.
 */

#include "GeomBasics.h"

class DelaunayMeshGen : public GeomBasics
{
public:
	bool inside = false;
	std::vector<Node*> irNodes;

	//
	// Initialize the class
	//
	// @param delaunayCompliant boolean to indicate whether to create a delaunay
	//                          compliant mesh or not.
	//
	bool delaunayCompliant = false;

	void init( bool delaunayCompliant );

	// Run the implementation on the give set of nodes. //
	void run();

private:
	int counter = 0;

public:
	//
	// Step through the insertions one by one. Upon completion, run
	// incrDelaunayPost().
	//
	/*@Override
		public void step()
	{
		Node n;
		if ( counter < nodeList.size() )
		{
			n = nodeList.get( counter );
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
	}*/

private:
	//
	// Find the triangle (in the list) that contains the specified Node. A method
	// highly inspired by the dart based localization procedure.
	//
	// @return the Triangle containing the Node, but if the Node is located on an
	//         Edge, this Edge is returned instead. Set inside= true if found. If
	//         the Node is located outside of the current triangulation, then set
	//         inside= false and return a boundary triangle that can be seen from
	//         the node.
	//
	Constants* findTriangleContaining( Node* newNode, Triangle* start );

	// Simple method to perform single swap. //
	void swap( Edge* e );

	// Recursive method that swaps Edges in order to maintain Delaunay property. //
	void recSwapDelaunay( Edge* e, Node* n );

	//
	// Recursive method that deletes the part of the mesh boundary that is no longer
	// Delaunay compliant (the "influence region") when a new node has been inserted
	// exterior to the current mesh. Also compiles a list of Nodes in this region.
	// The method must be called for each boundary triangle that is affected by the
	// newly inserted node. Some interior triangles are also affected, but you don't
	// need to worry about them, because they get deleted automagically by this
	// method.
	//
	// @param t a triangle on the boundary that is possibly no longer Delaunay
	//          compliant
	// @param e the boundary edge of this triangle
	// @param n the exterior node that has recently been inserted
	//
	void makeDelaunayTriangle( Triangle* t, Edge* e, Node* n )
	{
		Msg.debug( "Entering makeDelaunayTriangle(..)" );
		Msg.debug( "checking triangle t= " + t.descr() );
		int j;
		Edge e1, e2;
		Triangle t1, t2;
		Node p1, p2, p3, opposite = t.oppositeOfEdge( e );
		Quad q = new Quad( e, opposite, n );

		if ( !q.isStrictlyConvex() )
		{
			Msg.debug( "Leaving makeDelaunayTriangle(..): non-convex quad" );
			j = irNodes.indexOf( e.leftNode );
			if ( j == -1 )
			{
				irNodes.add( e.leftNode );
			}
			j = irNodes.indexOf( e.rightNode );
			if ( j == -1 )
			{
				irNodes.add( e.rightNode );
			}

			return;
		}

		p1 = q.nextCCWNode( n );
		p2 = q.nextCCWNode( p1 );
		p3 = q.nextCCWNode( p2 );

		Msg.debug( "n: " + n.descr() );
		Msg.debug( "p1: " + p1.descr() );
		Msg.debug( "p2: " + p2.descr() );
		Msg.debug( "p3: " + p3.descr() );

		if ( n.inCircle( p1, p2, p3 ) )
		{
			j = irNodes.indexOf( p1 );
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

			e1 = t.otherEdge( e );
			t1 = (Triangle)t.neighbor( e1 );
			e2 = t.otherEdge( e, e1 );
			t2 = (Triangle)t.neighbor( e2 );

			e.disconnectNodes();
			j = edgeList.indexOf( e );
			if ( j != -1 )
			{
				edgeList.remove( j );
			}

			if ( t1 == null )
			{
				e1.disconnectNodes();
				j = edgeList.indexOf( e1 );
				if ( j != -1 )
				{
					edgeList.remove( j );
				}
			}
			if ( t2 == null )
			{
				e2.disconnectNodes();
				j = edgeList.indexOf( e2 );
				if ( j != -1 )
				{
					edgeList.remove( j );
				}
			}

			t.disconnectEdges();
			triangleList.remove( triangleList.indexOf( t ) );

			if ( t1 != null )
			{
				makeDelaunayTriangle( t1, e1, n );
			}
			if ( t2 != null )
			{
				makeDelaunayTriangle( t2, e2, n );
			}

			Msg.debug( "Leaving makeDelaunayTriangle(..)... done!" );
		}
		else
		{
			j = irNodes.indexOf( e.leftNode );
			if ( j == -1 )
			{
				irNodes.add( e.leftNode );
			}
			j = irNodes.indexOf( e.rightNode );
			if ( j == -1 )
			{
				irNodes.add( e.rightNode );
			}

			Msg.debug( "Leaving makeDelaunayTriangle(..), n lies outside circumcircle" );
		}
	}

private:
	//
	// Insert a interior/ exterior Node and update mesh to remain Delaunay
	// compliant.
	//
	void insertNode( Node* n, bool remainDelaunay );

};
