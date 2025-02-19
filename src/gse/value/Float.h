#pragma once

namespace gse {
namespace value {

class Float : public Value {
public:

	static const type_t GetType() { return Value::T_FLOAT; }

	Float( const float initial_value )
		: Value( GetType() )
		, value( initial_value ) {}

	float value;

	Float( const Float& other )
		: Float( other.value ) {}

};

}
}
