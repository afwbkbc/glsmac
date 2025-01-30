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

	Callable( const bool cleanup_needed )
		: type::Type( GetType() )
		, m_cleanup_needed( cleanup_needed ) {}

	virtual ~Callable() {}

	virtual Value Run( GSE_CALLABLE, const function_arguments_t& arguments ) = 0;

	const bool m_cleanup_needed;

};

}
}
