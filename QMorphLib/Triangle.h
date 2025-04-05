#pragma once
#include "Element.h"

class Triangle : public Element
{
public:
	bool equals( const std::shared_ptr<Constants>& elem ) const override { return false; }
};