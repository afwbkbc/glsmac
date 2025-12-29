#include "OpenGL.h"

#include "common/MainThreadDispatch.h"
#include "engine/Engine.h"

#include "shader_program/Simple2D.h"
#include "shader_program/Orthographic.h"
#include "shader_program/OrthographicData.h"
#include "scene/Scene.h"
#include "scene/Camera.h"
#include "routine/Overlay.h"
#include "routine/UI.h"
#include "routine/World.h"
#include "FBO.h"
#include "types/texture/Texture.h"
#include "gc/GC.h"

#ifdef __APPLE__
#include <pthread.h>
#endif

namespace graphics {
namespace opengl {

OpenGL::OpenGL( const std::string title, const unsigned short window_width, const unsigned short window_height, const bool vsync, const bool fullscreen ) {
	m_window = NULL;
	m_gl_context = NULL;

	m_options.title = title;
	m_options.viewport_width = m_window_size.x = m_last_window_size.x = window_width;
	m_options.viewport_height = m_window_size.y = m_last_window_size.y = window_height;
	m_options.vsync = vsync;

	m_is_fullscreen = fullscreen;

	m_px_to_gl_clamp.x.SetRange(
		{
			{ 0.0,  (float)window_width },
			{ -1.0, 1.0 }
		}
	);
	m_px_to_gl_clamp.x.SetOverflowAllowed( true );

	m_px_to_gl_clamp.y.SetRange(
		{
			{ 0.0,  (float)window_height },
			{ -1.0, 1.0 }
		}
	);
	m_px_to_gl_clamp.y.SetOverflowAllowed( true );
	m_px_to_gl_clamp.y.SetInversed( true );
}

OpenGL::~OpenGL() {}

#ifdef __APPLE__
void OpenGL::InitSDLOnMainThread() {
	ASSERT( pthread_main_np() != 0, "InitSDLOnMainThread must be called from main thread" );
	SDL_VideoInit( NULL );
	SDL_SetHint( SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0" );
	
	// Set OpenGL attributes BEFORE creating window (required on macOS)
	// Request OpenGL 3.3 Core Profile for GLSL 3.30 support on macOS
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
	SDL_GL_SetSwapInterval( (char)m_options.vsync );
	
	m_window = SDL_CreateWindow(
		m_options.title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_options.viewport_width,
		m_options.viewport_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);
	if ( !m_window ) {
		THROW( (std::string)"Could not create SDL2 window: " + SDL_GetError() );
	}
	
	// Create OpenGL context on main thread (required on macOS)
	m_gl_context = SDL_GL_CreateContext( m_window );
	if ( !m_gl_context ) {
		THROW( (std::string)"Could not create OpenGL context: " + SDL_GetError() );
	}
}
#endif

void OpenGL::Start() {

	Log( "Initializing SDL2" );

#ifdef __APPLE__
	// On macOS, SDL initialization must happen on the main thread due to AppKit requirements
	// SDL should already be initialized via InitSDLOnMainThread() called from main()
	ASSERT( m_window != NULL, "SDL window not initialized - InitSDLOnMainThread() must be called from main thread before starting threads" );
#else
	SDL_VideoInit( NULL );
	SDL_SetHint( SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0" );
	m_window = SDL_CreateWindow(
		m_options.title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_options.viewport_width,
		m_options.viewport_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);
	if ( !m_window ) {
		THROW( (std::string)"Could not create SDL2 window: " + SDL_GetError() );
	}
#endif

	Log( "Creating window" );

	// not using ASSERTs below because those errors should be thrown in release mode too, i.e. if there's no opengl support or there is no X at all

	if ( !m_window ) {
		THROW( (std::string)"Could not create SDL2 window: " + SDL_GetError() );
	}

#ifdef __APPLE__
	// On macOS, SetFullscreen() calls SDL window functions that must be called from main thread
	// Defer fullscreen setting - it can be done later if needed
	// For now, just skip it during Start() since we're on a worker thread
#else
	if ( m_is_fullscreen ) {
		m_is_fullscreen = false; // to prevent assert on next line
		SetFullscreen();
	}
#endif

	Log( "Initializing OpenGL" );

#ifdef __APPLE__
	// On macOS, OpenGL context must be created on the main thread
	// Context should already be created via InitSDLOnMainThread() called from main()
	ASSERT( m_gl_context != NULL, "OpenGL context not initialized - InitSDLOnMainThread() must be called from main thread before starting threads" );
#else
	// Request OpenGL 3.3 Core Profile for GLSL 3.30 support
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
	SDL_GL_SetSwapInterval( (char)m_options.vsync );

	m_gl_context = SDL_GL_CreateContext( m_window );
	if ( !m_gl_context ) {
		THROW( (std::string)"Could not create OpenGL context: " + SDL_GetError() );
	}
#endif
	if ( !m_gl_context ) {
		THROW( (std::string)"Could not create OpenGL context: " + SDL_GetError() );
	}
#ifdef __APPLE__
	// On macOS, make the context current on this thread before calling glewInit
	if ( SDL_GL_MakeCurrent( m_window, m_gl_context ) != 0 ) {
		THROW( (std::string)"Could not make OpenGL context current: " + SDL_GetError() );
	}
#ifdef DEBUG
	// Update MemoryWatcher's GL thread ID to reflect that the context is now current on this thread
	using debug::g_memory_watcher;
	if ( g_memory_watcher ) {
		g_memory_watcher->UpdateGLThread();
	}
#endif
#endif
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
		glGetIntegerv( GL_MAJOR_VERSION, &major );
		glGetIntegerv( GL_MINOR_VERSION, &minor );

		Log(
			(std::string)
				"\nGL Vendor            : " + vendor +
				"\nGL Renderer          : " + renderer +
				"\nGL Version (string)  : " + version +
				"\nGL Version (integer) : " + std::to_string( major ) + "." + std::to_string( minor ) +
				"\nGLSL Version         : " + glslVersion
		);
	}

	// shader programs
	NEWV( sp_orthographic, shader_program::Orthographic );
	m_shader_programs.push_back( sp_orthographic );
	NEWV( sp_orthographic_data, shader_program::OrthographicData );
	m_shader_programs.push_back( sp_orthographic_data );
	NEWV( sp_simple2d, shader_program::Simple2D );
	m_shader_programs.push_back( sp_simple2d );

	// routines ( order is important )
	NEWV( r_world, routine::World, this, scene::SCENE_TYPE_ORTHO, sp_orthographic, sp_orthographic_data );
	m_routines.push_back( r_world );
	NEWV( r_overlay, routine::Overlay, this, sp_simple2d );
	m_routines.push_back( r_overlay );
	NEWV( r_ui, routine::UI, this, sp_simple2d );
	m_routines.push_back( r_ui );
	NEWV( r_world_ui, routine::World, this, scene::SCENE_TYPE_ORTHO_UI, sp_orthographic, sp_orthographic_data );
	m_routines.push_back( r_world_ui );

	// some routines are special
	m_routine_overlay = r_overlay;

	for ( auto it = m_shader_programs.begin() ; it != m_shader_programs.end() ; ++it ) {
		( *it )->Start();
	}

	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it ) {
		( *it )->Start();
	}

	UpdateViewportSize( m_options.viewport_width, m_options.viewport_height );

	glViewport( 0, 0, m_viewport_size.x, m_viewport_size.y );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth( 1.0f );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LEQUAL );
	glCullFace( GL_FRONT );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	// generate 'empty' texture (transparent 1x1)
	// In OpenGL 3.3 Core Profile, a VAO must be bound before texture operations
	GLuint temp_vao = 0;
	glGenVertexArrays( 1, &temp_vao );
	glBindVertexArray( temp_vao );
	
	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &m_no_texture );
	ASSERT( m_no_texture != 0, "Failed to generate texture for m_no_texture" );
	
