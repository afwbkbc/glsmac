#include "Object.h"

#include <unordered_map>

#include "Undefined.h"
#include "ObjectRef.h"
#include "Exception.h"
#include "String.h"

#include "gse/Wrappable.h"

namespace gse {
namespace type {

static Value s_undefined = VALUE( type::Undefined );

Object::Object( object_properties_t initial_value, const object_class_t object_class, Wrappable* wrapobj, wrapsetter_t* wrapsetter )
	: Type( GetType() )
	, value( initial_value )
	, object_class( object_class )
	, wrapobj( wrapobj )
	, wrapsetter( wrapsetter ) {
	if ( wrapobj ) {
		wrapobj->Link( this );
	}
}

Object::~Object() {
	if ( wrapobj ) {
		wrapobj->Unlink( this );
	}
}

const Value& Object::Get( const object_key_t& key ) const {
	const auto& it = value.find( key );
	return it == value.end()
		? s_undefined
		: it->second;
}

void Object::Set( const object_key_t& key, const Value& new_value, context::Context* ctx, const si_t& si ) {
	const bool has_value = new_value.Get()->type != T_UNDEFINED;
	const auto it = value.find( key );
	if (
		( has_value && ( it == value.end() || new_value != it->second ) ) ||
			( !has_value && it != value.end() )
		) {
		if ( wrapobj ) {
			if ( !wrapsetter ) {
				throw gse::Exception( EC.INVALID_ASSIGNMENT, "Property is read-only", ctx, si );
			}
			wrapsetter( wrapobj, key, new_value, ctx, si );
		}
		if ( has_value ) {
			value.insert_or_assign( key, new_value );
		}
		else {
			value.erase( key );
		}
	}
}

const Value Object::GetRef( const object_key_t& key ) {
	return VALUE( ObjectRef, this, key );
}

void Object::Unlink() {
	ASSERT_NOLOG( wrapobj, "wrapobj not linked" );
	wrapobj = nullptr;
	wrapsetter = nullptr;
}

}
}
