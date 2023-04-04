#include "Settings.h"

namespace task {

const Buffer MapSettings::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( type );
	buf.WriteInt( size );
	buf.WriteInt( custom_size.x );
	buf.WriteInt( custom_size.y );
	buf.WriteInt( ocean );
	buf.WriteInt( erosive );
	buf.WriteInt( lifeforms );
	buf.WriteInt( clouds );
	
	return buf;
}

void MapSettings::Unserialize( Buffer buf ) {
	type = (type_t) buf.ReadInt();
	size = buf.ReadInt();
	custom_size.x = buf.ReadInt();
	custom_size.y = buf.ReadInt();
	ocean = buf.ReadInt();
	erosive = buf.ReadInt();
	lifeforms = buf.ReadInt();
	clouds = buf.ReadInt();
}

const Buffer GlobalSettings::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( game_mode );
	buf.WriteString( map.Serialize().ToString() );
	buf.WriteInt( difficulty );
	buf.WriteInt( game_rules );
	buf.WriteInt( network_type );
	buf.WriteString( game_name );
	
	return buf;	
}

void GlobalSettings::Unserialize( Buffer buf ) {
	game_mode = (game_mode_t) buf.ReadInt();
	map.Unserialize( buf.ReadString() );
	difficulty = buf.ReadInt();
	game_rules = (game_rules_t) buf.ReadInt();
	network_type = (network_type_t) buf.ReadInt();
	game_name = buf.ReadString();
}


const Buffer LocalSettings::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( network_role );
	buf.WriteString( player_name );
	buf.WriteString( remote_address );
	buf.WriteString( map_file );
		
	return buf;
}

void LocalSettings::Unserialize( Buffer buf ) {
	network_role = (network_role_t) buf.ReadInt();
	player_name = buf.ReadString();
	remote_address = buf.ReadString();
	map_file = buf.ReadString();
}


const Buffer Settings::Serialize() const {
	Buffer buf;
	
	buf.WriteString( global.Serialize().ToString() );
	buf.WriteString( local.Serialize().ToString() );
	
	return buf;
}

void Settings::Unserialize( Buffer buf ) {
	global.Unserialize( Buffer( buf.ReadString() ) );
	local.Unserialize( Buffer( buf.ReadString() ) );
}

}
