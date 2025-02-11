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

#include "game/frontend/Game.h"

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

	if ( m_load_thread ) {
		Log( "Waiting for load thread to stop" );
		m_load_thread.load()->join();
		delete m_load_thread;
	}

	if ( m_is_loader_shown ) {
		HideLoader();
	}

	Log( "Destroying global state" );

	if ( m_game ) {
		m_game->Stop();
		delete m_game;
	}

	if ( m_state ) {
		delete m_state;
	}

	m_ctx->DecRefs();

	m_gse->GetAsync()->StopTimers();

	{
		gse::ExecutionPointer ep;
		m_ui->Destroy( m_ctx, { "" }, ep );
	}

	DELETE( m_gse );

	s_glsmac = nullptr;
}

void GLSMAC::Iterate() {
	{
		bool ticked = false;
		while ( m_loader_dots_timer.HasTicked() ) {
			m_loader_dots++;
			if ( m_loader_dots > 3 ) {
				m_loader_dots = 1;
			}
			ticked = true;
		}
		if ( ticked ) {
			UpdateLoaderText();
		}
	}
	if ( m_load_thread ) {
		if ( !m_is_loading ) {
			ASSERT_NOLOG( m_f_after_load, "f_after_load not set" );
			m_load_thread.load()->join();
			delete m_load_thread;
			m_load_thread = nullptr;
			HideLoader();
			m_f_after_load();
			m_f_after_load = nullptr;
		}
	}
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
				N_EXPECT_ARGS( 1 );
				N_PERSIST_CALLABLE( on_complete, 0 );
				InitGameState( GSE_CALL, [ this, on_complete, ctx, si, ep ] () {
					m_state->m_settings.local.game_mode = game::backend::settings::LocalSettings::GM_SINGLEPLAYER;
					m_state->m_settings.global.Initialize();
					auto ep2 = ep;
					on_complete->Run( ctx, si, ep2, {} );
					N_UNPERSIST_CALLABLE( on_complete );
				} );
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
				if ( m_is_loading ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Game still initializing" );
				}
				AddSinglePlayerSlot();
				StartGame( GSE_CALL );
				return VALUE( gse::type::Undefined );
			} )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( GLSMAC )

void GLSMAC::ShowLoader( const std::string& text ) {
	ASSERT_NOLOG( !m_is_loader_shown, "loader already shown" );
	m_is_loader_shown = true;
	m_loader_text = text;
	m_loader_dots = 1;
	m_loader_dots_timer.SetInterval( 100 );
	gse::ExecutionPointer ep;
	Trigger( m_ctx, {}, ep, "loader_show", {} );
	UpdateLoaderText();
}

void GLSMAC::HideLoader() {
	ASSERT_NOLOG( m_is_loader_shown, "loader not shown" );
	m_is_loader_shown = false;
	m_loader_dots_timer.Stop();
	gse::ExecutionPointer ep;
	Trigger( m_ctx, {}, ep, "loader_hide", {} );
}

void GLSMAC::AsyncLoad( const std::string& text, const std::function< void() >& f, const std::function< void() >& f_after_load ) {
	ASSERT_NOLOG( !m_load_thread, "load thread already set" );
	ASSERT_NOLOG( !m_f_after_load, "f_after load already set" );
	m_is_loading = true;
	m_f_after_load = f_after_load;
	ShowLoader( text );
	m_load_thread = new std::thread( [ this, f ] {
		f();
		m_is_loading = false;
	});
}

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
	Trigger( GSE_CALL, "mainmenu_show", {} );
}

void GLSMAC::S_Game( GSE_CALLABLE ) {
	Trigger( GSE_CALL, "mainmenu_hide", {} );
	m_game->Start();
	Trigger( GSE_CALL, "game", {} );
}

void GLSMAC::UpdateLoaderText() {
	ASSERT( m_is_loader_shown, "loader not shown" );
	gse::ExecutionPointer ep;
	std::string text = m_loader_text + std::string( m_loader_dots, '.' ) + std::string( 3 - m_loader_dots, ' ' );
	Trigger( m_ctx, {}, ep, "loader_text", {
		{
			"text", VALUE( gse::type::String, text )
		}
	} );
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

void GLSMAC::InitGameState( GSE_CALLABLE, const f_t& on_complete  ) {
	if ( m_state ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Game is already initialized" );
	}
	if ( m_is_game_running ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Game is already running" );
	}
	AsyncLoad( "Initializing game state", [ this ] {
		m_state = new game::backend::State();
		m_state->InitBindings();
		m_state->Configure();
	}, on_complete );
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

	m_game = new ::game::frontend::Game( nullptr, this, real_state, UH( this ) {
		//g_engine->GetScheduler()->RemoveTask( this );
		THROW( "TODO: reset" );
	}, UH( this, real_state ) {
		//m_menu_object->MaybeClose();
		THROW( "TODO: cancel" );
	} );

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
