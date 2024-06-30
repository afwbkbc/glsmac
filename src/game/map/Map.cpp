#include "Map.h"

#include "game/Game.h"
#include "game/settings/Settings.h"
#include "generator/SimplePerlin.h"
#include "engine/Engine.h"
#include "config/Config.h"
#include "util/random/Random.h"
#include "util/FS.h"
#include "ui/UI.h"
#include "loader/texture/TextureLoader.h"
#include "module/Prepare.h"
#include "module/LandMoisture.h"
#include "module/LandSurface.h"
#include "module/LandSurfacePP.h"
#include "module/WaterSurface.h"
#include "module/WaterSurfacePP.h"
#include "module/CalculateCoords.h"
#include "module/Coastlines1.h"
#include "module/Coastlines2.h"
#include "module/Sprites.h"
#include "module/Finalize.h"
#include "types/texture/Texture.h"
#include "types/mesh/Render.h"
#include "types/mesh/Data.h"
#include "game/State.h"
#include "game/map/MapState.h"
#include "game/map/tile/Tiles.h"
#include "Consts.h"

#ifdef DEBUG

#include "util/Timer.h"

#endif

namespace game {
namespace map {

#define B( x ) S_to_binary_(#x)

static inline unsigned char S_to_binary_( const char* s ) {
	unsigned long long i = 0;
	while ( *s ) {
		i <<= 1;
		i += *s++ - '0';
	}
	return i;
}

Map::Map( Game* game )
	: m_game( game ) {
	// add texture variant bitmap maps
	CalculateTextureVariants(
		TVT_TILES, {
			{ B( 00000000 ), B( 10101010 ) }, // 0
			{ B( 00001000 ), B( 10101010 ) }, // 1
			{ B( 00101000 ), B( 10111010 ) }, // 2
			{ B( 00111000 ), B( 10111010 ) }, // 3
			{ B( 10001000 ), B( 10101010 ) }, // 4
			{ B( 10101000 ), B( 11111010 ) }, // 5
			{ B( 10111000 ), B( 11111010 ) }, // 6
			{ B( 11101000 ), B( 11111010 ) }, // 7
			{ B( 11111000 ), B( 11111010 ) }, // 8
			{ B( 10101010 ), B( 11111111 ) }, // 9
			{ B( 10101110 ), B( 11111111 ) }, // 10
			{ B( 10111110 ), B( 11111111 ) }, // 11
			{ B( 11101110 ), B( 11111111 ) }, // 12
			{ B( 11111110 ), B( 11111111 ) }, // 13
			{ B( 11111111 ), B( 11111111 ) }, // 14
			// 15 ???
		}
	);
	CalculateTextureVariants(
		TVT_RIVERS_FORESTS, {
			{ B( 00000000 ), B( 10101010 ) }, // 0
			{ B( 00100000 ), B( 10101010 ) }, // 1
			{ B( 10000000 ), B( 10101010 ) }, // 2
			{ B( 10100000 ), B( 10101010 ) }, // 3
			{ B( 00000010 ), B( 10101010 ) }, // 4
			{ B( 00100010 ), B( 10101010 ) }, // 5
			{ B( 10000010 ), B( 10101010 ) }, // 6
			{ B( 10100010 ), B( 10101010 ) }, // 7
			{ B( 00001000 ), B( 10101010 ) }, // 8
			{ B( 00101000 ), B( 10101010 ) }, // 9
			{ B( 10001000 ), B( 10101010 ) }, // 10
			{ B( 10101000 ), B( 10101010 ) }, // 11
			{ B( 00001010 ), B( 10101010 ) }, // 12
			{ B( 00101010 ), B( 10101010 ) }, // 13
			{ B( 10001010 ), B( 10101010 ) }, // 14
			{ B( 10101010 ), B( 10101010 ) }, // 15
		}
	);

	// main source textures
	m_textures.source.texture_pcx = g_engine->GetTextureLoader()->LoadTexture( resource::PCX_TEXTURE );
	m_textures.source.ter1_pcx = g_engine->GetTextureLoader()->LoadTexture( resource::PCX_TER1 );

	// add map modules
	//   order of passes is important
	//   order of modules within pass is important too

	module::Module* m;
	module_pass_t module_pass;

	{ // prepare tile states
		module_pass.clear();
		NEW( m, module::Prepare, this );
		module_pass.push_back( m ); // needs to always be first
		m_modules.push_back( module_pass );
	}

	{ // needs to be in separate pass because moisture original textures need to be available for all tiles in next pass
		module_pass.clear();
		NEW( m, module::LandMoisture, this );
		module_pass.push_back( m );
		m_modules.push_back( module_pass );
	}

	{ // main pass
		module_pass.clear();
		NEW( m, module::LandSurface, this );
		module_pass.push_back( m );
		NEW( m, module::WaterSurface, this );
		module_pass.push_back( m );
		NEW( m, module::CalculateCoords, this );
		module_pass.push_back( m );
		NEW( m, module::Coastlines1, this );
		module_pass.push_back( m );
		m_modules.push_back( module_pass );
	}

	{ // second pass. needed to read and write tile states of tile or neighbors after they are fully generated in main pass
		module_pass.clear();
		NEW( m, module::Coastlines2, this );
		module_pass.push_back( m );
		m_modules.push_back( module_pass );
	}

	{ // finalize and write vertices to mesh
		module_pass.clear();
		NEW( m, module::Finalize, this );
		module_pass.push_back( m );
		m_modules.push_back( module_pass );
	}

	// deferred passes (after all other passes and deferred calls are processed)
	{
		module_pass.clear();
		NEW( m, module::LandSurfacePP, this );
		module_pass.push_back( m );
		m_modules_deferred.push_back( module_pass );
	}
	{
		module_pass.clear();
		NEW( m, module::WaterSurfacePP, this );
		module_pass.push_back( m );
		NEW( m, module::Sprites, this );
		module_pass.push_back( m );
		m_modules_deferred.push_back( module_pass );
	}
}

Map::~Map() {
	if ( m_tiles ) {
		DELETE( m_tiles );
	}
	for ( auto& module_pass : m_modules ) {
		for ( auto& m : module_pass ) {
			DELETE( m );
		}
	}
	for ( auto& module_pass : m_modules_deferred ) {
		for ( auto& m : module_pass ) {
			DELETE( m );
		}
	}
	if ( m_map_state ) {
		DELETE( m_map_state );
	}
}

const types::Buffer Map::Serialize() const {

	types::Buffer buf;

	buf.WriteString( m_tiles->Serialize().ToString() );
	buf.WriteString( m_map_state->Serialize().ToString() );

	buf.WriteString( m_meshes.terrain->Serialize().ToString() );
	buf.WriteString( m_meshes.terrain_data->Serialize().ToString() );

	buf.WriteString( m_textures.terrain->Serialize().ToString() );

	buf.WriteInt( m_sprite_actors.size() );
	for ( auto& it : m_sprite_actors ) {
		buf.WriteString( SerializeSpriteActor( it.second ).ToString() );
		buf.WriteString( it.first );
	}
	buf.WriteInt( m_sprite_instances.size() );
	for ( auto& it : m_sprite_instances ) {
		buf.WriteString( it.second.first );
		buf.WriteVec3( it.second.second );
		buf.WriteInt( it.first );
	}
	buf.WriteInt( m_next_sprite_instance_id );

	return buf;
}

void Map::Unserialize( types::Buffer buf ) {

	ASSERT( !m_tiles, "tiles already set" );
	NEW( m_tiles, tile::Tiles );
	m_tiles->Unserialize( buf.ReadString() );

	ASSERT( !m_map_state, "map state already set" );
	NEW( m_map_state, MapState );
	m_map_state->Unserialize( buf.ReadString() );

	InitTextureAndMesh();
	m_meshes.terrain->Unserialize( buf.ReadString() );
	m_meshes.terrain_data->Unserialize( buf.ReadString() );
	m_textures.terrain->Unserialize( buf.ReadString() );

	size_t sz = buf.ReadInt();
	m_sprite_actors.clear();
	for ( auto i = 0 ; i < sz ; i++ ) {
		m_sprite_actors[ buf.ReadString() ] = UnserializeSpriteActor( buf.ReadString() );
	}

	sz = buf.ReadInt();
	m_sprite_instances.clear();
	for ( auto i = 0 ; i < sz ; i++ ) {
		m_sprite_instances[ buf.ReadInt() ] = {
			buf.ReadString(),
			buf.ReadVec3()
		};
	}

	m_next_sprite_instance_id = buf.ReadInt();

	m_sprite_actors_to_add.clear();
	m_sprite_instances_to_remove.clear();
	m_sprite_instances_to_add.clear();
}

const std::string& Map::GetErrorString( const error_code_t& code ) {

	static const std::unordered_map< error_code_t, const std::string > m_error_code_strings = {
		{ EC_UNKNOWN,              "Unknown error" },
		{ EC_MAPFILE_FORMAT_ERROR, "Invalid map file format" }
	};

	auto it = m_error_code_strings.find( code );
	if ( it == m_error_code_strings.end() ) {
		it = m_error_code_strings.find( EC_UNKNOWN );
	}
	return it->second;
}

tile::Tile* Map::GetTile( const size_t x, const size_t y ) const {
	ASSERT( m_tiles, "tiles not set" );
	return &m_tiles->At( x, y );
}

tile::TileState* Map::GetTileState( const size_t x, const size_t y ) const {
	ASSERT( m_map_state, "map state not set" );
	return m_map_state->At( x, y );
}

tile::TileState* Map::GetTileState( const tile::Tile* tile ) const {
	// TODO: link by pointer?
	return GetTileState( tile->coord.x, tile->coord.y );
}

const MapState* Map::GetMapState() const {
	ASSERT( m_map_state, "map state not set" );
	return m_map_state;
}

void Map::ClearTexture() {
	ASSERT( m_map_state, "map state not set" );
	ASSERT( m_current_ts, "ClearTexture called outside of tile generation" );
	for ( auto lt = 0 ; lt < tile::LAYER_MAX ; lt++ ) {
		m_textures.terrain->Erase(
			m_current_ts->tex_coord.x1,
			lt * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + m_current_ts->tex_coord.y1,
			m_current_ts->tex_coord.x2 - 1,
			lt * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + m_current_ts->tex_coord.y2 - 1
		);
	}
}

void Map::AddTexture( const tile::tile_layer_type_t tile_layer, const pcx_texture_coordinates_t& tc, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha, util::Perlin* perlin ) {
	ASSERT( m_map_state, "map state not set" );
	ASSERT( m_current_ts, "AddTexture called outside of tile generation" );
	m_textures.terrain->AddFrom(
		m_textures.source.texture_pcx,
		mode,
		tc.x,
		tc.y,
		tc.x + s_consts.tc.texture_pcx.dimensions.x - 1,
		tc.y + s_consts.tc.texture_pcx.dimensions.y - 1,
		m_current_ts->tex_coord.x1,
		tile_layer * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + m_current_ts->tex_coord.y1,
		rotate,
		alpha,
		GetRandom(),
		perlin
	);
};

void Map::CopyTextureFromLayer( const tile::tile_layer_type_t tile_layer_from, const size_t tx_from, const size_t ty_from, const tile::tile_layer_type_t tile_layer, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha, util::Perlin* perlin ) {
	ASSERT( m_map_state, "map state not set" );
	ASSERT( m_current_ts, "CopyTextureFromLayer called outside of tile generation" );
	m_textures.terrain->AddFrom(
		m_textures.terrain,
		mode,
		tx_from,
		tile_layer_from * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + ty_from,
		tx_from + s_consts.tc.texture_pcx.dimensions.x - 1,
		tile_layer_from * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + ty_from + s_consts.tc.texture_pcx.dimensions.y - 1,
		m_current_ts->tex_coord.x1,
		tile_layer * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + m_current_ts->tex_coord.y1,
		rotate,
		alpha,
		GetRandom(),
		perlin
	);
};

void Map::CopyTexture( const tile::tile_layer_type_t tile_layer_from, const tile::tile_layer_type_t tile_layer, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha, util::Perlin* perlin ) {
	ASSERT( m_current_ts, "CopyTexture called outside of tile generation" );
	CopyTextureFromLayer(
		tile_layer_from,
		m_current_ts->tex_coord.x1,
		m_current_ts->tex_coord.y1,
		tile_layer,
		mode,
		rotate,
		alpha,
		perlin
	);
};

void Map::CopyTextureDeferred( const tile::tile_layer_type_t tile_layer_from, const size_t tx_from, const size_t ty_from, const tile::tile_layer_type_t tile_layer, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha, util::Perlin* perlin ) {
	ASSERT( m_map_state, "map state not set" );
	ASSERT( m_current_ts, "CopyTextureDeferred called outside of tile generation" );
	m_map_state->copy_from_after.push_back(
		{
			mode,
			tx_from,
			tile_layer_from * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + ty_from,
			tx_from + s_consts.tc.texture_pcx.dimensions.x - 1,
			tile_layer_from * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + ty_from + s_consts.tc.texture_pcx.dimensions.y - 1,
			(size_t)m_current_ts->tex_coord.x1,
			tile_layer * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + (size_t)m_current_ts->tex_coord.y1,
			rotate,
			alpha,
			perlin
		}
	);
};

void Map::GetTexture( types::texture::Texture* dest_texture, const pcx_texture_coordinates_t& tc, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha ) {
	ASSERT( m_current_ts, "GetTexture called outside of tile generation" );
	ASSERT( dest_texture->m_width == s_consts.tc.texture_pcx.dimensions.x, "tile dest texture width mismatch" );
	ASSERT( dest_texture->m_height == s_consts.tc.texture_pcx.dimensions.y, "tile dest texture height mismatch" );
	dest_texture->AddFrom(
		m_textures.source.texture_pcx,
		mode,
		tc.x,
		tc.y,
		tc.x + s_consts.tc.texture_pcx.dimensions.x - 1,
		tc.y + s_consts.tc.texture_pcx.dimensions.y - 1,
		0,
		0,
		rotate,
		alpha,
		GetRandom()
	);
}

void Map::GetTextureFromLayer( types::texture::Texture* dest_texture, const tile::tile_layer_type_t tile_layer, const size_t tx_from, const size_t ty_from, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha ) const {
	ASSERT( m_map_state, "map state not set" );
	ASSERT( dest_texture->m_width == s_consts.tc.texture_pcx.dimensions.x, "tile dest texture width mismatch" );
	ASSERT( dest_texture->m_height == s_consts.tc.texture_pcx.dimensions.y, "tile dest texture height mismatch" );
	dest_texture->AddFrom(
		m_textures.terrain,
		mode,
		tx_from,
		tile_layer * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + ty_from,
		tx_from + s_consts.tc.texture_pcx.dimensions.x - 1,
		tile_layer * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + ty_from + s_consts.tc.texture_pcx.dimensions.y - 1,
		0,
		0,
		rotate,
		alpha,
		GetRandom()
	);
}

void Map::SetTexture( const tile::tile_layer_type_t tile_layer, tile::TileState* ts, types::texture::Texture* src_texture, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha ) {
	ASSERT( m_map_state, "map state not set" );
	ASSERT( m_current_ts, "SetTexture called outside of tile generation" );
	ASSERT( src_texture->m_width == s_consts.tc.texture_pcx.dimensions.x, "tile src texture width mismatch" );
	ASSERT( src_texture->m_height == s_consts.tc.texture_pcx.dimensions.y, "tile src texture height mismatch" );
	m_textures.terrain->AddFrom(
		src_texture,
		mode,
		0,
		0,
		s_consts.tc.texture_pcx.dimensions.x - 1,
		s_consts.tc.texture_pcx.dimensions.x - 1,
		ts->tex_coord.x1,
		tile_layer * m_map_state->dimensions.y * s_consts.tc.texture_pcx.dimensions.y + ts->tex_coord.y1,
		rotate,
		alpha,
		GetRandom()
	);
}

void Map::SetTexture( const tile::tile_layer_type_t tile_layer, types::texture::Texture* src_texture, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha ) {
	SetTexture( tile_layer, m_current_ts, src_texture, mode, rotate, alpha );
}

const Map::tile_texture_info_t Map::GetTileTextureInfo( const texture_variants_type_t type, const tile::Tile* tile, const tile_grouping_criteria_t criteria, const uint16_t value ) const {
	ASSERT( m_current_ts, "GetTileTextureInfo called outside of tile generation" );
	Map::tile_texture_info_t info;

	bool matches[16];
	uint8_t idx = 0;
	for ( uint8_t i = 0 ; i < 2 ; i++ ) {
		for ( auto& t : tile->neighbours ) {
			switch ( criteria ) {
				case TG_MOISTURE: {
					matches[ idx++ ] = t->moisture >= tile->moisture;
					break;
				}
				case TG_FEATURE: {
					matches[ idx++ ] = (
						( t->features & value ) == ( tile->features & value ) ||
							( type == TVT_RIVERS_FORESTS && !tile->is_water_tile && t->is_water_tile ) // rivers end in oceans
					);
					break;
				}
				case TG_TERRAFORMING: {
					matches[ idx++ ] = (
						( t->terraforming & value ) == ( tile->terraforming & value ) &&
							t->is_water_tile == tile->is_water_tile // terraforming doesn't continue into water
					);
					break;
				}
				default: {
					THROW( "invalid tile grouping criteria" );
				}
			}
		}
	}

	std::unordered_map< uint8_t, std::vector< uint8_t > > possible_variants = {}; // variant -> rotations

	const auto& variants = m_texture_variants.find( type );
	ASSERT( variants != m_texture_variants.end(), "texture variants for " + std::to_string( type ) + " not calculated" );

	for ( info.rotate_direction = 0 ; info.rotate_direction < 8 ; info.rotate_direction += 2 ) {
		if ( criteria == TG_TERRAFORMING && value == tile::TERRAFORMING_FOREST ) {
			// forests must always be vertical
			if ( info.rotate_direction != 6 ) {
				continue;
			}
		}
		uint8_t bitmask = 0;
		for ( uint8_t i = 0 ; i < 8 ; i++ ) {
			bitmask |= matches[ i + info.rotate_direction ] << i;
		}
		const auto& it = variants->second.find( bitmask );
		if ( it != variants->second.end() ) {
			for ( auto& v : it->second ) {
				possible_variants[ v ].push_back( info.rotate_direction );
			}
		}
	}

	if ( !possible_variants.empty() ) {
		// pick random variant if multiple
		auto it = possible_variants.begin();
		std::advance( it, GetRandom()->GetUInt( 0, possible_variants.size() - 1 ) );
		info.texture_variant = it->first;
		info.rotate_direction = it->second[ GetRandom()->GetUInt( 0, it->second.size() - 1 ) ] / 2;
	}
	else {
		THROW( "could not find texture variant" );
	}

	info.texture_flags = types::texture::AM_DEFAULT;

	if ( type == TVT_TILES ) {
		if ( info.texture_variant >= 14 ) {
			// no important edges so we can shuffle harder
			info.texture_flags |=
				types::texture::AM_RANDOM_MIRROR_X |
					types::texture::AM_RANDOM_MIRROR_Y |
					types::texture::AM_RANDOM_STRETCH_SHUFFLE;
		}
		else {
			info.texture_flags |= types::texture::AM_RANDOM_STRETCH;
		}
	}
	return info;
}

util::random::Random* Map::GetRandom() const {
	return m_game->GetRandom();
}

const size_t Map::GetWidth() const {
	ASSERT( m_tiles, "tiles not set" );
	return m_tiles->GetWidth();
}

const size_t Map::GetHeight() const {
	ASSERT( m_tiles, "tiles not set" );
	return m_tiles->GetHeight();
}

tile::Tiles* Map::GetTilesPtr() const {
	ASSERT( m_tiles, "tiles not set" );
	return m_tiles;
}

const types::Buffer Map::SerializeSpriteActor( const sprite_actor_t& sprite_actor ) const {
	types::Buffer buf;

	buf.WriteString( sprite_actor.name );
	buf.WriteVec2u( sprite_actor.tex_coords );
	buf.WriteFloat( sprite_actor.z_index );

	return buf;
}

const sprite_actor_t Map::UnserializeSpriteActor( types::Buffer buf ) const {
	const auto name = buf.ReadString();
	const auto tex_coords = buf.ReadVec2u();
	const auto z_index = buf.ReadFloat();
	return sprite_actor_t{
		name,
		tex_coords,
		z_index
	};
}

const std::string Map::GetTerrainSpriteActor( const std::string& name, const pcx_texture_coordinates_t& tex_coords, const float z_index ) {
	const auto key = "Terrain_" + name + " " + tex_coords.ToString() + " " + ::game::map::s_consts.tc.ter1_pcx.dimensions.ToString();

	auto it = m_sprite_actors.find( key );
	if ( it == m_sprite_actors.end() ) {
		it = m_sprite_actors.insert(
			{
				key,
				{
					name,
					tex_coords,
					z_index
				}
			}
		).first;
	}

	if ( m_sprite_actors_to_add.find( key ) == m_sprite_actors_to_add.end() ) {
		m_sprite_actors_to_add.insert(
			{
				key,
				it->second
			}
		);
	}

	return key;
}

const size_t Map::AddTerrainSpriteActorInstance( const std::string& key, const types::Vec3& coords ) {
	ASSERT( m_sprite_actors.find( key ) != m_sprite_actors.end(), "actor not found" );
	const auto it = m_sprite_instances.insert(
		{
			m_next_sprite_instance_id,
			{
				key,
				coords
			}
		}
	).first->second;
	m_sprite_instances_to_add[ m_next_sprite_instance_id ] = it;
	return m_next_sprite_instance_id++;
}

void Map::RemoveTerrainSpriteActorInstance( const std::string& key, const size_t instance_id ) {
	ASSERT( m_sprite_actors.find( key ) != m_sprite_actors.end(), "actor not found" );
	ASSERT( m_sprite_instances_to_remove.find( instance_id ) == m_sprite_instances_to_remove.end(), "instance already pending removal" );
	m_sprite_instances_to_remove.insert(
		{
			instance_id,
			key
		}
	);
}

const Map::error_code_t Map::Generate( settings::MapSettings* map_settings, MT_CANCELABLE ) {
	auto* random = m_game->GetRandom();
	generator::SimplePerlin generator( random );
	types::Vec2< size_t > size = map_settings->size == settings::MAP_CONFIG_CUSTOM
		? map_settings->custom_size
		: map::s_consts.map_sizes.at( map_settings->size );
#ifdef DEBUG
	util::Timer timer;
	timer.Start();
	const auto* c = g_engine->GetConfig();
	if ( c->HasDebugFlag( config::Config::DF_QUICKSTART ) ) {
		if ( c->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_SIZE ) ) {
			size = c->GetQuickstartMapSize();
		}
		map_settings->ocean = c->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_OCEAN )
			? map_settings->ocean = c->GetQuickstartMapOcean()
			: map_settings->ocean = random->GetUInt( 1, 3 );
		map_settings->erosive = c->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_EROSIVE )
			? map_settings->erosive = c->GetQuickstartMapErosive()
			: map_settings->erosive = random->GetUInt( 1, 3 );
		map_settings->lifeforms = c->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_LIFEFORMS )
			? map_settings->lifeforms = c->GetQuickstartMapLifeforms()
			: map_settings->lifeforms = random->GetUInt( 1, 3 );
		map_settings->clouds = c->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_CLOUDS )
			? map_settings->clouds = c->GetQuickstartMapClouds()
			: map_settings->clouds = random->GetUInt( 1, 3 );
	}
