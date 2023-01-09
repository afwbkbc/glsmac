#include "engine/Engine.h"

#include <boost/chrono.hpp>
#include <boost/thread.hpp>

#include "OpenGLRenderer.h"

#include "opengl/shader_program/OrthographicOpenGLShaderProgram.h"
#include "opengl/shader_program/FontOpenGLShaderProgram.h"
#include "opengl/shader_program/SkyboxOpenGLShaderProgram.h"
#include "opengl/shader_program/WorldOpenGLShaderProgram.h"

#include "opengl/routine/OverlayOpenGLRoutine.h"
#include "opengl/routine/FontOpenGLRoutine.h"
#include "opengl/routine/SkyboxOpenGLRoutine.h"
#include "opengl/routine/WorldOpenGLRoutine.h"

namespace renderer {

OpenGLRenderer::OpenGLRenderer( const std::string title, const unsigned short window_width, const unsigned short window_height, const bool vsync, const float fov) {
	m_window = NULL;
	m_gl_context = NULL;

	m_options.title = title;
	m_options.window_width = window_width;
	m_options.window_height = window_height;
	m_options.vsync = vsync;
	m_options.fov = fov;

	m_aspect_ratio = (float) m_options.window_width/m_options.window_height;


	auto sp_skybox = new opengl::shader_program::SkyboxOpenGLShaderProgram;
	m_shader_programs.push_back( sp_skybox );
	auto r_skybox = new opengl::routine::SkyboxOpenGLRoutine( sp_skybox );
	m_routines.push_back( r_skybox );

	auto sp_world = new opengl::shader_program::WorldOpenGLShaderProgram;
	m_shader_programs.push_back( sp_world );
	auto r_world = new opengl::routine::WorldOpenGLRoutine( sp_world );
	m_routines.push_back( r_world );

	auto sp_orthographic = new opengl::shader_program::OrthographicOpenGLShaderProgram;
	m_shader_programs.push_back( sp_orthographic );
	auto r_overlay = new opengl::routine::OverlayOpenGLRoutine( sp_orthographic );
	m_routines.push_back( r_overlay );

	auto sp_font = new opengl::shader_program::FontOpenGLShaderProgram;
	m_shader_programs.push_back( sp_font );
	auto r_font = new opengl::routine::FontOpenGLRoutine( sp_font );
	m_routines.push_back( r_font );

}

OpenGLRenderer::~OpenGLRenderer() {
	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it )
		delete *it;

	for ( auto it = m_shader_programs.begin() ; it != m_shader_programs.end() ; ++it )
		delete *it;
}

void OpenGLRenderer::Start() {

	Log( "Creating window" );

	SDL_Init( SDL_INIT_VIDEO );   // Initialize SDL2

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

	SDL_GL_SetSwapInterval( (char)m_options.vsync );

	// Create an application window with the following settings:
	m_window = SDL_CreateWindow(
		m_options.title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_options.window_width,
		m_options.window_height,
		SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE
	);

	// Check that the window was successfully made
	if ( m_window == NULL ) {
		throw RendererError( "Could not create SDL2 window!" );
	}

	Log( "Initializing OpenGL" );

	m_gl_context = SDL_GL_CreateContext( m_window );
	if ( m_gl_context == NULL )
		throw RendererError( "Could not create OpenGL context!" );

	GLenum res = glewInit();
	if (res != GLEW_OK)
		throw RendererError( "Unable to initialize OpenGL!" );

	for ( auto it = m_shader_programs.begin() ; it != m_shader_programs.end() ; ++it )
		(*it)->Start();

	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it )
		(*it)->Start();

	glViewport( 0, 0, m_options.window_width, m_options.window_height );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth( 1.0f );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LEQUAL );
	glCullFace( GL_FRONT );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	//this->mState=MS_ACTIVE;

	/*for (int i=this->mStartRoutinesQueue.size()-1;i>=0;i--) {
		Log("Activating routine \""+this->mStartRoutinesQueue[i]->GetName()+"\" by demand");
		this->mStartRoutinesQueue[i]->Activate();
	}
	this->mStartRoutinesQueue.clear();*/

}

