#pragma once

#include "Triangle.h"
#include "Quad.h"

#include <memory>
namespace rcl
{
	template<typename T>
	static bool instanceOf( const std::shared_ptr<Element>& obj )
	{
		return std::dynamic_pointer_cast<T>(obj) != nullptr;
	}

	inline static std::shared_ptr<Node> origin = std::make_shared<Node>( 0.0, 0.0 );
};