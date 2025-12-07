#include "ChoiceList.h"

#include "gse/value/String.h"
#include "gse/value/Int.h"
#include "gse/value/Array.h"
#include "gse/value/Bool.h"

#include "Button.h"
#include "input/Event.h"

namespace ui {
namespace dom {

ChoiceList::ChoiceList( DOM_ARGS )
	: Container( DOM_ARGS_PASS, "choicelist", false, true ) {

	Events(
		{
			input::EV_SELECT,
		}
	);

	PROPERTY( "itemclass", gse::value::String, "", SetItemClass );
	PROPERTY( "itemheight", gse::value::Int, 0, SetItemHeight );
	PROPERTY( "itempadding", gse::value::Int, 0, SetItemPadding );
	PROPERTY( "value", gse::value::String, "", SetValue );
	PROPERTY( "items", gse::value::Array, {}, SetItems );
	PROPERTY( "keyboard", gse::value::Bool, false, SetKeyboard );
}

const bool ChoiceList::ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) {
	bool result = false;
	if ( !m_choices.empty() && m_keyboard ) {
		switch ( event.type ) {
			case input::EV_KEY_DOWN: {
				switch ( event.data.key.code ) {
					case input::K_UP:
					case input::K_KP_UP: {
						size_t next_idx = 0;
						if ( m_selected_choice && m_selected_choice->position > next_idx ) {
							next_idx = m_selected_choice->position - 1;
						}
						const auto& choice = m_choices.at( m_choices_order.at( next_idx ) );
						if ( !m_selected_choice || m_selected_choice != &choice ) {
							Select( GSE_CALL, choice, true );
						}
						break;
					}
					case input::K_DOWN:
					case input::K_KP_DOWN: {
						size_t next_idx = m_choices.size() - 1;
						if ( m_selected_choice && m_selected_choice->position < next_idx ) {
							next_idx = m_selected_choice->position + 1;
						}
						const auto& choice = m_choices.at( m_choices_order.at( next_idx ) );
						if ( !m_selected_choice || m_selected_choice != &choice ) {
							Select( GSE_CALL, choice, true );
						}
						break;
					}
					default: {
					}
				}
				result = true;
				break;
			}
			default: {
			}
		}
	}
	result |= Container::ProcessEventImpl( GSE_CALL, event );
	return result;
}

void ChoiceList::WrapEvent( GSE_CALLABLE, const input::Event& e, gse::value::object_properties_t& obj ) const {
	switch ( e.type ) {
		case input::EV_SELECT: {
			obj.insert(
				{
					"value",
					VALUE( gse::value::String, , *e.data.value.change_select.text )
				}
			);
			break;
		}
		default: {
			Container::WrapEvent( GSE_CALL, e, obj );
		}
	}
}

void ChoiceList::SetItemClass( GSE_CALLABLE, const std::string& cls ) {
	if ( cls != m_itemclass ) {
		m_itemclass = cls;
		for ( auto& it : m_choices ) {
			it.second.element->WrapSet( "class", VALUE( gse::value::String, , m_itemclass ), GSE_CALL );
		}
	}
}

void ChoiceList::SetItemHeight( GSE_CALLABLE, const size_t px ) {
	if ( px != m_itemheight ) {
		m_itemheight = px;
		size_t top = m_itempadding;
		for ( auto& it : m_choices ) {
			it.second.element->WrapSet( "top", VALUE( gse::value::Int, , top ), GSE_CALL );
			top += m_itemheight + m_itempadding;
		}
	}
}

void ChoiceList::SetItemPadding( GSE_CALLABLE, const size_t px ) {
	if ( px != m_itempadding ) {
		m_itempadding = px;
		size_t top = m_itempadding;
		for ( auto& it : m_choices ) {
			it.second.element->WrapSet( "top", VALUE( gse::value::Int, , top ), GSE_CALL );
			it.second.element->WrapSet( "left", VALUE( gse::value::Int, , m_itempadding ), GSE_CALL );
			it.second.element->WrapSet( "right", VALUE( gse::value::Int, , m_itempadding ), GSE_CALL );
			top += m_itemheight + m_itempadding;
		}
	}
}

void ChoiceList::Clear( GSE_CALLABLE ) {
	for ( const auto& it : m_choices ) {
		RemoveChild( GSE_CALL, it.second.element );
	}
	m_choices.clear();
	m_choices_order.clear();
	m_selected_choice = nullptr;
}

void ChoiceList::SetItems( GSE_CALLABLE, const gse::value::array_elements_t& items ) {
	if ( !m_choices.empty() ) {
		Clear( GSE_CALL );
	}
	m_choices.reserve( items.size() );
	m_choices_order.reserve( items.size() );
	size_t idx = 0;
	size_t top = m_itempadding;
	for ( const auto& item : items ) {
		if ( item->type != gse::Value::T_ARRAY ) {
			GSE_ERROR( gse::EC.TYPE_ERROR, "Choicelist items are expected to be value-label pairs, got " + item->GetTypeString() + ": " + item->ToString() );
		}
		const auto& kv = ( (gse::value::Array*)item )->value;
		if ( kv.size() < 2 || kv.size() > 3 || kv.at( 0 )->type != gse::Value::T_STRING || kv.at( 1 )->type != gse::Value::T_STRING ) {
			GSE_ERROR( gse::EC.TYPE_ERROR, "Choicelist items are expected to be pairs of two strings (value and label) (and optional style object), got: " + item->ToString() );
		}
		const auto& k = ( (gse::value::String*)kv.at( 0 ) )->value;
		const auto& v = ( (gse::value::String*)kv.at( 1 ) )->value;
		if ( m_choices.find( k ) != m_choices.end() ) {
			GSE_ERROR( gse::EC.TYPE_ERROR, "Choicelist duplicate value: " + k );
		}
		auto* element = new Button(
			GSE_CALL, m_ui, this, {
				{ "class", VALUE( gse::value::String, , m_itemclass ) },
				{ "text",  VALUE( gse::value::String, , v ) },
				{ "left",  VALUE( gse::value::Int, , m_itempadding ) },
				{ "right", VALUE( gse::value::Int, , m_itempadding ) },
				{ "top",   VALUE( gse::value::Int, , top ) },
			}
		);
		AddChild( GSE_CALL, element, true );
		ForwardProperty( GSE_CALL, "itemcolor", "color", element );
		if ( kv.size() >= 3 ) {
			const auto &c = kv.at(2);
			if ( c->type != gse::Value::T_OBJECT || ( !((gse::value::Object*)c)->object_class.empty() ) ) {
				GSE_ERROR( gse::EC.TYPE_ERROR, "Choicelist items are expected to have style object as third element, got: " + c->ToString() );
			}
			const auto* style = (gse::value::Object*)c;
			for ( const auto& it : style->value ) {
				element->WrapSet( it.first, it.second, GSE_CALL );
			}
		}
		const auto& choice = m_choices.insert(
			{ k, {
				k,
				v,
				idx++,
				element
			} }
		).first->second;
		m_choices_order.push_back( k );
		element->On( GSE_CALL, "mousedown", NATIVE_CALL( this, choice ) {
			const bool was_actually_changed = !m_selected_choice || m_selected_choice->value != choice.value;
			if ( was_actually_changed ) {
				Select( GSE_CALL, choice, true );
			}
			else if ( m_on_update ) {
				m_on_update( m_selected_choice->value, m_selected_choice->label, was_actually_changed );
			}
			return VALUE( gse::value::Bool,, true );
		} ) );
		top += m_itemheight + m_itempadding;
	}
	SetValue( GSE_CALL, m_value );
}

