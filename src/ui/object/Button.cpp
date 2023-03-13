#include "Button.h"

#define DOUBLECLICK_MAX_MS 1000

namespace ui {
namespace object {

Button::Button( const std::string& class_name ) : Panel( class_name ) {
	SetEventContexts( EC_MOUSE | EC_MOUSEMOVE );
}

void Button::Create() {
	Panel::Create();
	
	NEW( m_label, Label );
		m_label->SetText( m_label_text );
		m_label->SetMargin( 3 );
		m_label->ForwardStyleAttributesV({
			Style::A_FONT,
			Style::A_TEXTCOLOR,
		});
		m_label->ForwardStyleAttribute( Style::A_TEXTALIGN, Style::A_ALIGN );
	AddChild( m_label );
	
	NEW( m_click_sound, SoundEffect );
		m_click_sound->ForwardStyleAttribute( Style::A_BUTTON_CLICK_SOUND, Style::A_SOUND );
		m_click_sound->ForwardStyleAttribute( Style::A_SOUND_VOLUME );
	AddChild( m_click_sound );
	
	NEW( m_move_sound, SoundEffect );
		m_move_sound->ForwardStyleAttribute( Style::A_BUTTON_MOVE_SOUND, Style::A_SOUND );
		m_move_sound->ForwardStyleAttribute( Style::A_SOUND_VOLUME );
	AddChild( m_move_sound );
}

void Button::Destroy() {
	RemoveChild( m_label );
	m_label = nullptr;
	
	RemoveChild( m_click_sound );
	RemoveChild( m_move_sound );
	
	Panel::Destroy();
}


void Button::SetLabel( const std::string& text ) {
	if ( text != m_label_text ) {
		m_label_text = text;
		if ( m_label ) {
			m_label->SetText( m_label_text );
		}
	}
}

const std::string& Button::GetLabel() const {
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

void Button::ApplyStyle() {
	Panel::ApplyStyle();
	
}

bool Button::OnMouseOver( const UIEvent::event_data_t* data ) {
	return true;
}

bool Button::OnMouseOut( const UIEvent::event_data_t* data ) {
	if ( m_is_clicking ) {
		m_is_clicking = false;
		RemoveStyleModifier( Style::M_ACTIVE );
	}
	if ( m_maybe_doubleclick ) {
		m_doubleclick_timer.Stop();
		m_maybe_doubleclick = false;
	}
	return true;
}

bool Button::OnMouseDown( const UIEvent::event_data_t* data ) {
	if ( data->mouse.button == UIEvent::M_LEFT ) {
		AddStyleModifier( Style::M_ACTIVE );
		m_is_clicking = true;
	}
	return true;
}

bool Button::OnMouseUp( const UIEvent::event_data_t* data ) {
	if ( m_is_clicking ) {
		m_is_clicking = false;
		RemoveStyleModifier( Style::M_ACTIVE );
		bool is_double_click = false;
		if ( m_maybe_doubleclick ) {
			if ( !m_doubleclick_timer.HasTicked() ) {
				is_double_click = true;
			}
			m_doubleclick_timer.Stop();
			m_maybe_doubleclick = false;
		}
		else {
			m_doubleclick_timer.SetTimeout( DOUBLECLICK_MAX_MS );
			m_maybe_doubleclick = true;
		}
		m_click_sound->Play();
		// double click event should go after normal click
		bool ret = Trigger( UIEvent::EV_BUTTON_CLICK, data );
		if ( is_double_click ) {
			Trigger( UIEvent::EV_BUTTON_DOUBLE_CLICK, data );
		}
		return ret;
	}
	return false;
}


}
}
