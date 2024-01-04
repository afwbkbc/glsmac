#pragma once

#include <vector>

#include "gse/type/Type.h"
#include "gse/Value.h"

#include "gse/program/Element.h"

namespace gse {
class Context;
namespace type {

class Callable : public type::Type {
public:

	static const type_t GetType() { return Type::T_CALLABLE; }

	typedef std::vector< Value > function_arguments_t;

	Callable()
		: type::Type( GetType() ) {}

	virtual ~Callable() {}

	virtual Value Run( const Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) = 0;

};

}
}

#include "gse/Context.h"
