#include "MiddleArea.h"

#include "game/frontend/Game.h"
#include "ui/object/Button.h"
#include "Messages.h"
#include "InfoPanels.h"
#include "Toolbar.h"

namespace game {
namespace frontend {
namespace ui {

MiddleArea::MiddleArea( Game* game )
	: BBSection( game, "MiddleArea" ) {
	m_config.no_inner_border = true;
}

void MiddleArea::Create() {
	BBSection::Create();

	// buttons

	const std::string buttonclass = "BBMiddleAreaButton";
	NEW( m_buttons.messages, ::ui::object::Button, buttonclass );
	m_buttons.messages->SetLabel( "M" );
	m_buttons.messages->SetTop( 3 );
	m_buttons.messages->SetHeight( 19 );
	m_buttons.messages->On(
		::ui::event::EV_BUTTON_CLICK, EH( this ) {
			SwitchPage( m_buttons.messages, m_pages.messages );
			return true;
		}
	);
	AddChild( m_buttons.messages );

	if ( m_game->IsMapEditingAllowed() ) {
		NEW( m_buttons.toolbar, ::ui::object::Button, buttonclass );
		m_buttons.toolbar->SetLabel( "T" );
		m_buttons.toolbar->SetTop( 27 );
		m_buttons.toolbar->SetHeight( 37 );
		m_buttons.toolbar->On(
			::ui::event::EV_BUTTON_CLICK, EH( this ) {
				SwitchPage( m_buttons.toolbar, m_pages.toolbar );
				return true;
			}
		);
		AddChild( m_buttons.toolbar );
	}

	NEW( m_buttons.info_panels, ::ui::object::Button, buttonclass );
	m_buttons.info_panels->SetLabel( "I" );
	m_buttons.info_panels->SetTop( 69 );
	m_buttons.info_panels->SetHeight( 19 );
	m_buttons.info_panels->On(
		::ui::event::EV_BUTTON_CLICK, EH( this ) {
			SwitchPage( m_buttons.info_panels, m_pages.info_panels );
			return true;
		}
	);
	AddChild( m_buttons.info_panels );

	// pages
	NEW( m_pages.messages, Messages, m_game );
	m_pages.messages->Hide();
	AddChild( m_pages.messages );

	if ( m_game->IsMapEditingAllowed() ) {
		NEW( m_pages.toolbar, Toolbar, m_game );
		m_pages.toolbar->Hide();
		AddChild( m_pages.toolbar );
	}

	NEW( m_pages.info_panels, InfoPanels, m_game );
	m_pages.info_panels->Hide();
	AddChild( m_pages.info_panels );

	if ( m_buttons.toolbar ) {
		SwitchPage( m_buttons.toolbar, m_pages.toolbar );
	}
	else {
		SwitchPage( m_buttons.messages, m_pages.messages );
	}
}

void MiddleArea::Destroy() {

	// buttons

	RemoveChild( m_buttons.messages );
	if ( m_buttons.toolbar ) {
		RemoveChild( m_buttons.toolbar );
	}
	RemoveChild( m_buttons.info_panels );

	// panels
	RemoveChild( m_pages.messages );
	if ( m_pages.toolbar ) {
		RemoveChild( m_pages.toolbar );
	}
	RemoveChild( m_pages.info_panels );

	BBSection::Destroy();
}

void MiddleArea::AddMessage( const std::string& text ) {
	m_pages.messages->AddMessage( text );
}

void MiddleArea::UpdateMapFileName() {
	if ( m_pages.toolbar ) { // map editor may be unavailable
		m_pages.toolbar->UpdateMapFileName();
	}
}

void MiddleArea::SwitchPage( ::ui::object::Button* active_button, MiddleAreaPage* active_page ) {
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
