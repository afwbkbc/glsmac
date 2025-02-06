#include "GLSMAC.h"

#include <iostream>

#include "util/FS.h"
#include "engine/Engine.h"
#include "config/Config.h"
#include "resource/ResourceManager.h"
#include "task/game/Game.h"
#include "task/common/Common.h"
#include "scheduler/Scheduler.h"

#include "ui/UI.h"

#include "gse/GSE.h"
#include "gse/ExecutionPointer.h"
#include "gse/Async.h"
#include "gse/context/GlobalContext.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Undefined.h"

#include "game/backend/State.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/Player.h"

static GLSMAC* s_glsmac = nullptr;

GLSMAC::GLSMAC() {
	// there can only be one GLSMAC
	ASSERT( !s_glsmac, "GLSMAC already defined" );
	s_glsmac = this;

	Log( "Creating global state" );

	// scripting stuff
	NEW( m_gse, gse::GSE );
	m_gse->AddBindings( this );
	m_ctx = m_gse->CreateGlobalContext();
	m_ctx->IncRefs();

	const auto& c = g_engine->GetConfig();

	const auto entry_script =
		util::FS::GeneratePath(
			{
				c->GetDataPath(),
				"default", // only 'default' mod for now
				c->GetNewUIMainScript() // script name (extension is appended automatically)
			}, gse::GSE::PATH_SEPARATOR
		)
	;

	{
		gse::ExecutionPointer ep;

		// global objects
		NEW( m_ui, ui::UI, m_ctx, { "" }, ep );

		// run main(s)
		m_gse->RunScript( m_ctx, {}, ep, entry_script );
		for ( const auto& mod_path : g_engine->GetConfig()->GetModPaths() ) {
			m_gse->RunScript(
				m_ctx, {}, ep, util::FS::GeneratePath(
					{
						mod_path,
						"main"
					}
				)
			);
		}
	}
}

GLSMAC::~GLSMAC() {

	Log( "Destroying global state" );

	if ( m_state ) {
		delete m_state;
	}

	m_ctx->DecRefs();

	m_gse->GetAsync()->StopTimers();

	DELETE( m_ui );
	DELETE( m_gse );

	s_glsmac = nullptr;
}

void GLSMAC::Iterate() {
	m_gse->Iterate();
	m_ui->Iterate();
}

WRAPIMPL_BEGIN( GLSMAC )
	WRAPIMPL_PROPS
		{
			"ui",
			m_ui->Wrap( true )
		},
		{
			"run",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				S_Init( GSE_CALL, {} );
				return VALUE( gse::type::Undefined );
			} )
		},
		{
			"exit",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				g_engine->ShutDown();
				return VALUE( gse::type::Undefined );
			} )
		},
		{
			"deinit",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				DeinitGameState( GSE_CALL );
				return VALUE( gse::type::Undefined );
			} )
		},
		{
			"init_single_player",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				InitGameState( GSE_CALL );
				m_state->m_settings.local.game_mode = game::backend::settings::LocalSettings::GM_SINGLEPLAYER;
				m_state->m_settings.global.Initialize();
				return VALUE( gse::type::Undefined );
			} )
		},
		{
			"randomize_settings",
			NATIVE_CALL( this ) {
				RandomizeSettings( GSE_CALL );
				return VALUE( gse::type::Undefined );
			} )
		},
		{
			"start_game",
			NATIVE_CALL( this ) {
				if ( !m_state ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Game not initialized" );
				}
				if ( m_is_game_running ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Game is already running" );
				}
				AddSinglePlayerSlot();
				StartGame( GSE_CALL );
				return VALUE( gse::type::Undefined );
			} )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( GLSMAC )

void GLSMAC::S_Init( GSE_CALLABLE, const std::optional< std::string >& path ) {
	const auto& c = g_engine->GetConfig();
	auto* r = g_engine->GetResourceManager();
	try {
		if ( path.has_value() ) {
			r->Init( { path.value() }, config::ST_AUTO );
		}
		else {
			r->Init( c->GetPossibleSMACPaths(), c->GetSMACType() );
		}
	} catch ( const std::runtime_error& e ) {
		gse::type::object_properties_t args = {
			{
				"set_smacpath", VALUE( gse::callable::Native, [ this ]( GSE_CALLABLE, const gse::type::function_arguments_t& arguments ) -> gse::Value {
					N_EXPECT_ARGS( 1 );
					N_GETVALUE( path, 0, String );
					S_Init( GSE_CALL, path );
					return VALUE( gse::type::Undefined );
				} )
			}
		};
		if ( path.has_value() ) {
			args.insert({ "last_failed_path", VALUE( gse::type::String, path.value() ) } );
		}
		Trigger( GSE_CALL, "smacpath_prompt", args );
		return;
	}
	if ( path.has_value() ) {
		c->SetSMACPath( path.value() );
	}

	if ( c->HasLaunchFlag( config::Config::LF_QUICKSTART ) ) {
		THROW( "TODO: QUICKSTART" );
	}
	else if (
		c->HasLaunchFlag( config::Config::LF_SKIPINTRO ) ||
		r->GetDetectedSMACType() == config::ST_LEGACY // it doesn't have firaxis logo image
	) {
		S_MainMenu( GSE_CALL );
	}
	else {
		S_Intro( GSE_CALL );
	}
}

