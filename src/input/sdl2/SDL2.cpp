#include "SDL2.h"

#ifdef __APPLE__
#include <pthread.h>
#endif

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
	// Ensure any pending dispatch operations complete before destruction
	// This prevents ProcessQueue from executing lambdas after the object is destroyed
	common::MainThreadDispatch::GetInstance()->ProcessQueue();
}

#ifdef __APPLE__
void SDL2::InitSDLOnMainThread() {
	ASSERT( pthread_main_np() != 0, "InitSDLOnMainThread must be called from main thread" );
	if ( SDL_Init( SDL_INIT_EVENTS ) ) {
		THROW( (std::string)"Failed to initialize SDL events: " + SDL_GetError() );
	}
}
#endif

void SDL2::Start() {
	Log( "Initializing SDL2" );
#ifdef __APPLE__
	// On macOS, SDL initialization must happen on the main thread due to AppKit requirements
	// SDL should already be initialized via InitSDLOnMainThread() called from main()
	ASSERT( SDL_WasInit( SDL_INIT_EVENTS ) != 0, "SDL events not initialized - InitSDLOnMainThread() must be called from main thread before starting threads" );
#else
	auto ret = SDL_Init( SDL_INIT_EVENTS );
#endif

}

void SDL2::Stop() {
	Log( "Deinitializing SDL2" );
	SDL_Quit();
}

void SDL2::PollEventsOnMainThread() const {
#ifdef __APPLE__
	ASSERT( pthread_main_np() != 0, "PollEventsOnMainThread must be called from main thread" );
#endif
	
	// Check if SDL is initialized before calling SDL_PollEvent
	if ( !SDL_WasInit( SDL_INIT_EVENTS ) ) {
		// SDL not initialized yet, skip polling
		return;
	}
	
	// Note: SDL_VideoInit() doesn't set SDL_INIT_VIDEO flag, so we can't check it here
	// SDL_PollEvent() works fine after SDL_VideoInit() even without SDL_INIT_VIDEO flag
	
	SDL_Event event;
	int event_count = 0;
	
	try {
		while ( SDL_PollEvent( &event ) ) {
			std::lock_guard<std::mutex> lock( m_event_buffer_mutex );
			m_event_buffer.push( event );
			event_count++;
		}
	} catch ( const std::exception& e ) {
	} catch ( ... ) {
	}
}

void SDL2::Iterate() {
	
	// Only dispatch if SDL events subsystem is initialized
	// Note: SDL_VideoInit() doesn't set SDL_INIT_VIDEO flag, so we only check SDL_INIT_EVENTS
	if ( !SDL_WasInit( SDL_INIT_EVENTS ) ) {
		return;
	}
	
	input::Event e = {}; // new
	ui_legacy::event::UIEvent* le = nullptr; // legacy

	// Poll events on main thread and store in buffer
	common::MainThreadDispatch::GetInstance()->DispatchVoid(
		[this]() { this->PollEventsOnMainThread(); }
	);
	
	// Process events from buffer (on worker thread)
	SDL_Event event;
	while ( true ) {
		{
			std::lock_guard<std::mutex> lock( m_event_buffer_mutex );
			if ( m_event_buffer.empty() ) {
				break;
			}
			event = m_event_buffer.front();
			m_event_buffer.pop();
		}
		e.SetType( EV_NONE );
		
		// Safety check: ensure g_engine is valid before accessing it
		if ( !g_engine ) {
			continue; // Skip event processing if engine is not available
		}
		
		switch ( event.type ) {
			case SDL_QUIT: {
				if ( g_engine ) {
					g_engine->ShutDown();
				}
				break;
			}
			case SDL_WINDOWEVENT: {
				if ( event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) {
					if ( g_engine && g_engine->GetGraphics() ) {
						g_engine->GetGraphics()->ResizeWindow( event.window.data1, event.window.data2 );
					}
				}
				break;
			}
			case SDL_MOUSEMOTION: {
				m_last_mouse_position = {
					event.motion.x,
					event.motion.y
				};
				// legacy
				NEW( le, ui_legacy::event::MouseMove, event.motion.x, event.motion.y );
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
				NEW( le, ui_legacy::event::MouseDown, event.motion.x, event.motion.y, button );
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
						// NEW( le, ui_legacy::event::MouseClick, event.motion.x, event.motion.y, GetMouseButton( event.button.button ) );
						// TODO: conflicts with Button OnClick logic
					}
					m_active_mousedowns.erase( event.button.button );
				}
				// legacy
				NEW( le, ui_legacy::event::MouseUp, event.motion.x, event.motion.y, button );
				// new ui
				e.SetType( EV_MOUSE_UP );
				e.data.mouse.x = event.motion.x;
				e.data.mouse.y = event.motion.y;
				e.data.mouse.button = button;
				break;
			}
			case SDL_MOUSEWHEEL: {
				// legacy
				NEW( le, ui_legacy::event::MouseScroll, m_last_mouse_position.x, m_last_mouse_position.y, event.wheel.y );
				// new ui
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
					ui_legacy::event::key_modifier_t key_modifiers = GetModifiers( modifiers );

					// legacy ui
					NEW( le, ui_legacy::event::KeyDown, (ui_legacy::event::key_code_t)scancode, keycode, key_modifiers );

					// new ui
					e.SetType( EV_KEY_DOWN );
					e.data.key.key = keycode;
					e.data.key.code = scancode;
					e.data.key.is_printable = keycode > 0;
					e.data.key.modifiers = key_modifiers;
				}
				break;
			}
			case SDL_KEYUP: {
				auto modifiers = SDL_GetModState();
				auto scancode = GetScanCode( event.key.keysym.scancode, modifiers );
				char keycode = GetKeyCode( event.key.keysym.sym, modifiers );
				if ( scancode || keycode ) {
					ui_legacy::event::key_modifier_t key_modifiers = GetModifiers( modifiers );

					// legacy ui
					NEW( le, ui_legacy::event::KeyUp, (ui_legacy::event::key_code_t)scancode, keycode, key_modifiers );

					// new ui
					e.SetType( EV_KEY_UP );
					e.data.key.key = keycode;
					e.data.key.code = scancode;
					e.data.key.is_printable = keycode > 0;
					e.data.key.modifiers = key_modifiers;
				}
				break;
			}
				// TODO: keypress?
		}
		if ( e.type != EV_NONE ) {
			// try new
			if ( !ProcessEvent( e ) ) {
				if ( le && g_engine && g_engine->GetUI() ) {
					// try legacy
					g_engine->GetUI()->ProcessEvent( le );
					DELETE( le );
				} else if ( le ) {
					DELETE( le );
			}
		}
	}
}

}

