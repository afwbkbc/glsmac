#include "Lobby.h"

#include "engine/Engine.h"

#include "types/Packet.h"

using namespace types;
using namespace network;

namespace task {
namespace mainmenu {

Lobby::Lobby( MainMenu* mainmenu ) : PopupMenu( mainmenu, "MULTIPLAYER SETUP" ) {
	SetWidth( 800 );
	SetHeight( 600 );
	
	m_settings_backup = m_mainmenu->m_settings.global;
}

void Lobby::Show() {
	PopupMenu::Show();
	
	NEW( m_map_settings_section, Section, "PopupSection" );
		m_map_settings_section->SetTitleText( " " ); // to have header created
		m_map_settings_section->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_map_settings_section->SetWidth( 310 );
		m_map_settings_section->SetHeight( 364 );
	m_body->AddChild( m_map_settings_section );

	NEW( m_players_section, Section, "PopupSection" );
		m_players_section->SetTitleText( "PLAYERS" );
		m_players_section->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
		m_players_section->SetWidth( 488 );
		m_players_section->SetHeight( 204 );
	m_body->AddChild( m_players_section );
	
	NEW( m_launch_button, Button, "PopupButtonOkCancel" ); // TODO: correct style
		m_launch_button->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_BOTTOM );
		m_launch_button->SetLeft( 8 );
		m_launch_button->SetBottom( 4 );
		m_launch_button->SetWidth( 236 );
		m_launch_button->SetHeight( 22 );
		m_launch_button->SetLabel( "LAUNCH GAME" );
	m_players_section->AddChild( m_launch_button );
	
	NEW( m_cancel_button, Button, "PopupButtonOkCancel" ); // TODO: correct style
		m_cancel_button->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
		m_cancel_button->SetRight( 8 );
		m_cancel_button->SetBottom( 4 );
		m_cancel_button->SetWidth( 236 );
		m_cancel_button->SetHeight( 22 );
		m_cancel_button->SetLabel( "CANCEL" );
		m_cancel_button->On( UIEvent::EV_BUTTON_CLICK, EH( this ) {
			g_engine->GetNetwork()->MT_Disconnect();
			m_mainmenu->m_settings.global = m_settings_backup;
			GoBack();
			return true;
		});
	m_players_section->AddChild( m_cancel_button );
	
	NEW( m_chat_section, Section, "PopupSection" );
		m_chat_section->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
		m_chat_section->SetTop( 200 );
		m_chat_section->SetWidth( 488 );
		m_chat_section->SetHeight( 164 );
	m_body->AddChild( m_chat_section );

	NEW( m_game_settings_section, Section, "PopupSection" );
		m_game_settings_section->SetTitleText( "CUSTOM GAME OPTIONS" );
		m_game_settings_section->SetAlign( UIObject::ALIGN_BOTTOM );
		m_game_settings_section->SetHeight( 210 );
	m_body->AddChild( m_game_settings_section );
	
	if ( m_mainmenu->m_settings.local.network_role == LocalSettings::NR_SERVER ) {
		m_player_names = {
			{ 0, m_mainmenu->m_settings.local.player_name }
		};
	}
	else {
		m_player_names = {};
	}
	
	RefreshUI();
	
