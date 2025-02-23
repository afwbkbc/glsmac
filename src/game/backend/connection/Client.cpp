#include "Client.h"

#include <algorithm>

#include "game/backend/State.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/event/Event.h"
#include "types/Packet.h"
#include "network/Network.h"

namespace game {
namespace backend {
namespace connection {

Client::Client( settings::LocalSettings* const settings )
	: Connection( network::CM_CLIENT, settings ) {
	//
}

void Client::ProcessEvent( const network::Event& event ) {
	Connection::ProcessEvent( event );

	ASSERT( !event.cid, "client connection received event with non-zero cid" );

	switch ( event.type ) {
		case network::Event::ET_PACKET: {
			try {
				if ( !event.data.packet_data.empty() ) {
					types::Packet packet( types::Packet::PT_NONE );
					packet.Unserialize( types::Buffer( event.data.packet_data ) );
					switch ( packet.type ) {
						case types::Packet::PT_REQUEST_AUTH: {
							Log( "Authenticating" );
							types::Packet p( types::Packet::PT_AUTH );
							p.data.vec = {
								m_settings->account.GetGSID(),
								m_settings->player_name,
							};
							m_network->MT_SendPacket( &p );
							break;
						}
						case types::Packet::PT_PLAYERS: {
							Log( "Got players list from server" );
							if ( packet.data.num ) {
								// initial players list
								m_slot = packet.data.num;
							}
							else {
								// players list update (i.e. after resolving random players )
								m_state->m_slots->Clear();
							}
							m_state->m_slots->Unserialize( packet.data.str );
							for ( auto i = 0 ; i < m_state->m_slots->GetCount() ; i++ ) {
								const auto& slot = m_state->m_slots->GetSlot( i );
								if ( slot.GetState() == slot::Slot::SS_PLAYER ) {
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
						case types::Packet::PT_GLOBAL_SETTINGS: {
							Log( "Got global settings update from server" );
							const auto& host_slot = m_state->m_slots->GetSlot( 0 );
							bool ok = true;
							switch ( m_game_state ) {
								case Connection::GS_LOBBY: {
									if ( host_slot.HasPlayerFlag( slot::PF_READY ) ) {
										Error( "host updated settings while ready" );
										ok = false;
									}
									break;
								}
								case Connection::GS_INITIALIZING:
								case Connection::GS_RUNNING: {
									if ( m_are_global_settings_received ) {
										Error( "duplicate global settings update" );
										ok = false;
									}
									break;
								}
								default: {
									Error( "unexpected global settings update for this state" );
									ok = false;
								}
							}
							if ( !ok ) {
								break; // something went wrong
							}
							//m_state->m_settings.global.Unserialize( types::Buffer( packet.data.str ) );
							m_state->Unserialize( types::Buffer( packet.data.str ) );
							if ( m_on_global_settings_update ) {
								m_on_global_settings_update();
							}
							m_are_global_settings_received = true;
							break;
						}
						case types::Packet::PT_SLOT_UPDATE:
						case types::Packet::PT_FLAGS_UPDATE: {
							const bool only_flags = packet.type == types::Packet::PT_FLAGS_UPDATE;
							const size_t slot_num = only_flags
								? packet.udata.flags.slot_num
								: packet.data.num;
							if ( slot_num >= m_state->m_slots->GetSlots().size() ) {
								Error( "slot index mismatch" );
								return;
							}
							auto& slot = m_state->m_slots->GetSlot( slot_num );
							const auto old_flags = slot.GetState() == slot::Slot::SS_PLAYER
								? slot.GetPlayerFlags()
								: 0;
							if ( only_flags ) {
								Log( "Got flags update from server (slot: " + std::to_string( slot_num ) + ")" );
								slot.SetPlayerFlags( packet.udata.flags.flags );
							}
							else {
								Log( "Got slot update from server (slot: " + std::to_string( slot_num ) + ")" );
								const bool wasReady = slot.GetState() == slot::Slot::SS_PLAYER
									? slot.HasPlayerFlag( slot::PF_READY ) // check readyness of player
									: m_state->m_slots->GetSlot( 0 ).HasPlayerFlag( slot::PF_READY ) // check readyness of host
								;
								slot.Unserialize( packet.data.str );
								if ( m_game_state == GS_LOBBY ) {
									if ( slot.GetState() == slot::Slot::SS_PLAYER ) {
										if ( wasReady && slot.HasPlayerFlag( slot::PF_READY ) ) {
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
								}
							}
							if ( !only_flags ) {
								if ( m_on_slot_update ) {
									m_on_slot_update( slot_num, &slot );
								}
							}
							if ( m_on_flags_update ) {
								m_on_flags_update( slot_num, &slot, old_flags, slot.GetPlayerFlags() );
							}
							break;
						}
						case types::Packet::PT_KICK: {
							Disconnect( packet.data.str );
							break;
						}
						case types::Packet::PT_MESSAGE: {
							Log( "Got chat message: " + packet.data.str );
							if ( m_on_message ) {
								m_on_message( packet.data.str );
							}
							break;
						}
						case types::Packet::PT_GAME_STATE: {
							Log( "Got game state: " + std::to_string( packet.data.num ) );
							if ( packet.udata.game_state.state != m_game_state ) {
								m_game_state = (game_state_t)packet.udata.game_state.state;
								if ( m_on_game_state_change ) {
									m_on_game_state_change( m_game_state );
								}
							}
							break;
						}
						case types::Packet::PT_DOWNLOAD_RESPONSE: {
							Log( "Got download response" );
							if ( !m_download_state.is_downloading ) {
								Error( "download response received while not downloading" );
							}
							if ( packet.data.num == 0 ) {
								Log( "No download response received from server" );
								Disconnect( "No download response received from server" );
							}
							else {
								m_download_state.total_size = packet.data.num;
								Log( "Allocating download buffer (" + std::to_string( m_download_state.total_size ) + " bytes)" );
								m_download_state.buffer.reserve( m_download_state.total_size );
								DownloadNextChunk();
							}
							break;
						}
						case types::Packet::PT_DOWNLOAD_NEXT_CHUNK_RESPONSE: {
							Log( "Downloaded next chunk ( offset=" + std::to_string( packet.udata.download.offset ) + " size=" + std::to_string( packet.udata.download.size ) + " )" );
							const size_t end = packet.udata.download.offset + packet.udata.download.size;
							if ( !m_download_state.is_downloading ) {
								Error( "chunk received while not downloading" );
							}
							else if ( end > m_download_state.total_size ) {
								Error( "chunk overflow ( " + std::to_string( packet.udata.download.offset ) + " + " + std::to_string( packet.udata.download.size ) + " >= " + std::to_string( m_download_state.total_size ) + " )" );
							}
							else if ( packet.udata.download.offset != m_download_state.downloaded_size ) {
								Error( "inconsistent chunk offset ( " + std::to_string( packet.udata.download.offset ) + " != " + std::to_string( m_download_state.downloaded_size ) + " )" );
							}
							else if (
								packet.udata.download.size != DOWNLOAD_CHUNK_SIZE &&
									end != m_download_state.total_size // last chunk can be smaller
								) {
								Error( "inconsistent map chunk size ( " );
							}
							else {
								ASSERT( packet.data.str.size() == packet.udata.download.size, "download buffer size mismatch" );
								m_download_state.buffer.append( packet.data.str );
								if ( end < m_download_state.total_size ) {
									if ( m_on_download_progress ) {
										m_on_download_progress( (float)m_download_state.downloaded_size / m_download_state.total_size );
									}
									m_download_state.downloaded_size = end;
									DownloadNextChunk();
								}
								else {
									Log( "Download completed successfully" );
									if ( m_on_download_complete ) {
										m_on_download_complete( m_download_state.buffer );
									}
									m_download_state.buffer.clear();
									m_download_state.is_downloading = false;
								}
							}
							break;
						}
						case types::Packet::PT_GAME_EVENTS: {
							Log( "Got game events packet" );
							if ( m_on_game_event_validate && m_on_game_event_apply ) {
								auto buf = types::Buffer( packet.data.str );
								std::vector< backend::event::Event* > game_events = {};
								m_state->WithGSE(
									[ &buf, &game_events ]( GSE_CALLABLE ) {
										backend::event::Event::UnserializeMultiple( GSE_CALL, buf, game_events );
									}
								);
								for ( const auto& game_event : game_events ) {
									Log( "Got game event: " + game_event->ToString() );
									m_on_game_event_validate( game_event );
									m_on_game_event_apply( game_event );
								}
							}
							else {
								Log( "WARNING: game event handler not set" );
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
		case network::Event::ET_ERROR: {
			Error( event.data.packet_data );
			break;
		}
		case network::Event::ET_DISCONNECT: {
			Disconnect( "Connection to server lost." );
			break;
		}
		default: {
			Log( "WARNING: invalid event type from server: " + std::to_string( event.type ) );
		}
	}
}

void Client::SendGameEvents( const game_events_t& game_events ) {
	Log( "Sending " + std::to_string( game_events.size() ) + " game events" );
	types::Packet p( types::Packet::PT_GAME_EVENTS );
	p.data.str = backend::event::Event::SerializeMultiple( game_events ).ToString();
	m_network->MT_SendPacket( &p );
}

void Client::UpdateSlot( const size_t slot_num, slot::Slot* slot, const bool only_flags ) {
	if ( only_flags ) {
		Log( "Sending flags update" );
		types::Packet p( types::Packet::PT_UPDATE_FLAGS );
		// not sending slot num because server knows it anyway
		p.udata.flags.flags = slot->GetPlayerFlags();
		m_network->MT_SendPacket( &p );
	}
	else {
		Log( "Sending slot update" );
		types::Packet p( types::Packet::PT_UPDATE_SLOT );
		// not sending slot num because server knows it anyway
		p.data.str = slot->Serialize().ToString();
		m_network->MT_SendPacket( &p );
	}
}

void Client::SendMessage( const std::string& message ) {
	Log( "Sending chat message: " + message );
	types::Packet p( types::Packet::PT_MESSAGE );
	p.data.str = message;
	m_network->MT_SendPacket( &p );
}

const Connection::game_state_t Client::GetGameState() const {
	return m_game_state;
}

void Client::RequestDownload() {
	Log( "Requesting download from server" );
	ASSERT( !m_download_state.is_downloading, "download already started" );
	ASSERT( m_on_download_complete, "download requested but m_on_download_complete is not set" );
	m_download_state.is_downloading = true;
	types::Packet p( types::Packet::PT_DOWNLOAD_REQUEST );
	m_network->MT_SendPacket( &p );
}

void Client::ResetHandlers() {
	Connection::ResetHandlers();
	m_on_players_list_update = nullptr;
	m_on_game_state_change = nullptr;
	m_on_download_progress = nullptr;
	m_on_download_complete = nullptr;
}

void Client::Error( const std::string& reason ) {
	Log( "Network protocol error: " + reason );
	Disconnect( "Network protocol error" );
}

void Client::DownloadNextChunk() {
	ASSERT( m_download_state.is_downloading, "download not initialized" );
	ASSERT( m_download_state.buffer.size() == m_download_state.downloaded_size, "download buffer size mismatch" );
	ASSERT( m_download_state.downloaded_size < m_download_state.total_size, "download already finished" );
	const size_t size = std::min( DOWNLOAD_CHUNK_SIZE, m_download_state.total_size - m_download_state.downloaded_size );
	Log( "Requesting next chunk ( offset=" + std::to_string( m_download_state.downloaded_size ) + " size=" + std::to_string( size ) + " )" );
	types::Packet p( types::Packet::PT_DOWNLOAD_NEXT_CHUNK_REQUEST );
	p.udata.download.offset = m_download_state.downloaded_size;
	p.udata.download.size = size;
	m_network->MT_SendPacket( &p );
}

}
}
}
