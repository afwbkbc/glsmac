#pragma once

#include <unordered_map>
#include <cstdint>

#define SDL_MAIN_HANDLED 1
#include <SDL.h>

#include "input/Input.h"

#include "ui/event/Types.h"

#include "types/Vec2.h"

namespace input {
namespace sdl2 {

CLASS( SDL2, Input )
	SDL2();
	~SDL2();

	void Start() override;
	void Stop() override;
	void Iterate() override;

private:

	types::Vec2< Sint32 > m_last_mouse_position = {
		0,
		0
	};

	ui::event::mouse_button_t GetMouseButton( uint8_t sdl_mouse_button ) const;
	char GetKeyCode( SDL_Keycode sdl_key_code, SDL_Keymod modifiers ) const;
	ui::event::key_code_t GetScanCode( SDL_Scancode code, SDL_Keymod modifiers ) const;
	ui::event::key_modifier_t GetModifiers( SDL_Keymod modifiers ) const;

	std::unordered_map< uint8_t, types::Vec2< ssize_t > > m_active_mousedowns;

};

}
}
