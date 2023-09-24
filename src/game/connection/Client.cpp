#include "Client.h"

using namespace network;

namespace game {
namespace connection {

Client::Client( LocalSettings* const settings )
	: Connection( CM_CLIENT, settings ) {
	//
}

void Client::ProcessEvent( const Event& event ) {
	Connection::ProcessEvent( event );

	ASSERT( !event.cid, "client connection received event with cid" );

	switch ( event.type ) {
		case Event::ET_PACKET: {
			try {
				if ( !event.data.packet_data.empty() ) {
					Packet packet;
					packet.Unserialize( Buffer( event.data.packet_data ) );
					switch ( packet.type ) {
						case Packet::PT_REQUEST_AUTH: {
							Log( "Authenticating" );
							Packet p;
							p.type = Packet::PT_AUTH;
							p.data.str = m_settings->player_name;
							m_network->MT_SendPacket( p );
							break;
						}
						case Packet::PT_GLOBAL_SETTINGS: {
							Log( "Got global settings update from server" );
							m_state->m_settings.global.Unserialize( Buffer( packet.data.str ) );
							if ( m_on_global_settings_update ) {
								m_on_global_settings_update();
							}
							break;
						}
						case Packet::PT_PLAYERS: {
							Log( "Got players list from server" );
							m_slot = packet.data.num;
							m_state->m_slots.Unserialize( packet.data.str );
							for ( auto i = 0 ; i < m_state->m_slots.GetCount() ; i++ ) {
								const auto& slot = m_state->m_slots.GetSlot( i );
								if ( slot.GetState() == Slot::SS_PLAYER ) {
									const auto& player = slot.GetPlayer();
									m_state->AddPlayer( player );
									if ( i == m_slot ) {
										m_player = player;
									}
								}
							}
							if ( m_on_players_list_update ) {
								m_on_players_list_update();
							}
							break;
						}
						case Packet::PT_SLOT_UPDATE: {
							const size_t slot_num = packet.data.num;
							Log( "Got slot update from server (slot: " + std::to_string( slot_num ) + ")" );
							if ( slot_num >= m_state->m_slots.GetSlots().size() ) {
								Error( "slot index mismatch" );
								return;
							}
							auto& slot = m_state->m_slots.GetSlot( slot_num );
							slot.Unserialize( packet.data.str );
							if ( m_on_slot_update ) {
								m_on_slot_update( slot_num, &slot );
							}
							break;
						}
						case Packet::PT_KICK: {
							Disconnect( packet.data.str );
							break;
						}
						case Packet::PT_MESSAGE: {
							Log( "Got chat message: " + packet.data.str );
							if ( m_on_message ) {
								m_on_message( packet.data.str );
							}
							break;
						}
						default: {
							Log( "WARNING: invalid packet type from server: " + std::to_string( packet.type ) );
						}
					}
				}
			}
			catch ( std::runtime_error& err ) {
				Error( err.what() );
			}
			break;
		}
		case Event::ET_ERROR: {
			Error( event.data.packet_data );
			break;
		}
		case Event::ET_DISCONNECT: {
			Disconnect( "Connection to server lost." );
			break;
		}
		default: {
			Log( "WARNING: invalid event type from server: " + std::to_string( event.type ) );
		}
	}
}

void Client::UpdateSlot( const size_t slot_num, const Slot* slot ) {
	Log( "Sending slot update" );
	Packet p;
	p.type = Packet::PT_UPDATE_SLOT;
	// not sending slot num because server knows it anyway
	p.data.str = slot->Serialize().ToString();
	m_network->MT_SendPacket( p );
}

void Client::Message( const std::string& message ) {
	Log( "Sending chat message: " + message );
	Packet p;
	p.type = Packet::PT_MESSAGE;
	p.data.str = message;
	m_network->MT_SendPacket( p );
}

void Client::Error( const std::string& reason ) {
	Log( "Network protocol error: " + reason );
	Disconnect( "Network protocol error" );
}

}
}
