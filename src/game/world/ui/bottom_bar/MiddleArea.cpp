#include "MiddleArea.h"

namespace game {
namespace world {
namespace ui {

MiddleArea::MiddleArea( World* world )
	: Section( world, "MiddleArea" )
{
	m_config.no_inner_border = true;
}

void MiddleArea::Create() {
	Section::Create();
	
	// buttons
	
	const std::string buttonclass = "MapBottomBarMiddleAreaButton";
	NEW( m_buttons.messages, ::ui::Button, buttonclass );
		m_buttons.messages->SetLabel( "M" );
		m_buttons.messages->SetTop( 3 );
		m_buttons.messages->SetHeight( 19 );
		m_buttons.messages->On( UIEvent::EV_BUTTON_CLICK, EH( this ) {
			SwitchPage( m_buttons.messages, m_pages.messages );
			return true;
		});
	AddChild( m_buttons.messages );
	
	NEW( m_buttons.toolbar, ::ui::Button, buttonclass );
		m_buttons.toolbar->SetLabel( "T" );
		m_buttons.toolbar->SetTop( 27 );
		m_buttons.toolbar->SetHeight( 37 );
		m_buttons.toolbar->On( UIEvent::EV_BUTTON_CLICK, EH( this ) {
			SwitchPage( m_buttons.toolbar, m_pages.toolbar );
			return true;
		});
	AddChild( m_buttons.toolbar );
	
	NEW( m_buttons.info_panels, ::ui::Button, buttonclass );
		m_buttons.info_panels->SetLabel( "I" );
		m_buttons.info_panels->SetTop( 69 );
		m_buttons.info_panels->SetHeight( 19 );
		m_buttons.info_panels->On( UIEvent::EV_BUTTON_CLICK, EH( this ) {
			SwitchPage( m_buttons.info_panels, m_pages.info_panels );
			return true;
		});
	AddChild( m_buttons.info_panels );
	
	// pages
	NEW( m_pages.messages, Messages, m_world );
		m_pages.messages->Hide();
	AddChild( m_pages.messages );
	
	NEW( m_pages.toolbar, Toolbar, m_world );
		m_pages.toolbar->Hide();
	AddChild( m_pages.toolbar );
	
	NEW( m_pages.info_panels, InfoPanels, m_world );
		m_pages.info_panels->Hide();
	AddChild( m_pages.info_panels );
	
	SwitchPage( m_buttons.info_panels, m_pages.info_panels );
}

void MiddleArea::Destroy() {
	
	// buttons
	
	RemoveChild( m_buttons.messages );
	RemoveChild( m_buttons.toolbar );
	RemoveChild( m_buttons.info_panels );
	
	// panels
	RemoveChild( m_pages.messages );
	RemoveChild( m_pages.toolbar );
	RemoveChild( m_pages.info_panels );
	
	Section::Destroy();
}

void MiddleArea::Align() {
	Section::Align();
	
	
}

void MiddleArea::SwitchPage( ::ui::Button* active_button, MiddleAreaPage* active_page ) {
	if ( m_active_button != active_button ) {
		if ( m_active_button ) {
			m_active_button->Deselect();
		}
		m_active_button = active_button;
		m_active_button->Select();
	}
	if ( m_active_page != active_page ) {
		if ( m_active_page ) {
			m_active_page->Hide();
		}
		m_active_page = active_page;
		m_active_page->Show();
	}
}

}
}
}
