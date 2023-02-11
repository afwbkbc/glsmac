#include "ChoiceList.h"

namespace ui {
namespace object {

ChoiceList::ChoiceList( const std::string& class_name ) : UIContainer( class_name ) {
	SetEventContexts( EC_KEYBOARD );
}

void ChoiceList::SetChoices( const choices_t& choices ) {
	ASSERT( m_choices.empty(), "choices already set" );
	
	m_choices = choices;
	
	if ( m_created ) {
		UpdateButtons();
	}
}

void ChoiceList::SetValue( const std::string& value ) {
	auto it = m_buttons.find( value );
	ASSERT( it != m_buttons.end(), "value does not exist in choices" );
	SetActiveButton( it->second );
}

const std::string& ChoiceList::GetValue() const {
	ASSERT( !m_choices.empty(), "choices are empty" );
	ASSERT( m_value < m_choices.size(), "choices value overflow" );
	return m_choices[ m_value ];
}

void ChoiceList::Create() {
	UIContainer::Create();
	
	if ( m_buttons.empty() && !m_choices.empty() ) {
		UpdateButtons();
	}
	
}

void ChoiceList::Destroy() {
	
	for ( auto& button : m_buttons ) {
		RemoveChild( button.second );
	}
	m_buttons.clear();
	
	UIContainer::Destroy();
}

/*void ChoiceList::OnChange( UIEventHandler::handler_function_t func ) {
	On( UIEvent::EV_CHANGE, func );
}*/

void ChoiceList::ApplyStyle() {
	UIContainer::ApplyStyle();
	
}

void ChoiceList::UpdateButtons() {
	if ( m_created ) {
		for ( auto& button : m_buttons ) {
			RemoveChild( button.second );
		}
		m_buttons.clear();
		size_t value = 0;
		for ( auto& choice : m_choices ) {
			NEWV( button, Button );
				button->SetLabel( choice );
				button->SetAlign( ALIGN_TOP );
				button->SetTextAlign( ALIGN_LEFT | ALIGN_VCENTER );
				button->SetLeft( 3 );
				button->SetRight( 3 );
				button->SetHeight( 20 );
				button->SetTop( 2 + ( button->GetHeight() + 2 ) * (value) );
				button->ForwardStyleAttributesV( m_forwarded_style_attributes );
				button->On( UIEvent::EV_BUTTON_CLICK, EH( this, button ) {
					if ( !button->HasStyleModifier( Style::M_SELECTED ) ) {
						SetActiveButton( button );
					}
					return true;
				});
				button->On( UIEvent::EV_BUTTON_DOUBLE_CLICK, EH( this ) {
					return Trigger( UIEvent::EV_BUTTON_DOUBLE_CLICK, data );
				});
			AddChild( button );
			m_button_values[ button ] = value;
			m_buttons[ choice ] = button;
			value++;
		}
		SetValue( m_choices[0] ); // activate first by default
	}
}

bool ChoiceList::OnKeyDown( const UIEvent::event_data_t* data ) {
	switch ( data->key.code ) {
		case UIEvent::K_DOWN: {
			if ( m_value < m_choices.size() - 1 ) {
				SetValue( m_choices[ m_value + 1 ] );
			}
			break;
		}
		case UIEvent::K_UP: {
			if ( m_value > 0 ) {
				SetValue( m_choices[ m_value - 1 ] );
			}
			break;
		}
	}
	return true;
}

bool ChoiceList::OnKeyUp( const UIEvent::event_data_t* data ) {
	//Log( "KEY UP" );
	return true;
}

bool ChoiceList::OnKeyPress( const UIEvent::event_data_t* data ) {
	//Log( "KEY PRESS" );
	return true;
}

void ChoiceList::SetActiveButton( Button* button ) {
	auto it = m_button_values.find( button );
	ASSERT( it != m_button_values.end(), "button not in buttons list" );
	for ( auto& b : m_buttons ) {
		if ( b.second != button && b.second->HasStyleModifier( Style::M_SELECTED ) ) {
			b.second->RemoveStyleModifier( Style::M_SELECTED );
		}
	}
	if ( !button->HasStyleModifier( Style::M_SELECTED ) ) {
		button->AddStyleModifier( Style::M_SELECTED );
	}
	ASSERT( it->second < m_choices.size(), "button value overflow" );
	m_value = it->second;
}

}
}
