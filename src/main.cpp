#include <thread>
#include <fstream>
#include <sstream>
#include <chrono>

#if defined( DEBUG ) || defined ( FASTDEBUG )

#include <string>
#include <stdlib.h>

#endif

#include "config/Config.h"

#ifdef _WIN32
#include "error_handler/Win32.h"
#else

#include "error_handler/Stdout.h"

#endif

#include "logger/Console.h"

#if defined( DEBUG ) || defined( FASTDEBUG )

#include "logger/Stdout.h"
#include "graphics/Null.h"
#include "loader/font/Null.h"
#include "loader/texture/Null.h"
#include "loader/sound/Null.h"
#include "input/Null.h"
#include "audio/Null.h"

#endif

#include "resource/ResourceManager.h"

#include "loader/font/FreeType.h"
#include "loader/texture/SDL2.h"
#include "loader/sound/SDL2.h"
#include "loader/txt/TXTLoaders.h"
#include "input/sdl2/SDL2.h"
#include "graphics/opengl/OpenGL.h"
#include "audio/sdl2/SDL2.h"
#include "network/simpletcp/SimpleTCP.h"

#include "scheduler/Simple.h"
#include "ui_legacy/Default.h"

#include "task/common/Common.h"

#if defined( DEBUG ) || defined( FASTDEBUG )

#include "task/gseprompt/GSEPrompt.h"
#include "task/gsetests/GSETests.h"

#endif

#include "task/console/Console.h"

#include "task/main/Main.h"
#include "task/intro/Intro.h"
#include "task/mainmenu/MainMenu.h"
#include "task/game/Game.h"

#include "game/backend/Game.h"

#include "engine/Engine.h"

#include "version.h"

#include "game/backend/State.h"
#include "game/backend/faction/Faction.h"
#include "game/backend/faction/FactionManager.h"
#include "game/backend/Player.h"
#include "game/backend/slot/Slots.h"

// TODO: move to config
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define VSYNC true
#if defined( DEBUG ) || defined( FASTDEBUG )
#define START_FULLSCREEN false
#else
#define START_FULLSCREEN true
#endif

#include "util/FS.h"
#include "util/LogHelper.h"

#if defined( DEBUG ) || defined( FASTDEBUG )
#include "util/System.h"
#endif

#ifdef DEBUG

#include "debug/MemoryWatcher.h"
#include "debug/DebugOverlay.h"

#endif