	m_getevents_mt_id = 0;
}

void Lobby::Hide() {
	
	for ( auto& label : m_player_labels ) {
		m_players_section->RemoveChild( label.second );
	}
	m_player_labels.clear();
	
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
						switch ( m_mainmenu->m_settings.local.network_role ) {
							case LocalSettings::NR_CLIENT: {
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
													p.data.str = m_mainmenu->m_settings.local.player_name;
													network->MT_SendPacket( p );
													break;
												}
												case Packet::PT_GLOBAL_SETTINGS: {
													Log( "Got global settings update from server" );
													m_mainmenu->m_settings.global.Unserialize( Buffer( packet.data.str ) );
													RefreshUI();
													break;
												}
												case Packet::PT_PLAYERS: {
													Log( "Got players list from server" );
													m_player_names.clear();
													size_t i = 0; // TODO: cids?
													for ( auto& s : packet.data.vec ) {
														m_player_names[ i++ ] = s;
													}
													RefreshUI();
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
										if ( m_mainmenu->m_settings.local.network_role == LocalSettings::NR_CLIENT ) {
											m_mainmenu->m_settings.global = m_settings_backup;
											GoBack();
											MenuError( "Connection to server lost." );
										}
										break;
									}
									default: {
										Log( "WARNING: invalid event type from server: " + std::to_string( event.type ) );
									}
								}
								break;
							}
							case LocalSettings::NR_SERVER: {
								if ( !event.cid ) {
									break; // old event?
								}
								switch ( event.type ) {
									case Event::ET_CONNECT: {

										break;
									}
									case Event::ET_CLIENT_CONNECT: {
										Log( std::to_string( event.cid ) + " connected" );
										ASSERT( m_player_names.find( event.cid ) == m_player_names.end(), "player cid already in player names" );
										m_player_names[ event.cid ] = ""; // to be queried
										{
											Packet packet;
											packet.type = Packet::PT_REQUEST_AUTH; // ask to authenticate
											network->MT_SendPacket( packet, event.cid );
										}
										break;
									}
									case Event::ET_CLIENT_DISCONNECT: {
										Log( std::to_string( event.cid ) + " disconnected" );
										auto it = m_player_names.find( event.cid );
										if ( it != m_player_names.end() ) {
											m_player_names.erase( it );
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
												}
												else {
													Log( "Got authentication from " + std::to_string( event.cid ) + ": " + packet.data.str );
													
													// update name
													ASSERT( m_player_names.find( event.cid ) != m_player_names.end(), "player cid not found" );
													m_player_names[ event.cid ] = packet.data.str;
													
													Log( "Sending global settings to " + std::to_string( event.cid ) );
													Packet p;
													p.type = Packet::PT_GLOBAL_SETTINGS;
													p.data.str = m_mainmenu->m_settings.global.Serialize().ToString();
													network->MT_SendPacket( p, event.cid );
													
													RefreshUI();
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
								break;
							}
							default: {
								ASSERT(false, "unknown network role " + std::to_string(m_mainmenu->m_settings.local.network_role));
							}
						}
					}
				}
			}
			m_getevents_mt_id = 0;
		}
	}
}
	
bool Lobby::OnCancel() {
	
	g_engine->GetNetwork()->MT_Disconnect();
	
	// reset anything received from server
	m_mainmenu->m_settings.global = m_settings_backup;
	
	return true;
}

void Lobby::RefreshUI() {
	m_map_settings_section->SetTitleText( m_mainmenu->m_settings.global.game_name );
	
	// TODO: buttons etc
	for ( auto& label : m_player_labels ) {
		m_players_section->RemoveChild( label.second );
	}
	m_player_labels.clear();
	for ( auto& name : m_player_names ) {
		// TODO: styles etc
		NEWV( label, Label );
			label->SetText( name.second );
			label->SetFont( g_engine->GetFontLoader()->LoadFont( "arialnb.ttf", 18 ) );
			label->SetTextColor( Color::FromRGB(
				m_mainmenu->GetRandom()->GetUInt( 100, 155 ),
				m_mainmenu->GetRandom()->GetUInt( 100, 155 ),
				m_mainmenu->GetRandom()->GetUInt( 100, 155 )
			));
			label->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
			label->SetLeft( 10 );
			label->SetTop( 6 + m_player_labels.size() * 22 );
		m_players_section->AddChild( label );
		m_player_labels[ name.first ] = label;
	}
	
	if ( m_mainmenu->m_settings.local.network_role == LocalSettings::NR_SERVER ) {
		// update UIs of others aswell
		// TODO: optimize?
		
		Packet p;
		p.type = Packet::PT_PLAYERS;
		for ( auto& it : m_player_names ) {
			p.data.vec.push_back( it.second );
		}
		
		Log( "Sending player list to all players" );
		for ( auto& it : m_player_names ) {
			if ( it.first != 0 ) { // don't send to self
				g_engine->GetNetwork()->MT_SendPacket( p, it.first );
			}
		}
	}

}

}
}
