#include "Wrappable.h"

#include "gse/type/Object.h"

namespace gse {

Wrappable::~Wrappable() {
	for ( const auto& wrapobj : m_wrapobjs ) {
		wrapobj->Unlink();
	}
}

void Wrappable::Link( type::Object* wrapobj ) {
	ASSERT_NOLOG( m_wrapobjs.find( wrapobj ) == m_wrapobjs.end(), "wrapobj already linked" );
	m_wrapobjs.insert( wrapobj );
}

void Wrappable::Unlink( type::Object* wrapobj ) {
	ASSERT_NOLOG( m_wrapobjs.find( wrapobj ) != m_wrapobjs.end(), "wrapobj not linked" );
	m_wrapobjs.erase( wrapobj );
}

const Wrappable::callback_id_t Wrappable::On( GSE_CALLABLE, const std::string& event, const gse::Value& callback ) {
	ASSERT_NOLOG( callback.Get()->type == type::Type::T_CALLABLE, "callback not callable" );
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
				call_si
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

const bool Wrappable::HasHandlers( const std::string& event ) const {
	return m_callbacks.find( event ) != m_callbacks.end();
}

const Value Wrappable::Trigger( GSE_CALLABLE, const std::string& event, const type::object_properties_t& args, const std::optional< type::Type::type_t > expected_return_type ) {
	const auto& it = m_callbacks.find( event );
	Value result = VALUE( gse::type::Undefined );
	if ( it != m_callbacks.end() ) {
		auto e = VALUE( gse::type::Object, args );
		for ( const auto& it2 : it->second ) {
			const auto& cb = it2.second.callable;
			ASSERT_NOLOG( cb.Get()->type == type::Type::T_CALLABLE, "callback not callable" );
			result = ( (type::Callable*)cb.Get() )->Run( it2.second.ctx, it2.second.si, { e } );
			if ( expected_return_type.has_value() ) {
				if ( result.Get()->type != expected_return_type.value() ) {
					throw gse::Exception( gse::EC.INVALID_HANDLER, "Event handler is expected to return " + type::Type::GetTypeString( expected_return_type.value() ) + ", got " + result.GetTypeString() + ": " + result.ToString(), it2.second.ctx, it2.second.si );
				}
			}
			if ( result.Get()->type != type::Type::T_UNDEFINED ) {
				// TODO: resolve result conflicts somehow
			}
		}
	}
	return result;
}

}
