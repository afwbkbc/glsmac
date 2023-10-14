#include "Client.h"

#include <algorithm>

using namespace network;

namespace game {
namespace connection {

Client::Client( LocalSettings* const settings )
	: Connection( CM_CLIENT, settings ) {
	//
}

void Client::ProcessEvent( const Event& event ) {
	Connection::ProcessEvent( event );

	ASSERT( !event.cid, "client connection received event with non-zero cid" );

	switch ( event.type ) {
		case Event::ET_PACKET: {
			try {
				if ( !event.data.packet_data.empty() ) {
					Packet packet( Packet::PT_NONE );
					packet.Unserialize( Buffer( event.data.packet_data ) );
					switch ( packet.type ) {
						case Packet::PT_REQUEST_AUTH: {
							Log( "Authenticating" );
							Packet p( Packet::PT_AUTH );
							p.data.vec = {
								m_settings->account.GetGSID(),
								m_settings->player_name,
							};
							m_network->MT_SendPacket( p );
							break;
						}
						case Packet::PT_GLOBAL_SETTINGS: {
							Log( "Got global settings update from server" );
							const auto& host_slot = m_state->m_slots.GetSlot( 0 );
							if ( host_slot.HasPlayerFlag( Slot::PF_READY ) ) {
								Error( "host updated settings while ready" );
								break;
							}
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
							const bool wasReady = slot.GetState() == Slot::SS_PLAYER
								? slot.HasPlayerFlag( Slot::PF_READY ) // check readyness of player
								: m_state->m_slots.GetSlot( 0 ).HasPlayerFlag( Slot::PF_READY ) // check readyness of host
							;
							slot.Unserialize( packet.data.str );
							if ( slot.GetState() == Slot::SS_PLAYER ) {
								if ( wasReady && slot.HasPlayerFlag( Slot::PF_READY ) ) {
									Error( "player updated slot while ready" );
									break;
								}
							}
							else {
								if ( wasReady ) {
									Error( "host updated slot while ready" );
									break;
								}
							}
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
						case Packet::PT_GAME_STATE: {
							Log( "Got game state: " + std::to_string( packet.data.num ) );
							if ( packet.data.num != m_game_state ) {
								m_game_state = (game_state_t)packet.data.num;
								if ( m_on_game_state_change ) {
									m_on_game_state_change( m_game_state );
								}
							}
							break;
						}
						case Packet::PT_MAP_HEADER: {
							Log( "Got map data header" );
							if ( !m_map_download_state.is_downloading ) {
								Error( "map header received while not downloading" );
							}
							if ( packet.data.num == 0 ) {
								Log( "No map received from server" );
								Disconnect( "No map received from server" );
							}
							else {
								m_map_download_state.total_size = packet.data.num;
								Log( "Allocating map buffer (" + std::to_string( m_map_download_state.total_size ) + " bytes)" );
								m_map_download_state.buffer.reserve( m_map_download_state.total_size );
								RequestNextMapChunk();
							}
							break;
						}
						case Packet::PT_MAP_CHUNK: {
							Log( "Got map chunk ( offset=" + std::to_string( packet.udata.map.offset ) + " size=" + std::to_string( packet.udata.map.size ) + " )" );
							const size_t end = packet.udata.map.offset + packet.udata.map.size;
							if ( !m_map_download_state.is_downloading ) {
								Error( "map chunk received while not downloading" );
							}
							else if ( end > m_map_download_state.total_size ) {
								Error( "map chunk overflow ( " + std::to_string( packet.udata.map.offset ) + " + " + std::to_string( packet.udata.map.size ) + " >= " + std::to_string( m_map_download_state.total_size ) + " )" );
							}
							else if ( packet.udata.map.offset != m_map_download_state.downloaded_size ) {
								Error( "inconsistent map chunk offset ( " + std::to_string( packet.udata.map.offset ) + " != " + std::to_string( m_map_download_state.downloaded_size ) + " )" );
							}
							else if (
								packet.udata.map.size != MAP_DOWNLOAD_CHUNK_SIZE &&
									end != m_map_download_state.total_size // last chunk can be smaller
								) {
								Error( "inconsistent map chunk size ( " );
							}
							else {
								ASSERT( packet.data.str.size() == packet.udata.map.size, "map download buffer size mismatch" );
								m_map_download_state.buffer.append( packet.data.str );
								if ( end < m_map_download_state.total_size ) {
									if ( m_on_map_progress ) {
										m_on_map_progress( (float)m_map_download_state.downloaded_size / m_map_download_state.total_size );
									}
									m_map_download_state.downloaded_size = end;
									RequestNextMapChunk();
								}
								else {
									Log( "Map downloaded successfully" );
									if ( m_on_map_data ) {
										m_on_map_data( m_map_download_state.buffer );
									}
									m_map_download_state.buffer.clear();
									m_map_download_state.is_downloading = false;
								}
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

void Client::UpdateSlot( const size_t slot_num, Slot* slot ) {
	Log( "Sending slot update" );
	Packet p( Packet::PT_UPDATE_SLOT );
	// not sending slot num because server knows it anyway
	p.data.str = slot->Serialize().ToString();
	m_network->MT_SendPacket( p );
}

void Client::Message( const std::string& message ) {
	Log( "Sending chat message: " + message );
	Packet p( Packet::PT_MESSAGE );
	p.data.str = message;
	m_network->MT_SendPacket( p );
}

const Connection::game_state_t Client::GetGameState() const {
	return m_game_state;
}

void Client::RequestMap() {
	Log( "Requesting map from server" );
	ASSERT( !m_map_download_state.is_downloading, "map already being downloaded" );
	ASSERT( m_on_map_data, "map download requested but m_on_map_data is not set" );
	m_map_download_state.is_downloading = true;
	Packet p( Packet::PT_GET_MAP_HEADER );
	m_network->MT_SendPacket( p );
}

void Client::ResetHandlers() {
	Connection::ResetHandlers();
	m_on_players_list_update = nullptr;
	m_on_game_state_change = nullptr;
	m_on_map_progress = nullptr;
	m_on_map_data = nullptr;
}

void Client::Error( const std::string& reason ) {
	Log( "Network protocol error: " + reason );
	Disconnect( "Network protocol error" );
}

void Client::RequestNextMapChunk() {
	ASSERT( m_map_download_state.is_downloading, "map download not initialized" );
	ASSERT( m_map_download_state.buffer.size() == m_map_download_state.downloaded_size, "map buffer size mismatch" );
	ASSERT( m_map_download_state.downloaded_size < m_map_download_state.total_size, "map already downloaded" );
	const size_t size = std::min( MAP_DOWNLOAD_CHUNK_SIZE, m_map_download_state.total_size - m_map_download_state.downloaded_size );
	Log( "Requesting map chunk ( offset=" + std::to_string( m_map_download_state.downloaded_size ) + " size=" + std::to_string( size ) + " )" );
	Packet p( Packet::PT_GET_MAP_CHUNK );
	p.udata.map.offset = m_map_download_state.downloaded_size;
	p.udata.map.size = size;
	m_network->MT_SendPacket( p );
}

}
}