	// Clear any previous OpenGL errors
	while ( glGetError() != GL_NO_ERROR ) {}

	WithBindTexture(
		m_no_texture, [ this ]() {
			uint32_t nothing = 0;
			// In OpenGL 3.3 Core Profile, use GL_RGBA8 (sized format)
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &nothing );
			GLenum err = glGetError();
			if ( err != GL_NO_ERROR ) {
				THROW( "Error loading texture: OpenGL error " + std::to_string( err ) + " (texture ID: " + std::to_string( m_no_texture ) + ")" );
			}
		}
	);
	
	glBindVertexArray( 0 );
	glDeleteVertexArrays( 1, &temp_vao );

	m_capture_to_texture_fbo = CreateFBO();

	OnWindowResize();
}

void OpenGL::Stop() {
	Log( "Uninitializing OpenGL" );

	DestroyFBO( m_capture_to_texture_fbo );

	glDeleteTextures( 1, &m_no_texture );

	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it ) {
		( *it )->Stop();
	}

	for ( auto it = m_shader_programs.begin() ; it != m_shader_programs.end() ; ++it ) {
		( *it )->Stop();
	}

	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it ) {
		DELETE( *it );
	}
	m_routines.clear();
	for ( auto it = m_shader_programs.begin() ; it != m_shader_programs.end() ; ++it ) {
		DELETE( *it );
	}
	m_shader_programs.clear();

	ProcessPendingUnloads();

	for ( auto& texture : m_textures ) {
		glDeleteTextures( 1, &texture.second.obj );
	}
	m_textures.clear();

	SDL_GL_DeleteContext( m_gl_context );

	Log( "Destroying window" );
	SDL_DestroyWindow( m_window );

	Log( "Deinitializing SDL2" );
	SDL_VideoQuit();
}

