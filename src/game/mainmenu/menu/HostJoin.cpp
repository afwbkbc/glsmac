#include "HostJoin.h"

#include "Loading.h"
#include "Lobby.h"

namespace game {
namespace mainmenu {

HostJoin::HostJoin( MainMenu* mainmenu ) : PopupMenu( mainmenu, "MULTIPLAYER SETUP" ) {
	SetWidth( 500 );
	SetHeight( 150 );
	SetFlags( { PF_HAS_OK, PF_HAS_CANCEL } );
}

void HostJoin::Show() {
	PopupMenu::Show();
	
	NEW( m_section, Section, "PopupMenuSection" );
		m_section->SetTitleText( "Host or join?" );
	m_body->AddChild( m_section );
	
	NEW( m_choices, ChoiceList, "PopupMenuButtonList" );
		m_choices->SetPadding( 3 );
		m_choices->SetChoices({
			"Host new game",
			"Join existing game",
		});
		m_choices->On( UIEvent::EV_BUTTON_DOUBLE_CLICK, EH( this ) {
			OnNext();
			return true;
		});
	m_section->AddChild( m_choices );
}

void HostJoin::Hide() {
		m_section->RemoveChild( m_choices );
	m_body->RemoveChild( m_section );
	
	PopupMenu::Hide();
}

void HostJoin::OnNext() {
	const auto value = m_choices->GetValue();
	if ( value == "Host new game" ) {
		m_mainmenu->m_settings.network_role = Settings::NR_SERVER;
	}
	else {
		m_mainmenu->m_settings.network_role = Settings::NR_CLIENT;
	}
	
	/*
	NEWV( menu, Loading, m_mainmenu );
	menu->SetOnStart( LH( this ) {
		Log( "ONSTART" );
		return true;
		
	});
	menu->SetOnIterate( LH( this ) {
		Log( "ONITERATE" );
		return true;
	});
	menu->SetOnCancel( LH( this ) {
		Log( "ONCANCEL" );
		return true;
	});*/
	NEWV( menu, Lobby, m_mainmenu );
	
	NextMenu( menu );
}

const string HostJoin::GetChoice() const {
	return m_choices->GetValue();
}

void HostJoin::SetChoice( const string& choice ) {
	m_choices->SetValue( choice );
}


}
}
