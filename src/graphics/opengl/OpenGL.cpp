#include "OpenGL.h"

#include "engine/Engine.h"

#include "shader_program/Simple2D.h"
#include "shader_program/Orthographic.h"
#include "shader_program/Font.h"
#include "shader_program/Skybox.h"
#include "shader_program/World.h"

#include "routine/Overlay.h"
#include "routine/Font.h"
#include "routine/Skybox.h"
#include "routine/World.h"

namespace graphics {
namespace opengl {

OpenGL::OpenGL( const std::string title, const unsigned short viewport_width, const unsigned short viewport_height, const bool vsync, const bool fullscreen ) {
	m_window = NULL;
	m_gl_context = NULL;

	m_options.title = title;
	m_options.viewport_width = m_window_size.width = viewport_width;
	m_options.viewport_height = m_window_size.height = viewport_height;
	m_options.vsync = vsync;

	m_aspect_ratio = (float) m_options.viewport_width / m_options.viewport_height;

	m_is_fullscreen = fullscreen;

/*	NEWV( sp_skybox, shader_program::Skybox );
	m_shader_programs.push_back( sp_skybox );
	NEWV( r_skybox, routine::Skybox, sp_skybox );
	m_routines.push_back( r_skybox );

	NEWV( sp_world, shader_program::World );
	m_shader_programs.push_back( sp_world );
	NEWV( r_world, routine::World, sp_world );
	m_routines.push_back( r_world );
*/
	
	// shader programs
	NEWV( sp_orthographic, shader_program::Orthographic ); m_shader_programs.push_back( sp_orthographic );
	NEWV( sp_orthographic_data, shader_program::OrthographicData ); m_shader_programs.push_back( sp_orthographic_data );
	NEWV( sp_simple2d, shader_program::Simple2D ); m_shader_programs.push_back( sp_simple2d );
	NEWV( sp_font, shader_program::Font ); m_shader_programs.push_back( sp_font );

	// routines ( order is important )
	NEWV( r_world, routine::World, this, scene::SCENE_TYPE_ORTHO, sp_orthographic, sp_orthographic_data ); m_routines.push_back( r_world );
	NEWV( r_overlay, routine::Overlay, this, sp_simple2d ); m_routines.push_back( r_overlay );
	NEWV( r_world_ui, routine::World, this, scene::SCENE_TYPE_ORTHO_UI, sp_orthographic, sp_orthographic_data ); m_routines.push_back( r_world_ui );
	NEWV( r_font, routine::Font, this, sp_font ); m_routines.push_back( r_font );
	
	// some routines are special
	m_routine_overlay = r_overlay;
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
	
	SDL_SetHint( SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0" );
	
	m_window = SDL_CreateWindow(
		m_options.title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_options.viewport_width,
		m_options.viewport_height,
		SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE
	);
	
	// not using ASSERTs below because those errors should be thrown in release mode too, i.e. if there's no opengl support or there is no X at all
	
	if ( !m_window ) {
		THROW( (std::string) "Could not create SDL2 window: " + SDL_GetError() );
	}
	
	if ( m_is_fullscreen ) {
		m_is_fullscreen = false; // to prevent assert on next line
		SetFullscreen();
	}
	
	Log( "Initializing OpenGL" );
	
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	SDL_GL_SetSwapInterval( (char)m_options.vsync );

	m_gl_context = SDL_GL_CreateContext( m_window );
	if ( !m_gl_context ) {
		THROW( (std::string) "Could not create OpenGL context: " + SDL_GetError() );
	}
	GLenum res = glewInit();
	if ( res != GLEW_OK ) {
		THROW( "Unable to initialize OpenGL!" );
	}

	{ // print some OpenGL info
		auto* renderer = (const char*)glGetString( GL_RENDERER );
		auto* vendor = (const char*)glGetString( GL_VENDOR );
		auto* version = (const char*)glGetString( GL_VERSION );
		auto* glslVersion = (const char*)glGetString( GL_SHADING_LANGUAGE_VERSION );

		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		Log( (std::string) 
			"\nGL Vendor            : " + vendor +
			"\nGL Renderer          : " + renderer +
			"\nGL Version (string)  : " + version +
			"\nGL Version (integer) : " + std::to_string( major ) + "." + std::to_string( minor ) +
			"\nGLSL Version         : " + glslVersion
		);
	 }

	for ( auto it = m_shader_programs.begin() ; it != m_shader_programs.end() ; ++it )
		(*it)->Start();

	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it )
		(*it)->Start();

	glViewport( 0, 0, m_options.viewport_width, m_options.viewport_height );
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

	uint32_t nothing = 0;
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &nothing );
	ASSERT( !glGetError(), "Error loading texture" );
	
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	OnWindowResize();
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
	Graphics::Iterate();
	
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it ) {
		(*it)->Iterate();
	}

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );

	SDL_GL_SwapWindow( m_window );

	GLenum errcode;
	if ( ( errcode = glGetError() ) != GL_NO_ERROR ) {
		THROW( "OpenGL error occured in render loop, aborting" );
	}
	
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

const unsigned short OpenGL::GetViewportWidth() const {
	return m_options.viewport_width;
}

const unsigned short OpenGL::GetViewportHeight() const {
	return m_options.viewport_height;
}

