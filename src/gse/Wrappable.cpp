#include "Wrappable.h"

#include "gse/value/Bool.h"

#if defined( DEBUG ) || defined( FASTDEBUG )
#include "engine/Engine.h"
#endif

namespace gse {

Wrappable::Wrappable( const Wrappable& other ) {
	// not copying callbacks
}

Wrappable& Wrappable::operator=( const Wrappable& other ) {
	// not copying callbacks
	return *this;
}

Wrappable::~Wrappable() {
	for ( const auto& wrapobj : m_wrapobjs ) {
		wrapobj->Unlink();
	}
}

void Wrappable::Link( value::Object* wrapobj ) {
	ASSERT( m_wrapobjs.find( wrapobj ) == m_wrapobjs.end(), "wrapobj already linked" );
	m_wrapobjs.insert( wrapobj );
}

void Wrappable::Unlink( value::Object* wrapobj ) {
	ASSERT( m_wrapobjs.find( wrapobj ) != m_wrapobjs.end(), "wrapobj not linked" );
	m_wrapobjs.erase( wrapobj );
}

const Wrappable::callback_id_t Wrappable::On( GSE_CALLABLE, const std::string& event, gse::Value* const callback ) {
	std::lock_guard guard( m_callbacks_mutex );
	if ( event == "*" ) {
		m_catchall = true;
	}
	ASSERT( callback->type == VT_CALLABLE, "callback not callable" );
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
	std::lock_guard guard( m_callbacks_mutex );
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
	std::lock_guard guard( m_callbacks_mutex );
	return m_callbacks.find( event ) != m_callbacks.end() || m_callbacks.find( "*" ) != m_callbacks.end();
}

Value* const Wrappable::Trigger( GSE_CALLABLE, const std::string& event, const f_args_t& f_args, const std::optional< value_type_t > expected_return_type ) {
	bool has_handler = false;
	{
		std::lock_guard guard( m_callbacks_mutex );
		has_handler = m_callbacks.find( event ) != m_callbacks.end();
	}
	if ( has_handler ) {
		Value* result = nullptr;
		value::object_properties_t args = {};
		if ( f_args ) {
			f_args( GSE_CALL, args );
		}
		auto* args_obj = VALUEEXT( gse::value::Object, GSE_CALL, args );
		result = Trigger( GSE_CALL, event, args_obj, expected_return_type );
		if ( result ) {
			return result;
		}
	}
	if ( m_catchall && event != "*" ) {
		return Trigger( GSE_CALL, "*", f_args, expected_return_type );
	}
	return VALUE( gse::value::Bool, , false );
}

Value* const Wrappable::Trigger( GSE_CALLABLE, const std::string& event, gse::value::Object* const args_obj, const std::optional< value_type_t > expected_return_type ) {
	std::lock_guard guard( m_callbacks_mutex );
	ASSERT( args_obj, "args_obj is null" );
	Value* result = nullptr;
	const auto& it = m_callbacks.find( event );
	if ( it != m_callbacks.end() ) {
		const auto callbacks = it->second; // copy because callbacks may be changed during trigger
		for ( const auto& it2 : callbacks ) {
			const auto& cb = it2.second.callable;
			if ( cb->type != VT_CALLABLE ) {
				ASSERT( cb->type == VT_CALLABLE, "callback not callable" );
			}
			m_callbacks_mutex.unlock(); // TODO: improve?
			try {
				result = ( (value::Callable*)cb )->Run( gc_space, it2.second.ctx, it2.second.si, ep, { args_obj } );
			}
			catch ( const gse::Exception& e ) {
				m_callbacks_mutex.lock(); // to prevent error when guard exits
				throw e;
			}
			catch ( const std::runtime_error& e ) {
				m_callbacks_mutex.lock(); // to prevent error when guard exits
				throw e;
			}
			m_callbacks_mutex.lock();
			if ( expected_return_type.has_value() ) {
				if ( !result || result->type != expected_return_type.value() ) {
					throw gse::Exception( gse::EC.INVALID_HANDLER, "Event handler is expected to return " + Value::GetTypeStringStatic( expected_return_type.value() ) + ", got " + result->GetTypeString() + ": " + result->ToString(), it2.second.ctx, it2.second.si, ep );
				}
			}
			if ( result && result->type != VT_UNDEFINED ) {
				// TODO: resolve result conflicts somehow
			}
		}
	}
	return result
		? result
		: VALUE( gse::value::Bool, , false );
}

void Wrappable::ClearHandlers() {
	std::lock_guard guard( m_callbacks_mutex );
/*#if defined( DEBUG ) || defined( FASTDEBUG )
	for ( const auto& it : m_callbacks ) {
		g_engine->Log( "Removing " + std::to_string( it.second.size() ) + " handler(s) for \"" + it.first + "\"" );
	}
#endif*/
	m_callbacks.clear();
}

void Wrappable::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	GC_DEBUG_BEGIN( "GCWrappable" );

	GC_DEBUG_BEGIN( "callbacks" );
	{
		std::lock_guard guard( m_callbacks_mutex );
		for ( const auto& it1 : m_callbacks ) {
			for ( const auto& it2 : it1.second ) {
				GC_REACHABLE( it2.second.callable );
			}
		}
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

}
