#include "EventHandler.h"

#include "gse/value/String.h"
#include "gse/value/Bool.h"

namespace game {
namespace backend {
namespace event {

EventHandler::EventHandler(
	gc::Space* const gc_space, const std::string& name,
	gse::value::Callable* const validate,
	gse::value::Callable* const resolve,
	gse::value::Callable* const apply,
	gse::value::Callable* const rollback
)
	: gc::Object( gc_space )
	, m_name( name )
	, m_validate( validate )
	, m_resolve( resolve )
	, m_apply( apply )
	, m_rollback( rollback ) {
	ASSERT( validate, "validate not set" );
	ASSERT( apply, "apply not set" );
	ASSERT( rollback, "rollback not set" );
}

void EventHandler::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	gc::Object::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Event" );

	GC_DEBUG_BEGIN( "validate" );
	GC_REACHABLE( m_validate );
	GC_DEBUG_END();

	if ( m_resolve ) {
		GC_DEBUG_BEGIN( "resolve" );
		GC_REACHABLE( m_resolve );
		GC_DEBUG_END();
	}

	GC_DEBUG_BEGIN( "apply" );
	GC_REACHABLE( m_apply );
	GC_DEBUG_END();

	GC_DEBUG_BEGIN( "rollback" );
	GC_REACHABLE( m_rollback );
	GC_DEBUG_END();

	GC_DEBUG_END();
}

const std::string* const EventHandler::Validate( GSE_CALLABLE, const gse::value::function_arguments_t& args ) const {
	const auto* result = m_validate->Run( GSE_CALL, args );
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

const bool EventHandler::HasResolve() const {
	return m_resolve != nullptr;
}

gse::Value* const EventHandler::Resolve( GSE_CALLABLE, const gse::value::function_arguments_t& args ) const {
	ASSERT( m_resolve, "resolve not set" );
	return m_resolve->Run( GSE_CALL, args );
}

gse::Value* const EventHandler::Apply( GSE_CALLABLE, const gse::value::function_arguments_t& args ) const {
	return m_apply->Run( GSE_CALL, args );
}

void EventHandler::Rollback( GSE_CALLABLE, const gse::value::function_arguments_t& args ) const {
	m_rollback->Run( GSE_CALL, args );
}

}
}
}
