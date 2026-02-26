#pragma once

#include <cstdint>

namespace input {

enum mouse_button_t : uint8_t {
	MB_NONE,
	MB_LEFT,
	MB_RIGHT,
	MB_MIDDLE,
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
	K_INSERT,
	K_DELETE,
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
	EV_CLICK,
	EV_DOUBLE_CLICK,
	EV_ON,
	EV_OFF,
	EV_CHANGE,
	EV_SELECT,
	EV_INPUT,
	EV_RESIZE,
	EV_CONFIRM,
	EV_OFFCLICK,
};

enum event_flag_t : uint8_t {
	EF_NONE = 0,
	EF_MOUSE = 1 << 0,
	EF_KEYBOARD = 1 << 1,
	EF_SYNTHETIC = 1 << 2,
};

}
