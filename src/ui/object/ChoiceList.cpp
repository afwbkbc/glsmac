#include "ChoiceList.h"

namespace ui {
namespace object {

template< typename KEY_TYPE >
ChoiceList< KEY_TYPE >::ChoiceList( const std::string& class_name )
	: UIContainer( class_name ) {
	SetEventContexts( EC_KEYBOARD );

	// TODO: move to styles
	m_item_align.margin = 2;
	m_item_align.height = 20;
}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::SetImmediateMode( const bool immediate_mode ) {
	if ( m_immediate_mode != immediate_mode ) {
		ASSERT( m_buttons.empty(), "can't change mode after initialization" );
		m_immediate_mode = immediate_mode;
	}
}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::SetChoices( const choices_t& choices ) {
	bool wasSet = !m_values.empty();
	std::string oldValue = (
		wasSet &&
			m_labels.find( m_value ) != m_labels.end()
	)
		? GetValueString()
		: "";

	m_values.clear();
	m_labels.clear();
	for ( auto& choice : choices ) {
		m_values.push_back( choice.first );
		m_labels[ choice.first ] = choice.second;
	}

	if ( m_created ) {
		UpdateButtons();
	}

	if ( wasSet ) {
		SetValueString( oldValue, true );
	}
}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::SetValue( const value_t value ) {
	auto it = m_buttons.find( value );
	ASSERT( it != m_buttons.end(), "value does not exist in choices" );
	SetActiveButton( it->second );
	const auto& value_index_it = std::find( m_values.begin(), m_values.end(), value );
	ASSERT( value_index_it != m_values.end(), "value does not exist" );
	m_value_index = value_index_it - m_values.begin();
}

template< typename KEY_TYPE >
const typename ChoiceList< KEY_TYPE >::value_t ChoiceList< KEY_TYPE >::GetValue() const {
	ASSERT( !m_values.empty(), "choices are empty" );
	ASSERT( m_labels.find( m_value ) != m_labels.end(), "choices value not found" );
	return m_value;
}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::SetValueString( const std::string& choice, bool allowMissing ) {
	// ugh
	bool found = false;
	for ( const auto& label : m_labels ) {
		if ( label.second == choice ) {
			SetValue( label.first );
			found = true;
			break;
		}
	}
	if ( !allowMissing ) {
		ASSERT( found, "choice '" + choice + "' not found" );
	}
}

template< typename KEY_TYPE >
const std::string& ChoiceList< KEY_TYPE >::GetValueString() const {
	return m_labels.at( GetValue() );
}

template<>
void ChoiceList< size_t >::SetChoicesV( const std::vector< std::string >& labels ) {
	choices_t choices = {};
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
void ChoiceList< std::string >::SetChoicesV( const std::vector< std::string >& labels ) {
	THROW( "SetChoicesV not implemented for AssocChoiceList" );
}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::Create() {
	UIContainer::Create();

	if ( m_buttons.empty() && !m_values.empty() ) {
		UpdateButtons();
	}

}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::Destroy() {

	for ( auto& button : m_buttons ) {
		RemoveChild( button.second );
	}
	m_buttons.clear();

	UIContainer::Destroy();
}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::Align() {
	UIContainer::Align();

	if ( !m_buttons.empty() ) {
		size_t value = 0;
		for ( auto& choice : m_values ) {
			auto* button = m_buttons.at( choice );
			button->SetHeight( m_item_align.height );
			button->SetTop( m_item_align.margin - 1 + ( m_item_align.height + m_item_align.margin - 1 ) * ( value ) );
			value++;
		}
	}
	SetHeight( ( m_item_align.margin * 2 + m_item_align.height - 1 ) * m_values.size() + 1 );

}

/*template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::OnChange( UIEventHandler::handler_function_t func ) {
	On( UIEvent::EV_CHANGE, func );
}*/

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::SetItemMargin( const coord_t item_margin ) {
	m_item_align.margin = item_margin;
	Realign();
}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::SetItemHeight( const coord_t item_height ) {
	m_item_align.height = item_height;
	Realign();
}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::ApplyStyle() {
	UIContainer::ApplyStyle();

	if ( Has( Style::A_ITEM_MARGIN ) ) {
		SetItemMargin( Get( Style::A_ITEM_MARGIN ) );
	}

	if ( Has( Style::A_ITEM_HEIGHT ) ) {
		SetItemHeight( Get( Style::A_ITEM_HEIGHT ) );
	}
}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::UpdateButtons() {
	if ( m_created ) {
		for ( auto& button : m_buttons ) {
			RemoveChild( button.second );
		}
		m_buttons.clear();
		size_t value_index = 0;
		for ( auto& value : m_values ) {
			size_t value_index_local = value_index;
			NEWV( button, Button );
			button->SetLabel( m_labels.at( value ) );
			button->SetAlign( ALIGN_TOP );
			button->SetTextAlign( ALIGN_LEFT | ALIGN_VCENTER );
			button->SetLeft( 3 );
			button->SetRight( 3 );
			button->ForwardStyleAttributesV( m_forwarded_style_attributes );
			button->On(
				UIEvent::EV_BUTTON_CLICK, EH( this, button, value_index_local ) {
					m_value_index = value_index_local;
					if ( !button->HasStyleModifier( Style::M_SELECTED ) ) {
						SetActiveButton( button );
					}
					if ( m_immediate_mode ) {
						SelectChoice();
					}
					return true;
				}
			);
			button->On(
				UIEvent::EV_BUTTON_DOUBLE_CLICK, EH( this ) {
					if ( !m_immediate_mode ) {
						SelectChoice();
					}
					return true;
				}
			);
			AddChild( button );
			m_button_values[ button ] = value;
			m_buttons[ value ] = button;
			value_index++;
		}
		Realign();
		if ( !m_immediate_mode ) {
			// activate first by default
			SetValue( m_values[ 0 ] );
		}
	}
}

template< typename KEY_TYPE >
bool ChoiceList< KEY_TYPE >::OnKeyDown( const UIEvent::event_data_t* data ) {
	switch ( data->key.code ) {
		case UIEvent::K_DOWN: {
			if ( m_value_index < m_values.size() - 1 ) {
				m_value_index++;
			}
			SetValue( m_values[ m_value_index ] );
			break;
		}
		case UIEvent::K_UP: {
			if ( m_value_index > 0 ) {
				m_value_index--;
			}
			SetValue( m_values[ m_value_index ] );
			break;
		}
		case UIEvent::K_ENTER: {
			SelectChoice();
			break;
		}
		default: {
			return true;
		}
	}
	return true;
}

template< typename KEY_TYPE >
bool ChoiceList< KEY_TYPE >::OnKeyUp( const UIEvent::event_data_t* data ) {
	return true;
}

template< typename KEY_TYPE >
bool ChoiceList< KEY_TYPE >::OnKeyPress( const UIEvent::event_data_t* data ) {
	return true;
}

template< typename KEY_TYPE >
void ChoiceList< KEY_TYPE >::SetActiveButton( Button* button ) {
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
	ASSERT( m_labels.find( it->second ) != m_labels.end(), "unknown button value" );
	m_value = it->second;
}

template<>
void ChoiceList< size_t >::SelectChoice() {
	UIEvent::event_data_t d = {};
	d.value.change.id_num = m_value;
	d.value.change.text = &GetValueString();
	Trigger( UIEvent::EV_SELECT, &d );
}

template<>
void ChoiceList< std::string >::SelectChoice() {
	UIEvent::event_data_t d = {};
	d.value.change.id_str = &m_value;
	d.value.change.text = &GetValueString();
	Trigger( UIEvent::EV_SELECT, &d );
}

template
class ChoiceList< size_t >; // NumChoiceList
template
class ChoiceList< std::string >; // AssocChoiceList

}
}
