#include "Lobby.h"

#include "engine/Engine.h"
#include "game/connection/Server.h"

using namespace types;
using namespace network;

using namespace ::game::connection;

namespace task {
namespace mainmenu {
namespace lobby {

Lobby::Lobby( MainMenu* mainmenu, Connection* connection )
	: PopupMenu( mainmenu, "MULTIPLAYER SETUP" )
	, m_state( mainmenu->m_state ) {
	ASSERT( connection, "connection is null" );

	SetWidth( 800 );
	SetHeight( 600 );

	connection->ResetHandlers();
	connection->m_on_error = [ this ]( const std::string& message ) -> void {
		MenuError( message );
	};
	connection->m_on_disconnect = [ this ]() -> void {
		GoBack();
	};
	connection->m_on_global_settings_update = [ this ]() -> void {
		m_game_settings_section->UpdateRows();
		m_players_section->UpdateSlots( m_state->m_slots.GetSlots() );
	};
	connection->m_on_player_join = [ this ]( const size_t slot_num, game::Slot* slot, const game::Player* player ) -> void {
		m_players_section->UpdateSlot( slot_num, slot );
		GlobalMessage( "Player \"" + player->GetPlayerName() + "\" joined." );
	};
	connection->m_on_player_leave = [ this ]( const size_t slot_num, game::Slot* slot, const game::Player* player ) -> void {
		m_players_section->UpdateSlot( slot_num, slot );
		GlobalMessage( "Player \"" + player->GetPlayerName() + "\" left." );
	};
	connection->m_on_slot_update = [ this ]( const size_t slot_num, game::Slot* slot ) -> void {
		m_players_section->UpdateSlot( slot_num, slot );
		ManageCountdown();
	};
	connection->m_on_message = [ this ]( const std::string& message ) -> void {
		m_chat_section->AddMessage( message );
	};

	connection->IfServer(
		[ this ]( Server* connection ) -> void {
			connection->m_on_listen = [ this ]() -> void {
				size_t slots_i = 0;
				for ( ::game::Slot& slot : m_state->m_slots.GetSlots() ) {
					m_players_section->UpdateSlot( slots_i++, &slot );
				}
			};
			connection->SetGameState( Connection::GS_LOBBY );
		}
	);

	connection->IfClient(
		[ this ]( Client* connection ) -> void {
			connection->m_on_players_list_update = [ this ]() -> void {
				size_t slots_i = 0;
				for ( auto& slot : m_state->m_slots.GetSlots() ) {
					m_players_section->UpdateSlot( slots_i++, &slot );
				}
			};
			connection->m_on_game_state_change = [ this, connection ]( const Connection::game_state_t state ) -> void {
				if ( state == Connection::GS_INITIALIZING ) {
					// server tells us to change state

					// cleanup for next state
					connection->ResetHandlers();

					// detach state because game thread will own it now
					m_state = nullptr;

					// switch to game task
					m_mainmenu->StartGame();
					GoBack();
				}
				else {
					ASSERT( false, "unexpected game state" );
				}
			};
		}
	);
	m_connection = connection; // shortcut
}

Lobby::~Lobby() {
}

void Lobby::Show() {
	PopupMenu::Show();

	NEW( m_game_settings_section, GameSettingsSection, this, &m_state->m_settings.global );
	m_body->AddChild( m_game_settings_section );

	NEW( m_players_section, PlayersSection, this );
	m_body->AddChild( m_players_section );

	NEW( m_ready_button, Button, "PopupButtonOkCancel" ); // TODO: correct style
	m_ready_button->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_BOTTOM );
	m_ready_button->SetLeft( 8 );
	m_ready_button->SetBottom( 4 );
	m_ready_button->SetWidth( 234 );
	m_ready_button->SetHeight( 22 );
	m_ready_button->SetLabel( "READY" );
	m_ready_button->On(
		UIEvent::EV_BUTTON_CLICK, EH( this ) {
			auto* slot = GetPlayer()->GetSlot();
			if ( !m_ready_button->HasStyleModifier( Style::M_SELECTED ) ) {
				m_ready_button->AddStyleModifier( Style::M_SELECTED );
				slot->SetReady( true );
			}
			else {
				m_ready_button->RemoveStyleModifier( Style::M_SELECTED );
				slot->SetReady( false );
			}
			UpdateSlot( m_state->GetConnection()->GetSlotNum(), slot );
			return true;
		}
	);
	m_players_section->AddChild( m_ready_button );

