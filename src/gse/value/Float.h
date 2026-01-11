#pragma once

namespace gse {
namespace value {

class Float : public Value {
public:

	static const value_type_t GetType() { return VT_FLOAT; }

	Float( gc::Space* const gc_space, const float initial_value )
		: Value( gc_space, GetType() )
		, value( initial_value ) {}

	float value;

	Float( gc::Space* const gc_space, const Float& other )
		: Float( gc_space, other.value ) {}

};

}
}
