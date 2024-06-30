#include "Input.h"

#include "engine/Engine.h"

#define CURSOR_BLINK_INTERVAL 200 // ms
#define CURSOR_BLINK_SYMBOL "_"

#include "Label.h"

namespace ui {
namespace object {

Input::Input( const std::string& class_name )
	: Panel( class_name ) {
	SetEventContexts( EC_MOUSE | EC_MOUSEMOVE );
}

void Input::Create() {
	Panel::Create();

	NEW( m_value_label, Label );
	m_value_label->SetText( m_value );
	m_value_label->SetMargin( 5 ); // TODO: align properly
	m_value_label->ForwardStyleAttributesM( m_forwarded_style_attributes.value_label );
	m_value_label->SetAlign( ALIGN_LEFT | ALIGN_BOTTOM );
	AddChild( m_value_label );

	if ( !m_hint.empty() ) {
		UpdateHintLabel();
	}

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

	RemoveChild( m_value_label );
	m_value_label = nullptr;

	if ( m_hint_label ) {
		RemoveChild( m_hint_label );
		m_hint_label = nullptr;
	}

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

void Input::SetHint( const std::string& hint ) {
	if ( m_hint != hint ) {
		m_hint = hint;
		UpdateHintLabel();
	}
}

const std::string& Input::GetHint() const {
	return m_hint;
}

void Input::SetMaxLength( const size_t max_length ) {
	if ( max_length != m_max_length ) {
		m_max_length = max_length;
		if ( m_max_length > 0 && m_value.size() > m_max_length ) {
			SetValue( m_value.substr( 0, m_max_length ) );
		}
	}
}

void Input::Clear() {
	SetValue( "" );
	SetHint( "" );
}

bool Input::OnKeyDown( const event::event_data_t* data ) {
	if ( data->key.code == event::K_BACKSPACE ) {
		if ( !m_value.empty() ) {
			m_value = m_value.substr( 0, m_value.size() - 1 );
			UpdateValueLabel( true );
		}
	}
	else if ( data->key.is_printable ) {
		if ( m_max_length > 0 && m_value.size() < m_max_length - 1 ) {
			m_value += std::string( 1, data->key.key );
			UpdateValueLabel( true );
		}
	}
	return true;
}

void Input::UpdateValueLabel( const bool send_event ) {
	if ( m_value_label ) {
		m_value_label->SetText( m_value + m_cursor_blink_value );
		if ( send_event ) {
			event::event_data_t data = {};
			data.value.change.text = &m_value;
			Trigger( event::EV_CHANGE, &data );
		}
	}
}

void Input::UpdateHintLabel() {
	if ( !m_hint_label ) {
		NEW( m_hint_label, Label );
		m_hint_label->SetMargin( 5 ); // TODO: align properly
		m_hint_label->ForwardStyleAttributesM( m_forwarded_style_attributes.hint_label );
		m_hint_label->SetAlign( ALIGN_LEFT | ALIGN_BOTTOM );
		AddChild( m_hint_label );
	}
	m_hint_label->SetText( m_hint );
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
