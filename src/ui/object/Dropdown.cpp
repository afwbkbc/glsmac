#include "Dropdown.h"

#include "engine/Engine.h"

#include "../event/MouseMove.h"

namespace ui {
namespace object {

Dropdown::Dropdown( const std::string& class_name ) : Panel( class_name ) {
	
}

void Dropdown::SetChoices( const ChoiceList::choices_t& choices ) {
	m_choices = choices;
	if ( m_elements.choices ) {
		m_elements.choices->SetChoices( m_choices );
	}
	if ( m_mode == DM_SELECT ) {
		/*if ( std::find( m_choices.begin(), m_choices.end(), m_value ) == m_choices.end() ) {
			m_value = "";
		}*/
	}
	if ( m_value.empty() ) {
		if ( !choices.empty() ) {
			for ( auto& choice: m_choices ) {
				SetValue( choice.second );
				break;
			}
		}
		else if ( m_elements.value ) {
			m_elements.value->SetText( m_value );
		}
	}
}

void Dropdown::SetValue( const std::string& value ) {
	if ( m_mode == DM_SELECT ) {
		//ASSERT( std::find( m_choices.begin(), m_choices.end(), value ) != m_choices.end(), "value '" + value + "' not found in choices" );
	}
	m_value = value;
	if ( m_elements.value ) {
		m_elements.value->SetText( m_value );
	}
}

void Dropdown::SetMode( const dropdown_mode_t mode ) {
	m_mode = mode;
}

void Dropdown::SetChoicesV( const std::vector< std::string >& labels ) {
	ChoiceList::choices_t choices = {};
	size_t index = 1;
	for ( auto& label : labels ) {
		choices.push_back({ index++, label });
	}
	SetChoices( choices );
}

void Dropdown::SetTextColor( const Color& color ) {
	m_elements.value->SetTextColor( color );
}

void Dropdown::Create() {
	Panel::Create();
	
	NEW( m_elements.value, Label );
		m_elements.value->SetText( m_value );
		m_elements.value->ForwardStyleAttributesV({
			Style::A_FONT,
			Style::A_TEXT_COLOR,
			Style::A_TEXT_ALIGN,
		});
		m_elements.value->ForwardStyleAttribute( Style::A_TEXT_LEFT, Style::A_LEFT );
	AddChild( m_elements.value );
	
	NEW( m_elements.open_close, SimpleButton, GetStyleClass() + "OpenClose" );
		m_elements.open_close->SetAlign( ALIGN_HCENTER | ALIGN_TOP );
		m_elements.open_close->AddEventContexts( EC_PARENTAREA );
		m_elements.open_close->On( UIEvent::EV_MOUSE_DOWN, EH( this ) {
			if ( IsExpanded() ) {
				Collapse();
			}
			else {
				Expand();
			}
			return true;
		});
	AddChild( m_elements.open_close );
	
	NEW( m_elements.choices, ChoiceList, GetStyleClass() + "Choices" );
		m_elements.choices->Hide();
		m_elements.choices->SetImmediateMode( true );
		m_elements.choices->SetChoices( m_choices );
		m_elements.choices->SetZIndex( 0.8f );
		m_elements.choices->On( UIEvent::EV_SELECT, EH( this ) {
			Collapse();
			const auto* value = data->value.change.text;
			if ( *value != m_value ) {
				if ( m_mode == DM_SELECT ) {
					SetValue( *value );
				}
				Trigger( UIEvent::EV_CHANGE, data );
			}
			return true;
		});
	g_engine->GetUI()->AddObject( m_elements.choices );
}

void Dropdown::Destroy() {
	
	RemoveChild( m_elements.value );
	RemoveChild( m_elements.open_close );
	
	g_engine->GetUI()->RemoveObject( m_elements.choices );
	
	Panel::Destroy();
}

void Dropdown::Align() {
	Panel::Align();
	
	if ( m_elements.value ) {
		m_elements.value->SetWidth( GetWidth() );
		m_elements.value->SetHeight( GetHeight() );
	}
	
	if ( m_elements.choices ) {
		m_elements.choices->SetTop( m_object_area.top + GetHeight() );
		m_elements.choices->SetLeft( m_object_area.left );
		m_elements.choices->SetWidth( GetWidth() );
	}
}

const bool Dropdown::IsExpanded() const {
	return m_elements.open_close->HasStyleModifier( Style::M_ACTIVE );
}

void Dropdown::Expand() {
	m_elements.open_close->AddStyleModifier( Style::M_ACTIVE );
	m_elements.choices->Show();
}

void Dropdown::Collapse() {
	m_elements.choices->Hide();
	m_elements.open_close->RemoveStyleModifier( Style::M_ACTIVE );
}

}
}
