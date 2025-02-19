#pragma once

#include <optional>

namespace gse {
namespace value {

class Array;

class ArrayRangeRef : public Value {
public:

	static const type_t GetType() { return Value::T_ARRAYRANGEREF; }

	ArrayRangeRef( Array* array, const std::optional< size_t > from, const std::optional< size_t > to )
		: Value( GetType() )
		, array( array )
		, from( from )
		, to( to ) {}

	Array* array;
	const std::optional< size_t > from;
	const std::optional< size_t > to;

};

}
}
