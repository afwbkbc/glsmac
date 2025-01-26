#include "Button.h"

#include "Text.h"
#include "ui/geometry/Geometry.h"
#include "input/Event.h"

namespace ui {
namespace dom {

Button::Button( DOM_ARGS )
	: Panel( DOM_ARGS_PASS, "button", false ) {

	m_label = new Text( GSE_CALL, ui, this, {} );
	m_label->GetGeometry()->SetAlign( geometry::Geometry::ALIGN_CENTER );
	Embed( m_label );

	ForwardProperty( GSE_CALL, "text", m_label );
	ForwardProperty( GSE_CALL, "color", m_label );
	ForwardProperty( GSE_CALL, "font", m_label );

	Events(
		{
			input::EV_CLICK
		}
	);
}

const bool Button::ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) {
	switch ( event.type ) {
		case input::EV_MOUSE_DOWN: {
			AddModifier( GSE_CALL, CM_ACTIVE );
			m_last_button = event.data.mouse.button;
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
			break;
		}
		default: {
		}
	}
	return Panel::ProcessEventImpl( GSE_CALL, event );
}

void Button::SerializeEvent( const input::Event& e, gse::type::object_properties_t& obj ) const {
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
					VALUE( gse::type::String, buttonstr )
				}
			);
			break;
		}
		default: {
			Panel::SerializeEvent( e, obj );
		}
	}
}

}
}
