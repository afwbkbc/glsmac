#pragma once

#include <cstdint>

#include "base/Base.h"

namespace ui {
namespace event {

CLASS( UIEvent, base::Base )

	enum event_type_t {
		EV_NONE,
		EV_MOUSE_MOVE,
		EV_MOUSE_OVER,
		EV_MOUSE_OUT,
		EV_MOUSE_DOWN,
		EV_MOUSE_UP,
		EV_MOUSE_SCROLL,
		EV_KEY_DOWN,
		EV_KEY_UP,
		EV_KEY_PRESS,
		EV_BUTTON_CLICK,
		EV_BUTTON_DOUBLE_CLICK,
		EV_CHANGE,
		EV_SELECT,
		EV_RESIZE,
		EV_CONFIRM,
		EV_OFFCLICK,
	};

	// event flags
	typedef uint8_t event_flag_t;
	const static event_flag_t EF_NONE = 0;
	const static event_flag_t EF_MOUSE = 1 << 0;
	const static event_flag_t EF_KEYBOARD = 1 << 1;

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

	// modifier flags
	typedef uint8_t key_modifier_t;
	const static event_flag_t KM_NONE = 0;
	const static event_flag_t KM_SHIFT = 1 << 0;
	const static event_flag_t KM_CTRL = 1 << 1;
	const static event_flag_t KM_ALT = 1 << 2;

	typedef union {
		struct {
			struct {
				ssize_t x;
				ssize_t y;
			} absolute, relative;
			uint8_t button;
			ssize_t scroll_y;
			bool is_outside_parent;
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

	const event_type_t m_type = EV_NONE;
	event_flag_t m_flags = EF_NONE;
	event_data_t m_data = {};

	// event will stop propagating after this
	void SetProcessed();
	const bool IsProcessed() const;

	// needed to avoid mouseover duplications on objects underneath front one
	void SetMouseOverHappened();
	const bool IsMouseOverHappened();

	// copy from other event
	UIEvent( const UIEvent* other );

protected:
	UIEvent( const event_type_t type );

private:
	bool m_is_processed = false;
	bool m_is_mouseover_happened = false;

};

}
}