void GLSMAC::S_Intro( GSE_CALLABLE ) {
	Trigger( GSE_CALL, "intro", {
		{
			"mainmenu",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				S_MainMenu( GSE_CALL );
				return VALUE( gse::type::Undefined );
			} )
		}
	} );
}

void GLSMAC::S_MainMenu( GSE_CALLABLE ) {
	Trigger( GSE_CALL, "mainmenu", {} );
}

void GLSMAC::S_Game( GSE_CALLABLE ) {
	Trigger( GSE_CALL, "game", {} );
}

void GLSMAC::DeinitGameState( GSE_CALLABLE ) {
	if ( m_state ) {
		if ( m_is_game_running ) {
			GSE_ERROR( gse::EC.GAME_ERROR, "Game is still running" );
		}
		delete m_state;
		m_state = nullptr;
	}
}

void GLSMAC::InitGameState( GSE_CALLABLE ) {
	if ( m_state ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Game is already initialized" );
	}
	if ( m_is_game_running ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Game is already running" );
	}
	m_state = new game::backend::State();
	m_state->InitBindings();
	m_state->Configure();
}

void GLSMAC::RandomizeSettings( GSE_CALLABLE ) {
	if ( !m_state ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Game is not initialized" );
	}
	if ( m_is_game_running ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Game is already running" );
	}
}

void GLSMAC::AddSinglePlayerSlot() {
	m_state->m_slots->Resize( 7 ); // TODO: make dynamic?
	const auto& rules = m_state->m_settings.global.game_rules;
	m_state->m_settings.local.player_name = "Player";
	NEWV( player, ::game::backend::Player,
		m_state->m_settings.local.player_name,
		::game::backend::Player::PR_SINGLE,
		{},
		rules.GetDefaultDifficultyLevel() // TODO: make configurable
	);
	m_state->AddPlayer( player );
	size_t slot_num = 0; // player always has slot 0
	m_state->AddCIDSlot( 0, slot_num ); // for consistency
	auto& slot = m_state->m_slots->GetSlot( slot_num );
	slot.SetPlayer( player, 0, "" );
	slot.SetPlayerFlag( ::game::backend::slot::PF_READY );
}

void GLSMAC::StartGame( GSE_CALLABLE ) {
	// real state belongs to game task now
	// save it as backup, then make temporary shallow copy (no connection, players etc)
	//   just for the sake of passing settings to previous menu
	auto* real_state = m_state;
	m_state = nullptr;
	m_is_game_running = true;

	// TODO: don't spawn new tasks
	NEWV( task, task::Common );
	g_engine->GetScheduler()->AddTask( task );
	NEWV( task2, task::game::Game, real_state, UH( this ) {
		//g_engine->GetScheduler()->RemoveTask( this );
	}, UH( this, real_state ) {
		//m_menu_object->MaybeClose();
	} );
	g_engine->GetScheduler()->AddTask( task2 );
	S_Game( GSE_CALL );
}

void GLSMAC::RunMain() {
	try {
		for ( const auto& main : m_main_callables ) {
			ASSERT_NOLOG( main.Get()->type == gse::type::Type::T_CALLABLE, "main not callable" );
			gse::ExecutionPointer ep;
			( (gse::type::Callable*)main.Get() )->Run( m_ctx, {}, ep, { Wrap() } );
		}
	} catch ( gse::Exception& e ) {
		std::cout << e.ToString() << std::endl;
		throw e;
	}
}

void GLSMAC::AddToContext( gse::context::Context* ctx, gse::ExecutionPointer& ep ) {
	ctx->CreateBuiltin( "main", NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 1 );
		const auto& main = arguments.at(0);
		N_CHECKARG( main.Get(), 0, Callable );
		m_main_callables.push_back( main );
		return VALUE( gse::type::Undefined );
	} ), ep );
}
