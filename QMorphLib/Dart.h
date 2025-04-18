#pragma once

#include <memory>
#include <string>

class Node;
class Edge;
class Element;

/** A very simple implementation of a dart. */
class Dart
{
public:
	Dart();

	Dart( const std::shared_ptr<Node>& n,
		  const std::shared_ptr<Edge>& e,
		  const std::shared_ptr<Element>& elem );

	std::string descr();

	std::shared_ptr<Node> n;
	std::shared_ptr<Edge> e;
	std::shared_ptr<Element> elem;
};
