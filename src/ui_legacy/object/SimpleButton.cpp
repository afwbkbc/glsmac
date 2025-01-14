#include "SimpleButton.h"

#define DOUBLECLICK_MAX_MS 1000

#include "Surface.h"
#include "SoundEffect.h"

namespace ui_legacy {
namespace object {

SimpleButton::SimpleButton( const std::string& class_name )
	: UIContainer( class_name ) {
	SetEventContexts( EC_MOUSE | EC_MOUSEMOVE );
}

void SimpleButton::Create() {
	UIContainer::Create();

	NEW( m_background, Surface );
	m_background->ForwardStyleAttribute( A_TEXTURE );
	AddChild( m_background );

	NEW( m_click_sound, SoundEffect );
	m_click_sound->ForwardStyleAttribute( A_BUTTON_CLICK_SOUND, A_SOUND );
	m_click_sound->ForwardStyleAttribute( A_SOUND_VOLUME );
	AddChild( m_click_sound );
}

void SimpleButton::Destroy() {
	RemoveChild( m_background );
	RemoveChild( m_click_sound );

	UIContainer::Destroy();
}

bool SimpleButton::OnMouseOver( const event::event_data_t* data ) {
	return true;
}

bool SimpleButton::OnMouseOut( const event::event_data_t* data ) {
	if ( m_is_clicking ) {
		m_is_clicking = false;
		RemoveStyleModifier( M_ACTIVE );
	}
	if ( m_maybe_doubleclick ) {
		m_doubleclick_timer.Stop();
		m_maybe_doubleclick = false;
	}
	return true;
}

bool SimpleButton::OnMouseDown( const event::event_data_t* data ) {
	if ( data->mouse.button == event::M_LEFT ) {
		AddStyleModifier( M_ACTIVE );
		m_is_clicking = true;
	}
	return true;
}

bool SimpleButton::OnMouseUp( const event::event_data_t* data ) {
	if ( m_is_clicking ) {
		m_is_clicking = false;
		RemoveStyleModifier( M_ACTIVE );
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
		bool ret = Trigger( event::EV_BUTTON_CLICK, data );
		if ( is_double_click ) {
			Trigger( event::EV_BUTTON_DOUBLE_CLICK, data );
		}
		return ret;
	}
	return false;
}

}
}
