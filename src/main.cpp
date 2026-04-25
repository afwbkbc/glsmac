#include <thread>

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

#if defined( DEBUG ) || defined( FASTDEBUG )

#include "logger/Stdout.h"
#include "graphics/Null.h"
#include "loader/font/Null.h"
#include "loader/texture/Null.h"
#include "loader/sound/Null.h"
#include "input/Null.h"
#include "audio/Null.h"

#else

#include "logger/Logger.h"

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

#if defined( DEBUG ) || defined( FASTDEBUG )

#include "task/gseprompt/GSEPrompt.h"
#include "task/gsetests/GSETests.h"

#endif

#include "task/main/Main.h"

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

#endif

#ifdef _WIN32
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hpInstance, LPSTR nCmdLine, int iCmdShow ) {
#endif

#ifdef DEBUG
	debug::MemoryWatcher memory_watcher;
#endif

#ifdef _WIN32
	int argc = 0;
	const LPWSTR* const argw = CommandLineToArgvW( GetCommandLineW(), &argc );
	char* argv[ argc ];
	for ( int i = 0; i < argc; i++ ) {
		const int sz = wcslen( argw[ i ] );
		argv[ i ] = (char*)malloc( sz + 1 );
		wcstombs( argv[ i ], argw[ i ], sz + 1 );
	}
#else
int main( const int argc, const char* argv[] ) {
#endif

	config::Config config( argv[ 0 ] );
	config.Init( argc, argv );

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
	if ( config.HasDebugFlag( config::Config::DF_MEMORYDEBUG ) ) {
		memory_watcher.EnableMemoryDebug();
	}
	if ( config.HasDebugFlag( config::Config::DF_QUIET ) ) {
		memory_watcher.EnableQuiet();
	}
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
			&game
		);

		NEW( task, task::main::Main );

		scheduler.AddTask( task );

		result = engine.Run();
	}

	for ( const auto& logger : loggers ) {
		delete logger;
	}

	return result;
}
