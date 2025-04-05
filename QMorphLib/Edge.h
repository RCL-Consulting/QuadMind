#pragma once

#include "Constants.h"

class Edge : public Constants
{
public:
	bool equals( const std::shared_ptr<Constants>& elem ) const override { return false; }
};
  