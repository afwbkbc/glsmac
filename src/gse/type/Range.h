#pragma once

#include <optional>

#include "Type.h"

namespace gse {
namespace type {

class Range : public Type {
public:

	static const type_t GetType() { return Type::T_RANGE; }

	Range( const std::optional< size_t > from, const std::optional< size_t > to )
		: Type( GetType() )
		, from( from )
		, to( to ) {}

	const std::optional< size_t > from;
	const std::optional< size_t > to;

};

}
}
