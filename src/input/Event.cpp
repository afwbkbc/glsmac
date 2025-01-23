#include "Event.h"

#include <unordered_map>

#include "common/Common.h"

namespace input {

void Event::SetType( const event_type_t type ) {
	m_type = type;
	m_flags = EF_NONE;
	if (
		m_type == EV_MOUSE_MOVE ||
			m_type == EV_MOUSE_DOWN ||
			m_type == EV_MOUSE_UP ||
			m_type == EV_MOUSE_OVER ||
			m_type == EV_MOUSE_OUT ||
			m_type == EV_MOUSE_SCROLL
		) {
		m_flags = (event_flag_t)( m_flags | EF_MOUSE );
	}
}

const std::unordered_map< event_type_t, std::string > g_type_str = {
	{
		EV_NONE,
		"",
	},
	{
		EV_MOUSE_MOVE,
		"mousemove"
	},
	{
		EV_MOUSE_OVER,
		"mouseover"
	},
	{
		EV_MOUSE_OUT,
		"mouseout"
	},
	{
		EV_MOUSE_DOWN,
		"mousedown"
	},
	{
		EV_MOUSE_UP,
		"mouseup"
	},
	{
		EV_MOUSE_SCROLL,
		"mousescroll"
	},
	{
		EV_KEY_DOWN,
		"keydown"
	},
	{
		EV_KEY_UP,
		"keyup"
	},
	{
		EV_KEY_PRESS,
		"keypress"
	},
	{
		EV_CLICK,
		"click"
	},
	{
		EV_DOUBLE_CLICK,
		"dblclick"
	},
	{
		EV_CHANGE,
		"change"
	},
	{
		EV_SELECT,
		"select"
	},
	{
		EV_RESIZE,
		"resize"
	},
	{
		EV_CONFIRM,
		"confirm"
	},
	{
		EV_OFFCLICK,
		"offclick"
	},
};

const std::string& Event::GetTypeStr() const {
	ASSERT_NOLOG( g_type_str.find( type ) != g_type_str.end(), "code str not found: " + std::to_string( type ) );
	return g_type_str.at( type );
}

const std::unordered_map< Event::key_code_t, std::string > g_keycode_str = {
	{
		Event::K_NONE,
		"",
	},
	{
		Event::K_LEFT,
		"LEFT"
	},
	{
		Event::K_UP,
		"UP"
	},
	{
		Event::K_RIGHT,
		"RIGHT"
	},
	{
		Event::K_DOWN,
		"DOWN"
	},
	{
		Event::K_ENTER,
		"ENTER"
	},
	{
		Event::K_SPACE,
		"SPACE"
	},
	{
		Event::K_TAB,
		"TAB"
	},
	{
		Event::K_BACKSPACE,
		"BACKSPACE"
	},
	{
		Event::K_ESCAPE,
		"ESCAPE"
	},
	{
		Event::K_GRAVE,
		"GRAVE"
	},
	{
		Event::K_PAGEUP,
		"PAGEUP"
	},
	{
		Event::K_PAGEDOWN,
		"PAGEDOWN"
	},
	{
		Event::K_HOME,
		"HOME"
	},
	{
		Event::K_END,
		"END"
	},
	{
		Event::K_KP_LEFT,
		"KP_LEFT"
	},
	{
		Event::K_KP_LEFT_UP,
		"KP_LEFT_UP"
	},
	{
		Event::K_KP_UP,
		"KP_UP"
	},
	{
		Event::K_KP_RIGHT_UP,
		"KP_RIGHT_UP"
	},
	{
		Event::K_KP_RIGHT,
		"KP_RIGHT"
	},
	{
		Event::K_KP_RIGHT_DOWN,
		"KP_RIGHT_DOWN"
	},
	{
		Event::K_KP_DOWN,
		"KP_DOWN"
	},
	{
		Event::K_KP_LEFT_DOWN,
		"KP_LEFT_DOWN"
	},
	{
		Event::K_CTRL,
		"CTRL"
	},
};

const std::string& Event::GetKeyCodeStr() const {
	ASSERT_NOLOG( g_keycode_str.find( data.key.code ) != g_keycode_str.end(), "code str not found: " + std::to_string( data.key.code ) );
	return g_keycode_str.at( data.key.code );
}

const std::string GetModifiersStr( const Event::key_modifier_t modifiers ) {
	std::string result = "";
	if ( modifiers & Event::KM_SHIFT ) {
		result += "SHIFT,";
	}
	if ( modifiers & Event::KM_CTRL ) {
		result += "CTRL,";
	}
	if ( modifiers & Event::KM_ALT ) {
		result += "ALT,";
	}
	return result.substr( 0, result.size() - 1 );
}

const std::string Event::ToString() const {
	std::string result = "";
	switch ( type ) {
		case EV_KEY_DOWN: {
			result = "key=" + ( data.key.is_printable
				? std::string( 1, data.key.key )
				: ""
			) + " keycode=" + GetKeyCodeStr() + " modifiers=" + GetModifiersStr( data.key.modifiers );
			break;
		}
		default:
			ASSERT_NOLOG( false, "unknown event type: " + std::to_string( type ) );
	}
	return "event::" + GetTypeStr() + "( " + result + " )";
}

}