	NEW( m_cancel_button, Button, "PopupButtonOkCancel" ); // TODO: correct style
	m_cancel_button->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	m_cancel_button->SetRight( 8 );
	m_cancel_button->SetBottom( 4 );
	m_cancel_button->SetWidth( 234 );
	m_cancel_button->SetHeight( 22 );
	m_cancel_button->SetLabel( "CANCEL" );
	m_cancel_button->On(
		UIEvent::EV_BUTTON_CLICK, EH( this ) {
			m_connection->Disconnect();
			return true;
		}
	);
	m_players_section->AddChild( m_cancel_button );

	NEW( m_chat_section, ChatSection, this );
	m_body->AddChild( m_chat_section );

	NEW( m_game_options_section, Section, "PopupSection" );
	m_game_options_section->SetTitleText( "CUSTOM GAME OPTIONS" );
	m_game_options_section->SetAlign( UIObject::ALIGN_BOTTOM );
	m_game_options_section->SetHeight( 210 );
	m_body->AddChild( m_game_options_section );

}

void Lobby::Hide() {

	m_body->RemoveChild( m_game_settings_section );
	m_players_section->RemoveChild( m_ready_button );
	m_players_section->RemoveChild( m_cancel_button );
	m_body->RemoveChild( m_players_section );
	m_body->RemoveChild( m_chat_section );
	m_body->RemoveChild( m_game_options_section );

	if ( m_state ) {
		m_state->Reset();
	}

	PopupMenu::Hide();
}

void Lobby::Iterate() {
	PopupMenu::Iterate();

	if ( m_state ) {
		m_state->Iterate();
	}

	while ( m_countdown_timer.HasTicked() ) {
		m_countdown--;
		if ( m_countdown <= 0 ) {
			m_countdown_timer.Stop();

			Log( "Starting game" );

			// notify clients of initialization
			( (Server*)m_connection )->SetGameState( Server::GS_INITIALIZING );

			// detach state because game thread will own it now
			m_state = nullptr;

			// switch to game task
			m_mainmenu->StartGame();
			GoBack();

		}
	}
}

::game::Settings& Lobby::GetSettings() {
	return m_state->m_settings;
}

const ::game::Player* Lobby::GetPlayer() {
	const auto* connection = GetConnection();
	ASSERT( connection, "connection is null" );
	return connection->GetPlayer();
}

void Lobby::Message( const std::string& message ) {
	Log( "Sending message: " + message );
	m_connection->Message( message );
}

void Lobby::UpdateSlot( const size_t slot_num, ::game::Slot* slot ) {
	Log( "Updating slot " + slot->GetName() );
	m_players_section->UpdateSlot( slot_num, slot );
	m_connection->UpdateSlot( slot_num, slot );
	ManageCountdown();
}

void Lobby::KickFromSlot( const size_t slot_num ) {
	if ( m_connection->IsServer() ) {
		Log( "Kicking from slot " + std::to_string( slot_num ) );
		( (Server*)m_connection )->KickFromSlot( slot_num );
	}
}

void Lobby::BanFromSlot( const size_t slot_num ) {
	if ( m_connection->IsServer() ) {
		Log( "Banning from slot " + std::to_string( slot_num ) );
		( (Server*)m_connection )->BanFromSlot( slot_num );
	}
}

void Lobby::UpdateGameSettings() {
	if ( m_connection->IsServer() ) {
		Log( "Updating game settings" );
		( (Server*)m_connection )->UpdateGameSettings();
	}
}

void Lobby::GlobalMessage( const std::string& message ) {
	if ( m_connection->IsServer() ) {
		Log( "Sending global message: " + message );
		( (Server*)m_connection )->GlobalMessage( message );
	}
}

const Connection* Lobby::GetConnection() const {
	return m_connection;
}

bool Lobby::OnCancel() {
	m_connection->Disconnect();
	return true;
}

void Lobby::LockInput() {
	m_frame->Hide();
}

void Lobby::UnlockInput() {
	m_frame->Show();
}

void Lobby::ManageCountdown() {
	if ( m_connection->IsServer() ) {
		bool is_everyone_ready = true;
		for ( const auto& slot : m_state->m_slots.GetSlots() ) {
			if ( !slot.IsReady() ) {
				is_everyone_ready = false;
				break;
			}
		}
		if ( is_everyone_ready && !m_countdown_timer.IsRunning() ) {
			GlobalMessage( "Everyone is ready. Starting game in " + std::to_string( COUNTDOWN_SECONDS ) + " seconds..." );
			m_countdown = COUNTDOWN_SECONDS;
			m_countdown_timer.SetInterval( 1000 );
		}
		else if ( m_countdown_timer.IsRunning() && !is_everyone_ready ) {
			GlobalMessage( "Somebody is not ready. Game start canceled." );
			m_countdown_timer.Stop();
		}
	}
}

}
}
}