#endif
	Log( "Generating map of size " + size.ToString() );
	ASSERT( !m_tiles, "tiles already set" );
	NEW( m_tiles, tile::Tiles, size.x, size.y );
	generator.Generate( m_tiles, map_settings, MT_C );
	if ( canceled ) {
		Log( "Map generation canceled" );
		DELETE( m_tiles );
		m_tiles = nullptr;
		return EC_ABORTED;
	}
#ifdef DEBUG
	Log( "Map generation took " + std::to_string( timer.GetElapsed().count() ) + "ms" );
	// if crash happens - it's handy to have a map file to reproduce it
	if ( !c->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_FILE ) ) { // no point saving if we just loaded it
		Log( (std::string)"Saving map to " + c->GetDebugPath() + s_consts.debug.lastmap_filename );
		util::FS::WriteFile( c->GetDebugPath() + s_consts.debug.lastmap_filename, m_tiles->Serialize().ToString() );
	}
#endif

	return EC_NONE;
}

const Map::error_code_t Map::LoadFromBuffer( types::Buffer& buffer ) {
	if ( m_tiles ) {
		DELETE( m_tiles );
	}
	NEW( m_tiles, tile::Tiles );
	try {
		m_tiles->Unserialize( buffer );
		return EC_NONE;
	}
	catch ( std::runtime_error& e ) {
		Log( e.what() );
		DELETE( m_tiles );
		m_tiles = nullptr;
		return EC_MAPFILE_FORMAT_ERROR;
	}
}