int main( const int argc, const char* argv[] ) {
	// #region agent log
	{
		std::ofstream log("/Users/jhurliman/Documents/Code/jhurliman/glsmac/.cursor/debug.log", std::ios::app);
		auto tid = std::this_thread::get_id();
		std::stringstream ss;
		ss << tid;
		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		log << "{\"sessionId\":\"debug-session\",\"runId\":\"run1\",\"hypothesisId\":\"A\",\"location\":\"main.cpp:100\",\"message\":\"main() entry\",\"data\":{\"threadId\":\"" << ss.str() << "\",\"isMainThread\":true},\"timestamp\":" << now << "}\n";
	}
	// #endregion

	config::Config config( argv[ 0 ] );

	config.Init( argc, argv );

	// this changes the whole flow
	const auto legacy_ui = config.HasLaunchFlag( config::Config::LF_LEGACY_UI );

#if defined( DEBUG ) || defined( FASTDEBUG )
	if ( config.HasDebugFlag( config::Config::DF_GDB ) ) {
#ifdef __linux__
		// automatically start under gdb if possible
		if ( !util::System::AreWeUnderGDB() && util::System::IsGDBAvailable() ) {
			util::LogHelper::Println( "Restarting process under GDB..." );

			std::string cmdline = "printf \"r\\nbt\\n\" | gdb --args";
			for ( int c = 0 ; c < argc ; c++ ) {
				cmdline += (std::string)" " + argv[ c ];
			}
			cmdline += " 2>&1 | tee debug.log";

			util::LogHelper::Println( cmdline );
			int status = system( cmdline.c_str() );
			if ( status < 0 ) {
				util::LogHelper::Println( (std::string)"Error: " + strerror( errno ) );
				exit( EXIT_FAILURE );
			}
			else if ( WIFEXITED( status ) ) {
				util::LogHelper::Println( "Process finished, output saved to debug.log" );
				exit( EXIT_SUCCESS );
			}
			else {
				util::LogHelper::Println( "Process finished, output saved to debug.log" );
				exit( EXIT_FAILURE );
			}
		}
#else
		util::LogHelper::Println( "WARNING: gdb check skipped due to unsupported platform" );
#endif
	}
#ifdef DEBUG
	debug::MemoryWatcher memory_watcher( config.HasDebugFlag( config::Config::DF_MEMORYDEBUG ), config.HasDebugFlag( config::Config::DF_QUIET ) );
#endif
#endif

	util::FS::CreateDirectoryIfNotExists( config.GetPrefix() );
#if defined( DEBUG ) || defined( FASTDEBUG )
	util::FS::CreateDirectoryIfNotExists( config.GetDebugPath() );
#endif

	int result = EXIT_FAILURE;

	// logger needs to be outside of scope to be destroyed last

	std::vector< logger::Logger* > loggers = {};

#if defined( DEBUG ) || defined( FASTDEBUG )
	if ( !config.HasDebugFlag( config::Config::DF_QUIET ) ) {
		loggers.push_back( new logger::Stdout() );
	}
	if ( legacy_ui ) {
		loggers.push_back( new logger::Console() );
	}
#endif

#ifdef _WIN32
	error_handler::Win32 error_handler;
#else
	error_handler::Stdout error_handler;
#endif

	auto title = GLSMAC_VERSION_FULL;
#if defined( DEBUG ) || defined( FASTDEBUG )
	title += "-debug";
#elif PORTABLE
	title += "-portable";
#endif

	network::simpletcp::SimpleTCP network;
	ui_legacy::Default ui;
	scheduler::Simple scheduler;

#if defined( DEBUG ) || defined( FASTDEBUG )
	if ( config.HasDebugFlag( config::Config::DF_GSE_ONLY ) ) {

		loader::font::Null font_loader;
		loader::texture::Null texture_loader;
		loader::sound::Null sound_loader;
		input::Null input;
		graphics::Null graphics;
		audio::Null audio;

		if ( config.HasDebugFlag( config::Config::DF_GSE_TESTS ) ) {
			NEWV( task, task::gsetests::GSETests );
			scheduler.AddTask( task );
		}
		else if ( config.HasDebugFlag( config::Config::DF_GSE_PROMPT_JS ) ) {
			NEWV( task, task::gseprompt::GSEPrompt, "js" );
			scheduler.AddTask( task );
		}

		engine::Engine engine(
			&config,
			&error_handler,
			loggers,
			nullptr,
			&font_loader,
			&texture_loader,
			&sound_loader,
			nullptr,
			&scheduler,
			&input,
			&graphics,
			&audio,
			&network,
			&ui,
			nullptr
		);

		result = engine.Run();
	}
	else
#endif
	{
		game::backend::Game game;

		resource::ResourceManager resource_manager( config.GetDataPath() );

		loader::font::FreeType font_loader;
		loader::texture::SDL2 texture_loader;
		loader::sound::SDL2 sound_loader;
		loader::txt::TXTLoaders txt_loaders;

		bool vsync = VSYNC;
		if ( config.HasLaunchFlag( config::Config::LF_BENCHMARK ) ) {
			vsync = false;
		}
		types::Vec2< size_t > window_size;
		if ( config.HasLaunchFlag( config::Config::LF_WINDOW_SIZE ) ) {
			window_size = config.GetWindowSize();
		}
		else {
			window_size = {
				WINDOW_WIDTH,
				WINDOW_HEIGHT
			};
		}
		bool start_fullscreen = START_FULLSCREEN;
		if ( config.HasLaunchFlag( config::Config::LF_WINDOWED ) ) {
			start_fullscreen = false;
		}

		graphics::opengl::OpenGL graphics( title, window_size.x, window_size.y, vsync, start_fullscreen );
		audio::sdl2::SDL2 audio;
		input::sdl2::SDL2 input;
#ifdef __APPLE__
		// On macOS, SDL initialization must happen on the main thread before starting worker threads
		graphics.InitSDLOnMainThread();
		audio.InitSDLOnMainThread();
		input.InitSDLOnMainThread();
#endif

		if ( legacy_ui ) {
#ifdef DEBUG
			NEWV( debug_overlay, debug::DebugOverlay );
			scheduler.AddTask( debug_overlay );
#endif

			// game common stuff
			NEWV( task_common, task::Common );
			scheduler.AddTask( task_common );
		}

		// game entry point
		common::Task* task = nullptr;

		engine::Engine engine(
			&config,
			&error_handler,
			loggers,
			&resource_manager,
			&font_loader,
			&texture_loader,
			&sound_loader,
			&txt_loaders,
			&scheduler,
			&input,
			&graphics,
			&audio,
			&network,
			&ui,
			&game
		);

		if ( legacy_ui ) {
			NEWV( console_task, task::console::Console );
			scheduler.AddTask( console_task );
		}

		if ( !legacy_ui ) {
			NEW( task, task::main::Main );
		}
		else if ( config.HasLaunchFlag( config::Config::LF_QUICKSTART ) ) {
			NEWV( state, game::backend::State, nullptr, nullptr, nullptr ); // TODO: initialize settings randomly
			state->m_settings.global.rules.Initialize();
			state->InitBindings();
			state->Configure();
			const auto& rules = state->m_settings.global.rules;
			game::backend::faction::Faction* faction = nullptr;
			if ( config.HasLaunchFlag( config::Config::LF_QUICKSTART_FACTION ) ) {
				faction = state->GetFM()->Get( config.GetQuickstartFaction() );
				if ( !faction ) {
					std::string errmsg = "Faction \"" + config.GetQuickstartFaction() + "\" does not exist. Available factions:";
					for ( const auto& f : state->GetFM()->GetAll() ) {
						errmsg += " " + f->m_id;
					}
					THROW( errmsg );
				}
			}
			NEWV(
				player, game::backend::Player,
				"Player",
				game::backend::Player::PR_HOST,
				faction,
				rules.GetDefaultDifficultyLevel()
			);
			state->AddPlayer( player );
			state->AddCIDSlot( 0, 0 );
			state->m_slots->Resize( 1 );
			auto& slot = state->m_slots->GetSlot( 0 );
			slot.SetPlayer( player, 0, "" );
			slot.SetLinkedGSID( state->m_settings.local.account.GetGSID() );
			NEW( task, task::game::Game, state, 0, UH() {
				g_engine->ShutDown();
			} );
		}
		else if ( config.HasLaunchFlag( config::Config::LF_SKIPINTRO ) ) {
			NEW( task, task::mainmenu::MainMenu );
		}
		else {
			NEW( task, task::intro::Intro );
		}

		scheduler.AddTask( task );

		result = engine.Run();
	}

	for ( const auto& logger : loggers ) {
		delete logger;
	}

	return result;
}
