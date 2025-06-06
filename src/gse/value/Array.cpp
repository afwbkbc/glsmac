#include "Array.h"

#include "Undefined.h"
#include "ArrayRef.h"
#include "ArrayRangeRef.h"
#include "gse/Wrappable.h"
#include "gc/Space.h"

namespace gse {
namespace value {

Array::Array( gc::Space* const gc_space, array_elements_t initial_value )
	: Value( gc_space, GetType() )
	, value( initial_value ) {}

Value* const Array::Get( const size_t index ) {
	CHECKACCUM( m_gc_space );

	return ( index < value.size() )
		? value[ index ]
		: VALUEEXT( value::Undefined, m_gc_space );
}

Value* const Array::GetSubArray( const std::optional< size_t > from, const std::optional< size_t > to ) {
	CHECKACCUM( m_gc_space );

	ValidateFromTo( from, to );
	return VALUEEXT( value::Array, m_gc_space, std::vector(
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

void Array::Set( const size_t index, Value* const new_value ) {
	ASSERT( index < value.size(), "index out of bounds" );
	value[ index ] = new_value;
}

const void Array::SetSubArray( const std::optional< size_t > from, const std::optional< size_t > to, Value* const new_subarray ) {
	ValidateFromTo( from, to );
	ASSERT( new_subarray->type == Value::T_ARRAY, "operand of range assignment is not array: " + new_subarray->ToString() );
	auto* v = (value::Array*)new_subarray;
	const auto dest_begin = from.has_value()
		? value.begin() + from.value()
		: value.begin();
#ifdef DEBUG
	const auto dest_end = to.has_value()
		? value.begin() + to.value() + 1
		: value.end();
	ASSERT( v->value.size() == dest_end - dest_begin, "range assignment value size mismatch ( " + std::to_string( v->value.size() ) + " != " + std::to_string( dest_end - dest_begin ) + " ): " + v->ToString() );
#endif
	std::copy( v->value.begin(), v->value.end(), dest_begin );
}

void Array::Append( Value* const new_value ) {
	value.push_back( new_value );
}

Value* const Array::GetRef( const size_t index ) {
	return VALUEEXT( ArrayRef, m_gc_space, this, index );
}

Value* const Array::GetRangeRef( const std::optional< size_t > from, const std::optional< size_t > to ) {
	return VALUEEXT( ArrayRangeRef, m_gc_space, this, from, to );
}

Value* const Array::FromVector( GSE_CALLABLE, const std::vector< Wrappable* >* data, const bool dynamic ) {
	array_elements_t elements = {};
	elements.reserve( data->size() );
	for ( const auto& el : *data ) {
		elements.push_back( el->Wrap( GSE_CALL, dynamic ) );
	}
	return VALUE( Array, , elements );
}

void Array::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	Value::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Array" );

	GC_DEBUG_BEGIN( "elements" );
	for ( const auto& v : value ) {
		GC_REACHABLE( v );
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

void Array::ValidateFromTo( const std::optional< size_t >& from, const std::optional< size_t >& to ) const {
	if ( from.has_value() ) {
		ASSERT( from.value() < value.size(), "range beginning overflow ( " + std::to_string( from.value() ) + " >= " + std::to_string( value.size() ) + " )" );
	}
	if ( to.has_value() ) {
		ASSERT( to.value() < value.size(), "range end overflow ( " + std::to_string( to.value() ) + " >= " + std::to_string( value.size() ) + " )" );
		if ( from.has_value() ) {
			ASSERT( from.value() <= to.value(), "invalid range definition: [" + std::to_string( from.value() ) + ":" + std::to_string( to.value() ) );
		}
	}
}

}
}
