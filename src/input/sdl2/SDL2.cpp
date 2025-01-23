#include "SDL2.h"

#include "engine/Engine.h"
#include "graphics/Graphics.h"
#include "ui_legacy/UI.h"

#include "ui_legacy/event/MouseMove.h"
#include "ui_legacy/event/MouseDown.h"
#include "ui_legacy/event/MouseUp.h"
#include "ui_legacy/event/MouseScroll.h"
#include "ui_legacy/event/KeyDown.h"
#include "ui_legacy/event/KeyUp.h"

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

	input::Event e = {};

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
				// legacy
				NEWV( ui_event, ui_legacy::event::MouseMove, event.motion.x, event.motion.y );
				g_engine->GetUI()->ProcessEvent( ui_event );
				DELETE( ui_event );
				// new ui
				e.SetType( EV_MOUSE_MOVE );
				e.data.mouse.x = event.motion.x;
				e.data.mouse.y = event.motion.y;
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
				// legacy
				NEWV( ui_event, ui_legacy::event::MouseDown, event.motion.x, event.motion.y, button );
				g_engine->GetUI()->ProcessEvent( ui_event );
				DELETE( ui_event );
				// new ui
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
					if ( mousedown_data.x == event.motion.x && mousedown_data.y == event.motion.y ) {
						// mousedown + mouseup at same pixel = mouseclick
						/*NEWV( ui_event_2, ui_legacy::event::MouseClick, event.motion.x, event.motion.y, GetMouseButton( event.button.button ) );
						g_engine->GetUI()->ProcessEvent( ui_event_2 );
						DELETE( ui_event_2 );*/ // TODO: conflicts with Button OnClick logic
					}
					m_active_mousedowns.erase( event.button.button );
				}
				// legacy
				NEWV( ui_event, ui_legacy::event::MouseUp, event.motion.x, event.motion.y, button );
				g_engine->GetUI()->ProcessEvent( ui_event );
				DELETE( ui_event );
				// new ui
				e.SetType( EV_MOUSE_UP );
				e.data.mouse.x = event.motion.x;
				e.data.mouse.y = event.motion.y;
				e.data.mouse.button = button;
				break;
			}
			case SDL_MOUSEWHEEL: {
				NEWV( ui_event, ui_legacy::event::MouseScroll, m_last_mouse_position.x, m_last_mouse_position.y, event.wheel.y );
				g_engine->GetUI()->ProcessEvent( ui_event );
				DELETE( ui_event );
				break;
			}
			case SDL_KEYDOWN: {
				auto modifiers = SDL_GetModState();
				auto scancode = GetScanCode( event.key.keysym.scancode, modifiers );
				char keycode = GetKeyCode( event.key.keysym.sym, modifiers );
				if ( scancode || keycode ) {
					// legacy ui
					ui_legacy::event::key_modifier_t keymodifiers = GetModifiers( modifiers );
					NEWV( ui_event, ui_legacy::event::KeyDown, (ui_legacy::event::key_code_t)scancode, keycode, keymodifiers );
					g_engine->GetUI()->ProcessEvent( ui_event );
					DELETE( ui_event );

					// new ui
					e.SetType( EV_KEY_DOWN );
					e.data.key.key = keycode;
					e.data.key.code = scancode;
					e.data.key.is_printable = keycode > 0;
					e.data.key.modifiers = modifiers;
				}
				break;
			}
			case SDL_KEYUP: {
				auto modifiers = SDL_GetModState();
				auto scancode = GetScanCode( event.key.keysym.scancode, modifiers );
				char keycode = GetKeyCode( event.key.keysym.sym, modifiers );
				if ( scancode || keycode ) {
					ui_legacy::event::key_modifier_t keymodifiers = GetModifiers( modifiers );
					NEWV( ui_event, ui_legacy::event::KeyUp, (ui_legacy::event::key_code_t)scancode, keycode, keymodifiers );
					g_engine->GetUI()->ProcessEvent( ui_event );
					DELETE( ui_event );
				}
				break;
			}
				// TODO: keypress?
		}
		if ( e.type != EV_NONE ) {
			ProcessEvent( e );
		}
	}

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

Event::key_code_t SDL2::GetScanCode( SDL_Scancode code, SDL_Keymod modifiers ) const {
	//Log( "Scan code: " + std::to_string( code ) + " (modifiers: " + std::to_string( modifiers ) + ")" );
	switch ( code ) {
		case SDL_SCANCODE_RIGHT: {
			return Event::K_RIGHT;
		}
		case SDL_SCANCODE_LEFT: {
			return Event::K_LEFT;
		}
		case SDL_SCANCODE_DOWN: {
			return Event::K_DOWN;
		}
		case SDL_SCANCODE_UP: {
			return Event::K_UP;
		}
		case SDL_SCANCODE_RETURN: {
			return Event::K_ENTER;
		}
		case SDL_SCANCODE_SPACE: {
			return Event::K_SPACE;
		}
		case SDL_SCANCODE_TAB: {
			return Event::K_TAB;
		}
		case SDL_SCANCODE_BACKSPACE: {
			return Event::K_BACKSPACE;
		}
		case SDL_SCANCODE_ESCAPE: {
			return Event::K_ESCAPE;
		}
		case SDL_SCANCODE_GRAVE: {
			return Event::K_GRAVE;
		}
		case SDL_SCANCODE_PAGEUP: {
			return Event::K_PAGEUP;
		}
		case SDL_SCANCODE_PAGEDOWN: {
			return Event::K_PAGEDOWN;
		}
		case SDL_SCANCODE_HOME: {
			return Event::K_HOME;
		}
		case SDL_SCANCODE_END: {
			return Event::K_END;
		}
		case SDL_SCANCODE_KP_4: {
			return Event::K_KP_LEFT;
		}
		case SDL_SCANCODE_KP_7: {
			return Event::K_KP_LEFT_UP;
		}
		case SDL_SCANCODE_KP_8: {
			return Event::K_KP_UP;
		}
		case SDL_SCANCODE_KP_9: {
			return Event::K_KP_RIGHT_UP;
		}
		case SDL_SCANCODE_KP_6: {
			return Event::K_KP_RIGHT;
		}
		case SDL_SCANCODE_KP_3: {
			return Event::K_KP_RIGHT_DOWN;
		}
		case SDL_SCANCODE_KP_2: {
			return Event::K_KP_DOWN;
		}
		case SDL_SCANCODE_KP_1: {
			return Event::K_KP_LEFT_DOWN;
		}
		case SDL_SCANCODE_LCTRL: // do we need to differentiate?
		case SDL_SCANCODE_RCTRL: {
			return Event::K_CTRL;
		}
		default: {
			return Event::K_NONE;
		}
	}
	return Event::K_NONE;
}

Event::key_modifier_t SDL2::GetModifiers( SDL_Keymod modifiers ) const {
	ui_legacy::event::key_modifier_t result = ui_legacy::event::KM_NONE;

	if ( ( modifiers & KMOD_LSHIFT ) || ( modifiers & KMOD_RSHIFT ) ) {
		result |= ui_legacy::event::KM_SHIFT;
	}

	if ( ( modifiers & KMOD_LCTRL ) || ( modifiers & KMOD_RCTRL ) ) {
		result |= ui_legacy::event::KM_CTRL;
	}

	if ( ( modifiers & KMOD_LALT ) || ( modifiers & KMOD_RALT ) ) {
		result |= ui_legacy::event::KM_ALT;
	}

	return result;
}

}
}
