#include "Multiplayer.h"

#include "HostJoin.h"
#include "ui_legacy/object/Section.h"
#include "ui_legacy/object/Panel.h"
#include "ui_legacy/object/ChoiceList.h"
#include "task/mainmenu/MainMenu.h"
#include "game/backend/State.h"
#include "game/backend/settings/Settings.h"

namespace task {
namespace mainmenu {

enum {
	C_SIMPLE = 1,
	C_HOTSEAT = 2,
};

Multiplayer::Multiplayer( MainMenu* mainmenu )
	: PopupMenu( mainmenu, "MULTIPLAYER SETUP" ) {
	SetWidth( 500 );
	SetHeight( 150 );
	SetFlags(
		{
			PF_HAS_OK,
			PF_HAS_CANCEL
		}
	);
}

void Multiplayer::Show() {
	PopupMenu::Show();

	NEW( m_section, ui_legacy::object::Section, "PopupSection" );
	m_section->SetTitleText( "Select a service..." );
	m_body->AddChild( m_section );

	NEW( m_choices, ui_legacy::object::NumChoiceList, "PopupButtonList" );
	m_choices->SetImmediateMode( false );
	m_choices->SetMargin( 3 );
	m_choices->SetChoices(
		{
			{ C_SIMPLE,  "Simple Internet TCP/IP Connection" },
			{ C_HOTSEAT, "Hotseat/Play-by-Email" },
		}
	);
	m_choices->On(
		ui_legacy::event::EV_SELECT, EH( this ) {
			OnNext();
			return true;
		}
	);
	m_section->AddChild( m_choices );
}

void Multiplayer::Hide() {
	m_section->RemoveChild( m_choices );
	m_body->RemoveChild( m_section );

	PopupMenu::Hide();
}

void Multiplayer::OnNext() {
	const auto value = m_choices->GetValue();
	if ( value == C_SIMPLE ) {
		m_mainmenu->m_state->m_settings.local.network_type = game::backend::settings::LocalSettings::NT_SIMPLE_TCPIP;
		NEWV( menu, HostJoin, m_mainmenu );
		NextMenu( menu );
	}
	else {
		MenuError();
	}
}

const std::string Multiplayer::GetChoice() const {
	return m_choices->GetValueString();
}

void Multiplayer::SetChoice( const std::string& choice ) {
	m_choices->SetValueString( choice );
}

}
}
