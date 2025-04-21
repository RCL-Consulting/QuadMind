#pragma once

#include "ArrayList.h"

#include <memory>
#include <string>

class Triangle;
class Edge;
class Node;

#include <iostream>
#include <map>
#include <vector>
#include <array>

class Point2D
{
public:
	double x, y;

	Point2D() : x( 0.0 ), y( 0.0 ) {}
	Point2D( double x_, double y_ ) : x( x_ ), y( y_ ) {}

	double getX() const { return x; }
	double getY() const { return y; }

	bool operator==( const Point2D& other ) const
	{
		return x == other.x && y == other.y;
	}

	// Required to use Point2D as a key in std::map
	bool operator<( const Point2D& other ) const
	{
		if ( x != other.x ) return x < other.x;
		return y < other.y;
	}
};

// Optional: allow printing
std::ostream& operator<<( std::ostream& os, const Point2D& pt )
{
	return os << "(" << pt.x << ", " << pt.y << ")";
}


class MeshLoader
{
public:
	static ArrayList<std::shared_ptr<Triangle>> triangleList;
	static ArrayList<std::shared_ptr<Edge>> edgeList;
	static ArrayList<std::shared_ptr<Node>> nodeList;

private:
	inline static int cInd = 0;

public:
	static ArrayList<std::shared_ptr<Triangle>> loadTriangleMesh( const std::string& meshDirectory,
																  const std::string& meshFilename );
	
	/**
	 * Loads a triangle mesh from an array of triangle coordinates. The array should
	 * be in the format double[n][6], where each row represents a triangle with
	 * coordinates x1, y1, x2, y2, x3, y3.
	 *
	 * @param triangleCoordinates A 2D array of triangle coordinates.
	 * @return A list of triangles representing the mesh, or null if an error
	 *         occurred (e.g., invalid input array).
	 */
	static ArrayList<std::shared_ptr<Triangle>> loadTriangleMeshFromArray( const std::vector<std::array<double, 6>>& triangleCoordinates,
																		   bool meshLenOpt,
																		   bool meshAngOpt );

private:
	static std::shared_ptr<Node> getNode( std::map<Point2D, std::shared_ptr<Node>>& nodeMap,
										  ArrayList<std::shared_ptr<Node>>& usNodeList,
										  double x, double y );

	static std::shared_ptr<Edge> getEdge( ArrayList<std::shared_ptr<Edge>>& edgeList,
										  const std::shared_ptr<Node>& node1,
										  const std::shared_ptr<Node>& node2 );

	static double nextDouble( const std::string& inputLine );
};