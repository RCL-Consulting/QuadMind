#pragma once

#include <string>

class Node;
class Edge;
class Element;

class Dart
{
public:
	Dart()=default;
	Dart( Node* n, Edge* e, Element* elem );
	
	std::string descr();
private:
	Node* n = nullptr;
	Edge* e = nullptr;
	Element* elem = nullptr;
};

