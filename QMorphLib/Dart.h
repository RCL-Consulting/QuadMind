#pragma once

#include <string>

class Node;
class Edge;
class Element;

class Dart
{
public:
	Dart();
	Dart( Node* n, Edge* e, Element* elem ) { this->n = n; this->e = e; this->elem = elem; }
	
	std::string descr() { return "(elem: " + elem->descr() + ", e: " + e->descr() + ", n: " + n->descr() + ")"; }
private:
	Node* n = nullptr;
	Edge* e = nullptr;
	Element* elem = nullptr;
};

