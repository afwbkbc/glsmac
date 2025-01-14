#pragma once

#include <string>
#include <cstdint>

#include "Types.h"

namespace input {

class Event {
public:

	enum mouse_button_t : uint8_t {
		M_NONE,
		M_LEFT,
		M_RIGHT,
		M_MIDDLE,
	};

	enum key_code_t {
		K_NONE,
		K_LEFT,
		K_UP,
		K_RIGHT,
		K_DOWN,
		K_ENTER,
		K_SPACE,
		K_TAB,
		K_BACKSPACE,
		K_ESCAPE,
		K_GRAVE,
		K_PAGEUP,
		K_PAGEDOWN,
		K_HOME,
		K_END,
		K_KP_LEFT,
		K_KP_LEFT_UP,
		K_KP_UP,
		K_KP_RIGHT_UP,
		K_KP_RIGHT,
		K_KP_RIGHT_DOWN,
		K_KP_DOWN,
		K_KP_LEFT_DOWN,
		K_CTRL
	};

	typedef uint8_t key_modifier_t;
	const static key_modifier_t KM_NONE = 0;
	const static key_modifier_t KM_SHIFT = 1 << 0;
	const static key_modifier_t KM_CTRL = 1 << 1;
	const static key_modifier_t KM_ALT = 1 << 2;

	typedef union {
		struct {
			ssize_t x;
			ssize_t y;
			uint8_t button;
			ssize_t scroll_y;
		} mouse;
		struct {
			bool is_printable;
			key_code_t code;
			char key;
			key_modifier_t modifiers;
		} key;
		union {
			struct {
				bool is_scrolling;
				float percentage;
			} scroll;
			struct {
				size_t id_num;
				const std::string* id_str;
				const std::string* text;
			} change;
		} value;
	} event_data_t;

	const event_type_t& type = m_type;
	const event_flag_t& flags = m_flags;
	event_data_t data = {};

	void SetType( const event_type_t type );

	const std::string& GetKeyCodeStr() const;
	const std::string& GetTypeStr() const;
	const std::string ToString() const;

private:
	event_type_t m_type = EV_NONE;
	event_flag_t m_flags = EF_NONE;
};

}
