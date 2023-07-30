#include "Client.h"

using namespace network;

namespace game {
namespace connection {

Client::Client( LocalSettings* const settings )
	: Connection( CM_CLIENT, settings )
{
	//
}

void Client::ProcessEvent( const Event& event ) {
	Connection::ProcessEvent( event );

	ASSERT( !event.cid, "client connection received event with cid" );

	switch ( event.type ) {
		case Event::ET_PACKET: {
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
							const auto& player = m_state->m_slots.GetSlot( i ).GetPlayer();
							if ( player ) {
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
					case Packet::PT_KICK: {
						Log( "Kicked by server: " + packet.data.str );
						Disconnect( packet.data.str );
						break;
					}
					default: {
						Log( "WARNING: invalid packet type from server: " + std::to_string( packet.type ) );
					}
				}
			}
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

}
}
