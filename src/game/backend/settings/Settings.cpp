#include "Settings.h"

#include <cmath>

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
	const auto serialized_type = buf.ReadInt();
	const auto serialized_filename = buf.ReadString();
	const auto serialized_size_x = buf.ReadInt();
	const auto serialized_size_y = buf.ReadInt();
	const auto serialized_ocean_coverage = buf.ReadFloat();
	const auto serialized_erosive_forces = buf.ReadFloat();
	const auto serialized_native_lifeforms = buf.ReadFloat();
	const auto serialized_cloud_cover = buf.ReadFloat();
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized map settings" );
	}
	if ( serialized_type < MT_RANDOM || serialized_type > MT_MAPFILE ) {
		THROW( "invalid serialized map type: " + std::to_string( serialized_type ) );
	}
	if (
		serialized_filename != util::FS::GetBaseName( serialized_filename ) ||
		serialized_filename.find( '/' ) != std::string::npos ||
		serialized_filename.find( '\\' ) != std::string::npos
	) {
		THROW( "serialized map filename contains directory components" );
	}
	if ( serialized_type == MT_MAPFILE && serialized_filename.empty() ) {
		THROW( "serialized map file is empty" );
	}
	if (
		serialized_size_x < static_cast< int64_t >( MAP_MIN_DIMENSION ) ||
		serialized_size_y < static_cast< int64_t >( MAP_MIN_DIMENSION ) ||
		( serialized_size_x & 1 ) || ( serialized_size_y & 1 ) ||
		serialized_size_x > static_cast< int64_t >( MAP_MAX_AREA ) ||
		serialized_size_y > static_cast< int64_t >( MAP_MAX_AREA ) ||
		static_cast< uint64_t >( serialized_size_x ) * static_cast< uint64_t >( serialized_size_y ) > MAP_MAX_AREA
	) {
		THROW( "invalid serialized map dimensions" );
	}
	const auto validate_fraction = []( const float value, const std::string& name ) {
		if ( !std::isfinite( value ) || value < 0.0f || value > 1.0f ) {
			THROW( "invalid serialized map " + name );
		}
	};
	validate_fraction( serialized_ocean_coverage, "ocean coverage" );
	validate_fraction( serialized_erosive_forces, "erosive forces" );
	validate_fraction( serialized_native_lifeforms, "native lifeforms" );
	validate_fraction( serialized_cloud_cover, "cloud cover" );

	type = static_cast< type_t >( serialized_type );
	filename = serialized_filename;
	size_x = serialized_size_x;
	size_y = serialized_size_y;
	ocean_coverage = serialized_ocean_coverage;
	erosive_forces = serialized_erosive_forces;
	native_lifeforms = serialized_native_lifeforms;
	cloud_cover = serialized_cloud_cover;
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
	const auto serialized_map = buf.ReadString();
	const auto serialized_rules = buf.ReadString();
	const auto serialized_difficulty_level = buf.ReadInt< int >( "difficulty level" );
	const auto serialized_game_name = buf.ReadString();
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized global settings" );
	}

	MapSettings parsed_map;
	parsed_map.Deserialize( types::Buffer( serialized_map ) );
	game::backend::rules::Default parsed_rules;
	parsed_rules.Deserialize( types::Buffer( serialized_rules ) );
	if ( parsed_rules.m_difficulty_levels.GetVK().find( serialized_difficulty_level ) == parsed_rules.m_difficulty_levels.GetVK().end() ) {
		THROW( "invalid serialized difficulty level: " + std::to_string( serialized_difficulty_level ) );
	}

	map = parsed_map;
	rules.Deserialize( types::Buffer( serialized_rules ) );
	difficulty_level = serialized_difficulty_level;
	game_name = serialized_game_name;
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
	const auto serialized_game_mode = buf.ReadInt();
	const auto serialized_network_type = buf.ReadInt();
	const auto serialized_network_role = buf.ReadInt();
	const auto serialized_player_name = buf.ReadString();
	const auto serialized_remote_address = buf.ReadString();
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized local settings" );
	}
	if ( serialized_game_mode < GM_SINGLEPLAYER || serialized_game_mode > GM_SCENARIO ) {
		THROW( "invalid serialized game mode: " + std::to_string( serialized_game_mode ) );
	}
	if ( serialized_network_type < NT_NONE || serialized_network_type > NT_HOTSEAT ) {
		THROW( "invalid serialized network type: " + std::to_string( serialized_network_type ) );
	}
	if ( serialized_network_role < NR_NONE || serialized_network_role > NR_CLIENT ) {
		THROW( "invalid serialized network role: " + std::to_string( serialized_network_role ) );
	}

	game_mode = static_cast< game_mode_t >( serialized_game_mode );
	network_type = static_cast< network_type_t >( serialized_network_type );
	network_role = static_cast< network_role_t >( serialized_network_role );
	player_name = serialized_player_name;
	remote_address = serialized_remote_address;
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
	const auto serialized_global = buf.ReadString();
	const auto serialized_local = buf.ReadString();
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized settings" );
	}
	GlobalSettings parsed_global;
	parsed_global.Deserialize( types::Buffer( serialized_global ) );
	LocalSettings parsed_local;
	parsed_local.Deserialize( types::Buffer( serialized_local ) );

	global.Deserialize( types::Buffer( serialized_global ) );
	local.Deserialize( types::Buffer( serialized_local ) );
}

}
}
}
