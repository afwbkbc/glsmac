#include "Settings.h"

namespace game {

const Buffer GlobalSettings::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( game_mode );
	buf.WriteInt( map_type );
	buf.WriteInt( map_size );
	buf.WriteInt( map_custom_size.width );
	buf.WriteInt( map_custom_size.height );
	buf.WriteInt( map_ocean );
	buf.WriteInt( map_erosive );
	buf.WriteInt( map_lifeforms );
	buf.WriteInt( map_clouds );
	buf.WriteInt( difficulty );
	buf.WriteInt( game_rules );
	buf.WriteInt( network_type );
	buf.WriteString( game_name );
	
	return buf;	
}

void GlobalSettings::Unserialize( Buffer buf ) {
	game_mode = (game_mode_t) buf.ReadInt();
	map_type = (map_type_t) buf.ReadInt();
	map_size = buf.ReadInt();
	map_custom_size.width = buf.ReadInt();
	map_custom_size.height = buf.ReadInt();
	map_ocean = buf.ReadInt();
	map_erosive = buf.ReadInt();
	map_lifeforms = buf.ReadInt();
	map_clouds = buf.ReadInt();
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
	
	return buf;
}

void LocalSettings::Unserialize( Buffer buf ) {
	network_role = (network_role_t) buf.ReadInt();
	player_name = buf.ReadString();
	remote_address = buf.ReadString();
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
