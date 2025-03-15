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
	void makeDelaunayTriangle( Triangle* t, Edge* e, Node* n );

private:
	//
	// Insert a interior/ exterior Node and update mesh to remain Delaunay
	// compliant.
	//
	void insertNode( Node* n, bool remainDelaunay );

};