const Map::error_code_t Map::LoadFromFile( const std::string& path ) {
	ASSERT( util::FS::FileExists( path ), "map file \"" + path + "\" not found" );

	Log( "Loading map from " + path );
	auto b = types::Buffer( util::FS::ReadFile( path ) );
	return LoadFromBuffer( b );
}

void Map::SaveToBuffer( types::Buffer& buffer ) const {
	buffer.WriteString( m_tiles->Serialize().ToString() );
}

const Map::error_code_t Map::SaveToFile( const std::string& path ) const {
	try {
		util::FS::WriteFile( path, m_tiles->Serialize().ToString() );
		return EC_NONE;
	}
	catch ( std::runtime_error& e ) {
		return EC_MAPFILE_FORMAT_ERROR;
	}
}

const Map::error_code_t Map::Initialize( MT_CANCELABLE ) {
	ASSERT( m_tiles, "map tiles not set" );
	m_tiles->Validate( MT_C );
	MT_RETIFV( EC_ABORTED );

	Log( "Initializing map" );

	if ( m_map_state ) {
		DELETE( m_map_state );
	}
	NEW( m_map_state, MapState );

	m_map_state->dimensions = {
		m_tiles->GetWidth(),
		m_tiles->GetHeight()
	};
	m_map_state->coord = {
		-( s_consts.tile.scale.x * ( m_map_state->dimensions.x + 1 ) / 4 - s_consts.tile.radius.x ),
		-( s_consts.tile.scale.y * ( m_map_state->dimensions.y + 1 ) / 4 - s_consts.tile.radius.y )
	};
	m_map_state->variables.texture_scaling = {
		1.0f / s_consts.tc.texture_pcx.dimensions.x / ( m_map_state->dimensions.x + 1 ), // + 1 for overdraw column
		1.0f / s_consts.tc.texture_pcx.dimensions.y / m_map_state->dimensions.y / tile::LAYER_MAX
	};

	m_map_state->LinkTileStates( MT_C );
	MT_RETIFV( EC_ABORTED );

	InitTextureAndMesh();
	MT_RETIFV( EC_ABORTED );

	// some processing is only done on first run
	m_map_state->first_run = true;

	const auto tiles = m_tiles->GetVector( MT_C );
	MT_RETIFV( EC_ABORTED );

	LoadTiles( tiles, MT_C );
	MT_RETIFV( EC_ABORTED );

	m_meshes.terrain->Finalize();
	MT_RETIFV( EC_ABORTED );
	m_meshes.terrain_data->Finalize();
	MT_RETIFV( EC_ABORTED );

	FixNormals( tiles, MT_C );
	MT_RETIFV( EC_ABORTED );

	m_map_state->first_run = false;

	m_current_tile = nullptr;
	m_current_ts = nullptr;

	return EC_NONE;
}

