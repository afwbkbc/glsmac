#include "Button.h"

#include "Text.h"
#include "ui/geometry/Geometry.h"
#include "input/Event.h"
#include "gse/value/Bool.h"

namespace ui {
namespace dom {

Button::Button( DOM_ARGS )
	: Panel( DOM_ARGS_PASS, "button", false ) {

	m_label = new Text( GSE_CALL, ui, this, {} );
	m_label->GetGeometry()->SetAlign( geometry::Geometry::ALIGN_CENTER );
	Embed( m_label );

	ForwardProperty( GSE_CALL, "text", m_label );
	ForwardProperty( GSE_CALL, "transform", m_label );
	ForwardProperty( GSE_CALL, "color", m_label );
	ForwardProperty( GSE_CALL, "font", m_label );

	Events(
		{
			input::EV_CLICK,
			input::EV_KEY_DOWN,
		}
	);

	Property(
		GSE_CALL, "is_ok", gse::Value::T_BOOL, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			m_is_ok = ( (gse::value::Bool*)v )->value;
		},
		[ this ]( GSE_CALLABLE ) {
			m_is_ok = false;
		}
	);
	Property(
		GSE_CALL, "is_cancel", gse::Value::T_BOOL, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			m_is_cancel = ( (gse::value::Bool*)v )->value;
		},
		[ this ]( GSE_CALLABLE ) {
			m_is_cancel = false;
		}
	);
}

const bool Button::ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) {
	bool result = false;
	switch ( event.type ) {
		case input::EV_MOUSE_DOWN: {
			AddModifier( GSE_CALL, CM_ACTIVE );
			m_last_button = event.data.mouse.button;
			result = true;
			break;
		}
		case input::EV_MOUSE_UP:
		case input::EV_MOUSE_OUT: {
			RemoveModifier( GSE_CALL, CM_ACTIVE );
			if ( event.type == input::EV_MOUSE_UP && m_last_button != input::MB_NONE ) {
				// actual click happened
				input::Event e;
				e.SetType( input::EV_CLICK );
				e.data.mouse = event.data.mouse;
				ProcessEvent( GSE_CALL, e );
			}
			m_last_button = input::MB_NONE;
			result = true;
			break;
		}
		case input::EV_KEY_DOWN: {
			if (
				( m_is_ok && event.data.key.code == input::K_ENTER ) ||
					( m_is_cancel && event.data.key.code == input::K_ESCAPE )
				) {
				input::Event e;
				e.SetType( input::EV_CLICK );
				e.data.mouse.button = input::MB_LEFT;
				ProcessEvent( GSE_CALL, e );
				result = true;
			}
			break;
		}
		default: {
		}
	}
	result |= Panel::ProcessEventImpl( GSE_CALL, event );
	return result;
}

void Button::WrapEvent( gc::Space* const gc_space, const input::Event& e, gse::value::object_properties_t& obj ) const {
	switch ( e.type ) {
		case input::EV_CLICK: {
			std::string buttonstr = "";
			switch ( e.data.mouse.button ) {
				case input::MB_LEFT: {
					buttonstr = "left";
					break;
				}
				case input::MB_RIGHT: {
					buttonstr = "right";
					break;
				}
				case input::MB_MIDDLE: {
					buttonstr = "middle";
					break;
				}
				default: {
					ASSERT_NOLOG( false, "unknown button: " + std::to_string( e.data.mouse.button ) );
				}
			}
			obj.insert(
				{
					"button",
					VALUE( gse::value::String, , buttonstr )
				}
			);
			break;
		}
		default: {
			Panel::WrapEvent( gc_space, e, obj );
		}
	}
}

}
}
