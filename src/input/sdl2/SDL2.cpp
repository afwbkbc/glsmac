#include "SDL2.h"

#include "engine/Engine.h"

#include "ui/event/MouseMove.h"
#include "ui/event/MouseDown.h"
#include "ui/event/MouseUp.h"
#include "ui/event/MouseScroll.h"
#include "ui/event/KeyDown.h"
#include "ui/event/KeyUp.h"

#if ( !SDL_VERSION_ATLEAST( 2, 0, 18 ) )
#define KMOD_SCROLL 0x8000 // workaround for ancient systems
#endif

namespace input {
namespace sdl2 {

SDL2::SDL2() {

}

SDL2::~SDL2() {

}

void SDL2::Start() {
	Log( "Initializing SDL2" );
	SDL_Init( SDL_INIT_EVENTS );

}

void SDL2::Stop() {
	Log( "Deinitializing SDL2" );
	SDL_Quit();
}

void SDL2::Iterate() {
	SDL_Event event;

	while ( SDL_PollEvent( &event ) ) {
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
				NEWV( ui_event, event::MouseMove, event.motion.x, event.motion.y );
				g_engine->GetUI()->ProcessEvent( ui_event );
				DELETE( ui_event );
				break;
			}
			case SDL_MOUSEBUTTONDOWN: {
				m_last_mouse_position = { // just in case, maybe some devices like touchscreens don't produce mouse move event before click
					event.motion.x,
					event.motion.y
				};
				NEWV( ui_event, event::MouseDown, event.motion.x, event.motion.y, GetMouseButton( event.button.button ) );
				g_engine->GetUI()->ProcessEvent( ui_event );
				ASSERT( m_active_mousedowns.find( event.button.button ) == m_active_mousedowns.end(),
					"duplicate mousedown (button=" + std::to_string( event.button.button ) + ")"
				);
				m_active_mousedowns[ event.button.button ] = {
					event.motion.x,
					event.motion.y
				};
				DELETE( ui_event );
				break;
			}
			case SDL_MOUSEBUTTONUP: {
				NEWV( ui_event, event::MouseUp, event.motion.x, event.motion.y, GetMouseButton( event.button.button ) );
				g_engine->GetUI()->ProcessEvent( ui_event );
				auto it = m_active_mousedowns.find( event.button.button );
				if ( it != m_active_mousedowns.end() ) { // sometimes touchscreen sends mouseup without or before mousedown, just ignore it
					auto& mousedown_data = m_active_mousedowns.at( event.button.button );
					if ( mousedown_data.x == event.motion.x && mousedown_data.y == event.motion.y ) {
						// mousedown + mouseup at same pixel = mouseclick
						/*NEWV( ui_event_2, event::MouseClick, event.motion.x, event.motion.y, GetMouseButton( event.button.button ) );
						g_engine->GetUI()->ProcessEvent( ui_event_2 );
						DELETE( ui_event_2 );*/ // TODO: conflicts with Button OnClick logic
					}
					m_active_mousedowns.erase( event.button.button );
				}
				DELETE( ui_event );
				break;
			}
			case SDL_MOUSEWHEEL: {
				NEWV( ui_event, event::MouseScroll, m_last_mouse_position.x, m_last_mouse_position.y, event.wheel.y );
				g_engine->GetUI()->ProcessEvent( ui_event );
				DELETE( ui_event );
				break;
			}
			case SDL_KEYDOWN: {
				auto modifiers = SDL_GetModState();
				auto scancode = GetScanCode( event.key.keysym.scancode, modifiers );
				char keycode = GetKeyCode( event.key.keysym.sym, modifiers );
				if ( scancode || keycode ) {
					UIEvent::key_modifier_t keymodifiers = GetModifiers( modifiers );
					NEWV( ui_event, event::KeyDown, scancode, keycode, keymodifiers );
					g_engine->GetUI()->ProcessEvent( ui_event );
					DELETE( ui_event );
				}
				break;
			}
			case SDL_KEYUP: {
				auto modifiers = SDL_GetModState();
				auto scancode = GetScanCode( event.key.keysym.scancode, modifiers );
				char keycode = GetKeyCode( event.key.keysym.sym, modifiers );
				if ( scancode || keycode ) {
					UIEvent::key_modifier_t keymodifiers = GetModifiers( modifiers );
					NEWV( ui_event, event::KeyUp, scancode, keycode, keymodifiers );
					g_engine->GetUI()->ProcessEvent( ui_event );
					DELETE( ui_event );
				}
				break;
			}
				// TODO: keypress?
		}
	}

}