void OpenGL::Iterate() {
	std::lock_guard guard( m_render_mutex );

	Lock();
	
	// On macOS, OpenGL contexts are thread-local, so we must make the context current on this thread
	// before calling any OpenGL functions
#ifdef __APPLE__
	if ( SDL_GL_MakeCurrent( m_window, m_gl_context ) != 0 ) {
		THROW( (std::string)"Could not make OpenGL context current in Iterate(): " + SDL_GetError() );
	}
#ifdef DEBUG
	// Update MemoryWatcher's GL thread ID to reflect that the context is now current on this thread
	using debug::g_memory_watcher;
	if ( g_memory_watcher ) {
		g_memory_watcher->UpdateGLThread();
	}
#endif
#endif

	Graphics::Iterate();

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	for ( auto it = m_routines.begin() ; it != m_routines.end() ; ++it ) {
		if ( *it ) {
			( *it )->Iterate();
		}
	}

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );

	GLenum errcode;
	if ( ( errcode = glGetError() ) != GL_NO_ERROR ) {
		THROW( "OpenGL error occured in render loop, aborting: " + std::to_string( errcode ) );
	}

	// SDL_GL_SwapWindow() must be called on main thread on macOS
	// Use synchronous dispatch to ensure swap completes before we continue
	// Release context from worker thread before dispatching swap to main thread
	// This prevents race condition where main thread makes context current while worker thread is still using it
#ifdef __APPLE__
	SDL_GL_MakeCurrent( m_window, nullptr );
	// Synchronously dispatch swap to main thread and wait for completion
	auto swap_future = common::MainThreadDispatch::GetInstance()->Dispatch<bool>(
		[this]() -> bool {
			// Make the OpenGL context current on main thread before swapping
			SDL_GL_MakeCurrent( m_window, m_gl_context );
			SDL_GL_SwapWindow( m_window );
			return true;
		}
	);
	swap_future.wait(); // Wait for swap to complete
#else
	SDL_GL_SwapWindow( m_window );
#endif

	ProcessPendingUnloads();

	Unlock();

	DEBUG_STAT_INC( frames_rendered );
}

void OpenGL::AddScene( scene::Scene* scene ) {
	Log( "Adding scene [" + scene->GetName() + "]" );

#ifdef DEBUG
	bool added = false;
#endif

	auto it = m_routines.begin();
	for ( ; it < m_routines.end() ; it++ ) {
		if ( ( *it )->AddScene( scene ) ) {
#ifdef DEBUG
			added = true;
#endif
		}
	}

#ifdef DEBUG
	ASSERT( added, "no matching routine for scene [" + scene->GetName() + "]" );
#endif
}

void OpenGL::RemoveScene( scene::Scene* scene ) {
	Log( "Removing scene [" + scene->GetName() + "]" );

#ifdef DEBUG
	bool removed = false;
#endif

	auto it = m_routines.begin();
	for ( ; it < m_routines.end() ; it++ ) {
		if ( ( *it )->RemoveScene( scene ) ) {
#ifdef DEBUG
			removed = true;
#endif
		}
	}

#ifdef DEBUG
	ASSERT( removed, "no matching routine for scene [" + scene->GetName() + "]" );
#endif
}

