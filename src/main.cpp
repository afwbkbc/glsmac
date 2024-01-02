#include <thread>

#ifdef DEBUG

#include <iostream>
#include <string>
#include <stdlib.h>

#endif

#include "config/Config.h"

#ifdef _WIN32
#include "error_handler/Win32.h"
#else

#include "error_handler/Stdout.h"

#endif

#include "logger/Stdout.h"

#ifdef DEBUG

#include "logger/Noop.h"
#include "graphics/Null.h"
#include "loader/font/Null.h"
#include "loader/texture/Null.h"
#include "loader/sound/Null.h"
#include "input/Null.h"
#include "audio/Null.h"

#endif

#include "loader/font/FreeType.h"
#include "loader/texture/SDL2.h"
#include "loader/sound/SDL2.h"
#include "input/sdl2/SDL2.h"
#include "graphics/opengl/OpenGL.h"
#include "audio/sdl2/SDL2.h"
#include "network/simpletcp/SimpleTCP.h"

#include "scheduler/Simple.h"
#include "ui/Default.h"

#include "task/common/Common.h"

#if defined(DEBUG)

#include "task/gseprompt/GSEPrompt.h"
#include "task/gsetests/GSETests.h"
#include "task/game/Game.h"

#endif

#include "task/intro/Intro.h"
#include "task/mainmenu/MainMenu.h"

#include "game/Game.h"

#include "engine/Engine.h"

#include "version.h"

// TODO: move to config
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define VSYNC true
#ifdef DEBUG
#define START_FULLSCREEN false
#else
#define START_FULLSCREEN true
#endif

#ifdef DEBUG

#include "util/System.h"
#include "util/FS.h"
#include "debug/MemoryWatcher.h"
#include "debug/DebugOverlay.h"

using namespace util;
#endif

using namespace std;

#ifdef main
#undef main
#endif

#if !defined(_WIN32) || defined( VISUAL_STUDIO )

int main( const int argc, const char* argv[] ) {
#else
	int main_real(const int argc, const char* argv[]) {
#endif

	config::Config config( argc, argv );

#ifdef DEBUG
	if ( config.HasDebugFlag( config::Config::DF_GDB ) ) {
#ifdef __linux__
		// automatically start under gdb if possible
		if ( !System::AreWeUnderGDB() && System::IsGDBAvailable() ) {
			cout << "Restarting process under GDB..." << endl;

			string cmdline = "printf \"r\\nbt\\n\" | gdb --args";
			for ( int c = 0 ; c < argc ; c++ ) {
				cmdline += (string)" " + argv[ c ];
			}
			cmdline += " 2>&1 | tee debug.log";

			cout << cmdline << endl;
			int status = system( cmdline.c_str() );
			if ( status < 0 ) {
				cout << "Error: " << strerror( errno ) << endl;
				exit( EXIT_FAILURE );
			}
			else if ( WIFEXITED( status ) ) {
				cout << "Process finished, output saved to debug.log" << endl;
				exit( EXIT_SUCCESS );
			}
			else {
				cout << "Process finished, output saved to debug.log" << endl;
				exit( EXIT_FAILURE );
			}
		}
#else
		cout << "WARNING: gdb check skipped due to unsupported platform" << endl;
#endif
	}
	debug::MemoryWatcher memory_watcher( config.HasDebugFlag( config::Config::DF_MEMORYDEBUG ), config.HasDebugFlag( config::Config::DF_QUIET ) );
#endif

	FS::CreateDirectoryIfNotExists( config.GetPrefix() );
#ifdef DEBUG
	FS::CreateDirectoryIfNotExists( config.GetDebugPath() );
#endif

	int result = EXIT_FAILURE;

	// logger needs to be outside of scope to be destroyed last

#ifdef DEBUG
	logger::Logger* logger;
	if ( config.HasDebugFlag( config::Config::DF_QUIET ) ) {
		NEW( logger, logger::Noop );
	}
	else {
		NEW( logger, logger::Stdout );
	}
#else
	NEWV( logger, logger::Noop );
#endif
	{

#ifdef _WIN32
		error_handler::Win32 error_handler;
#else
		error_handler::Stdout error_handler;
#endif

		auto title = GLSMAC_VERSION_FULL;
#ifdef DEBUG
		title += "-debug";
#elif PORTABLE
		title += "-portable";
#endif

		network::simpletcp::SimpleTCP network;
		ui::Default ui;
		scheduler::Simple scheduler;

#ifdef DEBUG
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
			else if ( config.HasDebugFlag( config::Config::DF_GSE_PROMPT_GJS ) ) {
				NEWV( task, task::gseprompt::GSEPrompt, "gjs" );
				scheduler.AddTask( task );
			}

			engine::Engine engine(
				&config,
				&error_handler,
				logger,
				&font_loader,
				&texture_loader,
				&sound_loader,
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
			game::Game game;

			loader::font::FreeType font_loader;

			loader::texture::SDL2 texture_loader;
			texture_loader.SetTransparentColor( types::Color::RGBA( 255, 0, 255, 255 ) );

			loader::sound::SDL2 sound_loader;

			input::sdl2::SDL2 input;
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

#ifdef DEBUG
			NEWV( debug_overlay, debug::DebugOverlay );
			scheduler.AddTask( debug_overlay );
#endif

			// game common stuff
			NEWV( task_common, task::Common );
			scheduler.AddTask( task_common );

			// game entry point
			base::Task* task = nullptr;
#ifdef DEBUG
			if ( config.HasDebugFlag( config::Config::DF_QUICKSTART ) ) {
				NEWV( state, game::State ); // TODO: initialize settings randomly
				NEW( task, task::game::Game, state, 0, UH() {
					g_engine->ShutDown();
				} );
			}
			else
#endif
			if ( config.HasLaunchFlag( config::Config::LF_SKIPINTRO ) ) {
				NEW( task, task::mainmenu::MainMenu );
			}
			else {
				NEW( task, task::intro::Intro );
			}

			scheduler.AddTask( task );

			engine::Engine engine(
				&config,
				&error_handler,
				logger,
				&font_loader,
				&texture_loader,
				&sound_loader,
				&scheduler,
				&input,
				&graphics,
				&audio,
				&network,
				&ui,
				&game
			);

			result = engine.Run();
		}
	}
	DELETE( logger );

	return result;
}

#if defined(_WIN32) && !defined( VISUAL_STUDIO )
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	main_real(__argc, (const char**)__argv);
}
#endif
