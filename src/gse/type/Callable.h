#pragma once

#include <vector>

#include "Type.h"

#include "Types.h"
#include "gse/Types.h"
#include "gse/Value.h"

namespace gse {

namespace context {
class Context;
}

namespace type {

class Callable : public Type {
public:

	static const type_t GetType() { return Type::T_CALLABLE; }

	Callable()
		: type::Type( GetType() ) {}

	virtual ~Callable() {}

	virtual Value Run( context::Context* ctx, const si_t& call_si, const function_arguments_t& arguments ) = 0;

};

}
}