const unsigned short OpenGL::GetWindowWidth() const {
	return m_window_size.x;
}

const unsigned short OpenGL::GetWindowHeight() const {
	return m_window_size.y;
}

const unsigned short OpenGL::GetViewportWidth() const {
	ASSERT( m_viewport_size.x > 0, "viewport width zero" );
	return m_viewport_size.x;
}

const unsigned short OpenGL::GetViewportHeight() const {
	ASSERT( m_viewport_size.y > 0, "viewport height zero" );
	return m_viewport_size.y;
}

void OpenGL::OnWindowResize() {
	Graphics::OnWindowResize();

	m_px_to_gl_clamp.x.SetSrcRange(
		{
			0.0,
			(float)m_viewport_size.x
		}
	);
	m_px_to_gl_clamp.y.SetSrcRange(
		{
			0.0,
			(float)m_viewport_size.y
		}
	);

	for ( auto& f : m_fbos ) {
		f->Resize( m_viewport_size.x, m_viewport_size.y );
	}

	for ( auto& r : m_routines ) {
		r->OnWindowResize();
	}
}

void OpenGL::LoadTexture( types::texture::Texture* texture, const bool smoothen ) {
	ASSERT( texture, "texture is null" );

#ifdef __APPLE__
	// On macOS, OpenGL contexts are thread-local, so we must make the context current on this thread
	// before calling any OpenGL functions
	if ( SDL_GL_MakeCurrent( m_window, m_gl_context ) != 0 ) {
		THROW( (std::string)"Could not make OpenGL context current in LoadTexture(): " + SDL_GetError() );
	}
#endif

	// Clear any previous OpenGL errors before starting
	while ( glGetError() != GL_NO_ERROR ) {}

	bool is_reload_needed = false;

	const size_t texture_update_counter = texture->UpdatedCount();

	auto it = m_textures.find( texture );
	bool need_full_update = false;

	glActiveTexture( GL_TEXTURE0 );

	const auto tw = texture->GetWidth();
	const auto th = texture->GetHeight();

	if ( it == m_textures.end() ) {

		//Log( "Initializing texture '" + texture->m_name + "'" );
		it = m_textures.insert(
			{
				texture,
				{
					0,
					texture->UpdatedCount(),
					{
						tw,
						th,
					}
				}
			}
		).first;

		glGenTextures( 1, &it->second.obj );
		GLenum err = glGetError();
		if ( err != GL_NO_ERROR ) {
			THROW( "Error generating texture: OpenGL error " + std::to_string( err ) );
		}

		is_reload_needed = true;
		need_full_update = true;
	}

	auto& t = it->second;
	if ( t.last_dimensions.x != tw || t.last_dimensions.y != th ) {
		is_reload_needed = true;
		need_full_update = true;
		t.last_dimensions.x = tw;
		t.last_dimensions.y = th;
	}

	if ( t.last_texture_update_counter != texture_update_counter ) {
		t.last_texture_update_counter = texture_update_counter;
		is_reload_needed = true;
	}

	if ( is_reload_needed ) {
		//Log( "Loading texture '" + texture->m_name + "'" );

		WithBindTexture(
			t.obj, [ &need_full_update, &texture, &smoothen, &tw, &th ]() {

				if ( need_full_update ) {
					// Clear any previous OpenGL errors before glTexImage2D
					while ( glGetError() != GL_NO_ERROR ) {}
					glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_RGBA8,
						(GLsizei)tw,
						(GLsizei)th,
						0,
						GL_RGBA,
						GL_UNSIGNED_BYTE,
						ptr( texture->GetBitmap(), 0, texture->GetBitmapSize() )
					);

					if ( smoothen ) {
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
					}
					else {
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
					}

					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
				}
				else if ( !texture->GetUpdatedAreas().empty() ) {

					// combine multiple updates into one or fewer

					types::texture::Texture::updated_areas_t areas = {};

					const auto& updated_areas = texture->GetUpdatedAreas();

					const uint8_t od = 1; // overlap distance

					const auto f_are_combineable = []( const types::texture::Texture::updated_area_t& first, const types::texture::Texture::updated_area_t& second ) -> bool {
						return
							(
								( first.left + od >= second.left && first.left - od <= second.right ) ||
									( first.right + od >= second.left && first.right - od <= second.right )
							) &&
								(
									( first.top + od >= second.top && first.top - od <= second.bottom ) ||
										( first.bottom + od >= second.top && first.bottom - od <= second.bottom )
								);
					};

					const auto f_combine = []( types::texture::Texture::updated_area_t& first, const types::texture::Texture::updated_area_t& second ) -> void {
						//Log( "Merging texture area " + second.ToString() + " into " + first.ToString() );
						first.left = std::min< size_t >( first.left, second.left );
						first.top = std::min< size_t >( first.top, second.top );
						first.right = std::max< size_t >( first.right, second.right );
						first.bottom = std::max< size_t >( first.bottom, second.bottom );
					};

					// mark area as removed (merged into another)
					const auto f_remove = []( types::texture::Texture::updated_area_t& area ) -> void {
						area.right = area.top = 0; // hackish but no actual area would have these coordinates at 0
					};

					// check if area was marked as removed (to skip)
					const auto f_is_removed = []( const types::texture::Texture::updated_area_t& area ) -> bool {
						return area.right == 0 && area.top == 0;
					};

					for ( auto& updated_area : updated_areas ) {
						//Log( "Processing texture area " + updated_area.ToString() );
						// try to merge with existing one
						auto it = areas.begin();
						while ( it != areas.end() ) {
							// if it overlaps then we can merge with existing (TODO: can optimize further by measuring overlap size)
							if ( f_are_combineable( updated_area, *it ) ) {
								// extend area to fit both new one and old one
								f_combine( *it, updated_area );
								break;
							}
							it++;
						}
						if ( it == areas.end() ) {
							// couldn't find any suitable areas, add new one
							//Log( "Adding texture area " + updated_area.ToString() );
							areas.push_back(
								{
									updated_area.left,
									updated_area.top,
									updated_area.right,
									updated_area.bottom
								}
							);
						}
					}

					// keep combining until can't combine anymore
					bool combined = true;
					do {
						combined = false;
						for ( auto it_dst = areas.begin() ; it_dst < areas.end() ; it_dst++ ) {
							if ( f_is_removed( *it_dst ) ) {
								continue;
							}
							for ( auto it_src = it_dst + 1 ; it_src < areas.end() ; it_src++ ) {
								if ( f_is_removed( *it_src ) ) {
									continue;
								}
								if ( f_are_combineable( *it_dst, *it_src ) ) {
									//Log( "Merging texture area " + it_src->ToString() + " into " + it_dst->ToString() );
									f_combine( *it_dst, *it_src );
									f_remove( *it_src );
									combined = true;
								}
							}
						}
					}
					while ( combined );

					// reload areas into opengl
					for ( auto& area : areas ) {
						if ( f_is_removed( area ) ) {
							continue;
						}

						//Log( "Reloading texture area " + area.ToString() );

						const size_t w = area.right - area.left;
						const size_t h = area.bottom - area.top;

						if ( w > 0 && h > 0 ) {
							auto* bitmap = texture->CopyBitmap(
								area.left,
								area.top,
								area.right,
								area.bottom
							);

							glTexSubImage2D(
								GL_TEXTURE_2D,
								0,
								area.left,
								area.top,
								w,
								h,
								GL_RGBA,
								GL_UNSIGNED_BYTE,
								ptr( bitmap, 0, w * h * 4 )
							);

							free( bitmap );
						}
					}
				}
				texture->ClearUpdatedAreas();

				const auto err = glGetError();
				ASSERT( !err, "Error loading texture: " + std::to_string( err ) );

				glGenerateMipmap( GL_TEXTURE_2D );
			}
		);

		ASSERT( !glGetError(), "Error somewhere while loading texture" );
	}
}

