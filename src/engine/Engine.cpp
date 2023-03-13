#include <ctime>
#include <thread>

#include "Engine.h"

// TODO: move to config
const size_t g_max_fps = 500;

engine::Engine *g_engine = NULL;

namespace engine {

Engine::Engine(
	config::Config *config,
	error_handler::ErrorHandler *error_handler,
	logger::Logger *logger,
	loader::font::FontLoader *font_loader,
	loader::texture::TextureLoader *texture_loader,
	loader::sound::SoundLoader *sound_loader,
	scheduler::Scheduler *scheduler,
	input::Input *input,
	graphics::Graphics *graphics,
	audio::Audio *audio,
	network::Network *network,
	ui::UI *ui
) :
	m_config( config ),
	m_error_handler( error_handler),
	m_logger( logger ),
	m_font_loader( font_loader ),
	m_texture_loader( texture_loader ),
	m_sound_loader( sound_loader ),
	m_scheduler( scheduler ),
	m_input( input ),
	m_graphics( graphics ),
	m_audio( audio ),
	m_network( network ),
	m_ui( ui )
{
	ASSERT( g_engine == nullptr, "duplicate engine initialization" );

	g_engine = this;

	NEWV( t_main, Thread, "MAIN" );
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
		t_main->AddModule( m_logger );
		t_main->AddModule( m_input );
		t_main->AddModule( m_graphics );
		t_main->AddModule( m_audio );
		t_main->AddModule( m_ui );
		t_main->AddModule( m_scheduler );
	m_threads.push_back( t_main );
	m_main_thread = t_main;

	NEWV( t_network, Thread, "NETWORK" );
		t_network->SetIPS( 100 );
		t_network->AddModule( m_network );
	m_threads.push_back( t_network );
};

Engine::~Engine()
{
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
					break;
				}
			}
		}

	} catch ( std::runtime_error &e ) {
		result = EXIT_FAILURE;
		m_error_handler->HandleError( e );
	}

	return result;
}

void Engine::ShutDown() {
	m_is_shutting_down = true;
}

} /* namespace engine */