void OpenGL::OnWindowResize() {
	Graphics::OnWindowResize();
	
	for ( auto& f : m_fbos ) {
		f->Resize( m_options.viewport_width, m_options.viewport_height );
	}
	
	for ( auto& r : m_routines ) {
		r->OnWindowResize();
	}
}

void OpenGL::LoadTexture( const types::Texture* texture ) {
	
	m_textures_map::iterator it = m_textures.find( texture );
	if ( it == m_textures.end() ) {
	
		//Log("Loading texture '" + texture->m_name + "'");

		m_textures[texture] = 0;

		glActiveTexture( GL_TEXTURE0 );
		glGenTextures( 1, &m_textures[texture] );
		
		glBindTexture( GL_TEXTURE_2D, m_textures[texture] );

		ASSERT( !glGetError(), "Texture parameter error" );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)texture->m_width, (GLsizei)texture->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr( texture->m_bitmap, 0, texture->m_width * texture->m_height * 4 ) );
		ASSERT( !glGetError(), "Error loading texture" );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture( GL_TEXTURE_2D, 0 );
		
		ASSERT( !glGetError(), "Error somewhere while loading texture" );
	}
}

void OpenGL::UnloadTexture( const types::Texture* texture ) {
	m_textures_map::iterator it = m_textures.find( texture );
	if ( it != m_textures.end() ) {
		//Log("Unloading texture '" + texture->m_name + "'");
		glActiveTexture( GL_TEXTURE0 );
		glDeleteTextures(1, &it->second );
		m_textures.erase( it );
	}
}

void OpenGL::EnableTexture( const types::Texture* texture ) {
	if ( texture ) {
		auto it = m_textures.find( texture );
		ASSERT( it != m_textures.end(), "texture to be enabled ( " + texture->m_name + " ) not found" );
		glBindTexture( GL_TEXTURE_2D, it->second );
	}
	else {
		glBindTexture( GL_TEXTURE_2D, m_no_texture );
	}
}

void OpenGL::DisableTexture() {
	glBindTexture( GL_TEXTURE_2D, 0 );
}

FBO* OpenGL::CreateFBO() {
	NEWV( fbo, FBO, m_options.viewport_width, m_options.viewport_height );
	Log( "Created FBO " + fbo->GetName() );
	m_fbos.insert( fbo );
	return fbo;
}

void OpenGL::DestroyFBO( FBO* fbo ) {
	auto it = m_fbos.find( fbo );
	ASSERT( it != m_fbos.end(), "fbo not found" );
	m_fbos.erase( it );
	Log( "Destroyed FBO " + fbo->GetName() );
	DELETE( fbo );
}

void OpenGL::ResizeViewport( const size_t width, const size_t height ) {
	
	// I'm having weird texture tiling bugs at non-even window heights
	// also don't let them go below 2 or something will assert/crash
	m_options.viewport_width = ( width + 1 ) / 2 * 2;
	m_options.viewport_height = ( height + 1 ) / 2 * 2;
	
	if ( !m_is_fullscreen ) {
		m_window_size.width = m_options.viewport_width;
		m_window_size.height = m_options.viewport_height;
	}
	
	Log( "Resizing viewport to " + std::to_string( m_options.viewport_width ) + "x" + std::to_string( m_options.viewport_height ) );
	glViewport( 0, 0, m_options.viewport_width, m_options.viewport_height );
	m_aspect_ratio = (float) m_options.viewport_height / m_options.viewport_width;
	OnWindowResize();
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

const bool OpenGL::IsFullscreen() const {
	return m_is_fullscreen;
}

const bool OpenGL::IsMouseLocked() const {
	return m_is_fullscreen; // always locked in fullscreen, unlocked otherwise
}

void OpenGL::SetFullscreen() {
	ASSERT( !m_is_fullscreen, "already fullscreen" );
	
	Log( "Switching to fullscreen" );
	m_is_fullscreen = true;
	
	SDL_DisplayMode dm;
	SDL_GetDesktopDisplayMode( 0, &dm );
	
	//SDL_SetWindowResizable( m_window, SDL_FALSE );
	//SDL_SetWindowBordered( m_window, SDL_FALSE );
	SDL_SetWindowPosition( m_window, 0, 0 );
	SDL_SetWindowSize( m_window, dm.w, dm.h );
	//SDL_RaiseWindow( m_window );
	SDL_SetWindowFullscreen( m_window, true );
	SDL_SetWindowGrab( m_window, SDL_TRUE );
			
	ResizeViewport( dm.w, dm.h );
}

void OpenGL::SetWindowed() {
	ASSERT( m_is_fullscreen, "already windowed" );
	
	Log( "Switching to window" );
	m_is_fullscreen = false;
	
	SDL_DisplayMode dm;
	SDL_GetDesktopDisplayMode( 0, &dm );
	
	SDL_SetWindowFullscreen( m_window, false );
	//SDL_SetWindowBordered( m_window, SDL_TRUE );
	//SDL_SetWindowResizable( m_window, SDL_TRUE );
	SDL_SetWindowSize( m_window, m_window_size.width, m_window_size.height );
	SDL_SetWindowPosition( m_window, ( dm.w - m_window_size.width ) / 2, ( dm.h - m_window_size.height ) / 2 );
	
	SDL_SetWindowGrab( m_window, SDL_FALSE );
	
	ResizeViewport( m_window_size.width, m_window_size.height );
}

void OpenGL::RedrawOverlay() {
	m_routine_overlay->Redraw();
}

} /* namespace opengl */
} /* namespace graphics */