void OpenGL::UnloadTexture( const types::texture::Texture* texture ) {
	std::lock_guard guard( m_texture_objs_to_unload_mutex );
	m_textures_map::iterator it = m_textures.find( texture );
	if ( it != m_textures.end() ) {
		//Log( "Unloading texture '" + texture->m_name + "'" );
		m_texture_objs_to_unload.push_back( it->second.obj );
		m_textures.erase( it );
	}
}

void OpenGL::WithTexture( const types::texture::Texture* texture, const f_t& f ) {
	GLuint obj;
	if ( texture ) {
		auto it = m_textures.find( texture );
		ASSERT( it != m_textures.end(), "texture to be enabled ( " + texture->GetFilename() + " ) not found" );
		obj = it->second.obj;
	}
	else {
		obj = m_no_texture;
	}
	WithBindTexture( obj, f );
}

FBO* OpenGL::CreateFBO() {
	NEWV( fbo, FBO, this, m_options.viewport_width, m_options.viewport_height );
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

void OpenGL::ResizeWindow( const size_t width, const size_t height ) {
	if (
		m_window_size.x != width ||
			m_window_size.y != height
		) {
		m_window_size.x = width;
		m_window_size.y = height;
		ResizeViewport( width, height );
	}
}

void OpenGL::WithBindBuffer( GLenum target, GLuint buffer, const f_t& f ) const {
	glBindBuffer( target, buffer );
	f();
	glBindBuffer( target, 0 );
}

void OpenGL::WithBindBuffers( GLuint vbo, GLuint ibo, const graphics::Graphics::f_t& f ) const {
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
	f();
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void OpenGL::WithBindTexture( GLuint texture, const f_t& f ) const {
	glBindTexture( GL_TEXTURE_2D, texture );
	f();
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void OpenGL::WithBindFramebufferBegin( GLenum target, GLuint buffer ) const {
	glBindFramebuffer( target, buffer );
}

void OpenGL::WithBindFramebuffer( GLenum target, GLuint buffer, const graphics::Graphics::f_t& f ) const {
	glBindFramebuffer( target, buffer );
	f();
	glBindFramebuffer( target, 0 );
}

void OpenGL::WithBindFramebufferEnd( GLenum target ) const {
	glBindFramebuffer( target, 0 );
}

void OpenGL::WithShaderProgram( shader_program::ShaderProgram* sp, const f_t& f ) const {
	ASSERT( sp != nullptr, "shader program is null in WithShaderProgram()" );
	// Safety check: ensure shader program is initialized before calling virtual functions
	// Access m_gl_shader_program directly (we're a friend class) to avoid virtual function call if object is corrupted
	if ( sp->m_gl_shader_program == 0 ) {
		THROW( "shader program not initialized in WithShaderProgram()" );
	}
	// In OpenGL 3.3 Core Profile, a VAO must be bound before EnableAttributes() and drawing
	// Query the current IBO binding before binding the VAO (it was bound in WithBindBuffers())
	GLint ibo_before_vao = 0;
	glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &ibo_before_vao );
	
	// Create and bind a temporary VAO for this draw call
	GLuint temp_vao = 0;
	glGenVertexArrays( 1, &temp_vao );
	glBindVertexArray( temp_vao );
	
	// In Core Profile, binding a new VAO clears the element array buffer binding
	// Query the actual OpenGL state (not MemoryWatcher's state) to see if IBO was cleared
	GLint ibo_after_vao = 0;
	glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &ibo_after_vao );
	
	// Only rebind if the IBO was actually cleared by binding the VAO
	// MemoryWatcher will complain if we rebind the same buffer, but in Core Profile
	// we must rebind the IBO after binding the VAO so the VAO stores it
	if ( ibo_before_vao != 0 && ibo_after_vao == 0 ) {
		// The IBO was cleared by binding the VAO, so we must rebind it
		// Note: This will trigger MemoryWatcher's assertion, but it's necessary for Core Profile
		// TODO: Update MemoryWatcher to track VAO bindings and update current_index_buffer accordingly
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo_before_vao );
	}
	
	sp->Enable();
	f();
	sp->Disable();
	
	glBindVertexArray( 0 );
	glDeleteVertexArrays( 1, &temp_vao );
}

