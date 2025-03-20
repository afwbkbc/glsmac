#pragma once

namespace gse {
namespace value {

class Float : public Value {
public:

	static const type_t GetType() { return Value::T_FLOAT; }

	Float( gc::Space* const gc_space, const float initial_value )
		: Value( gc_space, GetType() )
		, value( initial_value ) {}

	float value;

	Float( gc::Space* const gc_space, const Float& other )
		: Float( gc_space, other.value ) {}

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override {
		return "gse::value::Float( " + std::to_string( value ) + " )";
	}
#endif

};

}
}
