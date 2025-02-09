#include "Array.h"

#include "Undefined.h"
#include "ArrayRef.h"
#include "ArrayRangeRef.h"
#include "String.h"
#include "gse/Wrappable.h"

namespace gse {
namespace type {

static Value s_undefined = VALUE( type::Undefined );

Array::Array( array_elements_t initial_value )
	: Type( GetType() )
	, value( initial_value ) {}

const Value& Array::Get( const size_t index ) const {
	return ( index < value.size() )
		? value[ index ]
		: s_undefined;
}

const Value Array::GetSubArray( const std::optional< size_t > from, const std::optional< size_t > to ) const {
	ValidateFromTo( from, to );
	return VALUE( type::Array, std::vector(
		( from.has_value()
			? value.begin() + from.value()
			: value.begin()
		),
			( to.has_value()
				? value.begin() + to.value() + 1
				: value.end()
			)
	) );
}

void Array::Set( const size_t index, const Value& new_value ) {
	ASSERT_NOLOG( index < value.size(), "index out of bounds" );
	value[ index ] = new_value;
}

const void Array::SetSubArray( const std::optional< size_t > from, const std::optional< size_t > to, const Value& new_subarray ) {
	ValidateFromTo( from, to );
	ASSERT_NOLOG( new_subarray.Get()->type == Type::T_ARRAY, "operand of range assignment is not array: " + new_subarray.ToString() );
	auto* v = (type::Array*)new_subarray.Get();
	const auto dest_begin = from.has_value()
		? value.begin() + from.value()
		: value.begin();
#ifdef DEBUG
	const auto dest_end = to.has_value()
		? value.begin() + to.value() + 1
		: value.end();
	ASSERT_NOLOG( v->value.size() == dest_end - dest_begin, "range assignment value size mismatch ( " + std::to_string( v->value.size() ) + " != " + std::to_string( dest_end - dest_begin ) + " ): " + v->ToString() );
#endif
	std::copy( v->value.begin(), v->value.end(), dest_begin );
}

void Array::Append( const Value& new_value ) {
	value.push_back( new_value );
}

const Value Array::GetRef( const size_t index ) {
	return VALUE( ArrayRef, this, index );
}

const Value Array::GetRangeRef( const std::optional< size_t > from, const std::optional< size_t > to ) {
	return VALUE( ArrayRangeRef, this, from, to );
}

const Value Array::FromVector( const std::vector< Wrappable* >* data, const bool dynamic ) {
	array_elements_t elements = {};
	elements.reserve( data->size() );
	for ( const auto& el : *data ) {
		elements.push_back( el->Wrap( dynamic ) );
	}
	return VALUE( Array, elements );
}

void Array::ValidateFromTo( const std::optional< size_t >& from, const std::optional< size_t >& to ) const {
	if ( from.has_value() ) {
		ASSERT_NOLOG( from.value() < value.size(), "range beginning overflow ( " + std::to_string( from.value() ) + " >= " + std::to_string( value.size() ) + " )" );
	}
	if ( to.has_value() ) {
		ASSERT_NOLOG( to.value() < value.size(), "range end overflow ( " + std::to_string( to.value() ) + " >= " + std::to_string( value.size() ) + " )" );
		if ( from.has_value() ) {
			ASSERT_NOLOG( from.value() <= to.value(), "invalid range definition: [" + std::to_string( from.value() ) + ":" + std::to_string( to.value() ) );
		}
	}
}

}
}
