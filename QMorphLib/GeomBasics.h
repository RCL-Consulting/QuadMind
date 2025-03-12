#pragma once

/**
 * This is a basic geometry class with methods for reading and writing meshes,
 * sorting Node lists, printing lists, topology inspection, etc.
 */

#include "Constants.h"
#include "Msg.h"

#include <vector>
#include <string>

class Element;
class Triangle;
class Node;
class Edge;
class TopoCleanup;
class GlobalSmooth;
class MyVector;

class GeomBasics : Constants
{
public:
	static std::vector<Element*> elementList;
	static std::vector<Triangle*> triangleList;
	static std::vector<Node*> nodeList;
	static std::vector<Edge*> edgeList;

	static Node* leftmost, * rightmost, * uppermost, * lowermost;

	static bool m_step;

	static TopoCleanup* topoCleanup;
	static GlobalSmooth* globalSmooth;

	static std::string meshFilename;
	static std::string meshDirectory;
	static bool meshLenOpt;
	static bool meshAngOpt;

	static void createNewLists();

	static void setParams( const std::string& filename,
						   const std::string& dir,
						   bool len, bool ang );

	// Return the edgeList //
	static std::vector<Edge*>& getEdgeList();

	// Return the nodeList //
	static std::vector<Node*>& getNodeList();

	// Return the triangleList //
	static std::vector<Triangle*>& getTriangleList();

	// Return the elementList /
	static std::vector<Element*>& getElementList();

private:
	static GeomBasics* curMethod;
	static size_t cInd;

public:
	static void setCurMethod( GeomBasics* method );

	static GeomBasics* getCurMethod();

	// This method should be implemented in each of the subclasses. //
	virtual void step()	{}

	// Delete all the edges in the mesh. //
	static void clearEdges();

	// Clear the nodeList, edgeList, triangleList and elementList. //
	static void clearLists();

	// Update distortion metric for all elements in mesh. //
	static void updateMeshMetrics();

	// @return a string containing the average and minimum element metrics. //
	static std::string meshMetricsReport();

	// Find inverted elements and paint them with red colour. //
	static void detectInvertedElements();

	// Output nr of tris and fake quads in mesh. //
	static void countTriangles();

	// Output warnings if mesh is not consistent. //
	static void consistencyCheck();

	// Load a mesh from a file. //
	static std::vector<Element*> loadMesh();

	// Load a triangle mesh from a file. //
	static std::vector<Triangle*> loadTriangleMesh();

	// A method to read node files. //
	static std::vector<Node*> loadNodes();

	//
	// Method for writing to a LaTeX drawing format (need the epic and eepic
	// packages).
	//
	static bool exportMeshToLaTeX( const std::string& filename,
								   int unitlength,
								   double xcorr,
								   double ycorr,
								   bool visibleNodes );

	// Write all elements in elementList to a file. //
	static bool writeQuadMesh( const std::string& filename,
							   std::vector<Element*>& list );