void OpenGLRenderer::Stop() {
	Log( "Uninitializing OpenGL" );

	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it )
		(*it)->Stop();

	for ( auto it = m_shader_programs.begin() ; it != m_shader_programs.end() ; ++it )
		(*it)->Stop();

	SDL_GL_DeleteContext( m_gl_context );

	Log( "Destroying window" );
	SDL_DestroyWindow( m_window );
	SDL_Quit();
}

void OpenGLRenderer::Iterate() {

	SDL_Event event;

	while ( SDL_PollEvent( &event ) ) {
		switch( event.type ) {
			case SDL_QUIT: {
				g_engine->ShutDown();
				break;
			}
			case SDL_WINDOWEVENT: {
				if ( event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) {
					m_options.window_width = event.window.data1;
					m_options.window_height = event.window.data2;
					Log( "Resizing viewport to " + std::to_string( m_options.window_width ) + "x" + std::to_string( m_options.window_height ) );
					glViewport( 0, 0, m_options.window_width, m_options.window_height );
					m_aspect_ratio = (float) m_options.window_width / m_options.window_height;
					g_engine->GetUI()->Resize();
					for ( auto routine = m_routines.begin() ; routine < m_routines.end() ; ++routine ) {
						for ( auto scene = (*routine)->m_scenes.begin() ; scene < (*routine)->m_scenes.end() ; ++scene ) {
							auto camera = (*scene)->GetCamera();
							if ( camera ) {
								camera->UpdateProjection();
								camera->UpdateMatrix();
							}
						}
					}
					/*for (int i=0;i<this->mWindowDependentShaderPrograms.size();i++) {
						this->mWindowDependentShaderPrograms[i]->OnWindowUpdate(this->mOptions.window_width,this->mOptions.window_height,this->mAspectRatio);
					}*/
				}
				break;
			}
			case SDL_KEYDOWN: {
/*				if (event.key.keysym.scancode==SDL_SCANCODE_ESCAPE) {
					opengl::Routine *logo_routine=this->GetRoutineByName("logo");
					if (logo_routine->IsActive()) {
						logo_routine->Deactivate();
						this->GetRoutineByName("menu")->Activate();
					}
					else
						this->Trigger(new NE::events::Core_Shutdown());
				}
*/
				break;
			}
		}
	}

	glEnable(GL_DEPTH_TEST);
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it )
		(*it)->Iterate();

	glFlush();

	glDisable( GL_BLEND );
	glDisable(GL_DEPTH_TEST);

	SDL_GL_SwapWindow( m_window );

	GLenum errcode;
	if ( ( errcode=glGetError() ) != GL_NO_ERROR ) {
		const GLubyte* errstring = gluErrorString( errcode );
		std::string msg = (char *)errstring;
		throw RendererError( "OpenGL error occured in render loop: \"" + msg + "\"" );
	}

}

void OpenGLRenderer::AddScene( scene::Scene *scene ) {
	Log( "Adding scene [" + scene->GetName() + "]" );

	bool added = false;

	auto it = m_routines.begin();
	for ( ; it < m_routines.end() ; it++ )
		if ( (*it)->AddScene( scene ) )
			added = true;

	if ( !added )
		throw RendererError( "no matching routine for scene [" + scene->GetName() + "]" );
}

void OpenGLRenderer::RemoveScene( scene::Scene *scene ) {
	Log( "Removing scene [" + scene->GetName() + "]" );

	bool removed = false;

	auto it = m_routines.begin();
	for ( ; it < m_routines.end() ; it++ )
		if ( (*it)->RemoveScene( scene ))
			removed = true;

	if ( !removed )
		throw RendererError( "no matching routine for scene [" + scene->GetName() + "]" );
}

void OpenGLRenderer::UpdateCamera() {

}

} /* namespace renderer */
