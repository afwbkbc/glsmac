#include "Settings.h"

#include "util/FS.h"

namespace game {
namespace settings {

const types::Buffer MapSettings::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( type );
	buf.WriteString( util::FS::GetBaseName( filename ) ); // don't send full path for security reasons, nobody needs it anyway
	buf.WriteInt( size );
	buf.WriteInt( custom_size.x );
	buf.WriteInt( custom_size.y );
	buf.WriteInt( ocean );
	buf.WriteInt( erosive );
	buf.WriteInt( lifeforms );
	buf.WriteInt( clouds );

	return buf;
}

void MapSettings::Unserialize( types::Buffer buf ) {
	type = (type_t)buf.ReadInt();
	filename = buf.ReadString();
	size = buf.ReadInt();
	custom_size.x = buf.ReadInt();
	custom_size.y = buf.ReadInt();
	ocean = buf.ReadInt();
	erosive = buf.ReadInt();
	lifeforms = buf.ReadInt();
	clouds = buf.ReadInt();
}

void GlobalSettings::Initialize() {
	game_rules.Initialize();
	global_difficulty = game_rules.GetDefaultDifficultyLevel();
}

const types::Buffer GlobalSettings::Serialize() const {
	types::Buffer buf;

	buf.WriteString( map.Serialize().ToString() );
	buf.WriteString( game_rules.Serialize().ToString() );
	buf.WriteString( global_difficulty.Serialize().ToString() );
	buf.WriteString( game_name );

	return buf;
}

void GlobalSettings::Unserialize( types::Buffer buf ) {
	map.Unserialize( buf.ReadString() );
	game_rules.Unserialize( buf.ReadString() );
	global_difficulty.Unserialize( buf.ReadString() );
	game_name = buf.ReadString();
}

const types::Buffer LocalSettings::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( game_mode );
	buf.WriteInt( network_type );
	buf.WriteInt( network_role );
	buf.WriteString( player_name );
	buf.WriteString( remote_address );

	return buf;
}

void LocalSettings::Unserialize( types::Buffer buf ) {
	game_mode = (game_mode_t)buf.ReadInt();
	network_type = (network_type_t)buf.ReadInt();
	network_role = (network_role_t)buf.ReadInt();
	player_name = buf.ReadString();
	remote_address = buf.ReadString();
}

const types::Buffer Settings::Serialize() const {
	types::Buffer buf;

	buf.WriteString( global.Serialize().ToString() );
	buf.WriteString( local.Serialize().ToString() );

	return buf;
}

void Settings::Unserialize( types::Buffer buf ) {
	global.Unserialize( types::Buffer( buf.ReadString() ) );
	local.Unserialize( types::Buffer( buf.ReadString() ) );
}

}
}