const std::string SDL2::GetClipboardText() const {
	// SDL_GetClipboardText() dispatched to main thread for consistency
	auto future = common::MainThreadDispatch::GetInstance()->Dispatch<std::string>([]() {
		std::string result = "";
		auto* t = SDL_GetClipboardText();
		if ( t ) {
			result = t;
			SDL_free( t );
		}
		return result;
	});
	// Wait for the operation to complete (this will block until main thread processes it)
	return future.get();
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
		case SDL_SCANCODE_RIGHT: {
			return K_RIGHT;
		}
		case SDL_SCANCODE_LEFT: {
			return K_LEFT;
		}
		case SDL_SCANCODE_DOWN: {
			return K_DOWN;
		}
		case SDL_SCANCODE_UP: {
			return K_UP;
		}
		case SDL_SCANCODE_RETURN: {
			return K_ENTER;
		}
		case SDL_SCANCODE_SPACE: {
			return K_SPACE;
		}
		case SDL_SCANCODE_TAB: {
			return K_TAB;
		}
		case SDL_SCANCODE_BACKSPACE: {
			return K_BACKSPACE;
		}
		case SDL_SCANCODE_ESCAPE: {
			return K_ESCAPE;
		}
		case SDL_SCANCODE_GRAVE: {
			return K_GRAVE;
		}
		case SDL_SCANCODE_PAGEUP: {
			return K_PAGEUP;
		}
		case SDL_SCANCODE_PAGEDOWN: {
			return K_PAGEDOWN;
		}
		case SDL_SCANCODE_HOME: {
			return K_HOME;
		}
		case SDL_SCANCODE_END: {
			return K_END;
		}
		case SDL_SCANCODE_INSERT: {
			return K_INSERT;
		}
		case SDL_SCANCODE_DELETE: {
			return K_DELETE;
		}
		case SDL_SCANCODE_KP_4: {
			return K_KP_LEFT;
		}
		case SDL_SCANCODE_KP_7: {
			return K_KP_LEFT_UP;
		}
		case SDL_SCANCODE_KP_8: {
			return K_KP_UP;
		}
		case SDL_SCANCODE_KP_9: {
			return K_KP_RIGHT_UP;
		}
		case SDL_SCANCODE_KP_6: {
			return K_KP_RIGHT;
		}
		case SDL_SCANCODE_KP_3: {
			return K_KP_RIGHT_DOWN;
		}
		case SDL_SCANCODE_KP_2: {
			return K_KP_DOWN;
		}
		case SDL_SCANCODE_KP_1: {
			return K_KP_LEFT_DOWN;
		}
		case SDL_SCANCODE_LCTRL: // do we need to differentiate?
		case SDL_SCANCODE_RCTRL: {
			return K_CTRL;
		}
		default: {
			return K_NONE;
		}
	}
	return K_NONE;
}

key_modifier_t SDL2::GetModifiers( SDL_Keymod modifiers ) const {
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
