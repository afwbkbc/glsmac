#include "Object.h"

#include "Undefined.h"
#include "ObjectRef.h"
#include "Exception.h"

#include "gse/Wrappable.h"
#include "gse/context/Context.h"
#include "ValueRef.h"
#include "gse/context/ChildContext.h"
#include "gc/Space.h"

namespace gse {
namespace value {

Object::Object( GSE_CALLABLE, object_properties_t initial_value, const object_class_t object_class, Wrappable* wrapobj, wrapsetter_t* wrapsetter )
	: Value( gc_space, GetType() )
	, m_value( initial_value )
	, object_class( object_class )
	, wrapobj( wrapobj )
	, wrapsetter( wrapsetter )
	, m_si( si )
	, m_ep( ep ) {
	ASSERT( ctx, "object ctx is null" );
	ctx->ForkAndExecute(
		GSE_CALL, false, [ this, &gc_space, &si, &ep, &wrapobj ]( context::ChildContext* const subctx ) {
			m_ctx = subctx;
			m_this = VALUE( value::ValueRef, , this );
			m_ctx->CreateConst( "this", m_this, si, ep );
			if ( wrapobj ) {
				wrapobj->Link( this );
			}
		}
	);
}

Object::~Object() {
	if ( wrapobj ) {
		wrapobj->Unlink( this );
	}
}

Value* const Object::Get( const object_key_t& key ) {
	CHECKACCUM( m_gc_space );

	const auto& it = m_value.find( key );
	return it != m_value.end()
		? it->second
		: VALUEEXT( value::Undefined, m_gc_space );
}

void Object::Assign( const object_key_t& key, Value* const new_value, const std::function< void() >& f_on_set ) {
	CHECKACCUM( m_gc_space );

	const bool has_value = new_value && new_value->type != T_UNDEFINED;
	const auto it = m_value.find( key );
	if (
		( has_value && ( it == m_value.end() || new_value != it->second ) ) ||
			( !has_value && it != m_value.end() )
		) {
		if ( f_on_set ) {
			f_on_set();
		}
		if ( has_value ) {
			m_value.insert_or_assign( key, new_value );
		}
		else {
			m_value.erase( key );
		}
	}
}

void Object::Set( const object_key_t& key, Value* const new_value, GSE_CALLABLE ) {
	Assign(
		key, new_value, [ this, &key, &new_value, &ctx, &si, &ep, &gc_space ]() {
			if ( wrapobj ) {
				if ( !wrapsetter ) {
					GSE_ERROR( EC.INVALID_ASSIGNMENT, "Property is read-only" );
				}
				wrapsetter( wrapobj, key, new_value, GSE_CALL );
			}
		}
	);
}

Value* const Object::GetRef( const object_key_t& key ) {
	CHECKACCUM( m_gc_space );
	return VALUEEXT( ObjectRef, m_gc_space, this, key );
}

void Object::Unlink() {
	ASSERT( wrapobj, "wrapobj not linked" );
	wrapobj = nullptr;
	wrapsetter = nullptr;
	type = T_UNDEFINED; // make sure all corresponding variables are inaccessible in scripts
}

void Object::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	Value::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Object" );

	ASSERT( m_ctx, "object ctx not set" );
	GC_DEBUG_BEGIN( "internal_context" );
	GC_REACHABLE( m_ctx );
	GC_DEBUG_END();

	ASSERT( m_this, "this not set" );
	GC_REACHABLE( m_this );

	GC_DEBUG_BEGIN( "properties" );
	for ( const auto& v : m_value ) {
		GC_DEBUG_BEGIN( v.first );
		GC_REACHABLE( v.second );
		GC_DEBUG_END();
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

context::ChildContext* const Object::GetContext() const {
	return m_ctx;
}

}
}
