#pragma once

namespace gse {
namespace value {

class Nothing : public Value {
public:

	static const type_t GetType() { return Value::T_NOTHING; }

	Nothing( gc::Space* const gc_space )
		: Value( gc_space, GetType() ) {}

	Nothing( gc::Space* const gc_space, const Nothing& other )
		: Nothing( gc_space ) {}
};

}
}
