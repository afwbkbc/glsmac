#pragma once

#include "gse/Value.h"

namespace gse {
namespace value {

class Undefined : public Value {
public:

	static const type_t GetType() { return Value::T_UNDEFINED; }

	Undefined( gc::Space* const gc_space )
		: Value( gc_space, GetType() ) {}

	Undefined( gc::Space* const gc_space, const Undefined& other )
		: Undefined( gc_space ) {}

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override {
		return "gse::value::Undefined()";
	}
#endif

};

}
}
