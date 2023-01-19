#include "Button.h"

namespace ui {
namespace object {

Button::Button( const string& class_name ) : Panel( class_name ) {
	m_is_focusable = true;
	m_is_hoverable = true;
}

void Button::Create() {
	Panel::Create();
	
	NEW( m_label, Label );
		m_label->SetText( m_label_text );
		m_label->SetPadding( 3 );
		m_label->ForwardStyleAttributes({
			Style::A_FONT,
			Style::A_TEXTCOLOR,
		});
		m_label->ForwardStyleAttribute( Style::A_TEXTALIGN, Style::A_ALIGN );
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

const string& Button::GetLabel() const {
	return m_label_text;
}

void Button::SetTextAlign( UIObject::alignment_t alignment ) {
	if ( alignment != m_label_alignment ) {
		m_label_alignment = alignment;
		if ( m_label ) {
			m_label->SetAlign( m_label_alignment );
		}
	}
}

void Button::OnClick( UIEventHandler::handler_function_t func ) {
	AddEventHandler( UIEvent::EV_MOUSECLICK, func );
}

void Button::ApplyStyle() {
	Panel::ApplyStyle();
	
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
		m_is_clicking = false;
		RemoveStyleModifier( Style::M_ACTIVE );
		Trigger( UIEvent::EV_MOUSECLICK, data );
	}
}


}
}
