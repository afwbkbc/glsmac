#include "Input.h"

#include "Text.h"
#include "ui/geometry/Geometry.h"
#include "input/Event.h"

namespace ui {
namespace dom {

Input::Input( DOM_ARGS )
	: Panel( DOM_ARGS_PASS, "Input", false ) {

	m_text = new Text( ctx, call_si, ui, this, {} );
	m_text->GetGeometry()->SetAlign( geometry::Geometry::ALIGN_LEFT_CENTER );
	m_text->GetGeometry()->SetLeft( 4 ); // TODO: configurable?
	Embed( m_text );

	m_value = "test input text"; // TMP
	m_text->SetText( m_value );

	ForwardProperty( ctx, call_si, "color", m_text );
	ForwardProperty( ctx, call_si, "font", m_text );

	Events(
		{
			input::EV_KEY_DOWN
		}
	);

	m_blinker.SetInterval( 250 ); // TODO: config?
	Iterable(
		[ this ]() {
			while ( m_blinker.HasTicked() ) {
				m_is_blink_cursor_visible = !m_is_blink_cursor_visible;
				m_text->SetText(
					m_value + ( m_is_blink_cursor_visible
						? "_"
						: ""
					)
				);
			}
		}
	);
}

const bool Input::ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) {
	switch ( event.type ) {
		case input::EV_KEY_DOWN: {
			if ( event.data.key.code == input::Event::K_BACKSPACE ) {
				if ( !m_value.empty() ) {
					SetValue( m_value.substr( 0, m_value.size() - 1 ) );
				}
			}
			else if ( event.data.key.is_printable ) {
				SetValue( m_value + std::string( 1, event.data.key.key ) );
			}
			break;
		}
		default: {
		}
	}
	return Panel::ProcessEventImpl( ctx, call_si, event );
}

void Input::SerializeEvent( const input::Event& e, gse::type::object_properties_t& obj ) const {
	switch ( e.type ) {
		case input::EV_CHANGE: {
			std::string Inputstr = "";
/*			obj.insert(
				{
					"Input",
					VALUE( gse::type::String, Inputstr )
				}
			);*/
			THROW( "TODO: SERIALIZE ONCHANGE" );
			break;
		}
		default: {
			Panel::SerializeEvent( e, obj );
		}
	}
}

void Input::SetValue( const std::string& value ) {
	if ( value != m_value ) {
		m_value = value;
		m_text->SetText(
			value + ( m_is_blink_cursor_visible
				? "_"
				: ""
			)
		);
	}
}

}
}
