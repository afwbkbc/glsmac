#include "Wrappable.h"

namespace gse {

Wrappable::~Wrappable() {
	for ( const auto& wrapobj : m_wrapobjs ) {
		wrapobj->Unlink();
	}
}

void Wrappable::Link( value::Object* wrapobj ) {
	ASSERT_NOLOG( m_wrapobjs.find( wrapobj ) == m_wrapobjs.end(), "wrapobj already linked" );
	m_wrapobjs.insert( wrapobj );
}

void Wrappable::Unlink( value::Object* wrapobj ) {
	ASSERT_NOLOG( m_wrapobjs.find( wrapobj ) != m_wrapobjs.end(), "wrapobj not linked" );
	m_wrapobjs.erase( wrapobj );
}

const Wrappable::callback_id_t Wrappable::On( GSE_CALLABLE, const std::string& event, gse::Value* const callback ) {
	ASSERT_NOLOG( callback->type == Value::T_CALLABLE, "callback not callable" );

	auto it = m_callbacks.find( event );
	if ( it == m_callbacks.end() ) {
		it = m_callbacks.insert(
			{
				event,
				{}
			}
		).first;
	}
	return it->second.insert(
		{
			++m_next_callback_id,
			{
				callback,
				ctx,
				si
			}
		}
	).first->first;
}

void Wrappable::Off( GSE_CALLABLE, const std::string& event, const callback_id_t callback_id ) {
	const auto& it = m_callbacks.find( event );
	if ( it != m_callbacks.end() ) {
		if ( callback_id ) {
			// delete one handler
			const auto& it2 = it->second.find( callback_id );
			if ( it2 != it->second.end() ) {
				it->second.erase( it2 );
			}
			else {
				GSE_ERROR( gse::EC.INVALID_CALL, "Callback [" + event + "/" + std::to_string( callback_id ) + "] not found" );
			}
		}
		else {
			// delete all handlers
			m_callbacks.erase( it );
		}
	}
}

const bool Wrappable::HasHandlers( const std::string& event ) {
	return m_callbacks.find( event ) != m_callbacks.end();
}

Value* const Wrappable::Trigger( GSE_CALLABLE, const std::string& event, const f_args_t& f_args, const std::optional< Value::type_t > expected_return_type ) {
	Value* result = nullptr;
	const auto& it = m_callbacks.find( event );
	if ( it != m_callbacks.end() ) {
		value::object_properties_t args = {};
		if ( f_args ) {
			f_args( args );
		}
		auto e = VALUEEXT( gse::value::Object, GSE_CALL, args );
		const auto callbacks = it->second; // copy because callbacks may be changed during trigger
		for ( const auto& it2 : callbacks ) {
			const auto& cb = it2.second.callable;
			ASSERT_NOLOG( cb->type == Value::T_CALLABLE, "callback not callable" );
			result = ( (value::Callable*)cb )->Run( gc_space, it2.second.ctx, it2.second.si, ep, { e } );
			if ( expected_return_type.has_value() ) {
				if ( result->type != expected_return_type.value() ) {
					throw gse::Exception( gse::EC.INVALID_HANDLER, "Event handler is expected to return " + Value::GetTypeStringStatic( expected_return_type.value() ) + ", got " + result->GetTypeString() + ": " + result->ToString(), it2.second.ctx, it2.second.si, ep );
				}
			}
			if ( result->type != Value::T_UNDEFINED ) {
				// TODO: resolve result conflicts somehow
			}
		}
	}
	return result
		? result
		: VALUE( gse::value::Undefined );
}

}
