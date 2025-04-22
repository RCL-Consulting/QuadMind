#include "pch.h"
#include "Dart.h"

#include "Node.h"
#include "Edge.h"
#include "Element.h"

Dart::Dart()
{
	n = nullptr;
	e = nullptr;
	elem = nullptr;
}

Dart::Dart( const std::shared_ptr<Node>& n,
			const std::shared_ptr<Edge>& e,
			const std::shared_ptr<Element>& elem )
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