void ChoiceList::SetValue( GSE_CALLABLE, const std::string& value ) {
	if ( ( m_value.empty() || m_value != value ) && !m_choices.empty() && ( !m_selected_choice || m_selected_choice->value != value ) ) {
		m_value = value;
		if ( m_choices.find( m_value ) == m_choices.end() ) {
			m_value = m_choices_order.front();
		}
		const auto& it = m_choices.find( m_value );
		if ( it == m_choices.end() ) {
			GSE_ERROR( gse::EC.TYPE_ERROR, "Choicelist does not have value: " + m_value );
		}
		Select( GSE_CALL, it->second, false );
	}
}

void ChoiceList::Select( GSE_CALLABLE, const choice_t& choice, const bool send_event ) {
	if ( m_selected_choice ) {
		m_selected_choice->element->RemoveModifier( GSE_CALL, CM_SELECTED );
	}
	m_selected_choice = &choice;
	m_value = m_selected_choice->value;
	m_selected_choice->element->AddModifier( GSE_CALL, CM_SELECTED );
	UpdateProperty( "value", VALUE( gse::value::String, , m_selected_choice->value ) );
	if ( m_on_update ) {
		// managed by parent object otherwise
		m_on_update( m_selected_choice->value, m_selected_choice->label, send_event );
	}
	else {
		if ( send_event ) {
			input::Event e;
			e.SetType( input::EV_SELECT );
			e.data.value.change_select.text = &m_selected_choice->value;
			ProcessEvent( GSE_CALL, e );
		}
	}
}

void ChoiceList::SetKeyboard( GSE_CALLABLE, const bool value ) {
	if ( value != m_keyboard ) {
		m_keyboard = value;
	}
}

}
}
