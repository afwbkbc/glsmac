#pragma once

namespace gse {
namespace value {

class Null : public Value {
public:
	Null( gc::Space* const gc_space )
		: Value( gc_space, Value::T_NULL ) {}

	Null( gc::Space* const gc_space, const Null& other )
		: Null( gc_space ) {}

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override {
		return "gse::value::Null()";
	}
#endif

};

}
}
