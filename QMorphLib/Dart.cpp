#include "pch.h"
#include "Dart.h"

#include "Edge.h"
#include "Element.h"
#include "Node.h"

Dart::Dart( Node* n, Edge* e, Element* elem ) 
{ 
	this->n = n; 
	this->e = e; 
	this->elem = elem; 
}

std::string 
Dart::descr() const
{ 
	return "(elem: " + elem->descr() + ", e: " + e->descr() + ", n: " + n->descr() + ")"; 
}