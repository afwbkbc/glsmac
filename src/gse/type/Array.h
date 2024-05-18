#pragma once

#include <vector>
#include <optional>

#include "Type.h"

#include "Types.h"

#include "gse/Value.h"

namespace gse {
namespace type {

class Array : public Type {
public:

	static const type_t GetType() { return Type::T_ARRAY; }

	Array( array_elements_t initial_value = {} );

	const Value& Get( const size_t index ) const;
	const Value GetSubArray( const std::optional< size_t > from, const std::optional< size_t > to ) const;
	void Set( const size_t index, const Value& new_value );
	const void SetSubArray( const std::optional< size_t > from, const std::optional< size_t > to, const Value& new_subarray );
	void Append( const Value& new_value );

	const Value GetRef( const size_t index );
	const Value GetRangeRef( const std::optional< size_t > from, const std::optional< size_t > to );

	array_elements_t value = {};

private:
	void ValidateFromTo( const std::optional< size_t >& from, const std::optional< size_t >& to ) const;
};

}
}