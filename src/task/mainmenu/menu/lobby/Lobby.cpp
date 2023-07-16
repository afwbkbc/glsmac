#include "Lobby.h"

#include "engine/Engine.h"

#include "types/Packet.h"

using namespace types;
using namespace network;

namespace task {
namespace mainmenu {
namespace lobby {

Lobby::Lobby( MainMenu* mainmenu )
	: PopupMenu( mainmenu, "MULTIPLAYER SETUP" )
	, m_state( mainmenu->m_settings )
{
	SetWidth( 800 );
	SetHeight( 600 );
	
	m_state.m_settings.global.game_rules.Initialize();
	m_state.m_slots.Resize( 7 ); // TODO: make dynamic
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
			g_engine->GetNetwork()->MT_Disconnect();
			GoBack();
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
	
	if ( m_state.m_settings.local.network_role == ::game::LocalSettings::NR_SERVER ) {
		ASSERT( !m_player, "player already set" );
		m_player = new ::game::Player{
			m_state.m_settings.local.player_name,
			::game::Player::PR_HOST,
			m_state.m_settings.global.game_rules.m_factions[ 0 ]
		};
		m_state.AddPlayer( m_player );
		m_slot = 0; // host always has slot 0
		m_state.AddCIDSlot( 0, m_slot );
		m_state.m_slots.GetSlot( m_slot ).SetPlayer( m_player );
	}

	RefreshUI();
	
	m_getevents_mt_id = 0;
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
	
	auto* network = g_engine->GetNetwork();
	
	if ( !m_getevents_mt_id ) {
		m_getevents_mt_id = network->MT_GetEvents();
	}
	else {
		auto result = network->MT_GetResult( m_getevents_mt_id );
		if ( result.result != R_NONE ) {
			if ( result.result == R_ERROR ) {
				Log( "WARNING: error event received from network" );
			}
			else if ( result.result == R_SUCCESS ) {
				if ( !result.events.empty() ) {
					//Log( "EVENTS COUNT: " + to_string( result.events.size() ) );
					for ( auto& event : result.events ) {
						switch ( m_state.m_settings.local.network_role ) {
							case ::game::LocalSettings::NR_CLIENT: {
								if ( event.cid ) {
									break; // old event?
								}
								switch ( event.type ) {
									case Event::ET_CONNECT: {
										
										break;
										}
									case Event::ET_PACKET: {
										if ( !event.data.packet_data.empty() ) {
											Packet packet;
											packet.Unserialize( Buffer( event.data.packet_data ) );
											switch ( packet.type ) {
												case Packet::PT_REQUEST_AUTH: {
													Log( "Authenticating" );
													Packet p;
													p.type = Packet::PT_AUTH;
													p.data.str = m_state.m_settings.local.player_name;
													network->MT_SendPacket( p );
													break;
												}
												case Packet::PT_GLOBAL_SETTINGS: {
													Log( "Got global settings update from server" );
													m_state.m_settings.global.Unserialize( Buffer( packet.data.str ) );
													RefreshUI();
													break;
												}
												case Packet::PT_PLAYERS: {
													Log( "Got players list from server" );
													m_slot = packet.data.num;
													m_state.m_slots.Unserialize( packet.data.str );
													for ( auto i = 0 ; i < m_state.m_slots.GetCount() ; i++ ) {
														const auto& player = m_state.m_slots.GetSlot( i ).GetPlayer();
														m_state.AddPlayer( player );
														if ( i == m_slot ) {
															m_player = player;
														}
													}
													RefreshUI();
													break;
												}
												case Packet::PT_KICK: {
													Log( "Kicked by server: " + packet.data.str );
													m_disconnect_message = packet.data.str;
													network->MT_Disconnect();
													GoBack();
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
										if ( m_state.m_settings.local.network_role == ::game::LocalSettings::NR_CLIENT ) {
											GoBack();
											MenuError( m_disconnect_message );
										}
										break;
									}
									default: {
										Log( "WARNING: invalid event type from server: " + std::to_string( event.type ) );
									}
								}
								break;
							}
							case ::game::LocalSettings::NR_SERVER: {
								if ( !event.cid ) {
									break; // old event?
								}
								switch ( event.type ) {
									case Event::ET_CONNECT: {

										break;
									}
									case Event::ET_CLIENT_CONNECT: {
										Log( std::to_string( event.cid ) + " connected" );
										ASSERT( m_state.GetCidSlots().find( event.cid ) == m_state.GetCidSlots().end(), "player cid already in slots" );
										//m_players[ event.cid ] = {}; // to be queried*/ // TODO
										{
											Packet packet;
											packet.type = Packet::PT_REQUEST_AUTH; // ask to authenticate
											network->MT_SendPacket( packet, event.cid );
										}
										break;
									}
									case Event::ET_CLIENT_DISCONNECT: {
										Log( std::to_string( event.cid ) + " disconnected" );
										auto it = m_state.GetCidSlots().find( event.cid );
										if ( it != m_state.GetCidSlots().end() ) {
											m_state.RemoveCIDSlot( event.cid );
											auto* player = m_state.m_slots.GetSlot( it->second ).GetPlayerAndClose();
											m_state.RemovePlayer( player );
											RefreshUI();
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
													network->MT_DisconnectClient( event.cid );
													break;
												}

												Log( "Got authentication from " + std::to_string( event.cid ) + ": " + packet.data.str );

												if ( m_state.GetCidSlots().find( event.cid ) != m_state.GetCidSlots().end() ) {
													Log( "Duplicate uthentication from " + std::to_string( event.cid ) + ", disconnecting" );
													network->MT_DisconnectClient( event.cid );
													break;
												}

												// find free slot
												size_t slot_num = 0; // 0 = 'not found'
												for ( auto& slot : m_state.m_slots.GetSlots() ) {
													if ( slot.GetState() == ::game::Slot::SS_OPEN ) {
														break;
													}
													slot_num++;
												}
												if ( slot_num >= m_state.m_slots.GetCount() ) { // no available slots left
													Log( "No free slots for player " + std::to_string( event.cid ) + " (" + packet.data.str + "), dropping" );
													Packet p;
													p.type = types::Packet::PT_KICK;
													p.data.str = "Server is full!";
													network->MT_SendPacket( p, event.cid );
													network->MT_DisconnectClient( event.cid );
													break;
												}

												auto* player = new ::game::Player{
													packet.data.str,
													::game::Player::PR_PLAYER,
													m_state.m_settings.global.game_rules.m_factions[ 0 ]
												};
												m_state.AddPlayer( player );

												m_state.AddCIDSlot( event.cid, slot_num );
												m_state.m_slots.GetSlot( slot_num ).SetPlayer( player );

												Log( "Sending global settings to " + std::to_string( event.cid ) );
												Packet p;
												p.type = Packet::PT_GLOBAL_SETTINGS;
												p.data.str = m_state.m_settings.global.Serialize().ToString();
												network->MT_SendPacket( p, event.cid );

												RefreshUI();
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
								break;
							}
							default: {
								ASSERT(false, "unknown network role " + std::to_string(m_state.m_settings.local.network_role));
							}
						}
					}
				}
			}
			m_getevents_mt_id = 0;
		}
	}
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
	
	g_engine->GetNetwork()->MT_Disconnect();
	
	return true;
}

void Lobby::RefreshUI() {
	m_map_settings_section->SetTitleText( m_state.m_settings.global.game_name );
	
	// m_players_section->SetPlayers( m_players ); // TODO
	
	if ( m_state.m_settings.local.network_role == ::game::LocalSettings::NR_SERVER ) {
		// update UIs of others aswell
		// TODO: optimize?
		
		Packet p;
		p.type = Packet::PT_PLAYERS;
		p.data.str = m_state.m_slots.Serialize().ToString();

		Log( "Sending player list to all players" );
		for ( auto& it : m_state.GetCidSlots() ) {
			if ( it.first == 0 ) {
				continue; // don't send to self
			}
			p.data.num = it.second; // send slot num to every player
			g_engine->GetNetwork()->MT_SendPacket( p, it.first );
		}
	}

}

}
}
}
