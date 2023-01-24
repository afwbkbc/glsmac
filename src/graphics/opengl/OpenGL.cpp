#include "shader_program/ShaderProgram.h"

#include "engine/Engine.h"

#include "shader_program/Orthographic.h"
#include "shader_program/Font.h"
#include "shader_program/Skybox.h"
#include "shader_program/World.h"

#include "routine/Overlay.h"
#include "routine/Font.h"
#include "routine/Skybox.h"
#include "routine/World.h"

using namespace std;
using namespace ui;
namespace ui {
using namespace event;
}

namespace graphics {
namespace opengl {

OpenGL::OpenGL( const string title, const unsigned short window_width, const unsigned short window_height, const bool vsync, const float fov) {
	m_window = NULL;
	m_gl_context = NULL;

	m_options.title = title;
	m_options.window_width = window_width;
	m_options.window_height = window_height;
	m_options.vsync = vsync;
	m_options.fov = fov;

	m_aspect_ratio = (float) m_options.window_height / m_options.window_width;


/*	NEWV( sp_skybox, shader_program::Skybox );
	m_shader_programs.push_back( sp_skybox );
	NEWV( r_skybox, routine::Skybox, sp_skybox );
	m_routines.push_back( r_skybox );

	NEWV( sp_world, shader_program::World );
	m_shader_programs.push_back( sp_world );
	NEWV( r_world, routine::World, sp_world );
	m_routines.push_back( r_world );
*/
	NEWV( sp_orthographic, shader_program::Orthographic );
	m_shader_programs.push_back( sp_orthographic );
	NEWV( r_overlay, routine::Overlay, sp_orthographic );
	m_routines.push_back( r_overlay );

	NEWV( sp_font, shader_program::Font );
	m_shader_programs.push_back( sp_font );
	NEWV( r_font, routine::Font, sp_font );
	m_routines.push_back( r_font );
}

OpenGL::~OpenGL() {
	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it ) {
		DELETE( *it );
	}
	for ( auto it = m_shader_programs.begin() ; it != m_shader_programs.end() ; ++it ) {
		DELETE( *it );
	}
}

void OpenGL::Start() {

	Log( "Initializing SDL2" );
	SDL_VideoInit( NULL );

	Log( "Creating window" );

	m_window = SDL_CreateWindow(
		m_options.title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_options.window_width,
		m_options.window_height,
		SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE
	);
	
	// not using ASSERTs below because those errors should be thrown in release mode too, i.e. if there's no opengl support or there is no X at all
	
	if ( !m_window ) {
		THROW( "Could not create SDL2 window!" );
	}
	Log( "Initializing OpenGL" );
	
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	SDL_GL_SetSwapInterval( (char)m_options.vsync );

	m_gl_context = SDL_GL_CreateContext( m_window );
	if ( !m_gl_context ) {
		THROW( "Could not create OpenGL context!" );
	}
	GLenum res = glewInit();
	if ( res != GLEW_OK ) {
		THROW( "Unable to initialize OpenGL!" );
	}

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
	ASSERT( !glGetError(), "Error loading texture" );
	
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

void OpenGL::Stop() {
	Log( "Uninitializing OpenGL" );

	glDeleteTextures(1, &m_no_texture );
	
	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it )
		(*it)->Stop();

	for ( auto it = m_shader_programs.begin() ; it != m_shader_programs.end() ; ++it )
		(*it)->Stop();

	for ( auto& texture : m_textures ) {
		glDeleteTextures(1, &texture.second );
	}
	m_textures.clear();
	
	SDL_GL_DeleteContext( m_gl_context );

	Log( "Destroying window" );
	SDL_DestroyWindow( m_window );
	
	Log( "Deinitializing SDL2" );
	SDL_VideoQuit();
}

void OpenGL::Iterate() {
	
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

#if DEBUG
	GLenum errcode;
	if ( ( errcode=glGetError() ) != GL_NO_ERROR ) {
		const GLubyte* errstring = gluErrorString( errcode );
		string msg = (char *)errstring;
		THROW( "OpenGL error occured in render loop: \"" + msg + "\"" );
	}
#endif
	
	DEBUG_STAT_INC( frames_rendered );
}

void OpenGL::AddScene( scene::Scene *scene ) {
	Log( "Adding scene [" + scene->GetName() + "]" );

	bool added = false;

	auto it = m_routines.begin();
	for ( ; it < m_routines.end() ; it++ ) {
		if ( (*it)->AddScene( scene ) ) {
			added = true;
		}
	}

	ASSERT( added, "no matching routine for scene [" + scene->GetName() + "]" );
}

void OpenGL::RemoveScene( scene::Scene *scene ) {
	Log( "Removing scene [" + scene->GetName() + "]" );

	bool removed = false;

	auto it = m_routines.begin();
	for ( ; it < m_routines.end() ; it++ ) {
		if ( (*it)->RemoveScene( scene )) {
			removed = true;
		}
	}

	ASSERT( removed, "no matching routine for scene [" + scene->GetName() + "]" );
}

void OpenGL::UpdateCamera() {

}

void OpenGL::LoadTexture( const types::Texture* texture ) {
	
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

		ASSERT( !glGetError(), "Texture parameter error" );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)texture->m_width, (GLsizei)texture->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr( texture->m_bitmap, 0, texture->m_width * texture->m_height * 4 ) );
		ASSERT( !glGetError(), "Error loading texture" );

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture( GL_TEXTURE_2D, 0 );
		
	}
}

/* it's better to keep everything loaded
void OpenGL::UnloadTexture( const types::Texture* texture ) {
	Log("Unloading texture '" + texture->m_name + "'");
	glDeleteTextures(1, &m_textures[texture] );
}
*/

void OpenGL::EnableTexture( const types::Texture* texture ) {
	if ( texture ) {
		glBindTexture( GL_TEXTURE_2D, m_textures[texture] );
	}
	else {
		glBindTexture( GL_TEXTURE_2D, m_no_texture );
	}
}

void OpenGL::DisableTexture() {
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void OpenGL::ResizeWindow( const size_t width, const size_t height ) {
	// I'm having weird texture tiling bugs at non-even window heights
	m_options.window_width = width / 2 * 2;
	m_options.window_height = height / 2 * 2;
	Log( "Resizing viewport to " + to_string( m_options.window_width ) + "x" + to_string( m_options.window_height ) );
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
}

} /* namespace opengl */
} /* namespace graphics */
