#pragma once

#include <unordered_map>

#define SDL_MAIN_HANDLED 1
#include <SDL2/SDL.h>

#include "../Input.h"

#include "types/Vec2.h"

#include "ui/event/UIEvent.h"

using namespace types;

using namespace std;
using namespace ui;
namespace ui {
using namespace event;
}

namespace input {
namespace sdl2 {

CLASS( SDL2, Input )
	SDL2();
	~SDL2();
	
	void Start();
	void Stop();
	void Iterate();
	
private:
	
	UIEvent::mouse_button_t GetMouseButton( uint8_t sdl_mouse_button ) const;
	UIEvent::key_code_t GetKeyCode( uint8_t sdl_scan_code ) const;
	
	unordered_map< uint8_t, Vec2< ssize_t > > m_active_mousedowns;
	
};

} /* namespace sdl2 */
} /* namespace input */
