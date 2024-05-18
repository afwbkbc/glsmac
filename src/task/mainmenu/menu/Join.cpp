#include "Join.h"

#include "engine/Engine.h"

#include "game/connection/Client.h"
#include "ui/object/Section.h"
#include "ui/object/Input.h"
#include "ui/object/Label.h"
#include "task/mainmenu/MainMenu.h"
#include "game/State.h"

namespace task {
namespace mainmenu {

Join::Join( MainMenu* mainmenu )
	: ConnectionPopupMenu( mainmenu, "JOIN MULTIPLAYER GAME" ) {
	SetWidth( 500 );
	SetHeight( 148 );
	SetFlags(
		{
			PF_HAS_OK,
			PF_HAS_CANCEL
		}
	);
}

Join::~Join() {
}

void Join::Show() {
	PopupMenu::Show();

	NEW( m_section, ui::object::Section, "PopupSection" );
	m_section->SetMargin( 4 );
	m_body->AddChild( m_section );

	NEW( m_label_yourname, ui::object::Label, "PopupLabel" );
	m_label_yourname->SetAlign( ui::ALIGN_LEFT | ui::ALIGN_TOP );
	m_label_yourname->SetLeft( 12 );
	m_label_yourname->SetTop( 5 );
	m_label_yourname->SetText( "Enter your name:" );
	m_section->AddChild( m_label_yourname );

	NEW( m_input_yourname, ui::object::Input, "PopupInput" );
	m_input_yourname->SetAlign( ui::ALIGN_LEFT | ui::ALIGN_TOP );
	m_input_yourname->SetLeft( 144 );
	m_input_yourname->SetTop( 5 );
	m_input_yourname->SetRight( 12 );
	m_input_yourname->SetValue( m_mainmenu->m_state->m_settings.local.account.GetLastPlayerName() );
	m_input_yourname->SetMaxLength( 20 ); // TODO: determine by rendered width
	m_section->AddChild( m_input_yourname );

	NEW( m_label_gameip, ui::object::Label, "PopupLabel" );
	m_label_gameip->SetAlign( ui::ALIGN_LEFT | ui::ALIGN_TOP );
	m_label_gameip->SetLeft( 12 );
	m_label_gameip->SetTop( 34 );
	m_label_gameip->SetText( "Enter game IP:" );
	m_section->AddChild( m_label_gameip );

	NEW( m_input_gameip, ui::object::Input, "PopupInput" );
	m_input_gameip->SetAlign( ui::ALIGN_LEFT | ui::ALIGN_TOP );
	m_input_gameip->SetLeft( 144 );
	m_input_gameip->SetTop( 34 );
	m_input_gameip->SetRight( 12 );
	m_input_gameip->SetValue( m_mainmenu->m_state->m_settings.local.account.GetLastRemoteAddress() );
	m_input_gameip->SetMaxLength( 20 ); // TODO: determine by rendered width
	m_section->AddChild( m_input_gameip );

}

void Join::Hide() {
	m_section->RemoveChild( m_label_yourname );
	m_section->RemoveChild( m_input_yourname );
	m_section->RemoveChild( m_label_gameip );
	m_section->RemoveChild( m_input_gameip );
	m_body->RemoveChild( m_section );

	PopupMenu::Hide();
}

const std::string Join::GetChoice() const {
	return m_input_yourname->GetValue() + ";" + m_input_gameip->GetValue(); // HACK
}

void Join::SetChoice( const std::string& choice ) {
	// HACK
	size_t pos = choice.find( ";" );
	ASSERT( pos >= 0, "; not found in choice" );
	m_input_yourname->SetValue( choice.substr( 0, pos ) );
	m_input_gameip->SetValue( choice.substr( pos + 1 ) );
}

void Join::OnNext() {

	m_mainmenu->m_state->m_settings.local.player_name = m_input_yourname->GetValue();
	m_mainmenu->m_state->m_settings.local.remote_address = m_input_gameip->GetValue();

	if ( m_mainmenu->m_state->m_settings.local.player_name.empty() ) {
		MenuError( "Please enter your name." );
	}
	else if ( m_mainmenu->m_state->m_settings.local.remote_address.empty() ) {
		MenuError( "Player enter game IP." );
	}
	else {

		m_mainmenu->m_state->m_settings.local.account.SetLastPlayerName( m_mainmenu->m_state->m_settings.local.player_name );
		m_mainmenu->m_state->m_settings.local.account.SetLastRemoteAddress( m_mainmenu->m_state->m_settings.local.remote_address );

		Hide();
		NEWV( connection, ::game::connection::Client, &m_mainmenu->m_state->m_settings.local );
		SetConnection( connection );
	}

}

}
}
