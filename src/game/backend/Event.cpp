#include "Event.h"

#include "gse/value/String.h"
#include "gse/value/Bool.h"

namespace game {
namespace backend {

Event::Event( gc::Space* const gc_space, const std::string& name, gse::value::Callable* const verify, gse::value::Callable* const apply, gse::value::Callable* const rollback )
	: gc::Object( gc_space )
	, m_name( name )
	, m_verify( verify )
	, m_apply( apply )
	, m_rollback( rollback ) {
	ASSERT_NOLOG( verify, "verify not set" );
	ASSERT_NOLOG( apply, "apply not set" );
	ASSERT_NOLOG( rollback, "rollback not set" );
}

void Event::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	gc::Object::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Event" );

	GC_DEBUG_BEGIN( "verify" );
	GC_REACHABLE( m_verify );
	GC_DEBUG_END();

	GC_DEBUG_BEGIN( "apply" );
	GC_REACHABLE( m_apply );
	GC_DEBUG_END();

	GC_DEBUG_BEGIN( "rollback" );
	GC_REACHABLE( m_rollback );
	GC_DEBUG_END();

	GC_DEBUG_END();
}

const std::string Event::ToString() {
	return "EventHandler( " + m_name + " )";
}

const std::string* const Event::Verify( GSE_CALLABLE, const gse::value::function_arguments_t& args ) const {
	const auto* result = m_verify->Run( GSE_CALL, args );
	if ( !result ) {
		return nullptr;
	}
	switch ( result->type ) {
		case gse::Value::T_STRING: {
			const auto& v = ( (gse::value::String*)result )->value;
			return v.empty()
				? nullptr
				: new std::string( v );
		}
		case gse::Value::T_UNDEFINED:
		case gse::Value::T_NULL:
			return nullptr;
		default:
			return new std::string( "unexpected result type from handler: " + result->GetTypeString() );
	}
}

void Event::Apply( GSE_CALLABLE, const gse::value::function_arguments_t& args ) const {
	m_apply->Run( GSE_CALL, args );
}

void Event::Rollback( GSE_CALLABLE, const gse::value::function_arguments_t& args ) const {
	m_rollback->Run( GSE_CALL, args );
}

}
}
