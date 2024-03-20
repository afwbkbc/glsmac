#include "Bindings.h"

#include "game/Game.h"

#include "gse/type/String.h"
#include "gse/type/Int.h"
#include "gse/type/Object.h"
#include "gse/type/Callable.h"

#include "game/State.h"

using namespace gse;

namespace game {
namespace bindings {

Bindings::Bindings( State* state )
	: m_state( state ) {
	NEW( m_gse, gse::GSE );
	m_gse->AddBindings( this );
	m_gse_context = m_gse->CreateGlobalContext();
	m_gse_context->IncRefs();

#define B( _name ) new BINDING( _name )( this )
	m_bindings = {
		B( message ),
		B( exit ),
		B( random ),
		B( on ),
		B( players ),
		B( factions ),
		B( units ),
		B( map ),
	};
#undef B
}

Bindings::~Bindings() {
	for ( auto& it : m_bindings ) {
		delete it;
	}
	m_gse_context->DecRefs();
	DELETE( m_gse );
}

void Bindings::AddToContext( gse::Context* ctx ) {
	type::Object::properties_t methods = {};
	for ( auto& it : m_bindings ) {
		it->Add( methods );
	}
	ctx->CreateBuiltin( "game", VALUE( type::Object, methods ) );
}

void Bindings::RunMain() {
	m_gse->GetInclude( m_gse_context, m_si_internal, m_entry_script );
}

gse::Value Bindings::Call( const callback_slot_t slot, const callback_arguments_t& arguments ) {
	const auto& it = m_callbacks.find( slot );
	if ( it != m_callbacks.end() ) {
		try {
			type::Object::properties_t properties = arguments;
			return ( (gse::type::Callable*)it->second.Get() )->Run(
				m_gse_context, m_si_internal, {
					VALUE( type::Object, properties ),
				}
			);
		}
		catch ( gse::Exception& e ) {
			if ( m_state->m_on_gse_error ) {
				m_state->m_on_gse_error( e );
			}
			else {
				throw std::runtime_error( e.ToStringAndCleanup() );
			}
		}
	}
	return VALUE( type::Undefined );
}

State* Bindings::GetState() const {
	return m_state;
}

Game* Bindings::GetGame( gse::Context* ctx, const si_t& call_si ) const {
	auto* game = m_state->GetGame();
	if ( !game ) {
		ERROR( EC.GAME_ERROR, "Game not started yet" );
	}
	return game;
}

void Bindings::SetCallback( const callback_slot_t slot, const gse::Value& callback, gse::Context* context, const si_t& si ) {
	if ( m_callbacks.find( slot ) != m_callbacks.end() ) {
		throw gse::Exception( EC.GAME_ERROR, "Callback slot already in use", context, si );
	}
	m_callbacks.insert_or_assign( slot, callback );
}

}
}
