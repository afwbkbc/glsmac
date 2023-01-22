#include "Lobby.h"

namespace game {
namespace mainmenu {

Lobby::Lobby( MainMenu* mainmenu ) : PopupMenu( mainmenu, "MULTIPLAYER SETUP" ) {
	SetWidth( 800 );
	SetHeight( 600 );
	
}

void Lobby::Show() {
	PopupMenu::Show();
	
	NEW( m_map_settings_section, Section, "PopupMenuSection" );
		m_map_settings_section->SetTitleText( "GAME TITLE" );
		m_map_settings_section->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_map_settings_section->SetWidth( 310 );
		m_map_settings_section->SetHeight( 364 );
	m_body->AddChild( m_map_settings_section );

	NEW( m_players_section, Section, "PopupMenuSection" );
		m_players_section->SetTitleText( "PLAYERS" );
		m_players_section->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
		m_players_section->SetWidth( 488 );
		m_players_section->SetHeight( 204 );
	m_body->AddChild( m_players_section );
	
	NEW( m_chat_section, Section, "PopupMenuSection" );
		m_chat_section->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
		m_chat_section->SetTop( 200 );
		m_chat_section->SetWidth( 488 );
		m_chat_section->SetHeight( 164 );
	m_body->AddChild( m_chat_section );

	NEW( m_game_settings_section, Section, "PopupMenuSection" );
		m_game_settings_section->SetTitleText( "CUSTOM GAME OPTIONS" );
		m_game_settings_section->SetAlign( UIObject::ALIGN_BOTTOM );
		m_game_settings_section->SetHeight( 210 );
	m_body->AddChild( m_game_settings_section );
}

void Lobby::Hide() {
	m_body->RemoveChild( m_map_settings_section );
	m_body->RemoveChild( m_players_section );
	m_body->RemoveChild( m_chat_section );
	m_body->RemoveChild( m_game_settings_section );
	
	PopupMenu::Hide();
}
}
}