void Map::InitTextureAndMesh() {

	if ( m_textures.terrain ) {
		DELETE( m_textures.terrain );
	}
	NEW( m_textures.terrain, types::texture::Texture, "TerrainTexture",
		( m_map_state->dimensions.x + 1 ) * s_consts.tc.texture_pcx.dimensions.x, // + 1 for overdraw_column
		( m_map_state->dimensions.y * tile::LAYER_MAX ) * s_consts.tc.texture_pcx.dimensions.y
	);

	// not deleting meshes because if they exist - it means they are already linked to actor and are deleted together when needed
	NEW( m_meshes.terrain, types::mesh::Render,
		( m_map_state->dimensions.x * tile::LAYER_MAX + 1 ) * m_map_state->dimensions.y * 5 / 2, // + 1 for overdraw column
		( m_map_state->dimensions.x * tile::LAYER_MAX + 1 ) * m_map_state->dimensions.y * 4 / 2 // + 1 for overdraw column
	);
	NEW( m_meshes.terrain_data, types::mesh::Data, // data mesh has only one layer and no overdraw column
		m_map_state->dimensions.x * m_map_state->dimensions.y * 5 / 2,
		m_map_state->dimensions.x * m_map_state->dimensions.y * 4 / 2
	);

	// TODO: refactor?
	m_map_state->terrain_texture = m_textures.terrain;
	m_map_state->ter1_pcx = m_textures.source.ter1_pcx;
}

