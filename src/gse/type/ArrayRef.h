#pragma once

#include "Type.h"

#include "Array.h"

namespace gse {
namespace type {

class ArrayRef : public Type {
public:

	static const type_t GetType() { return Type::T_ARRAYREF; }

	ArrayRef( Array* array, const size_t index )
		: Type( GetType() )
		, array( array )
		, index( index ) {}

	Array* array;
	const size_t index;

};

}
}
