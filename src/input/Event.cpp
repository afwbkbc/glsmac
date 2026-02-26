#include "Event.h"

#include <unordered_map>

#include "common/Common.h"

namespace input {

void Event::SetType( const event_type_t type ) {
	m_type = type;
	m_flags = EF_NONE;
	switch ( m_type ) {
		case EV_MOUSE_MOVE:
		case EV_MOUSE_DOWN:
		case EV_MOUSE_UP:
		case EV_MOUSE_OVER:
		case EV_MOUSE_OUT:
		case EV_MOUSE_SCROLL: {
			m_flags = (event_flag_t)( m_flags | EF_MOUSE );
			break;
		}
		case EV_KEY_DOWN:
		case EV_KEY_UP: {
			m_flags = (event_flag_t)( m_flags | EF_KEYBOARD );
			break;
		}
		case EV_CLICK:
		case EV_CHANGE:
		case EV_SELECT:
		case EV_INPUT: {
			m_flags = (event_flag_t)( m_flags | EF_SYNTHETIC );
			break;
		}
		default: {
		}
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
		EV_ON,
		"on",
	},
	{
		EV_OFF,
		"off",
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
		EV_INPUT,
		"input"
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
	ASSERT( g_type_str.find( type ) != g_type_str.end(), "code str not found: " + std::to_string( type ) );
	return g_type_str.at( type );
}

const std::unordered_map< key_code_t, std::string > g_keycode_str = {
	{
		K_NONE,
		"",
	},
	{
		K_LEFT,
		"LEFT"
	},
	{
		K_UP,
		"UP"
	},
	{
		K_RIGHT,
		"RIGHT"
	},
	{
		K_DOWN,
		"DOWN"
	},
	{
		K_ENTER,
		"ENTER"
	},
	{
		K_SPACE,
		"SPACE"
	},
	{
		K_TAB,
		"TAB"
	},
	{
		K_BACKSPACE,
		"BACKSPACE"
	},
	{
		K_ESCAPE,
		"ESCAPE"
	},
	{
		K_GRAVE,
		"GRAVE"
	},
	{
		K_PAGEUP,
		"PAGEUP"
	},
	{
		K_PAGEDOWN,
		"PAGEDOWN"
	},
	{
		K_HOME,
		"HOME"
	},
	{
		K_END,
		"END"
	},
	{
		K_INSERT,
		"INSERT",
	},
	{
		K_DELETE,
		"DELETE",
	},
	{
		K_KP_LEFT,
		"KP_LEFT"
	},
	{
		K_KP_LEFT_UP,
		"KP_LEFT_UP"
	},
	{
		K_KP_UP,
		"KP_UP"
	},
	{
		K_KP_RIGHT_UP,
		"KP_RIGHT_UP"
	},
	{
		K_KP_RIGHT,
		"KP_RIGHT"
	},
	{
		K_KP_RIGHT_DOWN,
		"KP_RIGHT_DOWN"
	},
	{
		K_KP_DOWN,
		"KP_DOWN"
	},
	{
		K_KP_LEFT_DOWN,
		"KP_LEFT_DOWN"
	},
	{
		K_CTRL,
		"CTRL"
	},
};

const std::string& Event::GetKeyCodeStr() const {
	ASSERT( g_keycode_str.find( data.key.code ) != g_keycode_str.end(), "code str not found: " + std::to_string( data.key.code ) );
	return g_keycode_str.at( data.key.code );
}

const std::string GetModifiersStr( const key_modifier_t modifiers ) {
	std::string result = "";
	if ( modifiers & KM_SHIFT ) {
		result += "SHIFT,";
	}
	if ( modifiers & KM_CTRL ) {
		result += "CTRL,";
	}
	if ( modifiers & KM_ALT ) {
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
			ASSERT( false, "unknown event type: " + std::to_string( type ) );
	}
	return "event::" + GetTypeStr() + "( " + result + " )";
}

}
