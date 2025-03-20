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

	Callable( gc::Space* const gc_space, context::Context* const ctx );

	virtual ~Callable() = default;

	virtual Value* Run( GSE_CALLABLE, const function_arguments_t& arguments ) = 0;

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

	context::Context* const m_ctx = nullptr;

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override;
#endif

};

}
}