void Map::ProcessTiles( module_passes_t& module_passes, const tiles_t& tiles, MT_CANCELABLE ) {
	ASSERT( m_map_state, "map state not set" );

	auto* ui = g_engine->GetUI();

	// small optimization to avoid reallocations
	const size_t percent_len = 2;
	std::string sp( percent_len, ' ' );
	std::string loading_text = "Processing tiles (" + sp + "%)";
	const size_t percent_pos = loading_text.size() - 2 - sp.size();

	size_t tile_i = 0;
	size_t total = 0;
	for ( auto& module_pass : module_passes ) {
		total += module_pass.size();
	}
	total *= tiles.size();

	uint8_t percent = 0, last_percent = 0;

	size_t state_iterate_eta = ITERATE_STATE_EVERY_N_TILES;

	for ( auto& module_pass : module_passes ) {
		for ( const auto& tile : tiles ) {
			m_current_tile = tile;
			m_current_ts = GetTileState( tile->coord.x, tile->coord.y );

			for ( auto& m : module_pass ) {

				m->GenerateTile( m_current_tile, m_current_ts, m_map_state );

				tile_i++;
				percent = (uint8_t)ceil( ( (float)tile_i * 100.0f / total ) ) - 1;

				if ( percent != last_percent ) {
					last_percent = percent;
					sp = std::to_string( percent );
					if ( sp.size() < percent_len ) {
						sp = std::string( percent_len - sp.size(), ' ' ) + sp;
					}
					loading_text.replace( percent_pos, sp.size(), sp.c_str() );
					ui->SetLoaderText( loading_text );
				}

				MT_RETIF();
			}

			if ( !--state_iterate_eta ) {
				// keep processing state (i.e. network events) while loading
				m_game->GetState()->Iterate();
				state_iterate_eta = ITERATE_STATE_EVERY_N_TILES;
			}
		}
	}
}

