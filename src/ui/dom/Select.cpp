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

Select::Select( DOM_ARGS )
	: Container( DOM_ARGS_PASS, "select", false ) {

	Events(
		{
			input::EV_SELECT,
		}
	);

	m_active_element = new Button( GSE_CALL, ui, this, {} );
	m_active_element->AddModifier( GSE_CALL, CM_SELECTED );
	ForwardProperty( GSE_CALL, "itemclass", "class", m_active_element );
	m_active_element->m_on_click = [ this ]() {
		if ( !m_choicelist->m_is_visible ) {
			if ( s_choicelist ) {
				s_choicelist->Hide();
			}
			s_choicelist = m_choicelist;
			s_choicelist->Show();
		}
		else {
			ASSERT_NOLOG( m_choicelist == s_choicelist, "choicelist mismatch" );
			m_choicelist->Hide();
			s_choicelist = nullptr;
		}
	};
	Embed( m_active_element );

	m_choicelist = new ChoiceList( GSE_CALL, ui, this, {} );
	m_choicelist->m_on_update = [ this, gc_space, ctx, si, ep ]( const std::string& value, const std::string& label, const bool send_event ) {
		m_choicelist->Hide();
		auto ep2 = ep;
		m_active_element->WrapSet( "text", VALUE( gse::value::String, , label ), gc_space, ctx, si, ep2 );
		if ( send_event ) {
			input::Event e;
			e.SetType( input::EV_SELECT );
			e.data.value.change_select.text = &value;
			ProcessEvent( gc_space, ctx, si, ep2, e );
		}
	};
	Embed( m_choicelist, true );
	m_choicelist->Hide();

	ForwardProperty( GSE_CALL, "itemclass", "itemclass", m_choicelist );
	ForwardProperty( GSE_CALL, "itemheight", "itemheight", m_choicelist );
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

}
}
