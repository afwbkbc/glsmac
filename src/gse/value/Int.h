#pragma once

#include <cstdint>

namespace gse {
namespace value {

class Int : public Value {
public:

	static const type_t GetType() { return Value::T_INT; }

	Int( const int64_t initial_value )
		: Value( GetType() )
		, value( initial_value ) {}

	int64_t value;

	Int( const Int& other )
		: Int( other.value ) {}

};

}
}
