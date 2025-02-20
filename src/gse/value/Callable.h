#pragma once

#include <vector>

#include "gse/Value.h"

#include "Types.h"
#include "gse/Types.h"

namespace gse {

namespace context {
class Context;
}

namespace value {

class Callable : public Value {
public:

	static const type_t GetType() { return Value::T_CALLABLE; }

	Callable( gc::Space* const gc_space, const bool cleanup_needed )
		: Value( gc_space, GetType() )
		, m_cleanup_needed( cleanup_needed ) {}

	virtual ~Callable() {}

	virtual Value* Run( GSE_CALLABLE, const function_arguments_t& arguments ) = 0;

	const bool m_cleanup_needed;

};

}
}
