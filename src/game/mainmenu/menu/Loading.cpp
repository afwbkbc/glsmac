#include "Loading.h"

#include "Lobby.h"

namespace game {
namespace mainmenu {

Loading::Loading( MainMenu* mainmenu ) : PopupMenu( mainmenu, "MULTIPLAYER SETUP" ) {
	SetWidth( 500 );
	SetHeight( 150 );
	SetFlags( { PF_HAS_CANCEL } );
}

void Loading::Show() {
	PopupMenu::Show();
	
	NEW( m_section, Section, "PopupMenuSection" );
		//m_section->SetTitleText( "Host or join?" );
	m_body->AddChild( m_section );
	
/*	NEW( m_choices, ChoiceList, "PopupMenuButtonList" );
		m_choices->SetPadding( 3 );
		m_choices->SetChoices({
			"Host new game",
			"Join existing game",
		});
		m_choices->On( UIEvent::EV_BUTTON_DOUBLE_CLICK, EH( this ) {
			OnNext();
			return true;
		});
	m_section->AddChild( m_choices );*/
	
	
}

void Loading::Hide() {
		//m_section->RemoveChild( m_choices );
	m_body->RemoveChild( m_section );
	
	PopupMenu::Hide();
}

void Loading::OnCancel() {
/*	const auto value = m_choices->GetValue();
	if ( value == "Host new game" ) {
		m_mainmenu->m_settings.network_role = Settings::NR_SERVER;
	}
	else {
		m_mainmenu->m_settings.network_role = Settings::NR_CLIENT;
	}
	
	NEWV( menu, Lobby, m_mainmenu );
	NextMenu( menu );*/
	Log( "CANCEL" );
}

void Loading::SetOnStart( const loading_handler_t handler ) {
	ASSERT( !m_handlers.on_start, "on_start already set" );
	m_handlers.on_start = handler;
}

void Loading::SetOnIterate( const loading_handler_t handler ) {
	ASSERT( !m_handlers.on_iterate, "on_iterate already set" );
	m_handlers.on_iterate = handler;
}

void Loading::SetOnCancel( const loading_handler_t handler ) {
	ASSERT( !m_handlers.on_cancel, "on_cancel already set" );
	m_handlers.on_cancel = handler;
}

}
}
