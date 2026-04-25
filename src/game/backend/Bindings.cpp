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
#include "gc/Space.h"

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

void Bindings::AddToContext( gc::Space* const gc_space, gse::context::Context* ctx, gse::ExecutionPointer& ep ) {
	ctx->CreateBuiltin( "main", NATIVE_CALL(this) {
		N_EXPECT_ARGS( 1 );
		const auto& main = arguments.at(0);
		N_CHECKARG( main, 0, Callable );
		m_main_callables.push_back( main );
		return VALUE( gse::value::Undefined );
	} ), ep );
}

void Bindings::RunMainScript() {
	auto* gc_space = m_gse->GetGCSpace();
	gc_space->Accumulate( nullptr, [ this ]( ) {
		gse::ExecutionPointer ep;
		m_gse->RunScript( m_gse->GetGCSpace(), m_gse_context, m_si_internal, ep, m_entry_script );
		for ( const auto& mod_path : g_engine->GetConfig()->GetModPaths() ) {
			m_gse->RunScript( m_gse->GetGCSpace(), m_gse_context, m_si_internal, ep, util::FS::GeneratePath( { mod_path, "main" } ) );
		}
	});
}

void Bindings::RunMain() {
	auto* gc_space = m_gse->GetGCSpace();
	gc_space->Accumulate( nullptr, [ this, &gc_space ]( ) {
		auto si = m_si_internal;
		auto* ctx = m_gse_context;
		gse::ExecutionPointer ep;
		ASSERT( gc_space, "gc space is null" );
		for ( const auto& main : m_main_callables ) {
			ASSERT( main->type == gse::VT_CALLABLE, "main not callable" );
			auto gm = m_state->Wrap( GSE_CALL, gc_space );
			( (gse::value::Callable*)main )->Run( gc_space, m_gse_context, m_si_internal, ep, { gm } );
		}
	});
}

gc::Space* const Bindings::GetGCSpace() const {
	ASSERT( m_gse, "gse not set" );
	return m_gse->GetGCSpace();
}

gse::context::Context* const Bindings::GetContext() const {
	ASSERT( m_gse_context, "gse context not set" );
	return m_gse_context;
}

gse::Value* const Bindings::Trigger( gse::GCWrappable* object, const std::string& event, const f_args_t& f_args ) {
	CHECKACCUM( m_gse->GetGCSpace() );
	auto* gc_space = m_gse->GetGCSpace();
	gse::Value* result = nullptr;
	try {
		{
			gse::ExecutionPointer ep;
			result = object->Trigger( gc_space, m_gse_context, m_si_internal, ep, event, f_args );
		}
		auto* game = m_state->GetGame();
		if ( game ) {
			game->GetUM()->PushUpdates();
			game->GetBM()->PushUpdates();
		}
		if ( !result ) {
			// return undefined by default
			return VALUEEXT( gse::value::Undefined, gc_space );
		}
	}
	catch ( const gse::Exception& e ) {
		if ( m_state->m_on_gse_error ) {
			m_state->m_on_gse_error( e );
		}
		else {
			throw;
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
