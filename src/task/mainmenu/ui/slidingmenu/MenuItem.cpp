#include "MenuItem.h"

namespace task {
namespace mainmenu {

MenuItem::MenuItem( MenuBlock* menu, const std::string& text, const bool is_title )
	: UIContainer( "SlidingMenuItem" )
	, m_menu( menu )
	, m_text( text )
	, m_is_title( is_title ) {
	SetEventContexts( EC_MOUSE | EC_MOUSEMOVE );
}

void MenuItem::Create() {
	UIContainer::Create();

	m_parts.clear();

#define ADDPART( _part ) \
    { \
        NEWV( part, Surface, _part ); \
        m_parts.push_back( part ); \
    }
	ADDPART( "SlidingMenuItemBody" );
	ADDPART( "SlidingMenuItemLeft" );
	ADDPART( "SlidingMenuItemRight" );
	ADDPART( "SlidingMenuItemLowerTop" );
	ADDPART( "SlidingMenuItemUpperTop" );
	ADDPART( "SlidingMenuItemBottom" );
#undef ADDPART

	for ( auto& part : m_parts ) {
		AddChild( part );
	}

	NEW( m_label, Label );
	m_label->SetText( m_text );
	m_label->SetClass(
		m_is_title
			? "SlidingMenuTitleText"
			: "SlidingMenuButtonText"
	);
	AddChild( m_label );
}

void MenuItem::Destroy() {
	for ( auto& part : m_parts ) {
		RemoveChild( part );
	};
	m_parts.clear();

	RemoveChild( m_label );

	UIContainer::Destroy();
}

const std::string& MenuItem::GetText() const {
	return m_text;
}

bool MenuItem::OnMouseDown( const UIEvent::event_data_t* data ) {
	if ( !m_is_title && data->mouse.button == UIEvent::M_LEFT ) {
		m_menu->OnItemClick( m_text );
	}
	return true;
}

}
}
