#pragma once

namespace gse {
namespace value {

class Array;

class ArrayRef : public Value {
public:

	static const type_t GetType() { return Value::T_ARRAYREF; }

	ArrayRef( Array* array, const size_t index )
		: Value( GetType() )
		, array( array )
		, index( index ) {}

	Array* array;
	const size_t index;

};

}
}
