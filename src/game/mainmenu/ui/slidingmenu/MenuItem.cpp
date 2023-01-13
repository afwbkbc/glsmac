#include "MenuItem.h"

using namespace std;
using namespace ui::object;
using namespace ui::event;

namespace game {
namespace mainmenu {

MenuItem::MenuItem( MenuBlock* menu, const string& text, const bool is_title )
	: UIContainer( "SlidingMenuItem" )
	, m_menu( menu )
	, m_text( text )
	, m_is_title( is_title )
{
	//
}

void MenuItem::Create() {
	UIContainer::Create();

	m_parts = {
		new Surface( "SlidingMenuItemBody" ),
		new Surface( "SlidingMenuItemLeft" ),
		new Surface( "SlidingMenuItemRight" ),
		new Surface( "SlidingMenuItemLowerTop" ),
		new Surface( "SlidingMenuItemUpperTop" ),
		new Surface( "SlidingMenuItemBottom" )
	};
	
	for (auto& part : m_parts) {
		AddChild( part );
	}
	
	m_label = new Label( m_text );
	m_label->SetClass( m_is_title ? "SlidingMenuTitleText" : "SlidingMenuButtonText" );
	AddChild(m_label);
}

void MenuItem::Destroy() {
	for (auto& part : m_parts) {
		RemoveChild( part );
	};
	m_parts.clear();
	
	RemoveChild(m_label);
	
	UIContainer::Destroy();
}

void MenuItem::OnMouseOver( const UIEvent::event_data_t* data ) {
	/*if (!m_is_title) {
		m_label->SetTextColor( m_menu->m_menu->m_mainmenu->m_menu_item_text_color_highlight );
	}*/
}

void MenuItem::OnMouseOut( const UIEvent::event_data_t* data ) {
	/*if (!m_is_title) {
		m_label->SetTextColor( m_menu->m_menu->m_mainmenu->m_menu_item_text_color_normal );
	}*/
}

void MenuItem::OnMouseDown( const UIEvent::event_data_t* data ) {
	if (!m_is_title) {
		m_menu->OnItemClick( m_text );
	}
}

}
}
