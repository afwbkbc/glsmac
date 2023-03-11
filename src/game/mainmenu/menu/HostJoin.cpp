#include "HostJoin.h"

#include "engine/Engine.h"

#include "types/ThreadCall.h"

#include "Host.h"
#include "Join.h"

namespace game {
namespace mainmenu {

HostJoin::HostJoin( MainMenu* mainmenu ) : PopupMenu( mainmenu, "HOST OR JOIN SESSION" ) {
	SetWidth( 500 );
	SetHeight( 150 );
	SetFlags( { PF_HAS_OK, PF_HAS_CANCEL } );
}

HostJoin::~HostJoin() {
}

void HostJoin::Show() {
	PopupMenu::Show();
	
	NEW( m_section, Section, "PopupSection" );
		m_section->SetTitleText( "Would you like to host a new game or join an existing one?" );
	m_body->AddChild( m_section );
	
	NEW( m_choices, ChoiceList, "PopupButtonList" );
		m_choices->SetMargin( 3 );
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
	MenuObject* menu = nullptr;
	if ( value == "Host new game" ) {
		m_mainmenu->m_settings.local.network_role = LocalSettings::NR_SERVER;
		NEW( menu, Host, m_mainmenu );
		NextMenu( menu );
	}
	else {
		m_mainmenu->m_settings.local.network_role = LocalSettings::NR_CLIENT;
		NEW( menu, Join, m_mainmenu );
		NextMenu( menu );
	}
}

const std::string HostJoin::GetChoice() const {
	return m_choices->GetValue();
}

void HostJoin::SetChoice( const std::string& choice ) {
	m_choices->SetValue( choice );
}


}
}
