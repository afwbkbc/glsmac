#pragma once

namespace gse {
namespace value {

class Array;

class ArrayRef : public Value {
public:

	static const type_t GetType() { return Value::T_ARRAYREF; }

	ArrayRef( gc::Space* const gc_space, Array* array, const size_t index )
		: Value( gc_space, GetType() )
		, array( array )
		, index( index ) {}

	Array* array;
	const size_t index;

};

}
}
