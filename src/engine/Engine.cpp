#include <ctime>
#include <thread>

#include "Engine.h"
#include "config/Config.h"
#include "common/Thread.h"
#include "error_handler/ErrorHandler.h"
#include "logger/Logger.h"
#include "resource/ResourceManager.h"
#include "loader/font/FontLoader.h"
#include "loader/texture/TextureLoader.h"
#include "loader/sound/SoundLoader.h"
#include "loader/txt/TXTLoaders.h"
#include "scheduler/Scheduler.h"
#include "input/Input.h"
#include "graphics/Graphics.h"
#include "audio/Audio.h"
#include "network/Network.h"
#include "ui_legacy/UI.h"
#include "game/backend/Game.h"

#ifdef DEBUG
#include "util/Timer.h"
#endif

// TODO: move to config
const size_t g_max_fps = 500;

engine::Engine* g_engine = NULL;

namespace engine {

Engine::Engine(
	config::Config* config,
	error_handler::ErrorHandler* error_handler,
	const std::vector< logger::Logger* >& loggers,
	resource::ResourceManager* resource_manager,
	loader::font::FontLoader* font_loader,
	loader::texture::TextureLoader* texture_loader,
	loader::sound::SoundLoader* sound_loader,
	loader::txt::TXTLoaders* txt_loaders,
	scheduler::Scheduler* scheduler,
	input::Input* input,
	graphics::Graphics* graphics,
	audio::Audio* audio,
	network::Network* network,
	ui_legacy::UI* ui,
	game::backend::Game* game
)
	:
	m_config( config )
	, m_error_handler( error_handler )
	, m_loggers( loggers )
	, m_resource_manager( resource_manager )
	, m_font_loader( font_loader )
	, m_texture_loader( texture_loader )
	, m_sound_loader( sound_loader )
	, m_txt_loaders( txt_loaders )
	, m_scheduler( scheduler )
	, m_input( input )
	, m_graphics( graphics )
	, m_audio( audio )
	, m_network( network )
	, m_ui( ui )
	, m_game( game ) {
	ASSERT( g_engine == nullptr, "duplicate engine initialization" );

	g_engine = this;

	NEWV( t_main, common::Thread, "MAIN" );
	if ( m_config->HasLaunchFlag( config::Config::LF_BENCHMARK ) ) {
		t_main->SetIPS( 999999.9f );
	}
	else {
		t_main->SetIPS( g_max_fps );
	}
	t_main->AddModule( m_config );
	t_main->AddModule( m_error_handler );
	t_main->AddModule( m_font_loader );
	t_main->AddModule( m_texture_loader );
	t_main->AddModule( m_sound_loader );
	for ( const auto& logger : m_loggers ) {
		t_main->AddModule( logger );
	}
#if defined( DEBUG ) || defined ( FASTDEBUG )
	if ( !m_config->HasDebugFlag( config::Config::DF_GSE_ONLY ) )
#endif
	{
		if ( !m_config->HasLaunchFlag( config::Config::LF_NEWUI ) ) {
			// with new ui this will happen in script
			m_resource_manager->Init( m_config->GetPossibleSMACPaths(), m_config->GetSMACType() );
		}
		t_main->AddModule( m_resource_manager );
	}
	t_main->AddModule( m_input );
	t_main->AddModule( m_graphics );
	t_main->AddModule( m_audio );
	t_main->AddModule( m_ui );
	t_main->AddModule( m_scheduler );
	m_threads.push_back( t_main );

	NEWV( t_network, common::Thread, "NETWORK" );
	t_network->SetIPS( 100 );
	t_network->AddModule( m_network );
	m_threads.push_back( t_network );

	if ( m_game ) {
		NEWV( t_game, common::Thread, "GAME" );
		t_game->SetIPS( g_max_fps );
		t_game->AddModule( m_game );
		m_threads.push_back( t_game );
	}
};

Engine::~Engine() {
	g_engine = NULL;
	for ( auto& thread : m_threads ) {
		if ( thread->T_IsRunning() ) {
			Log( "WARNING: thread " + thread->GetThreadName() + " still running!" );
		}
		else {
			DELETE( thread );
		}
	}
}

int Engine::Run() {
	int result = EXIT_SUCCESS;

	// TODO: dynamic threadpool

	for ( auto& thread : m_threads ) {
		thread->T_Start();
	}

	try {
		while ( !m_is_shutting_down ) {
			for ( auto& thread : m_threads ) {
				// ?
			}
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
		}
		Log( "Shutting down" );

		for ( auto& thread : m_threads ) {
			thread->T_Stop();
		}
#ifdef DEBUG
		util::Timer thread_running_timer;
		thread_running_timer.SetInterval( 1000 );
#endif
		bool any_thread_running = true;
		while ( any_thread_running ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
			any_thread_running = false;
#ifdef DEBUG
			const bool announce_frozen_threads = thread_running_timer.HasTicked();
#endif
			for ( auto& thread : m_threads ) {
				if ( thread->T_IsRunning() ) {
#ifdef DEBUG
					if ( announce_frozen_threads ) {
						Log( "Thread " + thread->GetThreadName() + " still running" );
					}
#endif
					any_thread_running = true;
#ifdef DEBUG
					if ( !announce_frozen_threads )
#endif
					{
						break;
					}
				}
			}
		}

	}
	catch ( std::runtime_error& e ) {
		result = EXIT_FAILURE;
		m_error_handler->HandleError( e );
	}

	return result;
}

void Engine::ShutDown() {

	m_is_shutting_down = true;
}

void Engine::Log( const std::string& text ) const {
	for ( const auto& logger : m_loggers ) {
		logger->Log( text );
	}
}

}
