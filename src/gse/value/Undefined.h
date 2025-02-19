#pragma once

#include "gse/Value.h"

namespace gse {
namespace value {

class Undefined : public Value {
public:

	static const type_t GetType() { return Value::T_UNDEFINED; }

	Undefined()
		: Value( GetType() ) {}

	Undefined( const Undefined& other )
		: Undefined() {}
};

}
}