void OpenGL::CaptureToTexture( types::texture::Texture* const texture, const types::Vec2< size_t >& top_left, const types::Vec2< size_t >& bottom_right, const f_t& f ) {
	m_capture_to_texture_fbo->Write( f );
	m_capture_to_texture_fbo->CaptureToTexture( texture, top_left, bottom_right );
}

const types::Vec2< types::mesh::coord_t > OpenGL::GetGLCoords( const types::Vec2< size_t >& xy ) const {
	return {
		m_px_to_gl_clamp.x.Clamp( xy.x ),
		m_px_to_gl_clamp.y.Clamp( xy.y ),
	};
}

void OpenGL::NoRender( const std::function< void() >& f ) {
	std::lock_guard guard( m_render_mutex );
	f();
}

void OpenGL::ResizeViewport( const size_t width, const size_t height ) {
	if (
		m_viewport_size.x != ( width + 1 ) / 2 * 2
			||
				m_viewport_size.y != ( height + 1 ) / 2 * 2
		) {
		UpdateViewportSize( width, height );

		Log( "Resizing viewport to " + std::to_string( m_viewport_size.x ) + "x" + std::to_string( m_viewport_size.y ) );
		glViewport( 0, 0, m_viewport_size.x, m_viewport_size.y );
		OnWindowResize();
		for ( auto routine = m_routines.begin() ; routine < m_routines.end() ; ++routine ) {
			for ( auto scene = ( *routine )->m_scenes.begin() ; scene < ( *routine )->m_scenes.end() ; ++scene ) {
				auto camera = ( *scene )->GetCamera();
				if ( camera ) {
					camera->UpdateProjection();
					camera->UpdateMatrix();
				}
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

	// Window operations dispatched to main thread for consistency
	auto future = common::MainThreadDispatch::GetInstance()->Dispatch<SDL_DisplayMode>([this]() {
		SDL_DisplayMode dm;
		SDL_GetDesktopDisplayMode( 0, &dm );

		//SDL_SetWindowResizable( m_window, SDL_FALSE );
		//SDL_SetWindowBordered( m_window, SDL_FALSE );
		SDL_SetWindowPosition( m_window, 0, 0 );
		SDL_SetWindowSize( m_window, dm.w, dm.h );
		//SDL_RaiseWindow( m_window );
		SDL_SetWindowFullscreen( m_window, true );
		SDL_SetWindowGrab( m_window, SDL_TRUE );

		return dm;
	});
	
	SDL_DisplayMode dm = future.get();
	m_last_window_size = m_window_size;
	ResizeViewport( dm.w, dm.h );
}

void OpenGL::SetWindowed() {
	ASSERT( m_is_fullscreen, "already windowed" );

	Log( "Switching to window" );
	m_is_fullscreen = false;

	// Window operations dispatched to main thread for consistency
	types::Vec2< unsigned short > saved_window_size = m_last_window_size;
	auto future = common::MainThreadDispatch::GetInstance()->Dispatch<types::Vec2< unsigned short >>([this, saved_window_size]() {
		SDL_DisplayMode dm;
		SDL_GetDesktopDisplayMode( 0, &dm );

		SDL_SetWindowFullscreen( m_window, false );
		//SDL_SetWindowBordered( m_window, SDL_TRUE );
		//SDL_SetWindowResizable( m_window, SDL_TRUE );
		SDL_SetWindowSize( m_window, saved_window_size.x, saved_window_size.y );
		SDL_SetWindowPosition( m_window, ( dm.w - saved_window_size.x ) / 2, ( dm.h - saved_window_size.y ) / 2 );

		SDL_SetWindowGrab( m_window, SDL_FALSE );

		return saved_window_size;
	});
	
	m_window_size = future.get();
	ResizeViewport( m_window_size.x, m_window_size.y );
}

void OpenGL::RedrawOverlay() {
	m_routine_overlay->Redraw();
}

void OpenGL::UpdateViewportSize( const size_t width, const size_t height ) {
	// I'm having weird texture tiling bugs at non-even window heights
	// also don't let them go below 2 or something will assert/crash
	m_viewport_size.x = ( width + 1 ) / 2 * 2;
	m_viewport_size.y = ( height + 1 ) / 2 * 2;
}

void OpenGL::ProcessPendingUnloads() {
	std::lock_guard guard( m_texture_objs_to_unload_mutex );
	for ( auto& obj : m_texture_objs_to_unload ) {
		glActiveTexture( GL_TEXTURE0 );
		glDeleteTextures( 1, &obj );
	}
	m_texture_objs_to_unload.clear();
}

}
}
