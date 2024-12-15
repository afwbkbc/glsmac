#include "Host.h"

#include "engine/Engine.h"
#include "game/backend/connection/Server.h"
#include "ui_legacy/object/Section.h"
#include "ui_legacy/object/Input.h"
#include "ui_legacy/object/Label.h"
#include "task/mainmenu/MainMenu.h"
#include "game/backend/State.h"

namespace task {
namespace mainmenu {

Host::Host( MainMenu* mainmenu )
	: ConnectionPopupMenu( mainmenu, "HOST MULTIPLAYER GAME" ) {
	SetWidth( 500 );
	SetHeight( 148 );
	SetFlags(
		{
			PF_HAS_OK,
			PF_HAS_CANCEL
		}
	);
}

Host::~Host() {
}

void Host::Show() {
	PopupMenu::Show();

	NEW( m_section, ui_legacy::object::Section, "PopupSection" );
	m_section->SetMargin( 4 );
	m_body->AddChild( m_section );

	NEW( m_label_yourname, ui_legacy::object::Label, "PopupLabel" );
	m_label_yourname->SetAlign( ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_TOP );
	m_label_yourname->SetLeft( 12 );
	m_label_yourname->SetTop( 5 );
	m_label_yourname->SetText( "Enter your name:" );
	m_section->AddChild( m_label_yourname );

	NEW( m_input_yourname, ui_legacy::object::Input, "PopupInput" );
	m_input_yourname->SetAlign( ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_TOP );
	m_input_yourname->SetLeft( 144 );
	m_input_yourname->SetTop( 5 );
	m_input_yourname->SetRight( 12 );
	m_input_yourname->SetValue( m_mainmenu->m_state->m_settings.local.account.GetLastPlayerName() );
	m_input_yourname->SetMaxLength( 20 ); // TODO: determine by rendered width
	m_section->AddChild( m_input_yourname );

	NEW( m_label_gamename, ui_legacy::object::Label, "PopupLabel" );
	m_label_gamename->SetAlign( ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_TOP );
	m_label_gamename->SetLeft( 12 );
	m_label_gamename->SetTop( 34 );
	m_label_gamename->SetText( "Enter game name:" );
	m_section->AddChild( m_label_gamename );

	NEW( m_input_gamename, ui_legacy::object::Input, "PopupInput" );
	m_input_gamename->SetAlign( ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_TOP );
	m_input_gamename->SetLeft( 144 );
	m_input_gamename->SetTop( 34 );
	m_input_gamename->SetRight( 12 );
	m_input_gamename->SetValue( m_mainmenu->m_state->m_settings.local.account.GetLastGameName() );
	m_input_gamename->SetMaxLength( 20 ); // TODO: determine by rendered width
	m_section->AddChild( m_input_gamename );

}

void Host::Hide() {
	m_section->RemoveChild( m_label_yourname );
	m_section->RemoveChild( m_input_yourname );
	m_section->RemoveChild( m_label_gamename );
	m_section->RemoveChild( m_input_gamename );
	m_body->RemoveChild( m_section );

	PopupMenu::Hide();
}

const std::string Host::GetChoice() const {
	return m_input_yourname->GetValue() + ";" + m_input_gamename->GetValue(); // HACK
}

void Host::SetChoice( const std::string& choice ) {
	// HACK
	size_t pos = choice.find( ";" );
	ASSERT( pos >= 0, "; not found in choice" );
	m_input_yourname->SetValue( choice.substr( 0, pos ) );
	m_input_gamename->SetValue( choice.substr( pos + 1 ) );
}

void Host::OnNext() {

	m_mainmenu->m_state->m_settings.local.player_name = m_input_yourname->GetValue();
	m_mainmenu->m_state->m_settings.global.game_name = m_input_gamename->GetValue();

	if ( m_mainmenu->m_state->m_settings.local.player_name.empty() ) {
		MenuError( "Please enter your name." );
	}
	else if ( m_mainmenu->m_state->m_settings.global.game_name.empty() ) {
		MenuError( "Player enter game name." );
	}
	else {

		m_mainmenu->m_state->m_settings.local.account.SetLastPlayerName( m_mainmenu->m_state->m_settings.local.player_name );
		m_mainmenu->m_state->m_settings.local.account.SetLastGameName( m_mainmenu->m_state->m_settings.global.game_name );

		m_mainmenu->m_state->m_settings.local.banned_addresses.clear(); // TODO: persist?

		Hide();
		NEWV( connection, game::backend::connection::Server, &m_mainmenu->m_state->m_settings.local );
		SetConnection( connection );
	}

}

}
}
