#include "Multiplayer.h"

#include "HostJoin.h"
#include "ui/object/Section.h"

namespace game {
namespace mainmenu {

Multiplayer::Multiplayer( MainMenu* mainmenu ) : PopupMenu( mainmenu, "MULTIPLAYER SETUP" ) {
	SetWidth( 500 );
	SetHeight( 150 );
	SetFlags( { PF_HAS_OK, PF_HAS_CANCEL } );
}

void Multiplayer::Show() {
	PopupMenu::Show();

	NEW( m_section, Section, "PopupMenuSection" );
		m_section->SetTitleText( "Select a service..." );
	m_body->AddChild( m_section );
	
	NEW( m_choices, ChoiceList, "PopupMenuButtonList" );
		m_choices->SetPadding( 3 );
		m_choices->SetChoices({
			"Simple Internet TCP/IP Connection",
			"Hotseat/Play-by-Email",
		});
		m_choices->On( UIEvent::EV_BUTTON_DOUBLE_CLICK, EH( this ) {
			OnNext();
			return true;
		});
	m_section->AddChild( m_choices );
}

void Multiplayer::Hide() {
		m_section->RemoveChild( m_choices );
	m_body->RemoveChild( m_section );
	
	PopupMenu::Hide();
}

void Multiplayer::OnNext() {
	const auto value = m_choices->GetValue();
	if ( value == "Simple Internet TCP/IP Connection" ) {
		m_mainmenu->m_settings.network_type = Settings::NT_SIMPLETCP;
		NEWV( menu, HostJoin, m_mainmenu );
		NextMenu( menu );
	}
	else {
		MenuError( "This feature is not available yet." );
	}
}

const string Multiplayer::GetChoice() const {
	return m_choices->GetValue();
}

void Multiplayer::SetChoice( const string& choice ) {
	m_choices->SetValue( choice );
}

}
}
