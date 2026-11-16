#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

#include "graphics/opengl/OpenGL.h"
#include "common/Mutex.h"

// Linked against the real game objects. Only the driver entry points used
// before shader initialization are substituted; no display or game assets
// are needed. No replacement OpenGL implementation is compiled here.
namespace {
enum class Failure { VIDEO, WINDOW, CONTEXT, MAKE_CURRENT, GLEW, SHADER };
Failure failure;
bool video = false, window = false, context = false, loader = false;
unsigned video_quits = 0, window_destroys = 0, context_deletes = 0;
unsigned early_gl_calls = 0, shader_calls = 0;
std::array< char, 3 > cleanup_order;
size_t cleanup_count = 0;
int window_token, context_token;

void Require( bool condition, const std::string& message ) {
	if ( !condition ) { throw std::runtime_error( message ); }
}

void CheckGL() {
	if ( !loader || !context ) { ++early_gl_calls; }
}

GLuint FailShaderStart() {
	CheckGL();
	++shader_calls;
	throw std::runtime_error( "injected shader startup failure" );
}

class TestOpenGL : public graphics::opengl::OpenGL {
public:
	explicit TestOpenGL( bool fullscreen ) : OpenGL( "startup test", 640, 480, false, fullscreen ) {}
	bool IsReset() const {
		return !m_window && !m_gl_context && m_shader_programs.empty() && m_routines.empty();
	}
};
}

extern "C" {
int SDL_VideoInit( const char* ) {
	if ( failure == Failure::VIDEO ) { return -1; }
	video = true;
	return 0;
}
void SDL_VideoQuit() {
	Require( video && !window && !context, "video cleanup order or ownership incorrect" );
	video = false;
	++video_quits;
	cleanup_order.at( cleanup_count++ ) = 'V';
}
const char* SDL_GetError() { return "injected driver failure"; }
SDL_bool SDL_SetHint( const char*, const char* ) { return SDL_TRUE; }
int SDL_GL_SetAttribute( SDL_GLattr, int ) { return 0; }
SDL_Window* SDL_CreateWindow( const char*, int, int, int, int, Uint32 ) {
	Require( video, "window created without video" );
	if ( failure == Failure::WINDOW ) { return nullptr; }
	window = true;
	return reinterpret_cast< SDL_Window* >( &window_token );
}
void SDL_DestroyWindow( SDL_Window* value ) {
	Require( window && !context && value == reinterpret_cast< SDL_Window* >( &window_token ), "window cleanup order or ownership incorrect" );
	window = false;
	++window_destroys;
	cleanup_order.at( cleanup_count++ ) = 'W';
}
SDL_GLContext SDL_GL_CreateContext( SDL_Window* ) {
	if ( failure == Failure::CONTEXT ) { return nullptr; }
	context = true;
	return &context_token;
}
void SDL_GL_DeleteContext( SDL_GLContext value ) {
	Require( context && window && value == &context_token, "context cleanup ownership incorrect" );
	context = false;
	loader = false;
	++context_deletes;
	cleanup_order.at( cleanup_count++ ) = 'C';
}
int SDL_GL_MakeCurrent( SDL_Window*, SDL_GLContext ) { return failure == Failure::MAKE_CURRENT ? -1 : 0; }
int SDL_GL_SetSwapInterval( int ) { return 0; }
GLenum glewInit() {
	if ( failure == Failure::GLEW ) { return GLEW_ERROR_NO_GL_VERSION; }
	loader = true;
	__glewCreateProgram = FailShaderStart;
	return GLEW_OK;
}
const GLubyte* glGetString( GLenum ) {
	CheckGL();
	return reinterpret_cast< const GLubyte* >( "test driver" );
}
void glGetIntegerv( GLenum, GLint* value ) { CheckGL(); *value = 4; }
void glViewport( GLint, GLint, GLsizei, GLsizei ) { CheckGL(); }
int SDL_GetDesktopDisplayMode( int, SDL_DisplayMode* mode ) { *mode = {}; mode->w = 640; mode->h = 480; return 0; }
void SDL_SetWindowPosition( SDL_Window*, int, int ) {}
void SDL_SetWindowSize( SDL_Window*, int, int ) {}
int SDL_SetWindowFullscreen( SDL_Window*, Uint32 ) { return 0; }
void SDL_SetWindowGrab( SDL_Window*, SDL_bool ) {}
}

int main() {
	try {
		common::Mutex::SetMutexMode( common::MM_DEFAULT );
		const std::array< std::string, 6 > errors = {
			"Could not initialize SDL2 video", "Could not create SDL2 window", "Could not create OpenGL context",
			"Could not activate OpenGL context", "Unable to initialize OpenGL", "injected shader startup failure"
		};
		unsigned failures = 0;
		for ( const bool fullscreen : { false, true } ) {
			for ( size_t stage = 0; stage < errors.size(); ++stage ) {
				try {
					failure = static_cast< Failure >( stage );
					video = window = context = loader = false;
					video_quits = window_destroys = context_deletes = early_gl_calls = shader_calls = cleanup_count = 0;
					cleanup_order = {};
					TestOpenGL graphics( fullscreen );
					bool caught = false;
					try { graphics.Start(); }
					catch ( const std::runtime_error& error ) {
						caught = true;
						Require( std::string( error.what() ).find( errors.at( stage ) ) != std::string::npos, "original startup error was replaced" );
					}
					Require( caught, "startup fault was not exercised" );
					Require( !video && !window && !context, "failed graphics startup retained driver resources at stage " + std::to_string( stage ) );
					Require( graphics.IsReset(), "failed graphics startup retained member handles or CPU children" );
					Require( !early_gl_calls, "graphics called OpenGL before the loader was ready" );
					Require( video_quits == ( stage > 0 ) && window_destroys == ( stage > 1 ) && context_deletes == ( stage > 2 ), "startup cleanup count mismatch" );
					Require( shader_calls == ( failure == Failure::SHADER ), "shader failure was not reached exactly once" );
					const std::string order( cleanup_order.data(), cleanup_count );
					Require( order == ( stage == 0 ? "" : stage == 1 ? "V" : stage == 2 ? "WV" : "CWV" ), "startup cleanup did not reverse acquisition order" );
				}
				catch ( const std::exception& error ) {
					++failures;
					std::cerr << "OpenGL startup regression (fullscreen=" << fullscreen << ", stage=" << stage << "): " << error.what() << "\n";
				}
			}
		}
		if ( failures ) { return 1; }
		std::cout << "12 OpenGL startup failure cases passed\n";
		return 0;
	}
	catch ( const std::exception& error ) {
		std::cerr << "OpenGL startup regression: " << error.what() << "\n";
		return 1;
	}
}
