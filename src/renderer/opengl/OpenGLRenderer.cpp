#include "engine/Engine.h"

#include "OpenGLRenderer.h"

#include "shader_program/OrthographicOpenGLShaderProgram.h"
#include "shader_program/FontOpenGLShaderProgram.h"
#include "shader_program/SkyboxOpenGLShaderProgram.h"
#include "shader_program/WorldOpenGLShaderProgram.h"

#include "routine/OverlayOpenGLRoutine.h"
#include "routine/FontOpenGLRoutine.h"
#include "routine/SkyboxOpenGLRoutine.h"
#include "routine/WorldOpenGLRoutine.h"

#include "ui/event/MouseMove.h"
#include "ui/event/MouseDown.h"
#include "ui/event/MouseUp.h"
#include "ui/event/MouseClick.h"
#include "ui/event/KeyDown.h"

using namespace ui;

namespace renderer {
namespace opengl {

OpenGLRenderer::OpenGLRenderer( const std::string title, const unsigned short window_width, const unsigned short window_height, const bool vsync, const float fov) {
	m_window = NULL;
	m_gl_context = NULL;

	m_options.title = title;
	m_options.window_width = window_width;
	m_options.window_height = window_height;
	m_options.vsync = vsync;
	m_options.fov = fov;

	m_aspect_ratio = (float) m_options.window_height / m_options.window_width;


/*	NEWV( sp_skybox, shader_program::SkyboxOpenGLShaderProgram );
	m_shader_programs.push_back( sp_skybox );
	NEWV( r_skybox, routine::SkyboxOpenGLRoutine, sp_skybox );
	m_routines.push_back( r_skybox );

	NEWV( sp_world, shader_program::WorldOpenGLShaderProgram );
	m_shader_programs.push_back( sp_world );
	NEWV( r_world, routine::WorldOpenGLRoutine, sp_world );
	m_routines.push_back( r_world );
*/
	NEWV( sp_orthographic, shader_program::OrthographicOpenGLShaderProgram );
	m_shader_programs.push_back( sp_orthographic );
	NEWV( r_overlay, routine::OverlayOpenGLRoutine, sp_orthographic );
	m_routines.push_back( r_overlay );

	NEWV( sp_font, shader_program::FontOpenGLShaderProgram );
	m_shader_programs.push_back( sp_font );
	NEWV( r_font, routine::FontOpenGLRoutine, sp_font );
	m_routines.push_back( r_font );

}

OpenGLRenderer::~OpenGLRenderer() {
	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it ) {
		DELETE( *it );
	}

	for ( auto it = m_shader_programs.begin() ; it != m_shader_programs.end() ; ++it ) {
		DELETE( *it );
	}
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

	// generate 'empty' texture (transparent 1x1)
	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &m_no_texture );

	glBindTexture( GL_TEXTURE_2D, m_no_texture );
	/*glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );*/

	//glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	uint32_t nothing = 0;
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &nothing );
	if ( glGetError() ) {
		throw RendererError( "Error loading texture" );
	};
	
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glGenerateMipmap(GL_TEXTURE_2D);

	//this->mState=MS_ACTIVE;

	/*for (int i=this->mStartRoutinesQueue.size()-1;i>=0;i--) {
		Log("Activating routine \""+this->mStartRoutinesQueue[i]->GetName()+"\" by demand");
		this->mStartRoutinesQueue[i]->Activate();
	}
	this->mStartRoutinesQueue.clear();*/

	glBindTexture( GL_TEXTURE_2D, 0 );
}

