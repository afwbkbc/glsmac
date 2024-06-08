#pragma once

namespace resource {

enum resource_t : uint8_t {

	NONE,

	PCX_PALETTE,
	PCX_INTERFACE,
	PCX_JACKAL,
	PCX_ICONS,
	PCX_FLAGS,
	PCX_UNITS,
	PCX_TEXTURE,
	PCX_TER1,
	PCX_CONSOLE_X,
	PCX_CONSOLE_X2_A,
	PCX_CONSOLE2_A,
	PCX_LOGO,
	PCX_OPENINGA,
	PCX_SPACE_SM,
#define xM( _m ) PCX_MOON##_m
	xM( 1 ),
	xM( 2 ),
	xM( 3 ),
#undef xM
#define xSLC( _s, _l, _c ) PCX_S##_s##L##_l##C##_c
#define xSL( _s, _l ) \
    xSLC( _s, _l, 1 ), \
    xSLC( _s, _l, 2 ), \
    xSLC( _s, _l, 3 )
#define xS( _s ) \
    xSL( _s, 1 ), \
    xSL( _s, 2 ), \
    xSL( _s, 3 )
	xS( 1 ),
	xS( 2 ),
	xS( 3 ),
#undef xS
#undef xSL
#undef xSLC
	PCX_XI,
	PCX_XF,
	PCX_GAIANS,
	PCX_HIVE,
	PCX_UNIV,
	PCX_MORGAN,
	PCX_SPARTANS,
	PCX_BELIEVE,
	PCX_PEACE,
	PCX_CYBORG,
	PCX_PIRATES,
	PCX_DRONE,
	PCX_ANGELS,
	PCX_FUNGBOY,
	PCX_CARETAKE,
	PCX_USURPER,
	
	TTF_ARIALN,
	TTF_ARIALNB,

	WAV_OPENING_MENU,
	WAV_OK,
	WAV_MENU_UP,
	WAV_MENU_DOWN,
	WAV_MENU_OUT,
	WAV_AMENU2,
	WAV_MMENU,
	WAV_TURN_COMPLETE,
	WAV_PLS_DONT_GO,

};

}