	// Write all elements in elementList and triangleList to a file. //
	static bool writeMesh( const std::string& filename )
	{
		FileOutputStream fos = null;
		Element elem;
		Triangle t;
		Quad q;

		try
		{
			fos = new FileOutputStream( filename );
		}
		catch ( Exception e )
		{
			Msg.error( "File " + filename + " not found." );
		}

		BufferedWriter out = new BufferedWriter( new OutputStreamWriter( fos ) );
		double x1, x2, x3, x4, y1, y2, y3, y4;

		if ( triangleList != null )
		{
			for ( Object element : triangleList )
			{
				t = (Triangle)element;
				x1 = t.edgeList[0].leftNode.x;
				y1 = t.edgeList[0].leftNode.y;
				x2 = t.edgeList[0].rightNode.x;
				y2 = t.edgeList[0].rightNode.y;
				if ( !t.edgeList[1].leftNode.equals( t.edgeList[0].leftNode ) && !t.edgeList[1].leftNode.equals( t.edgeList[0].rightNode ) )
				{
					x3 = t.edgeList[1].leftNode.x;
					y3 = t.edgeList[1].leftNode.y;
				}
				else
				{
					x3 = t.edgeList[1].rightNode.x;
					y3 = t.edgeList[1].rightNode.y;
				}
				try
				{
					out.write( x1 + ", " + y1 + ", " + x2 + ", " + y2 + ", " + x3 + ", " + y3 );
					out.newLine();
				}
				catch ( Exception e )
				{
					Msg.error( "Cannot write quad-mesh data." );
				}

			}
		}

		if ( elementList != null )
		{
			for ( Object element : elementList )
			{

				if ( element instanceof Quad )
				{
					q = (Quad)element;

					x1 = q.edgeList[base].leftNode.x;
					y1 = q.edgeList[base].leftNode.y;
					x2 = q.edgeList[base].rightNode.x;
					y2 = q.edgeList[base].rightNode.y;
					x3 = q.edgeList[left].otherNode( q.edgeList[base].leftNode ).x;
					y3 = q.edgeList[left].otherNode( q.edgeList[base].leftNode ).y;
					if ( !q.isFake )
					{
						x4 = q.edgeList[right].otherNode( q.edgeList[base].rightNode ).x;
						y4 = q.edgeList[right].otherNode( q.edgeList[base].rightNode ).y;
						try
						{
							out.write( x1 + ", " + y1 + ", " + x2 + ", " + y2 + ", " + x3 + ", " + y3 + ", " + x4 + ", " + y4 );
							out.newLine();
						}
						catch ( Exception e )
						{
							Msg.error( "Cannot write quad-mesh data." );
						}

					}
					else
					{
						try
						{
							out.write( x1 + ", " + y1 + ", " + x2 + ", " + y2 + ", " + x3 + ", " + y3 );
						}
						catch ( Exception e )
						{
							Msg.error( "Cannot write quad-mesh data." );
						}
					}
				}
				else if ( element instanceof Triangle )
				{
					t = (Triangle)element;
					x1 = t.edgeList[0].leftNode.x;
					y1 = t.edgeList[0].leftNode.y;
					x2 = t.edgeList[0].rightNode.x;
					y2 = t.edgeList[0].rightNode.y;
					if ( !t.edgeList[1].leftNode.equals( t.edgeList[0].leftNode ) && !t.edgeList[1].leftNode.equals( t.edgeList[0].rightNode ) )
					{
						x3 = t.edgeList[1].leftNode.x;
						y3 = t.edgeList[1].leftNode.y;
					}
					else
					{
						x3 = t.edgeList[1].rightNode.x;
						y3 = t.edgeList[1].rightNode.y;
					}
					try
					{
						out.write( x1 + ", " + y1 + ", " + x2 + ", " + y2 + ", " + x3 + ", " + y3 );
						out.newLine();
					}
					catch ( Exception e )
					{
						Msg.error( "Cannot write quad-mesh data." );
					}
				}
			}
		}

		try
		{
			out.close();
		}
		catch ( Exception e )
		{
			Msg.error( "Cannot write quad-mesh data." );
		}
		return true;
	}

	// Write all nodes in nodeList to a file. //
	/*public static boolean writeNodes(String filename)
	{
		FileOutputStream fos;

		try
		{
			fos = new FileOutputStream( filename );
			BufferedWriter out = new BufferedWriter( new OutputStreamWriter( fos ) );
			double x, y;

			try
			{
				if ( nodeList != null )
				{
					for ( Node n : nodeList )
					{
						x = n.x;
						y = n.y;
						out.write( x + ", " + y );
						out.newLine();
					}
				}
				out.close();
			}
			catch ( Exception e )
			{
				Msg.error( "Cannot write node data." );
			}
		}
		catch ( Exception e )
		{
			Msg.error( "Could not open file " + filename );
		}
		return true;
	}*/

	// Find the leftmost, rightmost, uppermost, and lowermost nodes. //
	static void findExtremeNodes();

	// Sort nodes left to right. Higher y-values are preferred to lower ones. //
	/*public static List<Node> sortNodes(List<Node> unsortedNodes)
	{
		List<Node> sortedNodes = new ArrayList<>();
		Node curNode, candNode;
		while ( unsortedNodes.size() > 0 )
		{
			curNode = unsortedNodes.get( 0 );
			for ( int i = 1; i < unsortedNodes.size(); i++ )
			{
				candNode = unsortedNodes.get( i );
				if ( candNode.x < curNode.x || (candNode.x == curNode.x && candNode.y < curNode.y) )
				{
					curNode = candNode;
				}
			}
			sortedNodes.add( curNode );
			unsortedNodes.remove( unsortedNodes.indexOf( curNode ) );
		}

		// Find the leftmost, rightmost, uppermost, and lowermost nodes.
		leftmost = sortedNodes.get( 0 );
		rightmost = sortedNodes.get( sortedNodes.size() - 1 );
		uppermost = leftmost;
		lowermost = leftmost;

		for ( int i = 1; i < sortedNodes.size(); i++ )
		{
			curNode = sortedNodes.get( i );
			if ( curNode.y > uppermost.y )
			{
				uppermost = curNode;
			}
			if ( curNode.y < lowermost.y )
			{
				lowermost = curNode;
			}
		}

		return sortedNodes;
	}*/

