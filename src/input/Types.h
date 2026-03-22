#pragma once

#include <cstdint>

namespace input {

#define X_KEY_CODES \
    X_KEY_CODE_1( A ) \
    X_KEY_CODE_1( B ) \
    X_KEY_CODE_1( C ) \
    X_KEY_CODE_1( D ) \
    X_KEY_CODE_1( E ) \
    X_KEY_CODE_1( F ) \
    X_KEY_CODE_1( G ) \
    X_KEY_CODE_1( H ) \
    X_KEY_CODE_1( I ) \
    X_KEY_CODE_1( J ) \
    X_KEY_CODE_1( K ) \
    X_KEY_CODE_1( L ) \
    X_KEY_CODE_1( M ) \
    X_KEY_CODE_1( N ) \
    X_KEY_CODE_1( O ) \
    X_KEY_CODE_1( P ) \
    X_KEY_CODE_1( Q ) \
    X_KEY_CODE_1( R ) \
    X_KEY_CODE_1( S ) \
    X_KEY_CODE_1( T ) \
    X_KEY_CODE_1( U ) \
    X_KEY_CODE_1( V ) \
    X_KEY_CODE_1( W ) \
    X_KEY_CODE_1( X ) \
    X_KEY_CODE_1( Y ) \
    X_KEY_CODE_1( Z ) \
    X_KEY_CODE_1( 1 ) \
    X_KEY_CODE_1( 2 ) \
    X_KEY_CODE_1( 3 ) \
    X_KEY_CODE_1( 4 ) \
    X_KEY_CODE_1( 5 ) \
    X_KEY_CODE_1( 6 ) \
    X_KEY_CODE_1( 7 ) \
    X_KEY_CODE_1( 8 ) \
    X_KEY_CODE_1( 9 ) \
    X_KEY_CODE_1( 0 ) \
    X_KEY_CODE_1( LEFT ) \
    X_KEY_CODE_1( UP ) \
    X_KEY_CODE_1( RIGHT ) \
    X_KEY_CODE_1( DOWN ) \
    X_KEY_CODE_2( ENTER, RETURN ) \
    X_KEY_CODE_1( SPACE ) \
    X_KEY_CODE_1( TAB ) \
    X_KEY_CODE_1( BACKSPACE ) \
    X_KEY_CODE_1( ESCAPE ) \
    X_KEY_CODE_1( GRAVE ) \
    X_KEY_CODE_1( PAGEUP ) \
    X_KEY_CODE_1( PAGEDOWN ) \
    X_KEY_CODE_1( HOME ) \
    X_KEY_CODE_1( END ) \
    X_KEY_CODE_1( INSERT ) \
    X_KEY_CODE_1( DELETE ) \
    X_KEY_CODE_2( KP_LEFT_DOWN, KP_1 ) \
    X_KEY_CODE_2( KP_DOWN, KP_2 ) \
    X_KEY_CODE_2( KP_RIGHT_DOWN, KP_3 ) \
    X_KEY_CODE_2( KP_LEFT, KP_4 ) \
    X_KEY_CODE_2( KP_CENTER, KP_5 ) \
    X_KEY_CODE_2( KP_RIGHT, KP_6 ) \
    X_KEY_CODE_2( KP_LEFT_UP, KP_7 ) \
    X_KEY_CODE_2( KP_UP, KP_8 ) \
    X_KEY_CODE_2( KP_RIGHT_UP, KP_9 ) \
    X_KEY_CODE_3( CTRL, LCTRL, RCTRL ) \
    X_KEY_CODE_3( ALT, LALT, RALT ) \
    X_KEY_CODE_3( SHIFT, LSHIFT, RSHIFT )

enum mouse_button_t : uint8_t {
	MB_NONE,
	MB_LEFT,
	MB_RIGHT,
	MB_MIDDLE,
};

enum key_code_t {
	K_NONE,
#define X_KEY_CODE_1( _x ) K_##_x,
#define X_KEY_CODE_2( _x, _sdl1 ) K_##_x,
#define X_KEY_CODE_3( _x, _sdl1, _sdl2 ) K_##_x,
	X_KEY_CODES
#undef X_KEY_CODE_1
#undef X_KEY_CODE_2
#undef X_KEY_CODE_3
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