UIEvent::mouse_button_t SDL2::GetMouseButton( uint8_t sdl_mouse_button ) const {
	switch ( sdl_mouse_button ) {
		case SDL_BUTTON_LEFT: {
			return UIEvent::M_LEFT;
		}
		case SDL_BUTTON_MIDDLE: {
			return UIEvent::M_MIDDLE;
		}
		case SDL_BUTTON_RIGHT: {
			return UIEvent::M_RIGHT;
		}
		default: {
			Log( "unsupported mouse button " + std::to_string( sdl_mouse_button ) );
			return UIEvent::M_NONE;
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
				code == ';' ||
				code == '/'
		) ) {
			result = code;
		}
		else if ( is_shift ) {
			switch ( code ) {
				case ':':
					result = ';';
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

UIEvent::key_code_t SDL2::GetScanCode( SDL_Scancode code, SDL_Keymod modifiers ) const {
	//Log( "Scan code: " + std::to_string( code ) + " (modifiers: " + std::to_string( modifiers ) + ")" );
	switch ( code ) {
		case SDL_SCANCODE_RIGHT: {
			return UIEvent::K_RIGHT;
		}
		case SDL_SCANCODE_LEFT: {
			return UIEvent::K_LEFT;
		}
		case SDL_SCANCODE_DOWN: {
			return UIEvent::K_DOWN;
		}
		case SDL_SCANCODE_UP: {
			return UIEvent::K_UP;
		}
		case SDL_SCANCODE_RETURN: {
			return UIEvent::K_ENTER;
		}
		case SDL_SCANCODE_SPACE: {
			return UIEvent::K_SPACE;
		}
		case SDL_SCANCODE_TAB: {
			return UIEvent::K_TAB;
		}
		case SDL_SCANCODE_BACKSPACE: {
			return UIEvent::K_BACKSPACE;
		}
		case SDL_SCANCODE_ESCAPE: {
			return UIEvent::K_ESCAPE;
		}
		case SDL_SCANCODE_GRAVE: {
			return UIEvent::K_GRAVE;
		}
		case SDL_SCANCODE_PAGEUP: {
			return UIEvent::K_PAGEUP;
		}
		case SDL_SCANCODE_PAGEDOWN: {
			return UIEvent::K_PAGEDOWN;
		}
		case SDL_SCANCODE_HOME: {
			return UIEvent::K_HOME;
		}
		case SDL_SCANCODE_END: {
			return UIEvent::K_END;
		}
		case SDL_SCANCODE_KP_4: {
			return UIEvent::K_KP_LEFT;
		}
		case SDL_SCANCODE_KP_7: {
			return UIEvent::K_KP_LEFT_UP;
		}
		case SDL_SCANCODE_KP_8: {
			return UIEvent::K_KP_UP;
		}
		case SDL_SCANCODE_KP_9: {
			return UIEvent::K_KP_RIGHT_UP;
		}
		case SDL_SCANCODE_KP_6: {
			return UIEvent::K_KP_RIGHT;
		}
		case SDL_SCANCODE_KP_3: {
			return UIEvent::K_KP_RIGHT_DOWN;
		}
		case SDL_SCANCODE_KP_2: {
			return UIEvent::K_KP_DOWN;
		}
		case SDL_SCANCODE_KP_1: {
			return UIEvent::K_KP_LEFT_DOWN;
		}
		case SDL_SCANCODE_LCTRL: // do we need to differentiate?
		case SDL_SCANCODE_RCTRL: {
			return UIEvent::K_CTRL;
		}
		default: {
			return UIEvent::K_NONE;
		}
	}
	return UIEvent::K_NONE;
}

UIEvent::key_modifier_t SDL2::GetModifiers( SDL_Keymod modifiers ) const {
	UIEvent::key_modifier_t result = UIEvent::KM_NONE;

	if ( ( modifiers & KMOD_LSHIFT ) || ( modifiers & KMOD_RSHIFT ) ) {
		result |= UIEvent::KM_SHIFT;
	}

	if ( ( modifiers & KMOD_LCTRL ) || ( modifiers & KMOD_RCTRL ) ) {
		result |= UIEvent::KM_CTRL;
	}

	if ( ( modifiers & KMOD_LALT ) || ( modifiers & KMOD_RALT ) ) {
		result |= UIEvent::KM_ALT;
	}

	return result;
}

}
}
