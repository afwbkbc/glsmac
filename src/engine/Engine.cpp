#include <ctime>

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
	ASSERT( g_engine == nullptr, "duplicate engine initialization" );

	srand( time( nullptr ) );

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
		
	} catch ( runtime_error &e ) {
		result = EXIT_FAILURE;
		this->m_error_handler->HandleError( e );
	}

	return result;
}

void Engine::ShutDown() {
	m_threads.main.SetCommand( base::Thread::COMMAND_STOP );
}

} /* namespace engine */
