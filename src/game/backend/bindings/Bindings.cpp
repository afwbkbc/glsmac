#include "Bindings.h"

#include <iostream>

#include "util/FS.h"

#include "game/backend/Game.h"
#include "game/backend/unit/UnitManager.h"
#include "Binding.h"
#include "gse/GSE.h"
#include "gse/context/GlobalContext.h"
#include "gse/Exception.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"
#include "gse/type/Callable.h"
#include "gse/type/Undefined.h"
#include "gse/callable/Native.h"
#include "engine/Engine.h"
#include "config/Config.h"
#include "game/backend/State.h"
#include "game/backend/System.h"

namespace game {
namespace backend {
namespace bindings {

Bindings::Bindings( State* state )
	: m_state( state )
	, m_entry_script(
		util::FS::GeneratePath(
			{
				g_engine->GetConfig()->GetDataPath(),
				"default", // only 'default' mod for now
				"main" // script name (extension is appended automatically)
			}, gse::GSE::PATH_SEPARATOR
		)
	) {
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
		B( tiles ),
		B( units ),
		B( bases ),
		B( animations ),
		B( map ),
		B( resources ),
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

void Bindings::AddToContext( gse::context::Context* ctx ) {
	gse::type::object_properties_t methods = {};
	for ( auto& it : m_bindings ) {
		it->Add( methods );
	}
	ctx->CreateBuiltin( "game", VALUE( gse::type::Object, methods ) );
	ctx->CreateBuiltin( "system", m_state->GetSystem()->Wrap( true ) );
}

void Bindings::RunMain() {
	m_gse->GetInclude( m_gse_context, m_si_internal, m_entry_script );
}

gse::Value Bindings::Call( const callback_slot_t slot, const callback_arguments_t& arguments ) {
	const auto& it = m_callbacks.find( slot );
	if ( it != m_callbacks.end() ) {
		try {
			gse::type::object_properties_t properties = arguments;
			const gse::Value result = ( (gse::type::Callable*)it->second.Get() )->Run(
				m_gse_context, m_si_internal, {
					VALUE( gse::type::Object, properties ),
				}
			);
			auto* game = m_state->GetGame();
			if ( game ) {
				game->GetUM()->PushUnitUpdates();
			}
			if ( result.Get()->type == gse::type::Type::T_NOTHING ) {
				// return undefined by default
				return VALUE( gse::type::Undefined );
			}
			return result;
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
	return VALUE( gse::type::Undefined );
}

const gse::Value Bindings::Trigger( gse::Wrappable* object, const std::string& event, const gse::type::object_properties_t& args ) {
	auto result = VALUE( gse::type::Undefined );
	try {
		result = object->Trigger( m_gse_context, m_si_internal, event, args );
		if ( result.Get()->type == gse::type::Type::T_NOTHING ) {
			// return undefined by default
			return VALUE( gse::type::Undefined );
		}
	}
	catch ( gse::Exception& e ) {
		if ( m_state->m_on_gse_error ) {
			m_state->m_on_gse_error( e );
		}
		else {
			throw std::runtime_error( e.ToStringAndCleanup() );
		}
	}
	return result;
}

State* Bindings::GetState() const {
	return m_state;
}

Game* Bindings::GetGame( GSE_CALLABLE ) const {
	auto* game = m_state->GetGame();
	if ( !game ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Game not started yet" );
	}
	return game;
}

void Bindings::SetCallback( const callback_slot_t slot, const gse::Value& callback, gse::context::Context* context, const gse::si_t& si ) {
	if ( m_callbacks.find( slot ) != m_callbacks.end() ) {
		throw gse::Exception( gse::EC.GAME_ERROR, "Callback slot already in use", context, si );
	}
	m_callbacks.insert_or_assign( slot, callback );
}

}
}
}
