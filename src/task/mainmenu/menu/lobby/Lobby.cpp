#include "Lobby.h"

#include "engine/Engine.h"
#include "game/backend/connection/Server.h"
#include "task/mainmenu/MainMenu.h"
#include "game/backend/State.h"
#include "game/backend/Player.h"
#include "game/backend/slot/Slots.h"
#include "GameSettingsSection.h"
#include "PlayersSection.h"
#include "ui_legacy/object/Button.h"
#include "ui_legacy/object/Section.h"
#include "ChatSection.h"
#include "game/backend/connection/Client.h"

namespace task {
namespace mainmenu {
namespace lobby {

Lobby::Lobby( MainMenu* mainmenu, ::game::backend::connection::Connection* connection )
	: PopupMenu( mainmenu, "MULTIPLAYER SETUP" )
	, m_state( mainmenu->m_state ) {
	ASSERT( connection, "connection is null" );

	m_state->InitBindings();

	m_state->Configure();

	SetWidth( 800 );
	SetHeight( 600 );

	connection->ResetHandlers();
	connection->m_on_error = [ this ]( const std::string& message ) -> bool {
		MenuError( message );
		return false; // rly?
	};
	connection->m_on_disconnect = [ this ]() -> bool {
		GoBack();
		return false; // rly?
	};
	connection->m_on_global_settings_update = [ this ]() -> void {
		m_game_settings_section->UpdateRows();
		m_players_section->UpdateSlots( m_state->m_slots->GetSlots() );
	};
	/*connection->m_on_player_join = [ this ]( const size_t slot_num, game::backend::slot::Slot* slot, const game::backend::Player* player ) -> void {
		m_players_section->UpdateSlot( slot_num, slot );
		GlobalMessage( "Player \"" + player->GetPlayerName() + "\" joined." );
	};
	connection->m_on_player_leave = [ this ]( const size_t slot_num, game::backend::slot::Slot* slot, const game::backend::Player* player ) -> void {
		m_players_section->UpdateSlot( slot_num, slot );
		GlobalMessage( "Player \"" + player->GetPlayerName() + "\" left." );
	};*/
	connection->m_on_slot_update = [ this ]( const size_t slot_num, game::backend::slot::Slot* slot ) -> void {
		m_players_section->UpdateSlot( slot_num, slot );
	};
	connection->m_on_flags_update = [ this ]( const size_t slot_num, game::backend::slot::Slot* slot, const game::backend::slot::player_flag_t old_flags, const game::backend::slot::player_flag_t new_flags ) -> void {
		if ( slot_num == m_connection->GetSlotNum() ) {

			const bool is_ready = slot->HasPlayerFlag( ::game::backend::slot::PF_READY );

			// update 'ready' button to match
			if ( is_ready ) {
				m_ready_button->AddStyleModifier( ui_legacy::M_SELECTED );
			}
			else if ( !is_ready ) {
				m_ready_button->RemoveStyleModifier( ui_legacy::M_SELECTED );
			}

			if ( slot->GetPlayer()->GetRole() == ::game::backend::Player::PR_HOST ) {
				// lock/unlock game settings
				if ( is_ready ) {
					m_game_settings_section->Lock();
				}
				else {
					m_game_settings_section->Unlock();
				}
				// lock/unlock non-player slots
				auto& slots = m_state->m_slots->GetSlots();
				for ( size_t num = 0 ; num < slots.size() ; num++ ) {
					auto& s = slots.at( num );
					if ( s.GetState() != ::game::backend::slot::Slot::SS_PLAYER ) {
						m_players_section->UpdateSlot( num, &s );
					}
				}
			}
		}
		m_players_section->UpdateSlot( slot_num, slot );
		ManageCountdown();
	};
	connection->m_on_message = [ this ]( const std::string& message ) -> void {
		m_chat_section->AddMessage( message );
	};

	connection->IfServer(
		[ this ]( game::backend::connection::Server* connection ) -> void {
			connection->m_on_listen = [ this ]() -> void {
				size_t slots_i = 0;
				for ( ::game::backend::slot::Slot& slot : m_state->m_slots->GetSlots() ) {
					m_players_section->UpdateSlot( slots_i++, &slot );
				}
			};
			connection->SetGameState( game::backend::connection::Connection::GS_LOBBY );
		}
	);

	connection->IfClient(
		[ this ]( game::backend::connection::Client* connection ) -> void {
			connection->m_on_players_list_update = [ this ]() -> void {
				size_t slots_i = 0;
				for ( auto& slot : m_state->m_slots->GetSlots() ) {
					m_players_section->UpdateSlot( slots_i++, &slot );
				}
			};
			connection->m_on_game_state_change = [ this, connection ]( const game::backend::connection::Connection::game_state_t state ) -> void {
				if ( state == game::backend::connection::Connection::GS_INITIALIZING ) {
					// server tells us to change state

					// cleanup for next state
					connection->ResetHandlers();

					// detach state because game thread will own it now
					m_state = nullptr;

					// switch to game task
					m_mainmenu->StartGame();
					GoBack(); // show previous menu if canceled
				}
				else {
					THROW( "unexpected game state" + std::to_string( state ) );
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

	NEW( m_ready_button, ui_legacy::object::Button, "PopupButtonOkCancel" ); // TODO: correct style
	m_ready_button->SetAlign( ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_BOTTOM );
	m_ready_button->SetLeft( 8 );
	m_ready_button->SetBottom( 4 );
	m_ready_button->SetWidth( 234 );
	m_ready_button->SetHeight( 22 );
	m_ready_button->SetLabel( "READY" );
	m_ready_button->On(
		ui_legacy::event::EV_BUTTON_CLICK, EH( this ) {
			ASSERT( m_connection, "connection not set" );
			auto* slot = GetPlayer()->GetSlot();
			if ( !slot->HasPlayerFlag( ::game::backend::slot::PF_READY ) ) {
				slot->SetPlayerFlag( ::game::backend::slot::PF_READY );
			}
			else {
				slot->UnsetPlayerFlag( ::game::backend::slot::PF_READY );
			}
			UpdateSlot( m_connection->GetSlotNum(), slot, true );
			return true;
		}
	);
	m_players_section->AddChild( m_ready_button );

	NEW( m_cancel_button, ui_legacy::object::Button, "PopupButtonOkCancel" ); // TODO: correct style
	m_cancel_button->SetAlign( ui_legacy::ALIGN_RIGHT | ui_legacy::ALIGN_BOTTOM );
	m_cancel_button->SetRight( 8 );
	m_cancel_button->SetBottom( 4 );
	m_cancel_button->SetWidth( 234 );
	m_cancel_button->SetHeight( 22 );
	m_cancel_button->SetLabel( "CANCEL" );
	m_cancel_button->On(
		ui_legacy::event::EV_BUTTON_CLICK, EH( this ) {
			m_connection->Disconnect();
			return true;
		}
	);
	m_players_section->AddChild( m_cancel_button );

	NEW( m_chat_section, ChatSection, this );
	m_body->AddChild( m_chat_section );

	NEW( m_game_options_section, ui_legacy::object::Section, "PopupSection" );
	m_game_options_section->SetTitleText( "CUSTOM GAME OPTIONS" );
	m_game_options_section->SetAlign( ui_legacy::ALIGN_BOTTOM );
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

			// detach state because game thread will own it now
			m_state = nullptr;

			// switch to game task
			m_mainmenu->StartGame();
			GoBack(); // show previous menu if canceled

		}
	}
}

game::backend::settings::Settings* Lobby::GetSettings() {
	return &m_state->m_settings;
}

const ::game::backend::Player* Lobby::GetPlayer() {
	const auto* connection = GetConnection();
	ASSERT( connection, "connection is null" );
	return connection->GetPlayer();
}

::game::backend::State* Lobby::GetState() const {
	return m_state;
}

void Lobby::Message( const std::string& message ) {
	Log( "Sending message: " + message );
	m_connection->SendMessage( message );
}

void Lobby::UpdateSlot( const size_t slot_num, ::game::backend::slot::Slot* slot, const bool only_flags ) {
	if ( only_flags ) {
		Log( "Updating slot " + slot->GetName() );
	}
	else {
		Log( "Updating flags for slot " + slot->GetName() );
	}
	m_players_section->UpdateSlot( slot_num, slot );
	m_connection->UpdateSlot( slot_num, slot, only_flags );
	ManageCountdown();
}

void Lobby::KickFromSlot( const size_t slot_num ) {
	if ( m_connection->IsServer() ) {
		Log( "Kicking from slot " + std::to_string( slot_num ) );
		( (game::backend::connection::Server*)m_connection )->KickFromSlot( slot_num );
	}
}

void Lobby::BanFromSlot( const size_t slot_num ) {
	if ( m_connection->IsServer() ) {
		Log( "Banning from slot " + std::to_string( slot_num ) );
		( (game::backend::connection::Server*)m_connection )->BanFromSlot( slot_num );
	}
}

void Lobby::UpdateGameSettings() {
	if ( m_connection->IsServer() ) {
		Log( "Updating game settings" );
		( (game::backend::connection::Server*)m_connection )->UpdateGameSettings();
	}
}

void Lobby::GlobalMessage( const std::string& message ) {
	if ( m_connection->IsServer() ) {
		Log( "Sending global message: " + message );
		( (game::backend::connection::Server*)m_connection )->GlobalMessage( message );
	}
}

const game::backend::connection::Connection* Lobby::GetConnection() const {
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
		for ( const auto& slot : m_state->m_slots->GetSlots() ) {
			if ( slot.GetState() == game::backend::slot::Slot::SS_PLAYER && !slot.HasPlayerFlag( ::game::backend::slot::PF_READY ) ) {
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
