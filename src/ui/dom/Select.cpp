#include "Select.h"

#include "gse/value/String.h"
#include "gse/value/Int.h"
#include "gse/value/Array.h"
#include "gse/value/Bool.h"
#include "gse/value/Float.h"

#include "Button.h"
#include "ChoiceList.h"
#include "input/Event.h"
#include "ui/UI.h"

namespace ui {
namespace dom {

static ChoiceList* s_choicelist = nullptr;
static Button* s_active_element = nullptr;

Select::Select( DOM_ARGS )
	: Container( DOM_ARGS_PASS, "select", false ) {

	Events(
		{
			input::EV_SELECT,
		}
	);

	m_active_element = new Button( GSE_CALL, ui, this, {} );
	ForwardProperty( GSE_CALL, "itemclass", "class", m_active_element );
	m_active_element->m_on_click = [ this, ctx, si, ep ]() {
		auto ep2 = ep;
		if ( !m_choicelist->m_is_visible ) {
			if ( s_choicelist ) {
				s_choicelist->Hide();
			}
			if ( s_active_element ) {
				s_active_element->RemoveModifier( m_gc_space, ctx, si, ep2, CM_HIGHLIGHT );
			}
			s_choicelist = m_choicelist;
			s_active_element = m_active_element;
			s_choicelist->Show();
			s_active_element->AddModifier( m_gc_space, ctx, si, ep2, CM_HIGHLIGHT );
		}
		else {
			ASSERT_NOLOG( m_choicelist == s_choicelist, "choicelist mismatch" );
			s_choicelist->Hide();
			s_choicelist = nullptr;
			ASSERT_NOLOG( m_active_element == s_active_element, "active_element mismatch" );
			s_active_element->RemoveModifier( m_gc_space, ctx, si, ep2, CM_HIGHLIGHT );
			s_active_element = nullptr;
		}
	};
	Embed( m_active_element );

	m_choicelist = new ChoiceList( GSE_CALL, ui, this, {} );
	m_choicelist->m_on_update = [ this, gc_space, ctx, si, ep ]( const std::string& value, const std::string& label, const bool send_event ) {
		auto ep2 = ep;
		m_active_element->RemoveModifier( m_gc_space, ctx, si, ep2, CM_HIGHLIGHT );
		m_choicelist->Hide();
		m_active_element->WrapSet( "text", VALUE( gse::value::String, , label ), gc_space, ctx, si, ep2 );
		if ( send_event ) {
			input::Event e;
			e.SetType( input::EV_SELECT );
			e.data.value.change_select.text = &value;
			ProcessEvent( gc_space, ctx, si, ep2, e );
		}
	};
	Embed( m_choicelist );
	m_choicelist->Hide();

	ForwardProperty( GSE_CALL, "itemclass", "itemclass", m_choicelist );

	Property(
		GSE_CALL, "itemheight", gse::value::Int::GetType(), VALUE( gse::value::Int, , 0 ), PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			m_choicelist->WrapSet( "itemheight", v, GSE_CALL );
			m_choicelist->WrapSet( "top", v, GSE_CALL );
		},
		[ this ]( GSE_CALLABLE ) {
			auto* const v = VALUE( gse::value::Int, , 0 );
			m_choicelist->WrapSet( "itemheight", v, GSE_CALL );
			m_choicelist->WrapSet( "top", v, GSE_CALL );
		}
	);

	ForwardProperty( GSE_CALL, "itempadding", "itempadding", m_choicelist );
	ForwardProperty( GSE_CALL, "value", "value", m_choicelist );
	ForwardProperty( GSE_CALL, "items", "items", m_choicelist );
}

const bool Select::ProcessEventImpl( GSE_CALLABLE, const input::Event& event ) {
	return Container::ProcessEventImpl( GSE_CALL, event );
}

void Select::WrapEvent( GSE_CALLABLE, const input::Event& e, gse::value::object_properties_t& obj ) const {
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

void Select::SetItemHeight( GSE_CALLABLE, const size_t px ) {
	m_choicelist->SetItemHeight( GSE_CALL, px );
}

}
}
