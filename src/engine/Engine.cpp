#include <ctime>

#include "base/Error.h"
#include "task/IntroTask.h"

#include "Engine.h"

engine::Engine *g_engine = NULL;

namespace engine {

Engine::Engine(
	error_handler::ErrorHandler *error_handler,
	logger::Logger *logger,
	loader::font::FontLoader *font_loader,
	loader::texture::TextureLoader *texture_loader,
	scheduler::Scheduler *scheduler,
	renderer::Renderer *renderer,
	ui::UI *ui
) :
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

	m_threads.main.SetIPS( 200 );
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

} /* namespace engine */
