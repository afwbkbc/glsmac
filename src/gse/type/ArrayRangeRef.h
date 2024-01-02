#pragma once

#include <optional>

#include "Type.h"

#include "Array.h"

namespace gse {
namespace type {

class ArrayRangeRef : public Type {
public:

	static const type_t GetType() { return Type::T_ARRAYRANGEREF; }

	ArrayRangeRef( Array* array, const std::optional< size_t > from, const std::optional< size_t > to )
		: Type( GetType() )
		, array( array )
		, from( from )
		, to( to ) {}

	Array* array;
	const std::optional< size_t > from;
	const std::optional< size_t > to;

};

}
}
