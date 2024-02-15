#include "Server.h"

using namespace network;

namespace game {
namespace connection {

Server::Server( LocalSettings* const settings )
	: Connection( network::CM_SERVER, settings ) {
	//
}

void Server::ProcessEvent( const network::Event& event ) {
	Connection::ProcessEvent( event );

	ASSERT( event.cid || event.type == network::Event::ET_LISTEN, "server connection received event without cid" );

	switch ( event.type ) {
		case network::Event::ET_LISTEN: {
			ASSERT( !m_player, "player already set" );
			Log( "Listening" );
			m_state->m_settings.global.Initialize();
			m_state->m_slots.Resize( 7 ); // TODO: make dynamic?
			const auto& rules = m_state->m_settings.global.game_rules;
			NEW(
				m_player, ::game::Player,
				m_state->m_settings.local.player_name,
				::game::Player::PR_HOST,
				rules.GetDefaultFaction(),
				rules.GetDefaultDifficultyLevel()
			);
			m_state->AddPlayer( m_player );
			m_slot = 0; // host always has slot 0
			m_state->AddCIDSlot( 0, m_slot );
			auto& slot = m_state->m_slots.GetSlot( m_slot );
			slot.SetPlayer( m_player, 0, event.data.remote_address ); // host always has cid 0
			slot.SetLinkedGSID( m_state->m_settings.local.account.GetGSID() );
			if ( m_on_listen ) {
				m_on_listen();
			}
			if ( m_on_global_settings_update ) {
				m_on_global_settings_update();
			}
			if ( m_on_player_join ) {
				m_on_player_join( m_slot, &slot, m_player );
			}
			break;
		}
		case network::Event::ET_CLIENT_CONNECT: {
			Log( "Client " + std::to_string( event.cid ) + " connected" );
			ASSERT( m_state->GetCidSlots().find( event.cid ) == m_state->GetCidSlots().end(), "player cid already in slots" );

			const auto& banned = m_settings->banned_addresses;
			if ( banned.find( event.data.remote_address ) != banned.end() ) {
				Kick( event.cid, "You are banned" );
				break;
			}

			{
				Packet packet( Packet::PT_REQUEST_AUTH ); // ask to authenticate
				m_network->MT_SendPacket( packet, event.cid );
			}
			break;
		}
		case network::Event::ET_CLIENT_DISCONNECT: {
			Log( "Client " + std::to_string( event.cid ) + " disconnected" );
			auto it = m_state->GetCidSlots().find( event.cid );
			if ( it != m_state->GetCidSlots().end() ) {
				const auto slot_num = it->second;
				m_state->RemoveCIDSlot( event.cid );
				auto& slot = m_state->m_slots.GetSlot( slot_num );
				auto* player = slot.GetPlayerAndClose();
				ASSERT( player, "player in slot is null" );
				m_state->RemovePlayer( player );

				// cleanup
				const auto& download_data_it = m_download_data.find( event.cid );
				if ( download_data_it != m_download_data.end() ) {
					m_download_data.erase( download_data_it );
				}

				ASSERT( m_game_state != GS_NONE, "player disconnected but game state is not set" );
				if ( m_game_state == GS_LOBBY ) {
					// free slot for next player
					slot.UnsetLinkedGSID();
				}

				SendSlotUpdate( slot_num, &slot, event.cid ); // notify others

				if ( m_game_state == GS_LOBBY ) {
					ClearReadyFlags();
				}

				if ( m_on_player_leave ) {
					m_on_player_leave( slot_num, &slot, player );
				}
				DELETE( player );
			}
			break;
		}
		case network::Event::ET_PACKET: {
			try {
				Packet packet( Packet::PT_NONE );
				packet.Unserialize( Buffer( event.data.packet_data ) );
				switch ( packet.type ) {
					case Packet::PT_AUTH: {
						ASSERT( packet.data.vec.size() == 2, "unexpected vec size of PT_AUTH" );
						const std::string& gsid = packet.data.vec[ 0 ];
						const std::string& player_name = packet.data.vec[ 1 ];
						if ( gsid.empty() || player_name.empty() ) {
							Log( "Authentication from cid " + std::to_string( event.cid ) + " failed, disconnecting" );
							m_network->MT_DisconnectClient( event.cid );
							break;
						}

						if ( m_state->GetCidSlots().find( event.cid ) != m_state->GetCidSlots().end() ) {
							Log( "Duplicate authentication from cid " + std::to_string( event.cid ) + ", disconnecting" );
							m_network->MT_DisconnectClient( event.cid );
							break;
						}

						Log( "Got authentication from " + gsid + " (cid " + std::to_string( event.cid ) + ") as '" + player_name + "'" );
						ASSERT( m_game_state != GS_NONE, "player connected but game state not set" );

						// TODO: implementing 'load game' from lobby will need extra gsid logic

						// check if not already in game
						bool is_already_in_game = false;
						for ( auto& slot : m_state->m_slots.GetSlots() ) {
							if ( slot.GetLinkedGSID() == gsid && slot.GetState() == Slot::SS_PLAYER ) {
								is_already_in_game = true;
								break;
							}
						}
						if ( is_already_in_game ) {
							Log( "Duplicate connection from " + gsid + ", disconnecting" );
							Kick( event.cid, "You are already in this game" );
							break;
						}

						size_t slot_num = 0;
						if ( m_game_state == GS_LOBBY ) {
							// on lobby stage everyone can connect and occupy first free slot
							for ( auto& slot : m_state->m_slots.GetSlots() ) {
								if ( slot.GetState() == ::game::Slot::SS_OPEN ) {
									break;
								}
								slot_num++;
							}
							if ( slot_num >= m_state->m_slots.GetCount() ) { // no available slots left
								Log( "No free slots for player " + gsid + " (" + player_name + "), rejecting" );
								Kick( event.cid, "Server is full!" );
								break;
							}
						}
						else {
							// once game is started - only players that are linked to slots by gsids can join to their slots
							for ( auto& slot : m_state->m_slots.GetSlots() ) {
								if ( slot.GetState() == ::game::Slot::SS_OPEN && slot.GetLinkedGSID() == gsid ) {
									break;
								}
								slot_num++;
							}
							if ( slot_num >= m_state->m_slots.GetCount() ) { // no available slots left
								Log( "No linked slot found for player " + gsid + " (" + player_name + "), rejecting" );
								Kick( event.cid, "You do not belong to this game." );
								break;
							}
						}

						if ( m_game_state == GS_LOBBY ) {
							ClearReadyFlags();
						}

						const auto& rules = m_state->m_settings.global.game_rules;
						NEWV(
							player, ::game::Player,
							player_name,
							::game::Player::PR_PLAYER,
							rules.GetDefaultFaction(),
							rules.GetDefaultDifficultyLevel()
						);
						m_state->AddPlayer( player );

						m_state->AddCIDSlot( event.cid, slot_num );
						auto& slot = m_state->m_slots.GetSlot( slot_num );
						slot.SetPlayer( player, event.cid, event.data.remote_address );
						if ( m_game_state == GS_LOBBY ) {
							// link slot to player
							slot.SetLinkedGSID( gsid );
						}
						SendGameState( event.cid );
						{
							Log( "Sending players list to " + std::to_string( event.cid ) );
							Packet p( Packet::PT_PLAYERS );
							p.data.num = slot_num;
							p.data.str = m_state->m_slots.Serialize().ToString();
							g_engine->GetNetwork()->MT_SendPacket( p, event.cid );
						}
						SendGlobalSettings( event.cid );

						SendSlotUpdate( slot_num, &slot, event.cid ); // notify others

						if ( m_on_player_join ) {
							m_on_player_join( slot_num, &slot, player );
						}

						break;
					}
					case Packet::PT_UPDATE_SLOT:
					case Packet::PT_UPDATE_FLAGS: {
						const auto& slots = m_state->GetCidSlots();
						const auto& it = slots.find( event.cid );
						if ( it == slots.end() ) {
							Error( event.cid, "slot index mismatch" );
							break;
						}
						auto& slot = m_state->m_slots.GetSlot( it->second );
						if ( slot.GetState() != Slot::SS_PLAYER ) {
							Error( event.cid, "slot state mismatch" );
							break;
						}
						const bool only_flags = packet.type == Packet::PT_UPDATE_FLAGS;
						const auto old_flags = slot.GetState() == Slot::SS_PLAYER
							? slot.GetPlayerFlags()
							: 0;
						if ( only_flags ) {
							Log( "Got flags update from " + std::to_string( event.cid ) );
							slot.SetPlayerFlags( packet.udata.flags.flags );
							SendFlagsUpdate( it->second, &slot, event.cid ); // notify others
						}
						else {
							Log( "Got slot update from " + std::to_string( event.cid ) );
							const bool wasReady = slot.HasPlayerFlag( Slot::PF_READY );
							slot.Unserialize( packet.data.str );
							if ( wasReady && slot.HasPlayerFlag( Slot::PF_READY ) ) {
								Error( event.cid, "slot update while ready" );
								break;
							}
							SendSlotUpdate( it->second, &slot, event.cid ); // notify others
						}
						if ( !only_flags ) {
							if ( m_on_slot_update ) {
								m_on_slot_update( it->second, &slot );
							}
						}
						if ( m_on_flags_update ) {
							m_on_flags_update( it->second, &slot, old_flags, slot.GetPlayerFlags() );
						}
						break;
					}
					case Packet::PT_MESSAGE: {
						Log( "Got chat message from " + std::to_string( event.cid ) + ": " + packet.data.str );
						const auto& slots = m_state->GetCidSlots();
						const auto& it = slots.find( event.cid );
						if ( it == slots.end() ) {
							Error( event.cid, "slot index mismatch" );
							break;
						}
						GlobalMessage( FormatChatMessage( m_state->m_slots.GetSlot( it->second ).GetPlayer(), packet.data.str ) );
						break;
					}
					case Packet::PT_DOWNLOAD_REQUEST: {
						Log( "Got download request from " + std::to_string( event.cid ) );
						Packet p( types::Packet::PT_DOWNLOAD_RESPONSE );
						if ( m_on_download_request ) {
							m_download_data[ event.cid ] = download_data_t{ // override previous request
								0,
								m_on_download_request()
							};
							p.data.num = m_download_data.at( event.cid ).serialized_snapshot.size();
						}
						else {
							// no handler set - no data to return
							Log( "WARNING: download requested but no download handler was set, sending empty header" );
							p.data.num = 0;
						}
						m_network->MT_SendPacket( p, event.cid );
						break;
					}
					case Packet::PT_DOWNLOAD_NEXT_CHUNK_REQUEST: {
						Log( "Got next chunk request from " + std::to_string( event.cid ) + " ( offset=" + std::to_string( packet.udata.download.offset ) + " size=" + std::to_string( packet.udata.download.size ) + " )" );
						const auto& it = m_download_data.find( event.cid );
						const size_t end = packet.udata.download.offset + packet.udata.download.size;
						if ( it == m_download_data.end() ) {
							Error( event.cid, "download not initialized" );
						}
						else if ( end > it->second.serialized_snapshot.size() ) {
							Error( event.cid, "download offset overflow ( " + std::to_string( packet.udata.download.offset ) + " + " + std::to_string( packet.udata.download.size ) + " >= " + std::to_string( it->second.serialized_snapshot.size() ) + " )" );
						}
						else if ( packet.udata.download.offset != it->second.next_expected_offset ) {
							Error( event.cid, "inconsistent download offset ( " + std::to_string( packet.udata.download.offset ) + " != " + std::to_string( it->second.next_expected_offset ) + " )" );
						}
						else if (
							packet.udata.download.size != DOWNLOAD_CHUNK_SIZE &&
								end != it->second.serialized_snapshot.size() // last chunk can be smaller
							) {
							Error( event.cid, "inconsistent download size ( " );
						}
						else {
							Packet p( Packet::PT_DOWNLOAD_NEXT_CHUNK_RESPONSE );
							p.udata.download.offset = packet.udata.download.offset;
							p.udata.download.size = packet.udata.download.size;
							p.data.str = it->second.serialized_snapshot.substr( packet.udata.download.offset, packet.udata.download.size );
							m_network->MT_SendPacket( p, event.cid );
							if ( end < it->second.serialized_snapshot.size() ) {
								it->second.next_expected_offset = end;
							}
							else {
								// snapshot was sent fully, can free memory now
								Log( "Snapshot was sent successfully to " + std::to_string( event.cid ) + ", cleaning up" );
								m_download_data.erase( it );
							}
						}
						break;
					}
					case Packet::PT_GAME_EVENTS: {
						Log( "Got game events packet" );
						if ( m_on_game_event ) {
							auto buf = Buffer( packet.data.str );
							std::vector< const game::event::Event* > game_events = {};
							game::event::Event::UnserializeMultiple( buf, game_events );
							for ( const auto& game_event : game_events ) {
								m_on_game_event( game_event );
							}
						}
						else {
							Log( "WARNING: game event handler not set" );
						}
						// relay to other clients
						Broadcast(
							[ this, packet, event ]( const network::cid_t cid ) -> void {
								if ( cid != event.cid ) { // don't send back
									SendGameEventsTo( packet.data.str, cid );
								}
							}
						);
						break;
					}
					default: {
						Log( "WARNING: invalid packet type from client " + std::to_string( event.cid ) + " : " + std::to_string( packet.type ) );
					}
				}
			}
			catch ( std::runtime_error& err ) {
				Error( event.cid, err.what() );
			}
			break;
		}
		case network::Event::ET_ERROR: {
			Error( event.cid, event.data.packet_data );
			break;
		}
		default: {
			Log( "WARNING: invalid event type from client " + std::to_string( event.cid ) + " : " + std::to_string( event.type ) );
		}
	}
}

void Server::SendGameEvents( const game_events_t& game_events ) {
	Log( "Sending " + std::to_string( game_events.size() ) + " game events" );
	const auto serialized_events = game::event::Event::SerializeMultiple( game_events ).ToString();
	Broadcast(
		[ this, serialized_events ]( const network::cid_t cid ) -> void {
			SendGameEventsTo( serialized_events, cid );
		}
	);
}

void Server::Broadcast( std::function< void( const network::cid_t cid ) > callback ) {
	for ( const auto& slot : m_state->m_slots.GetSlots() ) {
		if ( slot.GetState() == Slot::SS_PLAYER ) {
			const auto cid = slot.GetCid();
			if ( cid != 0 ) { // don't send to self
				callback( cid );
			}
		}
	}
}

void Server::KickFromSlot( const size_t slot_num, const std::string& reason ) {
	ASSERT( slot_num < m_state->m_slots.GetSlots().size(), "slot index overflow" );
	auto& slot = m_state->m_slots.GetSlot( slot_num );
	ASSERT( slot.GetState() == Slot::SS_PLAYER, "kick on non-player slot" );
	KickFromSlot( slot, reason );
}

void Server::BanFromSlot( const size_t slot_num, const std::string& reason ) {
	ASSERT( slot_num < m_state->m_slots.GetSlots().size(), "slot index overflow" );
	auto& slot = m_state->m_slots.GetSlot( slot_num );
	ASSERT( slot.GetState() == Slot::SS_PLAYER, "ban on non-player slot" );
	Log( "Banning address: " + slot.GetRemoteAddress() );
	m_settings->banned_addresses.insert( slot.GetRemoteAddress() );
	KickFromSlot( slot, reason );
}

void Server::SetGameState( const game_state_t game_state ) {
	m_game_state = game_state;
	Broadcast(
		[ this ]( const network::cid_t cid ) -> void {
			SendGameState( cid );
		}
	);
}

void Server::UpdateSlot( const size_t slot_num, Slot* slot, const bool only_flags ) {
	if ( !only_flags ) {
		if ( m_on_slot_update ) {
			m_on_slot_update( slot_num, slot );
		}
		SendSlotUpdate( slot_num, slot );
	}
	if ( m_on_flags_update ) {
		m_on_flags_update( slot_num, slot, slot->GetPlayerFlags(), slot->GetPlayerFlags() ); // TODO: old flags?
	}
	if ( only_flags ) {
		SendFlagsUpdate( slot_num, slot );
	}
}

void Server::SendMessage( const std::string& message ) {
	GlobalMessage( FormatChatMessage( GetPlayer(), message ) );
}

void Server::ResetHandlers() {
	Connection::ResetHandlers();
	m_on_listen = nullptr;
	m_on_download_request = nullptr;
}

void Server::UpdateGameSettings() {
	Broadcast(
		[ this ]( const network::cid_t cid ) -> void {
			SendGlobalSettings( cid );
		}
	);
}

void Server::GlobalMessage( const std::string& message ) {
	if ( m_on_message ) {
		m_on_message( message );
	}
	Broadcast(
		[ this, message ]( const network::cid_t cid ) -> void {
			Packet p( Packet::PT_MESSAGE );
			p.data.str = message;
			m_network->MT_SendPacket( p, cid );
		}
	);
}

void Server::Kick( const network::cid_t cid, const std::string& reason = "" ) {
	Log(
		"Kicking " + std::to_string( cid ) + ( !reason.empty()
			? " (reason: " + reason + ")"
			: ""
		)
	);
	Packet p( Packet::PT_KICK );
	p.data.str = reason;
	m_network->MT_SendPacket( p, cid );
	m_network->MT_DisconnectClient( cid );
}

void Server::KickFromSlot( Slot& slot, const std::string& reason ) {
	slot.SetCloseAfterClear();
	Kick( slot.GetCid(), reason );
}

void Server::Error( const network::cid_t cid, const std::string& reason ) {
	Log( "Network protocol error (cid: " + std::to_string( cid ) + "): " + reason );
	Kick( cid, "Network protocol error" );
}

void Server::SendGlobalSettings( network::cid_t cid ) {
	Log( "Sending global settings to " + std::to_string( cid ) );
	Packet p( Packet::PT_GLOBAL_SETTINGS );
	p.data.str = m_state->m_settings.global.Serialize().ToString();
	m_network->MT_SendPacket( p, cid );
}

void Server::SendGameState( const network::cid_t cid ) {
	Log( "Sending game state change (" + std::to_string( m_game_state ) + ") to " + std::to_string( cid ) );
	Packet p( Packet::PT_GAME_STATE );
	p.udata.game_state.state = m_game_state;
	m_network->MT_SendPacket( p, cid );
}

void Server::SendSlotUpdate( const size_t slot_num, const Slot* slot, network::cid_t skip_cid ) {
	Broadcast(
		[ this, slot_num, slot, skip_cid ]( const network::cid_t cid ) -> void {
			if ( cid != skip_cid ) {
				Log( "Sending slot update to " + std::to_string( cid ) );
				Packet p( Packet::PT_SLOT_UPDATE );
				p.data.num = slot_num;
				p.data.str = slot->Serialize().ToString();
				m_network->MT_SendPacket( p, cid );
			}
		}
	);
}

void Server::SendFlagsUpdate( const size_t slot_num, const Slot* slot, network::cid_t skip_cid ) {
	Broadcast(
		[ this, slot_num, slot, skip_cid ]( const network::cid_t cid ) -> void {
			if ( cid != skip_cid ) {
				Log( "Sending flags update to " + std::to_string( cid ) );
				Packet p( Packet::PT_FLAGS_UPDATE );
				p.udata.flags.slot_num = slot_num;
				p.udata.flags.flags = slot->GetPlayerFlags();
				m_network->MT_SendPacket( p, cid );
			}
		}
	);
}

const std::string Server::FormatChatMessage( const Player* player, const std::string& message ) const {
	return "<" + player->GetPlayerName() + "> " + message;
}

void Server::SendGameEventsTo( const std::string& serialized_events, const network::cid_t cid ) {
	Packet p( Packet::PT_GAME_EVENTS );
	p.data.str = serialized_events;
	m_network->MT_SendPacket( p, cid );
}

void Server::ClearReadyFlags() {
	ASSERT( m_game_state, "unexpected game state" );
	// clear readyness of everyone when new player joins or leaves
	auto& slots = m_state->m_slots.GetSlots();
	for ( size_t num = 0 ; num < slots.size() ; num++ ) {
		auto& slot = slots.at( num );
		if ( slot.GetState() == Slot::SS_PLAYER && slot.HasPlayerFlag( Slot::PF_READY ) ) {
			Log( "Clearing 'ready' flag of " + slot.GetPlayer()->GetPlayerName() );
			const auto old_flags = slot.GetPlayerFlags();
			slot.UnsetPlayerFlag( Slot::PF_READY );
			UpdateSlot( num, &slot, true );
			if ( m_on_flags_update ) {
				m_on_flags_update( num, &slot, old_flags, slot.GetPlayerFlags() );
			}
		}
	}
}

}

}
