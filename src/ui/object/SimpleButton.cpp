#include "SimpleButton.h"

#define DOUBLECLICK_MAX_MS 1000

namespace ui {
namespace object {

SimpleButton::SimpleButton( const std::string& class_name )
	: Surface( class_name )
{	
	SetEventContexts( EC_MOUSE | EC_MOUSEMOVE );
}

bool SimpleButton::OnMouseOver( const UIEvent::event_data_t* data ) {
	return true;
}

bool SimpleButton::OnMouseOut( const UIEvent::event_data_t* data ) {
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

bool SimpleButton::OnMouseDown( const UIEvent::event_data_t* data ) {
	if ( data->mouse.button == UIEvent::M_LEFT ) {
		AddStyleModifier( Style::M_ACTIVE );
		m_is_clicking = true;
	}
	return true;
}

bool SimpleButton::OnMouseUp( const UIEvent::event_data_t* data ) {
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
