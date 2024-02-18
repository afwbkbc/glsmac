#include "Dropdown.h"

#include "engine/Engine.h"

#include "../event/MouseMove.h"

namespace ui {
namespace object {

template< typename KEY_TYPE >
Dropdown< KEY_TYPE >::Dropdown( const std::string& class_name )
	: Panel( class_name ) {

}

template< typename KEY_TYPE >
void Dropdown< KEY_TYPE >::SetChoices( const typename ChoiceList< KEY_TYPE >::choices_t& choices ) {
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
			for ( auto& choice : m_choices ) {
				SetValue( choice.second );
				break;
			}
		}
		else if ( m_elements.value ) {
			m_elements.value->SetText( m_value );
		}
	}
	if ( m_elements.open_close ) {
		if ( m_choices.empty() ) {
			m_elements.open_close->Hide();
		}
		else {
			m_elements.open_close->Show();
		}
	}
}

template< typename KEY_TYPE >
void Dropdown< KEY_TYPE >::SetValueByKey( const typename ChoiceList< KEY_TYPE >::value_t value ) {
	for ( const auto& it : m_choices ) {
		if ( it.first == value ) {
			SetValue( it.second );
			return;
		}
	}
}

template< typename KEY_TYPE >
void Dropdown< KEY_TYPE >::SetValue( const std::string& value ) {
	if ( m_mode == DM_SELECT ) {
		//ASSERT( std::find( m_choices.begin(), m_choices.end(), value ) != m_choices.end(), "value '" + value + "' not found in choices" );
	}
	m_value = value;
	if ( m_elements.value ) {
		m_elements.value->SetText( m_value );
	}
}

template< typename KEY_TYPE >
void Dropdown< KEY_TYPE >::SetMode( const dropdown_mode_t mode ) {
	m_mode = mode;
}

template<>
void Dropdown< size_t >::SetChoicesV( const std::vector< std::string >& labels ) {
	NumChoiceList::choices_t choices = {};
	size_t index = 1;
	for ( auto& label : labels ) {
		choices.push_back(
			{
				index++,
				label
			}
		);
	}
	SetChoices( choices );
}
template<>
void Dropdown< std::string >::SetChoicesV( const std::vector< std::string >& labels ) {
	THROW( "SetChoicesV not implemented for AssocDropdown" );
}

template< typename KEY_TYPE >
void Dropdown< KEY_TYPE >::SetTextColor( const Color& color ) {
	m_custom_text_color = true;
	m_text_color = color;
	if ( m_elements.value ) {
		m_elements.value->SetTextColor( m_text_color );
	}
}

template< typename KEY_TYPE >
void Dropdown< KEY_TYPE >::Create() {
	Panel::Create();

	NEW( m_elements.value, Label );
	m_elements.value->SetText( m_value );
	m_elements.value->ForwardStyleAttributesV(
		{
			Style::A_FONT,
			Style::A_TEXT_ALIGN,
		}
	);
	m_elements.value->ForwardStyleAttribute( Style::A_TEXT_LEFT, Style::A_LEFT );
	if ( m_custom_text_color ) {
		m_elements.value->SetTextColor( m_text_color );
	}
	else {
		m_elements.value->ForwardStyleAttribute( Style::A_TEXT_COLOR ); // TODO: global style overrides
	}
	AddChild( m_elements.value );

	NEW( m_elements.open_close, SimpleButton, GetStyleClass() + "OpenClose" );
	m_elements.open_close->SetAlign( ALIGN_HCENTER | ALIGN_TOP );
	m_elements.open_close->AddEventContexts( EC_PARENTAREA );
	m_elements.open_close->On(
		UIEvent::EV_MOUSE_DOWN, EH( this ) {
			if ( IsExpanded() ) {
				Collapse();
			}
			else {
				Expand();
			}
			return true;
		}
	);
	if ( m_choices.empty() ) {
		m_elements.open_close->Hide();
	}
	AddChild( m_elements.open_close );

	NEW( m_elements.choices, ChoiceList< KEY_TYPE >, GetStyleClass() + "Choices" );
	m_elements.choices->Hide();
	m_elements.choices->SetImmediateMode( true );
	m_elements.choices->SetChoices( m_choices );
	m_elements.choices->SetZIndex( 0.8f );
	m_elements.choices->On(
		UIEvent::EV_SELECT, EH( this ) {
			Collapse();
			const auto* value = data->value.change.text;
			if ( *value != m_value || m_mode == DM_MENU ) { // in menu mode allow to select same value again
				if ( m_mode == DM_SELECT ) {
					SetValue( *value );
				}
				Trigger( UIEvent::EV_CHANGE, data );
			}
			return true;
		}
	);
	m_elements.choices->AddEventContexts( UIObject::EC_OFFCLICK_AWARE );
	m_elements.choices->On(
		UIEvent::EV_OFFCLICK, EH( this ) {
			if ( !m_elements.open_close->IsPointInside( // prevent close-open because there will be mousedown event on value too
				data->mouse.absolute.x,
				data->mouse.absolute.y
			) ) {
				Collapse();
			}
			return true;
		}
	);
	g_engine->GetUI()->AddObject( m_elements.choices );

}

template< typename KEY_TYPE >
void Dropdown< KEY_TYPE >::Destroy() {

	RemoveChild( m_elements.value );
	RemoveChild( m_elements.open_close );

	g_engine->GetUI()->RemoveObject( m_elements.choices );

	Panel::Destroy();
}

template< typename KEY_TYPE >
void Dropdown< KEY_TYPE >::Align() {
	Panel::Align();

	if ( m_elements.value ) {
		m_elements.value->SetWidth( GetWidth() );
		m_elements.value->SetHeight( GetHeight() );
	}

	if ( m_elements.choices ) {
		m_elements.choices->SetTop( m_object_area.top + GetHeight() );
		m_elements.choices->SetLeft( m_object_area.left );
		m_elements.choices->SetWidth( m_object_area.width );
	}
}

template< typename KEY_TYPE >
const bool Dropdown< KEY_TYPE >::IsExpanded() const {
	return m_elements.open_close->HasStyleModifier( Style::M_ACTIVE );
}

template< typename KEY_TYPE >
void Dropdown< KEY_TYPE >::Expand() {
	m_elements.open_close->AddStyleModifier( Style::M_ACTIVE );
	m_elements.choices->Show();
}

template< typename KEY_TYPE >
void Dropdown< KEY_TYPE >::Collapse() {
	m_elements.choices->Hide();
	m_elements.open_close->RemoveStyleModifier( Style::M_ACTIVE );
}

template
class Dropdown< size_t >; // NumDropdown
template
class Dropdown< std::string >; // AssocDropdown

}
}
