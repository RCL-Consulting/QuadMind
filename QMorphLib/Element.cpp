#include "pch.h"
#include "Element.h"

#include "Node.h"

double 
Element::cross( const std::shared_ptr<Node>& o1,
				const std::shared_ptr<Node>& p1,
				const std::shared_ptr<Node>& o2,
				const std::shared_ptr<Node>& p2 )
{
	double x1 = p1->x - o1->x;
	double x2 = p2->x - o2->x;
	double y1 = p1->y - o1->y;
	double y2 = p2->y - o2->y; 
	return x1 * y2 - x2 * y1;
}