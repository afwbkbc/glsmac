#pragma once

#include <vector>
#include <optional>

#include "Type.h"
#include "../Value.h"

namespace gse {
namespace type {

class Array : public Type {
public:

	static const type_t GetType() { return Type::T_ARRAY; }

	typedef std::vector< Value > elements_t;

	Array( elements_t initial_value = {} );

	const Value& Get( const size_t index ) const;
	const Value GetSubArray( const std::optional< size_t > from, const std::optional< size_t > to ) const;
	void Set( const size_t index, const Value& new_value );
	void Append( const Value& new_value );

	const Value GetRef( const size_t index );
	const Value GetRangeRef( const std::optional< size_t > from, const std::optional< size_t > to );

	elements_t value = {};
};

}
}