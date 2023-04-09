#include "Multiplayer.h"

#include "HostJoin.h"
#include "ui/object/Section.h"

namespace task {
namespace mainmenu {

Multiplayer::Multiplayer( MainMenu* mainmenu ) : PopupMenu( mainmenu, "MULTIPLAYER SETUP" ) {
	SetWidth( 500 );
	SetHeight( 150 );
	SetFlags( { PF_HAS_OK, PF_HAS_CANCEL } );
}

void Multiplayer::Show() {
	PopupMenu::Show();

	NEW( m_section, Section, "PopupSection" );
		m_section->SetTitleText( "Select a service..." );
	m_body->AddChild( m_section );
	
	NEW( m_choices, ChoiceList, "PopupButtonList" );
		m_choices->SetImmediateMode( false );
		m_choices->SetMargin( 3 );
		m_choices->SetChoices({
			"Simple Internet TCP/IP Connection",
			"Hotseat/Play-by-Email",
		});
		m_choices->On( UIEvent::EV_SELECT, EH( this ) {
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
		m_mainmenu->m_settings.global.network_type = game::GlobalSettings::NT_SIMPLETCP;
		NEWV( menu, HostJoin, m_mainmenu );
		NextMenu( menu );
	}
	else {
		MenuError();
	}
}

const std::string Multiplayer::GetChoice() const {
	return m_choices->GetValue();
}

void Multiplayer::SetChoice( const std::string& choice ) {
	m_choices->SetValue( choice );
}

}
}
