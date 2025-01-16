#pragma once

#include "Type.h"

namespace gse {
namespace type {

class Object;

class ValueRef : public Type {
public:

	static const type_t GetType() { return Type::T_VALUEREF; }

	ValueRef( const Type* const target )
		: Type( GetType() )
		, target( target ) {}

	const Type* const target;

};

}
}
