#include "Bindings.h"

#include "util/FS.h"

#include "game/backend/Game.h"
#include "game/backend/unit/UnitManager.h"
#include "game/backend/base/BaseManager.h"
#include "gse/GSE.h"
#include "gse/context/GlobalContext.h"
#include "gse/Exception.h"
#include "gse/value/String.h"
#include "gse/value/Callable.h"
#include "gse/value/Undefined.h"
#include "gse/callable/Native.h"
#include "engine/Engine.h"
#include "config/Config.h"
#include "game/backend/State.h"
#include "gse/ExecutionPointer.h"

namespace game {
namespace backend {

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

}

Bindings::~Bindings() {
	DELETE( m_gse );
}

void Bindings::AddToContext( gse::context::Context* ctx, gse::ExecutionPointer& ep ) {
	ctx->CreateBuiltin( "main", NATIVE_CALL(this) {
		N_EXPECT_ARGS( 1 );
		const auto& main = arguments.at(0);
		N_CHECKARG( main, 0, Callable );
		m_main_callables.push_back( main );
		return VALUE( gse::value::Undefined );
	} ), ep );
}

void Bindings::RunMainScript() {
	gse::ExecutionPointer ep;
	m_gse->RunScript( m_gse_context, m_si_internal, ep, m_entry_script );
	for ( const auto& mod_path : g_engine->GetConfig()->GetModPaths() ) {
		m_gse->RunScript( m_gse_context, m_si_internal, ep, util::FS::GeneratePath( { mod_path, "main" } ) );
	}
}

void Bindings::RunMain() {
	for ( const auto& main : m_main_callables ) {
		ASSERT_NOLOG( main->type == gse::Value::T_CALLABLE, "main not callable" );
		auto gm = m_state->Wrap();
		gse::ExecutionPointer ep;
		((gse::value::Callable*)main)->Run( m_gse_context, m_si_internal, ep, { gm });
	}
}

gse::Value* const Bindings::Trigger( gse::Wrappable* object, const std::string& event, const gse::value::object_properties_t& args ) {
	gse::Value* result = VALUE( gse::value::Undefined );
	try {
		{
			gse::ExecutionPointer ep;
			result = object->Trigger( m_gse_context, m_si_internal, ep, event, args );
		}
		auto* game = m_state->GetGame();
		if ( game ) {
			game->GetUM()->PushUpdates();
			game->GetBM()->PushUpdates();
		}
		if ( result->type == gse::Value::T_NOTHING ) {
			// return undefined by default
			return VALUE( gse::value::Undefined );
		}
	}
	catch ( const gse::Exception& e ) {
		if ( m_state->m_on_gse_error ) {
			m_state->m_on_gse_error( e );
		}
		else {
			throw std::runtime_error( e.ToString() );
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

}
}
