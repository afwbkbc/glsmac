#pragma once

#include "gse/Value.h"

namespace gse {
namespace value {

class Object;

class ValueRef : public Value {
public:

	static const type_t GetType() { return Value::T_VALUEREF; }

	ValueRef( Value* const target )
		: Value( GetType() )
		, target( target ) {}

	Value* const target;

};

}
}
