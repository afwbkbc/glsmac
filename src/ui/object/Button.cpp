#include "Button.h"
#include "engine/Engine.h"

namespace ui {
namespace object {

Button::Button( const string& class_name ) : Panel( class_name ) {
	m_is_focusable = true;
}

void Button::Create() {
	Panel::Create();
	
	NEW( m_label, Label );
	m_label->SetText( m_label_text );
	AddChild( m_label );
}

void Button::Destroy() {
	RemoveChild( m_label );
	m_label = nullptr;
	
	Panel::Destroy();
}


void Button::SetLabel( const string& text ) {
	if ( text != m_label_text ) {
		m_label_text = text;
		if ( m_label ) {
			m_label->SetText( m_label_text );
		}
	}
}

void Button::OnClick( UIEventHandler::handler_function_t func ) {
	AddEventHandler( UIEvent::EV_MOUSECLICK, func );
}

void Button::ApplyStyle() {
	Panel::ApplyStyle();

	if ( Has( Style::A_FONT ) ) {
		m_label->SetFont( (Font*)GetObject( Style::A_FONT ) );
	}
	if ( Has( Style::A_TEXTCOLOR ) ) {
		m_label->SetTextColor( GetColor( Style::A_TEXTCOLOR ) );
	}
}

void Button::OnMouseOver( const UIEvent::event_data_t* data ) {
	
}

void Button::OnMouseOut( const UIEvent::event_data_t* data ) {
	if ( m_is_clicking ) {
		m_is_clicking = false;
		RemoveStyleModifier( Style::M_ACTIVE );
	}
}

void Button::OnMouseDown( const UIEvent::event_data_t* data ) {
	if ( data->mouse.button == UIEvent::M_LEFT ) {
		AddStyleModifier( Style::M_ACTIVE );
		m_is_clicking = true;
	}
}

void Button::OnMouseUp( const UIEvent::event_data_t* data ) {
	if ( m_is_clicking ) {
		Trigger( UIEvent::EV_MOUSECLICK, data );
		m_is_clicking = false;
		RemoveStyleModifier( Style::M_ACTIVE );
	}
}


}
}
