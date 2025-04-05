#include "Settings.h"

#include "util/FS.h"

#include "gse/value/Float.h"

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
			WRAPIMPL_GET( filename, String )
			WRAPIMPL_GET( size_x, Int )
			WRAPIMPL_GET( size_y, Int )
			WRAPIMPL_GET( ocean_coverage, Float )
			WRAPIMPL_GET( erosive_forces, Float )
			WRAPIMPL_GET( native_lifeforms, Float )
			WRAPIMPL_GET( cloud_cover, Float )
WRAPIMPL_DYNAMIC_SETTERS( MapSettings )
	WRAPIMPL_SET_MAPPED( type )
	WRAPIMPL_SET( filename, String )
	WRAPIMPL_SET( size_x, Int )
	WRAPIMPL_SET( size_y, Int )
	WRAPIMPL_SET( ocean_coverage, Float )
	WRAPIMPL_SET( erosive_forces, Float )
	WRAPIMPL_SET( native_lifeforms, Float )
	WRAPIMPL_SET( cloud_cover, Float )
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

void MapSettings::Unserialize( types::Buffer buf ) {
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
			WRAPIMPL_GET( game_name, String )
WRAPIMPL_DYNAMIC_SETTERS( GlobalSettings )
	WRAPIMPL_SET( game_name, String )
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

void GlobalSettings::Unserialize( types::Buffer buf ) {
	map.Unserialize( buf.ReadString() );
	rules.Unserialize( buf.ReadString() );
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
	{ LocalSettings::NT_SIMPLETCP, "tcp" },
)

WRAPMAP( network_role, LocalSettings::network_role_t,
	{ LocalSettings::NR_NONE, "none" },
	{ LocalSettings::NR_CLIENT, "client" },
	{ LocalSettings::NR_SERVER, "server" },
)

WRAPIMPL_DYNAMIC_GETTERS( LocalSettings )
			WRAPIMPL_GET_MAPPED( game_mode )
			WRAPIMPL_GET_MAPPED( network_type )
			WRAPIMPL_GET_MAPPED( network_role )
			WRAPIMPL_GET( player_name, String )
			WRAPIMPL_GET( remote_address, String )
WRAPIMPL_DYNAMIC_SETTERS( LocalSettings )
	WRAPIMPL_SET_MAPPED( game_mode )
	WRAPIMPL_SET_MAPPED( network_type )
	WRAPIMPL_SET_MAPPED( network_role )
	WRAPIMPL_SET( player_name, String )
	WRAPIMPL_SET( remote_address, String )
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

void LocalSettings::Unserialize( types::Buffer buf ) {
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
				global.Wrap( GSE_CALL, true ),
			},
			{
				"local",
				local.Wrap( GSE_CALL, true ),
			},
		};
WRAPIMPL_END_PTR()

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
}
