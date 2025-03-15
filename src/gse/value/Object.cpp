#include "Object.h"

#include "Undefined.h"
#include "ObjectRef.h"
#include "Exception.h"

#include "gse/Wrappable.h"
#include "gse/context/Context.h"
#include "ValueRef.h"
#include "gse/context/ChildContext.h"

namespace gse {
namespace value {

Object::Object( GSE_CALLABLE, object_properties_t initial_value, const object_class_t object_class, Wrappable* wrapobj, wrapsetter_t* wrapsetter )
	: Value( gc_space, GetType() )
	, value( initial_value )
	, object_class( object_class )
	, wrapobj( wrapobj )
	, wrapsetter( wrapsetter )
	, m_si( si )
	, m_ep( ep ) {
	ASSERT_NOLOG( ctx, "object ctx is null" );
	ctx->ForkAndExecute(
		GSE_CALL, false, [ this, &gc_space, &si, &ep, &wrapobj ]( context::ChildContext* const subctx ) {
			m_ctx = subctx;
			m_ctx->CreateConst( "this", VALUE( value::ValueRef, , this ), si, ep );
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
	std::lock_guard guard( m_gc_mutex );

	const auto& it = value.find( key );
	return it != value.end()
		? it->second
		: VALUEEXT( value::Undefined, m_gc_space );
}

void Object::Set( const object_key_t& key, Value* const new_value, GSE_CALLABLE ) {
	std::lock_guard guard( m_gc_mutex );

	const bool has_value = new_value && new_value->type != T_UNDEFINED;
	const auto it = value.find( key );
	if (
		( has_value && ( it == value.end() || new_value != it->second ) ) ||
			( !has_value && it != value.end() )
		) {
		if ( wrapobj ) {
			if ( !wrapsetter ) {
				GSE_ERROR( EC.INVALID_ASSIGNMENT, "Property is read-only" );
			}
			m_gc_mutex.unlock();
			wrapsetter( wrapobj, key, new_value, GSE_CALL );
			m_gc_mutex.lock();
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
	return VALUEEXT( ObjectRef, m_gc_space, this, key );
}

void Object::Unlink() {
	ASSERT_NOLOG( wrapobj, "wrapobj not linked" );
	wrapobj = nullptr;
	wrapsetter = nullptr;
}

void Object::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	Value::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Object" );

	ASSERT_NOLOG( m_ctx, "object ctx not set" );
	GC_DEBUG_BEGIN( "internal_context" );
	REACHABLE_EXT( m_ctx, CollectWithDependencies );
	GC_DEBUG_END();

	{
		std::lock_guard guard( m_gc_mutex );

		GC_DEBUG_BEGIN( "properties" );
		for ( const auto& v : value ) {
			GC_DEBUG_BEGIN( v.first );
			GC_REACHABLE( v.second );
			GC_DEBUG_END();
		}
		GC_DEBUG_END();
	}

	GC_DEBUG_END();
}

context::ChildContext* const Object::GetContext() const {
	return m_ctx;
}

}
}
