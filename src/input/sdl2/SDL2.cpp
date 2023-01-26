#include "SDL2.h"

#include "engine/Engine.h"

#include "ui/event/MouseMove.h"
#include "ui/event/MouseDown.h"
#include "ui/event/MouseUp.h"
#include "ui/event/MouseScroll.h"
#include "ui/event/ButtonClick.h"
#include "ui/event/KeyDown.h"
#include "ui/event/KeyUp.h"
#include "ui/event/KeyPress.h"

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
		switch( event.type ) {
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
				NEWV( ui_event, event::MouseMove, event.motion.x, event.motion.y );
				g_engine->GetUI()->ProcessEvent( ui_event );
				DELETE( ui_event );
				break;
			}
			case SDL_MOUSEBUTTONDOWN: {
				NEWV( ui_event, event::MouseDown, event.motion.x, event.motion.y, GetMouseButton( event.button.button ) );
				g_engine->GetUI()->ProcessEvent( ui_event );
				ASSERT( m_active_mousedowns.find( event.button.button ) == m_active_mousedowns.end(),
					"duplicate mousedown (button=" + to_string( event.button.button ) + ")"
				);
				m_active_mousedowns[ event.button.button ] = { event.motion.x, event.motion.y };
				DELETE( ui_event );
				break;
			}
			case SDL_MOUSEBUTTONUP: {
				NEWV( ui_event, event::MouseUp, event.motion.x, event.motion.y, GetMouseButton( event.button.button ) );
				g_engine->GetUI()->ProcessEvent( ui_event );
				ASSERT( m_active_mousedowns.find( event.button.button ) != m_active_mousedowns.end(),
					"mouseup without mousedown"
				);
				auto& mousedown_data = m_active_mousedowns.at( event.button.button );
				if ( mousedown_data.x == event.motion.x && mousedown_data.y == event.motion.y ) {
					// mousedown + mouseup at same pixel = mouseclick
					/*NEWV( ui_event_2, event::MouseClick, event.motion.x, event.motion.y, GetMouseButton( event.button.button ) );
					g_engine->GetUI()->ProcessEvent( ui_event_2 );
					DELETE( ui_event_2 );*/ // TODO: conflicts with Button OnClick logic
				}
				m_active_mousedowns.erase( event.button.button );
				DELETE( ui_event );
				break;
			}
			case SDL_MOUSEWHEEL: {
				NEWV( ui_event, event::MouseScroll, event.motion.x, event.motion.y, event.wheel.y );
				g_engine->GetUI()->ProcessEvent( ui_event );
				DELETE( ui_event );
				break;
			}
			case SDL_KEYDOWN: {
				auto modifiers = SDL_GetModState();
				auto scancode = GetScanCode( event.key.keysym.scancode, modifiers );
				char keycode = GetKeyCode( event.key.keysym.sym, modifiers );
				if ( scancode || keycode ) {
					NEWV( ui_event, event::KeyDown, scancode, keycode );
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
					NEWV( ui_event, event::KeyUp, scancode, keycode );
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
			Log( "unsupported mouse button " + to_string( sdl_mouse_button ) );
			return UIEvent::M_NONE;
		}
	}
}

#include <cstdio>

char SDL2::GetKeyCode( SDL_Keycode code, SDL_Keymod modifiers ) const {
	
	char result = 0;
	if ( ( modifiers & (~KMOD_LSHIFT) & (~KMOD_RSHIFT) & (~KMOD_NUM) & (~KMOD_CAPS) & (~KMOD_SCROLL) ) == 0 ) { // only shift or num/caps/scroll locks
		bool is_shift = ( modifiers & KMOD_LSHIFT ) || ( modifiers & KMOD_RSHIFT );
		bool is_caps = ( modifiers & KMOD_CAPS );
		if ( code >= '0' && code <= '9' && !is_shift ) { // numbers
			result = code;
		}
		else if ( // allowed symbols
		(
			code == ' ' ||
			code == '.'
		) && !is_shift )
		{
			result = code;
		}
		else if ( code >= 'a' && code <= 'z' ) { // letters
			result = code;
			if ( is_shift ^ is_caps ) {
				result -= 32;
			}
		}
	}
	if ( result ) {
		//printf( "KEY=%c\n", result );
		return result;
	}
	else {
		//Log( "Skipping unknown key code: " + to_string( code ) + " (modifiers: " + to_string( modifiers ) + ")" );
		return 0;
	}
}

UIEvent::key_code_t SDL2::GetScanCode( SDL_Scancode code, SDL_Keymod modifiers ) const {
	if ( ( modifiers & (~KMOD_NUM) & (~KMOD_CAPS) & (~KMOD_SCROLL) ) == 0 ) { // num/caps/scroll locks are allowed
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
		}
	}
	//Log( "Skipping unknown scan code: " + to_string( code ) + " (modifiers: " + to_string( modifiers ) + ")" );
	return UIEvent::K_NONE;
}

}
}