void Map::LoadTiles( const tiles_t& tiles, MT_CANCELABLE ) {

	Log( "Loading " + std::to_string( tiles.size() ) + " tiles" );

	ProcessTiles( m_modules, tiles, MT_C );
	MT_RETIF();

	for ( auto& c : m_map_state->copy_from_after ) {
		m_textures.terrain->AddFrom( m_textures.terrain, c.mode, c.tx1_from, c.ty1_from, c.tx2_from, c.ty2_from, c.tx_to, c.ty_to, c.rotate, c.alpha, GetRandom(), c.perlin );
	}
	m_map_state->copy_from_after.clear();
	MT_RETIF();

	ProcessTiles( m_modules_deferred, tiles, MT_C );
	MT_RETIF();
}

void Map::FixNormals( const tiles_t& tiles, MT_CANCELABLE ) {
	Log( "Fixing normals" );

	g_engine->GetUI()->SetLoaderText( "Fixing normals" );

	std::vector< types::mesh::surface_id_t > surfaces = {};

	// avoiding reallocations is more important than saving memory
	// worst case scenario: 4 surfaces per tile, 4 layers + overdraw column (only land layer)
	surfaces.reserve( tiles.size() * 4 * 5 );

	for ( auto& tile : tiles ) {
		auto* ts = GetTileState( tile->coord.x, tile->coord.y );
#define x( _layer ) \
            surfaces.push_back( _layer.surfaces.left_top ); \
            surfaces.push_back( _layer.surfaces.top_right ); \
            surfaces.push_back( _layer.surfaces.right_bottom ); \
            surfaces.push_back( _layer.surfaces.bottom_left )
		x( ts->layers[ tile::LAYER_LAND ] );
		if ( ts->has_water ) {
			x( ts->layers[ tile::LAYER_WATER ] );
			x( ts->layers[ tile::LAYER_WATER_SURFACE ] );
			x( ts->layers[ tile::LAYER_WATER_SURFACE_EXTRA ] );
		}
		if ( tile->coord.x == 0 ) {
			// also update overdraw column
			x( ts->overdraw_column );
		}
#undef x
		MT_RETIF();
	}
	m_meshes.terrain->UpdateNormals( surfaces );

	std::vector< types::mesh::index_t > v, vtmp;

	// to prevent reallocations
	vtmp.reserve( 4 );
	v.reserve( 8 );

	// normals will be combined at left vertex of tile
	const auto f_combine_normals_at_left_vertex = [ this, &v, &vtmp ]( const tile::Tile* tile ) -> void {
		auto* ts = GetTileState( tile->coord.x, tile->coord.y );
#define x( _lt ) { \
            ts->layers[ _lt ].indices.left, \
            ts->NW->layers[ _lt ].indices.bottom, \
            ts->W->layers[ _lt ].indices.right, \
            ts->SW->layers[ _lt ].indices.top, \
        }
		v = x( tile::LAYER_LAND );
		if ( tile->is_water_tile || tile->W->is_water_tile || tile->NW->is_water_tile ) {
			vtmp = x( tile::LAYER_WATER );
			v.insert( v.end(), vtmp.begin(), vtmp.end() );
		}
#undef x

		m_meshes.terrain->CombineNormals( v );
	};
	std::unordered_set< const tile::Tile* > processed_tiles = {}; // to prevent duplicate combining
	processed_tiles.reserve( tiles.size() * 4 ); // minimizing reallocations is more important than saving memory
	const auto f_combine_normals_maybe = [ &f_combine_normals_at_left_vertex, &processed_tiles ]( const tile::Tile* tile ) -> void {
		if ( processed_tiles.find( tile ) == processed_tiles.end() ) {
			f_combine_normals_at_left_vertex( tile );
			processed_tiles.insert( tile );
		}
	};

	for ( const auto& tile : tiles ) {
		f_combine_normals_maybe( tile );
		f_combine_normals_maybe( tile->NE );
		f_combine_normals_maybe( tile->E );
		f_combine_normals_maybe( tile->SE );
	}

	// average center normals
	for ( const auto& tile : tiles ) {
		auto* ts = GetTileState( tile->coord.x, tile->coord.y );

		m_meshes.terrain->SetVertexNormal(
			ts->layers[ tile::LAYER_LAND ].indices.center, (
				m_meshes.terrain->GetVertexNormal( ts->layers[ tile::LAYER_LAND ].indices.left ) +
					m_meshes.terrain->GetVertexNormal( ts->layers[ tile::LAYER_LAND ].indices.top ) +
					m_meshes.terrain->GetVertexNormal( ts->layers[ tile::LAYER_LAND ].indices.right ) +
					m_meshes.terrain->GetVertexNormal( ts->layers[ tile::LAYER_LAND ].indices.bottom )
			) / 4
		);
	}
}

void Map::CalculateTextureVariants( const texture_variants_type_t type, const texture_variants_rules_t& rules ) {
	ASSERT( m_texture_variants.find( type ) == m_texture_variants.end(), "texture variants for " + std::to_string( type ) + " already calculated" );
	auto& variants = m_texture_variants[ type ];
	for ( uint16_t bitmask = 0 ; bitmask < 256 ; bitmask++ ) {
		ssize_t i = rules.size() - 1;
		auto& v = variants[ bitmask ] = {};
		while ( i >= 0 ) {
			const auto& rule = rules.at( i );
			if ( ( bitmask & rule.checked_bits ) == ( rule.bitmask & rule.checked_bits ) ) {
				v.push_back( i );
			}
			i--;
		}
	}
}

}
}
