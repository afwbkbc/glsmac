#include <ctime>

#include "base/Error.h"

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
	scheduler::Scheduler *scheduler,
	renderer::Renderer *renderer,
	ui::UI *ui
) :
	m_config( config ),
	m_error_handler( error_handler),
	m_logger( logger ),
	m_font_loader( font_loader ),
	m_texture_loader( texture_loader ),
	m_scheduler( scheduler ),
	m_renderer( renderer ),
	m_ui( ui )
{
	if ( g_engine != NULL )
		throw EngineError("duplicate initialization");

	srand( time( NULL ) );

	g_engine = this;

	m_threads.main.SetIPS( g_max_fps );
	m_threads.main.AddModule( m_config );
	m_threads.main.AddModule( m_error_handler );
	m_threads.main.AddModule( m_font_loader );
	m_threads.main.AddModule( m_logger );
	m_threads.main.AddModule( m_renderer );
	m_threads.main.AddModule( m_ui );

	m_threads.main.AddModule( m_scheduler );
};

Engine::~Engine()
{
	g_engine = NULL;
}

int Engine::Run() {
	int result = EXIT_SUCCESS;

	try {

		m_threads.main.Run();
		
	} catch ( base::Error &e ) {
		result = EXIT_FAILURE;
		this->m_error_handler->HandleError( e );
	}

	return result;
}

void Engine::ShutDown() {
	m_threads.main.SetCommand( base::Thread::COMMAND_STOP );
}

#if DEBUG
bool Engine::IsDebugMode() {
	return m_is_debug_mode;
}

void Engine::EnableDebugMode() {
	if ( !m_is_debug_mode ) {
		m_is_debug_mode = true;
		GetUI()->ShowDebugOverlay();
	}
}

void Engine::DisableDebugMode() {
	if ( m_is_debug_mode ) {
		m_is_debug_mode = false;
		GetUI()->HideDebugOverlay();
	}
}
#endif

} /* namespace engine */
