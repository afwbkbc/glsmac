#include "Lobby.h"

#include "engine/Engine.h"
#include "game/connection/Server.h"

using namespace types;
using namespace network;

namespace task {
namespace mainmenu {
namespace lobby {

Lobby::Lobby( MainMenu* mainmenu, Connection* connection )
	: PopupMenu( mainmenu, "MULTIPLAYER SETUP" )
	, m_connection( connection )
	, m_state( mainmenu->m_settings )
{
	ASSERT( m_connection, "connection is null" );

	SetWidth( 800 );
	SetHeight( 600 );
	
	m_state.m_settings.global.game_rules.Initialize();

	m_connection->SetState( &m_state );
	m_connection->m_on_error = [ this ] ( const std::string& message ) -> void {
		MenuError( message );
	};
	m_connection->m_on_disconnect = [ this ] () -> void {
		GoBack();
	};

	// TODO: partial updates
	m_connection->m_on_global_settings_update = [ this ] () -> void {
		RefreshUI();
	};
	m_connection->m_on_players_list_update = [ this ] () -> void {
		RefreshUI();
	};
	m_connection->m_on_player_join = [ this ] ( game::Player* player ) -> void {
		RefreshUI();
	};
	m_connection->m_on_player_leave = [ this ] ( game::Player* player ) -> void {
		RefreshUI();
	};
}

Lobby::~Lobby() {
	DELETE( m_connection );
}

void Lobby::Show() {
	PopupMenu::Show();
	
	NEW( m_map_settings_section, Section, "PopupSection" );
		m_map_settings_section->SetTitleText( " " ); // to have header created
		m_map_settings_section->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_map_settings_section->SetWidth( 302 );
		m_map_settings_section->SetHeight( 364 );
	m_body->AddChild( m_map_settings_section );

	NEW( m_players_section, PlayersSection, this );
		m_players_section->SetTitleText( "PLAYERS" );
		m_players_section->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
		m_players_section->SetWidth( 496 );
		m_players_section->SetHeight( 204 );
	m_body->AddChild( m_players_section );
	
	NEW( m_launch_button, Button, "PopupButtonOkCancel" ); // TODO: correct style
		m_launch_button->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_BOTTOM );
		m_launch_button->SetLeft( 8 );
		m_launch_button->SetBottom( 4 );
		m_launch_button->SetWidth( 234 );
		m_launch_button->SetHeight( 22 );
		m_launch_button->SetLabel( "LAUNCH GAME" );
	m_players_section->AddChild( m_launch_button );
	
	NEW( m_cancel_button, Button, "PopupButtonOkCancel" ); // TODO: correct style
		m_cancel_button->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
		m_cancel_button->SetRight( 8 );
		m_cancel_button->SetBottom( 4 );
		m_cancel_button->SetWidth( 234 );
		m_cancel_button->SetHeight( 22 );
		m_cancel_button->SetLabel( "CANCEL" );
		m_cancel_button->On( UIEvent::EV_BUTTON_CLICK, EH( this ) {
			m_connection->Disconnect();
			return true;
		});
	m_players_section->AddChild( m_cancel_button );
	
	NEW( m_chat_section, Section, "PopupSection" );
		m_chat_section->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
		m_chat_section->SetTop( 200 );
		m_chat_section->SetWidth( 496 );
		m_chat_section->SetHeight( 164 );
	m_body->AddChild( m_chat_section );

	NEW( m_game_settings_section, Section, "PopupSection" );
		m_game_settings_section->SetTitleText( "CUSTOM GAME OPTIONS" );
		m_game_settings_section->SetAlign( UIObject::ALIGN_BOTTOM );
		m_game_settings_section->SetHeight( 210 );
	m_body->AddChild( m_game_settings_section );

	RefreshUI();
}

void Lobby::Hide() {
	
	m_body->RemoveChild( m_map_settings_section );
		m_players_section->RemoveChild( m_launch_button );
		m_players_section->RemoveChild( m_cancel_button );
	m_body->RemoveChild( m_players_section );
	m_body->RemoveChild( m_chat_section );
	m_body->RemoveChild( m_game_settings_section );
	
	PopupMenu::Hide();
}

void Lobby::Iterate() {
	PopupMenu::Iterate();

	m_connection->Iterate();
}

::game::Settings& Lobby::GetSettings() {
	return m_state.m_settings;
}

void Lobby::UpdatePlayer( const size_t cid, const ::game::Player& player ) {
	Log( "Updating player (cid " + std::to_string( cid ) + " )" );
	/*ASSERT( m_players.find( cid ) != m_players.end(), "player to update not found" );
	m_players[ cid ] = player;
	m_players_section->UpdatePlayer( cid, player ); // TODO: broadcast*/ // TODO
}

bool Lobby::OnCancel() {
	m_connection->Disconnect();
	return true;
}

void Lobby::RefreshUI() {
	if ( !IsShown() ) {
		return;
	}

	m_map_settings_section->SetTitleText( m_state.m_settings.global.game_name );
	
	// m_players_section->SetPlayers( m_players ); // TODO
	
	if ( m_state.m_settings.local.network_role == ::game::LocalSettings::NR_SERVER ) {
		// update UIs of others aswell
		// TODO: optimize?
		
	}

}

}
}
}