void OpenGLRenderer::Stop() {
	Log( "Uninitializing OpenGL" );

	glDeleteTextures(1, &m_no_texture );
	
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
					// I'm having weird texture tiling bugs at non-even window heights. let's round them a bit
					m_options.window_width = event.window.data1 / 2 * 2;
					m_options.window_height = event.window.data2 / 2 * 2;
					Log( "Resizing viewport to " + std::to_string( m_options.window_width ) + "x" + std::to_string( m_options.window_height ) );
					glViewport( 0, 0, m_options.window_width, m_options.window_height );
					m_aspect_ratio = (float) m_options.window_height / m_options.window_width;
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
			case SDL_MOUSEMOTION: {
				NEWV( ui_event, event::MouseMove, event.motion.x, event.motion.y );
				g_engine->GetUI()->SendEvent( ui_event );
				break;
			}
			case SDL_MOUSEBUTTONDOWN: {
				NEWV( ui_event, event::MouseDown, event.motion.x, event.motion.y, event.button.button );
				g_engine->GetUI()->SendEvent( ui_event );
#if DEBUG
				if ( m_active_mousedowns.find( event.button.button ) != m_active_mousedowns.end() ) {
					throw RendererError( "duplicate mousedown (button=" + to_string( event.button.button ) + ")" );
				}
#endif
				m_active_mousedowns[ event.button.button ] = { event.motion.x, event.motion.y };
				break;
			}
			case SDL_MOUSEBUTTONUP: {
				NEWV( ui_event, event::MouseUp, event.motion.x, event.motion.y, event.button.button );
				g_engine->GetUI()->SendEvent( ui_event );
#if DEBUG
				if ( m_active_mousedowns.find( event.button.button ) == m_active_mousedowns.end() ) {
					throw RendererError( "mouseup without mousedown" );
				}
#endif
				auto& mousedown_data = m_active_mousedowns.at( event.button.button );
				if ( mousedown_data.x == event.motion.x && mousedown_data.y == event.motion.y ) {
					// mousedown + mouseup at same pixel = mouseclick
					NEWV( ui_event_2, event::MouseClick, event.motion.x, event.motion.y, event.button.button );
					g_engine->GetUI()->SendEvent( ui_event_2 );
				}
				m_active_mousedowns.erase( event.button.button );
				break;
			}
			case SDL_KEYDOWN: {
				UIEvent::key_code_t code = UIEvent::K_NONE;
				switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_ESCAPE: {
						code = UIEvent::K_ESCAPE;
						break;
					}
				}
				if (code != UIEvent::K_NONE) {
					NEWV( event, event::KeyDown, code );
					g_engine->GetUI()->SendEvent( event );
				}
				else {
					Log("Skipping unknown keydown code: " + to_string(event.key.keysym.scancode));
				}
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
	
	DEBUG_STAT_INC( frames_rendered );
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

void OpenGLRenderer::LoadTexture( const types::Texture* texture ) {
	
	m_textures_map::iterator it = m_textures.find( texture );
	if ( it == m_textures.end() ) {
	
		Log("Loading texture '" + texture->m_name + "'");

		m_textures[texture] = 0;

		glActiveTexture( GL_TEXTURE0 );
		glGenTextures( 1, &m_textures[texture] );

		glBindTexture( GL_TEXTURE_2D, m_textures[texture] );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		if ( glGetError() ) {
			throw RendererError( "Texture uniform error" );
		}
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)texture->m_width, (GLsizei)texture->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr( texture->m_bitmap, 0, texture->m_width * texture->m_height * 4 ) );
		if ( glGetError() ) {
			throw RendererError( "Error loading texture" );
		};

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture( GL_TEXTURE_2D, 0 );
		
	}
}

/* it's better to keep everything loaded
void OpenGLRenderer::UnloadTexture( const types::Texture* texture ) {
	Log("Unloading texture '" + texture->m_name + "'");
	glDeleteTextures(1, &m_textures[texture] );
}
*/

void OpenGLRenderer::EnableTexture( const types::Texture* texture ) {
	if ( texture ) {
		glBindTexture( GL_TEXTURE_2D, m_textures[texture] );
	}
	else {
		glBindTexture( GL_TEXTURE_2D, m_no_texture );
	}
}

void OpenGLRenderer::DisableTexture() {
	glBindTexture( GL_TEXTURE_2D, 0 );
}

} /* namespace opengl */
} /* namespace renderer */
