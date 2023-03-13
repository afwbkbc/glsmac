#include "Input.h"

#include "engine/Engine.h"

#define CURSOR_BLINK_INTERVAL 200 // ms
#define CURSOR_BLINK_SYMBOL "_"

namespace ui {
namespace object {

Input::Input( const std::string& class_name ) : Panel( class_name ) {
	SetEventContexts( EC_MOUSE | EC_MOUSEMOVE );
}

void Input::Create() {
	Panel::Create();
	
	NEW( m_label, Label );
		m_label->SetText( m_value );
		m_label->SetMargin( 5 ); // TODO: align properly
		m_label->ForwardStyleAttributesV({
			Style::A_FONT,
			Style::A_TEXTCOLOR,
		});
		m_label->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_BOTTOM );
	AddChild( m_label );
	
	SetFocusable( true );
	
	m_cursor_blink_value = " ";
}

void Input::Iterate() {
	Panel::Iterate();
	
	if ( m_cursor_blink_timer.HasTicked() ) {
		if ( m_cursor_blink_value == " " ) {
			m_cursor_blink_value = CURSOR_BLINK_SYMBOL;
		}
		else {
			m_cursor_blink_value = " ";
		}
		UpdateValueLabel();
	}
}

void Input::Destroy() {
	m_cursor_blink_timer.Stop();
	
	RemoveChild( m_label );
	m_label = nullptr;
	
	Panel::Destroy();
}

void Input::SetValue( const std::string& value ) {
	if ( value != m_value ) {
		m_value = value;
		UpdateValueLabel();
	}
}

const std::string& Input::GetValue() const {
	return m_value;
}

void Input::SetMaxLength( const size_t max_length ) {
	if ( max_length != m_max_length ) {
		m_max_length = max_length;
		if ( m_max_length > 0 && m_value.size() > m_max_length ) {
			SetValue( m_value.substr( 0, m_max_length ) );
		}
	}
}

bool Input::OnKeyDown( const UIEvent::event_data_t* data ) {
	if ( data->key.code == UIEvent::K_BACKSPACE ) {
		if ( !m_value.empty() ) {
			m_value = m_value.substr( 0, m_value.size() - 1 );
			UpdateValueLabel();
		}
	}
	else if ( data->key.is_printable ) {
		if ( m_max_length > 0 && m_value.size() < m_max_length - 1 ) {
			m_value += std::string( 1, data->key.key );
			UpdateValueLabel();
		}
	}
	return true;
}

void Input::UpdateValueLabel() {
	if ( m_label ) {
		m_label->SetText( m_value + m_cursor_blink_value );
	}
}

void Input::Focus() {
	Panel::Focus();
	
	m_cursor_blink_value = CURSOR_BLINK_SYMBOL;
	UpdateValueLabel();
	m_cursor_blink_timer.SetInterval( CURSOR_BLINK_INTERVAL );	
}

void Input::Defocus() {
	Panel::Defocus();
	
	m_cursor_blink_timer.Stop();
	m_cursor_blink_value = " ";
	UpdateValueLabel();

}

}
}
