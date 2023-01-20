#include "Multiplayer.h"

namespace game {
namespace mainmenu {

Multiplayer::Multiplayer( MainMenu* mainmenu ) : PopupMenu( mainmenu, "MULTIPLAYER SETUP" ) {
	SetWidth( 500 );
	SetHeight( 154 );
	SetFlags( { PF_HAS_OK, PF_HAS_CANCEL } );
}

void Multiplayer::Show() {
	PopupMenu::Show();

	NEW( m_titlebar, Panel, "PopupMenuHeader" );
		m_titlebar->SetAlign( UIObject::ALIGN_TOP );
		m_titlebar->SetPadding( 3 );
		m_titlebar->SetHeight( 22 );
	m_body->AddChild( m_titlebar );
	NEW( m_titlelabel, Label, "PopupMenuHeaderLabel" );
		m_titlelabel->SetText( "Select a service..." );
		m_titlelabel->SetPadding( 2 );
		m_titlelabel->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_VCENTER );
	m_titlebar->AddChild( m_titlelabel );
	
	NEW( m_choices, ChoiceList, "PopupMenuButtonList" );
		m_choices->SetPadding( 3 );
		m_choices->SetTop( m_titlebar->GetHeight() + 6 );
		m_choices->SetChoices({
			"GLSMAC Internet TCP/IP Connection",
			"Hotseat/Play-by-Email",
		});
		m_choices->On( UIEvent::EV_BUTTON_DOUBLE_CLICK, EH( this ) {
			OnNext();
			return true;
		});
	m_body->AddChild( m_choices );
}

void Multiplayer::Hide() {
	m_body->RemoveChild( m_choices );
	
		m_titlebar->RemoveChild( m_titlelabel );
	m_body->RemoveChild( m_titlebar );
	
	PopupMenu::Hide();
}

void Multiplayer::OnNext() {
	Log( "SELECTED VALUE: " + m_choices->GetValue() );
	MenuError( "This feature is not available yet." );
}

const string Multiplayer::GetChoice() const {
	return m_choices->GetValue();
}

void Multiplayer::SetChoice( const string& choice ) {
	m_choices->SetValue( choice );
}

}
}
