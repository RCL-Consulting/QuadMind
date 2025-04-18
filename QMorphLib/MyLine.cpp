#include "pch.h"
#include "MyLine.h"

#include "Node.h"

MyLine::MyLine( const std::shared_ptr<Node>& n1,
				const std::shared_ptr<Node>& n2 )
{
	ref = n1;
	x = n2->x - n1->x;
	y = n2->y - n1->y;
}

MyLine::MyLine( const std::shared_ptr<Node>& n1,
				double x,
				double y )
{
	ref = n1;
	this->x = x;
	this->y = y;
}

double 
MyLine::cross( const MyLine& l )
{
	return x * l.y - l.x * y;
}