	//
	// Method to assist the different load... methods.
	//
	// @param iline a comma-separated string
	// @return the next double value from iline. If no more numbers, then return
	//         NaN.
	//
private:
	static double nextDouble( const std::string& iline );

public:
	static void printVectors( const std::vector<MyVector>& vectorList );

	template <typename T>
	static void printElements( const std::vector<T*>& list )
	{
		if ( Msg::debugMode )
		{
			for ( auto element : list )
				element->printMe();
		}
	}

	static void printTriangles( const std::vector<Triangle*>& triangleList );

	static void printQuads( const std::vector<Element*> list );

	/*public static void printEdgeList(List<Edge> list)
	{
		if ( Msg.debugMode )
		{
			for ( Object element : list )
			{
				Edge edge = (Edge)element;
				edge.printMe();
			}
		}
	}

	public static void printNodes( List<Node> nodeList )
	{
		if ( Msg.debugMode )
		{
			Msg.debug( "nodeList:" );
			for ( Node node : nodeList )
			{
				node.printMe();
			}
		}
	}

	//
	public static void printValences()
	{
		for ( Node n : nodeList )
		{
			Msg.debug( "Node " + n.descr() + " has valence " + n.valence() );
		}
	}

	//
	public static void printValPatterns()
	{
		Node[] neighbors;
		for ( Node n : nodeList )
		{
			if ( !n.boundaryNode() )
			{
				neighbors = n.ccwSortedNeighbors();
				n.createValencePattern( neighbors );
				Msg.debug( "Node " + n.descr() + " has valence pattern " + n.valDescr() );
			}
		}
	}

	//
	public static void printAnglesAtSurrondingNodes()
	{
		Node[] neighbors;
		double[] angles;
		for ( Node n : nodeList )
		{
			if ( !n.boundaryNode() )
			{
				neighbors = n.ccwSortedNeighbors();
				n.createValencePattern( neighbors );
				angles = n.surroundingAngles( neighbors, n.pattern[0] - 2 );

				Msg.debug( "Angles at the nodes surrounding node " + n.descr() + ":" );
				for ( int j = 0; j < n.pattern[0] - 2; j++ )
				{
					Msg.debug( "angles[" + j + "]== " + Math.toDegrees( angles[j] ) + " (in degrees)" );
				}
			}
		}
	}

	//
	// Do inversion test and repair inversion if requiered
	//
	// @return true if any repairing was neccessary, else return false.
	//
	public static boolean inversionCheckAndRepair( Node newN, Node oldPos )
	{
		Msg.debug( "Entering inversionCheckAndRepair(..), node oldPos: " + oldPos.descr() );
		Element elem = null;
		List<Element> elements = newN.adjElements();
		if ( newN.invertedOrZeroAreaElements( elements ) )
		{
			if ( !newN.incrAdjustUntilNotInvertedOrZeroArea( oldPos, elements ) )
			{

				for ( Object element : elements )
				{
					elem = (Element)element;
					if ( elem.invertedOrZeroArea() )
					{
						break;
					}
				}

				Msg.error( "It seems that an element was inverted initially: " + elem.descr() );
				return false;
			}
			Msg.debug( "Leaving inversionCheckAndRepair(..)" );
			return true;
		}
		else
		{
			Msg.debug( "Leaving inversionCheckAndRepair(..)" );
			return false;
		}
	}

	//
	// Quad q is to be collapsed. Nodes n1 and n2 are two opposite nodes in q. This
	// method tries to find a location inside the current q to which n1 and n2 can
	// safely be relocated and joined without causing any adjacent elements to
	// become inverted. The first candidate location is the centroid of the quad. If
	// that location is not suitable, the method tries locations on the vectors from
	// the centroid towards n1 and from the centroid towards n2. The first suitable
	// location found is returned.
	//
	// @param q  the quad to be collapsed
	// @param n1 the node in quad q that is to be joined with opposite node n2
	// @param n2 the node in quad q that is to be joined with opposite node n1
	// @return a position inside quad q to which both n1 and n2 can be relocated
	//         without inverting any of their adjacent elements.
	//
	public static Node safeNewPosWhenCollapsingQuad( Quad q, Node n1, Node n2 )
	{
		Msg.debug( "Entering safeNewPosWhenCollapsingQuad(..)" );

		Node n = q.centroid();
		MyVector back2n1 = new MyVector( n, n1 ), back2n2 = new MyVector( n, n2 );
		double startX = n.x, startY = n.y;
		double xstepn1 = back2n1.x / 50.0, ystepn1 = back2n1.y / 50.0, xstepn2 = back2n2.x / 50.0, ystepn2 = back2n2.y / 50.0;
		double xincn1, yincn1, xincn2, yincn2;
		int steps2n1, steps2n2, i;
		List<Element> l1 = n1.adjElements(), l2 = n2.adjElements();

		if ( !q.anyInvertedElementsWhenCollapsed( n, n1, n2, l1, l2 ) )
		{
			Msg.debug( "Leaving safeNewPosWhenCollapsingQuad(..): found" );
			return n;
		}

		// Calculate the parameters for direction n to n1
		if ( Math.abs( xstepn1 ) < COINCTOL || Math.abs( ystepn1 ) < COINCTOL )
		{
			Msg.debug( "...ok, resorting to use of minimum increment" );
			if ( Math.abs( back2n1.x ) < Math.abs( back2n1.y ) )
			{
				if ( back2n1.x < 0 )
				{
					xstepn1 = -COINCTOL;
				}
				else
				{
					xstepn1 = COINCTOL;
				}

				// abs(ystepn1/xstepn1) = abs(n1.y/n1.x)
				ystepn1 = Math.abs( n1.y ) * COINCTOL / Math.abs( n1.x );
				if ( back2n1.y < 0 )
				{
					ystepn1 = -ystepn1;
				}

				steps2n1 = (int)(back2n1.x / xstepn1);
			}
			else
			{
				if ( back2n1.y < 0 )
				{
					ystepn1 = -COINCTOL;
				}
				else
				{
					ystepn1 = COINCTOL;
				}

				// abs(xstepn1/ystepn1) = abs(n1.x/n1.y)
				xstepn1 = Math.abs( n1.x ) * COINCTOL / Math.abs( n1.y );
				if ( back2n1.x < 0 )
				{
					xstepn1 = -xstepn1;
				}

				steps2n1 = (int)(back2n1.y / ystepn1);
			}
		}
		else
		{
			xstepn1 = back2n1.x / 50.0;
			ystepn1 = back2n1.x / 50.0;
			steps2n1 = 50;
		}

		// Calculate the parameters for direction n to n2
		if ( Math.abs( xstepn2 ) < COINCTOL || Math.abs( ystepn2 ) < COINCTOL )
		{
			Msg.debug( "...ok, resorting to use of minimum increment" );
			if ( Math.abs( back2n2.x ) < Math.abs( back2n2.y ) )
			{
				if ( back2n2.x < 0 )
				{
					xstepn2 = -COINCTOL;
				}
				else
				{
					xstepn2 = COINCTOL;
				}

				// abs(ystepn2/xstepn2) = abs(n2.y/n2.x)
				ystepn2 = Math.abs( n2.y ) * COINCTOL / Math.abs( n2.x );
				if ( back2n2.y < 0 )
				{
					ystepn2 = -ystepn2;
				}

				steps2n2 = (int)(back2n2.x / xstepn2);
			}
			else
			{
				if ( back2n2.y < 0 )
				{
					ystepn2 = -COINCTOL;
				}
				else
				{
					ystepn2 = COINCTOL;
				}

				// abs(xstepn2/ystepn2) = abs(n2.x/n2.y)
				xstepn2 = Math.abs( n2.x ) * COINCTOL / Math.abs( n2.y );
				if ( back2n2.x < 0 )
				{
					xstepn2 = -xstepn2;
				}

				steps2n2 = (int)(back2n2.y / ystepn2);
			}
		}
		else
		{
			xstepn2 = back2n2.x / 50.0;
			ystepn2 = back2n2.x / 50.0;
			steps2n2 = 50;
		}

		Msg.debug( "...back2n1.x is: " + back2n1.x );
		Msg.debug( "...back2n1.y is: " + back2n1.y );
		Msg.debug( "...xstepn1 is: " + xstepn1 );
		Msg.debug( "...ystepn1 is: " + ystepn1 );

		Msg.debug( "...back2n2.x is: " + back2n2.x );
		Msg.debug( "...back2n2.y is: " + back2n2.y );
		Msg.debug( "...xstepn2 is: " + xstepn2 );
		Msg.debug( "...ystepn2 is: " + ystepn2 );

		// Try to find a location
		for ( i = 1; i <= steps2n1 || i <= steps2n2; i++ )
		{
			if ( i <= steps2n1 )
			{
				n.x = startX + xstepn1 * i;
				n.y = startY + ystepn1 * i;
				if ( !q.anyInvertedElementsWhenCollapsed( n, n1, n2, l1, l2 ) )
				{
					Msg.debug( "Leaving safeNewPosWhenCollapsingQuad(..): found" );
					return n;
				}
			}
			if ( i <= steps2n2 )
			{
				n.x = startX + xstepn2 * i;
				n.y = startY + ystepn2 * i;
				if ( !q.anyInvertedElementsWhenCollapsed( n, n1, n2, l1, l2 ) )
				{
					Msg.debug( "Leaving safeNewPosWhenCollapsingQuad(..): found" );
					return n;
				}
			}
		}

		Msg.debug( "Leaving safeNewPosWhenCollapsingQuad(..): not found" );
		return null;
	}

	//
	// To be used only with all-triangle meshes.
	//
	// @return true if any zero area triangles were removed.
	//
	boolean repairZeroAreaTriangles()
	{
		Msg.debug( "Entering GeomBasics.repairZeroAreaTriangles()" );
		boolean res = false;
		int j;
		Triangle t, old1, old2;
		Edge e, eS, e1, e2;

		for ( int i = 0; i < triangleList.size(); i++ )
		{
			if ( !(triangleList.get( i ) instanceof Triangle) )
			{
				continue;
			}
			t = triangleList.get( i );
			if ( t.zeroArea() )
			{
				e = t.longestEdge();
				e1 = t.otherEdge( e );
				e2 = t.otherEdge( e, e1 );
				res = true;

				Msg.debug( "...longest edge is " + e.descr() );
				if ( !e.boundaryEdge() )
				{
					Msg.debug( "...longest edge not on boundary!" );
					old1 = (Triangle)e.element1;
					old2 = (Triangle)e.element2;
					eS = e.getSwappedEdge();
					e.swapToAndSetElementsFor( eS );

					triangleList.set( triangleList.indexOf( old1 ), null );
					triangleList.set( triangleList.indexOf( old2 ), null );

					triangleList.add( (Triangle)eS.element1 );
					triangleList.add( (Triangle)eS.element2 );

					edgeList.remove( edgeList.indexOf( e ) );
					edgeList.add( eS );
				}
				else
				{
					// The zero area triangle has its longest edge on the boundary...
					// Then we can just remove the triangle and the long edge!
					// Note that we now get a new boundary node...
					Msg.debug( "...longest edge is on boundary!" );
					triangleList.set( triangleList.indexOf( t ), null );
					t.disconnectEdges();
					edgeList.remove( edgeList.indexOf( e ) );
					e.disconnectNodes();
				}

			}
		}

		// Remove those entries that were set to null above.
		int i = 0;
		do
		{
			t = triangleList.get( i );
			if ( t == null )
			{
				triangleList.remove( i );
			}
			else
			{
				i++;
			}
		} while ( i < triangleList.size() );

		Msg.debug( "Leaving GeomBasics.repairZeroAreaTriangles()" );
		return res;
	}*/

	//
	// A method for fast computation of the cross product of two vectors.
	//
	// @param o1 origin of first vector
	// @param p1 endpoint of first vector
	// @param o2 origin of second vector
	// @param p2 endpoint of second vector
	// @return the cross product of the two vectors
	//
	static double cross( const Node& o1, const Node& p1, const Node& o2, const Node& p2 );
}; // End of class GeomBasics
