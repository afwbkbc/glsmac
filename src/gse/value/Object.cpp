#include "Object.h"

#include "Undefined.h"
#include "ObjectRef.h"
#include "Exception.h"

#include "gse/Wrappable.h"
#include "gse/context/ChildContext.h"

namespace gse {
namespace value {

static Value* s_undefined = VALUE( value::Undefined );

Object::Object( context::ChildContext* const ctx, object_properties_t initial_value, const object_class_t object_class, Wrappable* wrapobj, wrapsetter_t* wrapsetter )
	: Value( GetType() )
	, m_ctx( ctx )
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

Value* const Object::Get( const object_key_t& key ) const {
	const auto& it = value.find( key );
	return it == value.end()
		? s_undefined
		: it->second;
}

void Object::Set( const object_key_t& key, Value* const new_value, GSE_CALLABLE ) {
	const bool has_value = new_value->type != T_UNDEFINED;
	const auto it = value.find( key );
	if (
		( has_value && ( it == value.end() || new_value != it->second ) ) ||
			( !has_value && it != value.end() )
		) {
		if ( wrapobj ) {
			if ( !wrapsetter ) {
				throw gse::Exception( EC.INVALID_ASSIGNMENT, "Property is read-only", GSE_CALL );
			}
			wrapsetter( wrapobj, key, new_value, GSE_CALL );
		}
		if ( has_value ) {
			value.insert_or_assign( key, new_value );
		}
		else {
			value.erase( key );
		}
	}
}

Value* const Object::GetRef( const object_key_t& key ) {
	return VALUE( ObjectRef, this, key );
}

void Object::Unlink() {
	ASSERT_NOLOG( wrapobj, "wrapobj not linked" );
	wrapobj = nullptr;
	wrapsetter = nullptr;
}

}
}
