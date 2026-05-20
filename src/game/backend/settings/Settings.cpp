#include "Settings.h"

#include "util/FS.h"

#include "gse/value/Float.h"
#include "gse/value/Ptr.h"

namespace game {
namespace backend {
namespace settings {

WRAPMAP( type, MapSettings::type_t,
	{ MapSettings::MT_RANDOM, "random" },
	{ MapSettings::MT_CUSTOM, "custom" },
	{ MapSettings::MT_MAPFILE, "mapfile" },
)

WRAPIMPL_DYNAMIC_GETTERS( MapSettings )
			WRAPIMPL_GET_MAPPED( type )
			WRAPIMPL_GET_PTR( "filename", filename )
			WRAPIMPL_GET_PTR( "size_x", size_x )
			WRAPIMPL_GET_PTR( "size_y", size_y )
			WRAPIMPL_GET_PTR( "ocean_coverage", ocean_coverage )
			WRAPIMPL_GET_PTR( "erosive_forces", erosive_forces )
			WRAPIMPL_GET_PTR( "native_lifeforms", native_lifeforms )
			WRAPIMPL_GET_PTR( "cloud_cover", cloud_cover )
WRAPIMPL_DYNAMIC_SETTERS( MapSettings )
	WRAPIMPL_SET_MAPPED( type )
	WRAPIMPL_SET_PTR( "filename", String, filename )
	WRAPIMPL_SET_PTR( "size_x", Int, size_x )
	WRAPIMPL_SET_PTR( "size_y", Int, size_y )
	WRAPIMPL_SET_PTR( "ocean_coverage", Float, ocean_coverage )
	WRAPIMPL_SET_PTR( "erosive_forces", Float, erosive_forces )
	WRAPIMPL_SET_PTR( "native_lifeforms", Float, native_lifeforms )
	WRAPIMPL_SET_PTR( "cloud_cover", Float, cloud_cover )
WRAPIMPL_DYNAMIC_ON_SET( MapSettings )
WRAPIMPL_DYNAMIC_END()

const types::Buffer MapSettings::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( type );
	buf.WriteString( util::FS::GetBaseName( filename ) ); // don't send full path for security reasons, nobody needs it anyway
	buf.WriteInt( size_x );
	buf.WriteInt( size_y );
	buf.WriteFloat( ocean_coverage );
	buf.WriteFloat( erosive_forces );
	buf.WriteFloat( native_lifeforms );
	buf.WriteFloat( cloud_cover );

	return buf;
}

void MapSettings::Deserialize( types::Buffer buf ) {
	type = (type_t)buf.ReadInt();
	filename = buf.ReadString();
	size_x = buf.ReadInt();
	size_y = buf.ReadInt();
	ocean_coverage = buf.ReadFloat();
	erosive_forces = buf.ReadFloat();
	native_lifeforms = buf.ReadFloat();
	cloud_cover = buf.ReadFloat();
}

void GlobalSettings::Initialize() {
	rules.Initialize();
	difficulty_level = rules.GetDefaultDifficultyLevelV();
}

WRAPIMPL_DYNAMIC_GETTERS( GlobalSettings )
			WRAPIMPL_GET_WRAPPED( map )
			WRAPIMPL_GET_WRAPPED( rules )
			WRAPIMPL_GET_MAPPED_CUSTOM( difficulty_level, rules.m_difficulty_levels )
			WRAPIMPL_GET_PTR( "game_name", game_name )
WRAPIMPL_DYNAMIC_SETTERS( GlobalSettings )
	WRAPIMPL_SET_PTR( "game_name", String, game_name )
	WRAPIMPL_SET_MAPPED_CUSTOM( difficulty_level, obj->rules.m_difficulty_levels )
WRAPIMPL_DYNAMIC_ON_SET( GlobalSettings )
WRAPIMPL_DYNAMIC_END()

const types::Buffer GlobalSettings::Serialize() const {
	types::Buffer buf;

	buf.WriteString( map.Serialize().ToString() );
	buf.WriteString( rules.Serialize().ToString() );
	buf.WriteInt( difficulty_level );
	buf.WriteString( game_name );

	return buf;
}

void GlobalSettings::Deserialize( types::Buffer buf ) {
	map.Deserialize( buf.ReadString() );
	rules.Deserialize( buf.ReadString() );
	difficulty_level = buf.ReadInt();
	game_name = buf.ReadString();
}

WRAPMAP( game_mode, LocalSettings::game_mode_t,
	{ LocalSettings::GM_SINGLEPLAYER, "single" },
	{ LocalSettings::GM_MULTIPLAYER, "multi" },
	{ LocalSettings::GM_SCENARIO, "scenario" },
)

WRAPMAP( network_type, LocalSettings::network_type_t,
	{ LocalSettings::NT_NONE, "none" },
	{ LocalSettings::NT_SIMPLE_TCPIP, "simple_tcpip" },
	{ LocalSettings::NT_HOTSEAT, "hotseat" },
)

WRAPMAP( network_role, LocalSettings::network_role_t,
	{ LocalSettings::NR_NONE, "none" },
	{ LocalSettings::NR_CLIENT, "client" },
	{ LocalSettings::NR_SERVER, "server" },
)

WRAPIMPL_DYNAMIC_GETTERS( LocalSettings )
			{
				"account",
				account.Wrap( GSE_CALL ),
			},
			WRAPIMPL_GET_MAPPED( game_mode )
			WRAPIMPL_GET_MAPPED( network_type )
			WRAPIMPL_GET_MAPPED( network_role )
			WRAPIMPL_GET_PTR( "player_name", player_name )
			WRAPIMPL_GET_PTR( "remote_address", remote_address )
WRAPIMPL_DYNAMIC_SETTERS( LocalSettings )
	WRAPIMPL_SET_MAPPED( game_mode )
	WRAPIMPL_SET_MAPPED( network_type )
	WRAPIMPL_SET_MAPPED( network_role )
	WRAPIMPL_SET_PTR( "player_name", String, player_name )
	WRAPIMPL_SET_PTR( "remote_address", String, remote_address )
WRAPIMPL_DYNAMIC_ON_SET( LocalSettings )
WRAPIMPL_DYNAMIC_END()

const types::Buffer LocalSettings::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( game_mode );
	buf.WriteInt( network_type );
	buf.WriteInt( network_role );
	buf.WriteString( player_name );
	buf.WriteString( remote_address );

	return buf;
}

void LocalSettings::Deserialize( types::Buffer buf ) {
	game_mode = (game_mode_t)buf.ReadInt();
	network_type = (network_type_t)buf.ReadInt();
	network_role = (network_role_t)buf.ReadInt();
	player_name = buf.ReadString();
	remote_address = buf.ReadString();
}

WRAPIMPL_BEGIN( Settings )
	WRAPIMPL_PROPS
			{
				"global",
				global.Wrap( GSE_CALL ),
			},
			{
				"local",
				local.Wrap( GSE_CALL ),
			},
		};
WRAPIMPL_END_PTR()

const types::Buffer Settings::Serialize() const {
	types::Buffer buf;

	buf.WriteString( global.Serialize().ToString() );
	buf.WriteString( local.Serialize().ToString() );

	return buf;
}

void Settings::Deserialize( types::Buffer buf ) {
	global.Deserialize( types::Buffer( buf.ReadString() ) );
	local.Deserialize( types::Buffer( buf.ReadString() ) );
}

}
}
}
