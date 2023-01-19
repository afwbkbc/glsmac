#include "ChoiceList.h"

namespace ui {
namespace object {

ChoiceList::ChoiceList( const string& class_name ) : UIContainer( class_name ) {
	
}

void ChoiceList::SetChoices( const choices_t& choices ) {
	ASSERT( m_choices.empty(), "choices already set" );
	
	m_choices = choices;
	
	m_value = "";
	for (auto& choice : m_choices ) {
		m_value = choice;
		break;
	}
	
	if ( m_created ) {
		UpdateButtons();
	}
}

const string& ChoiceList::GetValue() const {
	return m_value;
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
	AddEventHandler( UIEvent::EV_CHANGE, func );
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
		size_t line = 0;
		for ( auto& choice : m_choices ) {
			NEWV( button, Button );
				button->SetLabel( choice );
				button->SetAlign( ALIGN_TOP );
				button->SetTextAlign( ALIGN_LEFT | ALIGN_VCENTER );
				button->SetLeft( 3 );
				button->SetRight( 3 );
				button->SetHeight( 20 );
				button->SetTop( 2 + ( button->GetHeight() + 2 ) * (line) );
				button->ForwardStyleAttributes( m_forwarded_style_attributes );
				if ( line == 0) {
					button->AddStyleModifier( Style::M_SELECTED );
				}
				button->OnClick( EH( this, button ) {
					if ( !button->HasStyleModifier( Style::M_SELECTED ) ) {
						for ( auto& b : m_buttons ) {
							if ( b.second != button && b.second->HasStyleModifier( Style::M_SELECTED ) ) {
								b.second->RemoveStyleModifier( Style::M_SELECTED );
							}
						}
						button->AddStyleModifier( Style::M_SELECTED );
						m_value = button->GetLabel();
					}
				});
				line++;
			AddChild( button );
			m_buttons[ choice ] = button;
		}
	}
}

}
}
