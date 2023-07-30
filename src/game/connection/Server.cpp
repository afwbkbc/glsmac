#include "Server.h"

using namespace network;

namespace game {
namespace connection {

Server::Server( LocalSettings* const settings )
	: Connection( network::CM_SERVER, settings )
{
	//
}

void Server::ProcessEvent( const network::Event& event ) {
	Connection::ProcessEvent( event );

	ASSERT( event.cid || event.type == Event::ET_LISTEN, "server connection received event without cid" );

	switch ( event.type ) {
		case Event::ET_LISTEN: {
			ASSERT( !m_player, "player already set" );
			Log( "Listening" );
			m_state->m_slots.Resize( 3 ); // TODO: make dynamic
			m_player = new ::game::Player{
				m_state->m_settings.local.player_name,
				::game::Player::PR_HOST,
				m_state->m_settings.global.game_rules.m_factions[ 0 ]
			};
			m_state->AddPlayer( m_player );
			m_slot = 0; // host always has slot 0
			m_state->AddCIDSlot( 0, m_slot );
			m_state->m_slots.GetSlot( m_slot ).SetPlayer( m_player );
			if ( m_on_player_join ) {
				m_on_player_join( m_player );
			}
			break;
		}
		case Event::ET_CLIENT_CONNECT: {
			Log( std::to_string( event.cid ) + " connected" );
			ASSERT( m_state->GetCidSlots().find( event.cid ) == m_state->GetCidSlots().end(), "player cid already in slots" );
			//m_players[ event.cid ] = {}; // to be queried* / // TODO
			{
				Packet packet;
				packet.type = Packet::PT_REQUEST_AUTH; // ask to authenticate
				m_network->MT_SendPacket( packet, event.cid );
			}
			break;
		}
		case Event::ET_CLIENT_DISCONNECT: {
			Log( std::to_string( event.cid ) + " disconnected" );
			auto it = m_state->GetCidSlots().find( event.cid );
			if ( it != m_state->GetCidSlots().end() ) {
				m_state->RemoveCIDSlot( event.cid );
				auto* player = m_state->m_slots.GetSlot( it->second ).GetPlayerAndClose();
				ASSERT( player, "player in slot is null" );
				m_state->RemovePlayer( player );
				if ( m_on_player_leave ) {
					m_on_player_leave( player );
				}
			}
			break;
		}
		case Event::ET_PACKET: {
			Packet packet;
			packet.Unserialize( Buffer( event.data.packet_data ) );
			switch ( packet.type ) {
				case Packet::PT_AUTH: {
					if ( packet.data.str.empty() ) {
						Log( "Authentication from " + std::to_string( event.cid ) + " failed, disconnecting" );
						m_network->MT_DisconnectClient( event.cid );
						break;
					}

					Log( "Got authentication from " + std::to_string( event.cid ) + ": " + packet.data.str );

					if ( m_state->GetCidSlots().find( event.cid ) != m_state->GetCidSlots().end() ) {
						Log( "Duplicate uthentication from " + std::to_string( event.cid ) + ", disconnecting" );
						m_network->MT_DisconnectClient( event.cid );
						break;
					}

					// find free slot
					size_t slot_num = 0; // 0 = 'not found'
					for ( auto& slot : m_state->m_slots.GetSlots() ) {
						if ( slot.GetState() == ::game::Slot::SS_OPEN ) {
							break;
						}
						slot_num++;
					}
					if ( slot_num >= m_state->m_slots.GetCount() ) { // no available slots left
						Log( "No free slots for player " + std::to_string( event.cid ) + " (" + packet.data.str + "), dropping" );
						Kick( event.cid, "Server is full!" );
						break;
					}

					auto* player = new ::game::Player{
						packet.data.str,
						::game::Player::PR_PLAYER,
						m_state->m_settings.global.game_rules.m_factions[ 0 ]
					};
					m_state->AddPlayer( player );

					m_state->AddCIDSlot( event.cid, slot_num );
					m_state->m_slots.GetSlot( slot_num ).SetPlayer( player );

					{
						Log( "Sending global settings to " + std::to_string( event.cid ) );
						Packet p;
						p.type = Packet::PT_GLOBAL_SETTINGS;
						p.data.str = m_state->m_settings.global.Serialize().ToString();
						m_network->MT_SendPacket( p, event.cid );
					}
					{
						Log( "Sending players list to " + std::to_string( event.cid ) );
						Packet p;
						p.type = Packet::PT_PLAYERS;
						p.data.num = slot_num;
						p.data.str = m_state->m_slots.Serialize().ToString();
						g_engine->GetNetwork()->MT_SendPacket( p, event.cid );
					}

					if ( m_on_player_join ) {
						m_on_player_join( player );
					}

					break;
				}
				default: {
					Log( "WARNING: invalid packet type from client " + std::to_string( event.cid ) + " : " + std::to_string( packet.type ) );
				}
			}
			break;
		}
		default: {
			Log( "WARNING: invalid event type from client " + std::to_string( event.cid ) + " : " + std::to_string( event.type ) );
		}
	}

}

void Server::Kick( const size_t cid, const std::string& reason ) {
	Packet p;
	p.type = types::Packet::PT_KICK;
	p.data.str = "Server is full!";
	m_network->MT_SendPacket( p, cid );
	m_network->MT_DisconnectClient( cid );
}

}
}
