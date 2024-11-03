#pragma once

#include <vector>
#include <optional>

#include "Type.h"

#include "Types.h"

#include "gse/Value.h"

namespace gse {

class Wrappable;

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

	static const Value FromVector( const std::vector< Wrappable* >* data, const bool dynamic = false ); // be careful

private:
	void ValidateFromTo( const std::optional< size_t >& from, const std::optional< size_t >& to ) const;
};

}
}