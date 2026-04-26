#include "SDL2.h"

#include "engine/Engine.h"
#include "graphics/Graphics.h"

#if ( !SDL_VERSION_ATLEAST( 2, 0, 18 ) )
#define KMOD_SCROLL 0x8000 // workaround for ancient systems
#endif

namespace input {
namespace sdl2 {

SDL2::SDL2() {

}

SDL2::~SDL2() {

}

int filter( void* userdata, SDL_Event* event ) {
	switch ( event->type ) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEWHEEL:
		case SDL_QUIT:
		case SDL_WINDOWEVENT:
			return 1;
		default:
			return 0;
	}
}

void SDL2::Start() {
	Log( "Initializing SDL2" );
	SDL_Init( SDL_INIT_EVENTS );
	SDL_SetEventFilter( (SDL_EventFilter)&filter, nullptr );
}

void SDL2::Stop() {
	Log( "Deinitializing SDL2" );
	SDL_Quit();
}

void SDL2::Iterate() {
	SDL_Event event;

	input::Event e = {};
	input::Event e_mouse_move = {}; // send only once per iteration

	while ( SDL_PollEvent( &event ) ) {
		e.SetType( EV_NONE );
		switch ( event.type ) {
			case SDL_QUIT: {
				g_engine->ShutDown();
				break;
			}
			case SDL_WINDOWEVENT: {
				if ( event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) {
					g_engine->GetGraphics()->ResizeWindow( event.window.data1, event.window.data2 );
				}
				break;
			}
			case SDL_MOUSEMOTION: {
				m_last_mouse_position = {
					event.motion.x,
					event.motion.y
				};
				e_mouse_move.SetType( EV_MOUSE_MOVE );
				e_mouse_move.data.mouse.x = event.motion.x;
				e_mouse_move.data.mouse.y = event.motion.y;
				break;
			}
			case SDL_MOUSEBUTTONDOWN: {
				m_last_mouse_position = { // just in case, maybe some devices like touchscreens don't produce mouse move event before click
					event.motion.x,
					event.motion.y
				};
				const auto button = GetMouseButton( event.button.button );

				ASSERT( m_active_mousedowns.find( event.button.button ) == m_active_mousedowns.end(),
					"duplicate mousedown (button=" + std::to_string( event.button.button ) + ")"
				);
				m_active_mousedowns[ event.button.button ] = {
					event.motion.x,
					event.motion.y
				};
				e.SetType( EV_MOUSE_DOWN );
				e.data.mouse.x = event.motion.x;
				e.data.mouse.y = event.motion.y;
				e.data.mouse.button = button;
				break;
			}
			case SDL_MOUSEBUTTONUP: {
				const auto button = GetMouseButton( event.button.button );
				auto it = m_active_mousedowns.find( event.button.button );
				if ( it != m_active_mousedowns.end() ) { // sometimes touchscreen sends mouseup without or before mousedown, just ignore it
					auto& mousedown_data = m_active_mousedowns.at( event.button.button );
					m_active_mousedowns.erase( event.button.button );
				}
				e.SetType( EV_MOUSE_UP );
				e.data.mouse.x = event.motion.x;
				e.data.mouse.y = event.motion.y;
				e.data.mouse.button = button;
				break;
			}
			case SDL_MOUSEWHEEL: {
				e.SetType( EV_MOUSE_SCROLL );
				e.data.mouse.x = m_last_mouse_position.x;
				e.data.mouse.y = m_last_mouse_position.y;
				e.data.mouse.scroll_y = event.wheel.y;
				break;
			}
			case SDL_KEYDOWN: {
				auto modifiers = SDL_GetModState();
				auto scancode = GetScanCode( event.key.keysym.scancode, modifiers );

				char keycode = GetKeyCode( event.key.keysym.sym, modifiers );
				if ( scancode || keycode ) {
					e.SetType( EV_KEY_DOWN );
					e.data.key.key = keycode;
					e.data.key.code = scancode;
					e.data.key.is_printable = keycode > 0;
					e.data.key.modifiers = GetModifiers( modifiers );
				}
				break;
			}
			case SDL_KEYUP: {
				auto modifiers = SDL_GetModState();
				auto scancode = GetScanCode( event.key.keysym.scancode, modifiers );
				char keycode = GetKeyCode( event.key.keysym.sym, modifiers );
				if ( scancode || keycode ) {
					e.SetType( EV_KEY_UP );
					e.data.key.key = keycode;
					e.data.key.code = scancode;
					e.data.key.is_printable = keycode > 0;
					e.data.key.modifiers = GetModifiers( modifiers );
				}
				break;
			}
				// TODO: keypress?
		}

		if ( e.type != EV_NONE ) {
			ProcessEvent( e );
		}
	}
	if ( e_mouse_move.type != EV_NONE ) {
		ProcessEvent( e_mouse_move );
	}
}

const std::string SDL2::GetClipboardText() const {
	std::string result = "";
	auto* t = SDL_GetClipboardText();
	if ( t ) {
		result = t;
		SDL_free( t );
	}
	return result;
}

mouse_button_t SDL2::GetMouseButton( uint8_t sdl_mouse_button ) const {
	switch ( sdl_mouse_button ) {
		case SDL_BUTTON_LEFT: {
			return MB_LEFT;
		}
		case SDL_BUTTON_MIDDLE: {
			return MB_MIDDLE;
		}
		case SDL_BUTTON_RIGHT: {
			return MB_RIGHT;
		}
		default: {
			Log( "unsupported mouse button " + std::to_string( sdl_mouse_button ) );
			return MB_NONE;
		}
	}
}

// TODO: find something more cross-platform and portable
char SDL2::GetKeyCode( SDL_Keycode code, SDL_Keymod modifiers ) const {
	char result = 0;
	if ( ( modifiers & ( ~KMOD_LSHIFT ) & ( ~KMOD_RSHIFT ) & ( ~KMOD_NUM ) & ( ~KMOD_CAPS ) & ( ~KMOD_SCROLL ) ) == 0 ) { // only shift or num/caps/scroll locks
		bool is_shift = ( modifiers & KMOD_LSHIFT ) || ( modifiers & KMOD_RSHIFT );
		bool is_caps = ( modifiers & KMOD_CAPS );
		if ( code >= 'a' && code <= 'z' ) { // letters
			result = code;
			if ( is_shift ^ is_caps ) {
				result -= 32;
			}
		}
		else if ( !is_shift && (
			( code >= '0' && code <= '9' ) || // numbers
				code == ' ' ||
				code == '.' ||
				code == '-' ||
				code == '=' ||
				code == ':' ||
				code == ';' ||
				code == '/' ||
				code == '\\' ||
				code == '\''
		) ) {
			result = code;
		}
		else if ( is_shift ) {
			switch ( code ) {
				case ';':
					result = ':';
					break;
				case '/':
					result = '?';
					break;
				case '1':
					result = '!';
					break;
				case '2':
					result = '@';
					break;
				case '3':
					result = '#';
					break;
				case '4':
					result = '$';
					break;
				case '5':
					result = '%';
					break;
				case '6':
					result = '^';
					break;
				case '7':
					result = '&';
					break;
				case '8':
					result = '*';
					break;
				case '9':
					result = '(';
					break;
				case '0':
					result = ')';
					break;
				case '-':
					result = '_';
					break;
				case '=':
					result = '+';
					break;
				case '\'':
					result = '"';
					break;
			}
		}
	}
	if ( result ) {
		//printf( "KEY=%c\n", result );
		return result;
	}
	else {
		//Log( "Skipping unknown key code: " + std::to_string( code ) + " (modifiers: " + std::to_string( modifiers ) + ")" );
		return 0;
	}
}

key_code_t SDL2::GetScanCode( SDL_Scancode code, SDL_Keymod modifiers ) const {
	//Log( "Scan code: " + std::to_string( code ) + " (modifiers: " + std::to_string( modifiers ) + ")" );
	switch ( code ) {
#define X_KEY_CODE_1( _x ) case SDL_SCANCODE_##_x: return K_##_x;
#define X_KEY_CODE_2( _x, _sdl1 ) case SDL_SCANCODE_##_sdl1: return K_##_x;
#define X_KEY_CODE_3( _x, _sdl1, _sdl2 ) case SDL_SCANCODE_##_sdl1: return K_##_x; case SDL_SCANCODE_##_sdl2: return K_##_x;
		X_KEY_CODES
#undef X_KEY_CODE_1
#undef X_KEY_CODE_2
#undef X_KEY_CODE_3
		default: {
			return K_NONE;
		}
	}
}

key_modifier_t SDL2::GetModifiers( SDL_Keymod modifiers ) const {
	input::key_modifier_t result = input::KM_NONE;

	if ( ( modifiers & KMOD_LSHIFT ) || ( modifiers & KMOD_RSHIFT ) ) {
		result |= input::KM_SHIFT;
	}

	if ( ( modifiers & KMOD_LCTRL ) || ( modifiers & KMOD_RCTRL ) ) {
		result |= input::KM_CTRL;
	}

	if ( ( modifiers & KMOD_LALT ) || ( modifiers & KMOD_RALT ) ) {
		result |= input::KM_ALT;
	}

	return result;
}

}
}
