#include "Map.h"

#include "engine/Engine.h"

#include "types/mesh/Render.h"
#include "types/mesh/Data.h"
#include "scene/actor/InstancedMesh.h"

#include "module/Prepare.h"
#include "module/LandSurface.h"
#include "module/WaterSurface.h"
#include "module/Coastlines.h"
#include "module/Finalize.h"

namespace game {
namespace world {
namespace map {

const Map::consts_t Map::s_consts;

#if WIREFRAMES
static Texture* s_wireframe_texture = nullptr;
#endif

#define B(x) S_to_binary_(#x)

static inline unsigned char S_to_binary_(const char *s)
{
    unsigned long long i = 0;
    while (*s) {
        i <<= 1;
        i += *s++ - '0';
    }
    return i;
}

Map::Map( Random* random, Scene* scene )
	: m_random( random )
	, m_scene( scene )
{
	
#if WIREFRAMES
	NEW( s_wireframe_texture, Texture, "MapWireframe", Map::s_consts.pcx_texture_block.dimensions.x, Map::s_consts.pcx_texture_block.dimensions.y );
#if WIREFRAMES_GRADIENTS
	float mx = 1.0f / Map::s_consts.pcx_texture_block.dimensions.x;
	float my = 1.0f / Map::s_consts.pcx_texture_block.dimensions.y;
	for ( size_t y = 0 ; y < Map::s_consts.pcx_texture_block.dimensions.y ; y++ ) {
		for ( size_t x = 0 ; x < Map::s_consts.pcx_texture_block.dimensions.x ; x++ ) {
			s_wireframe_texture->SetPixel( x, y, { mx * x / 2, my * y / 2, 0f, 1.0f } );
		}
	}
#else
	Color color_h = { 0.0f, 1.0f, 0.0f, 1.0f };
	Color color_v = { 1.0f, 0.0f, 0.0f, 1.0f };
	Color color_hv = { 1.0f, 1.0f, 0.0f, 1.0f };
	Color value;
	bool value_x = true;
	bool value_y = true;
	for ( size_t y = 0 ; y < Map::s_consts.pcx_texture_block.dimensions.y ; y++ ) {
		for ( size_t x = 0 ; x < Map::s_consts.pcx_texture_block.dimensions.x ; x++ ) {
			if ( value_y ) {
				if ( value_x ) {
					value = color_hv;
				}
				else {
					value = color_h;
				}
			}
			else if ( value_x ) {
				value = color_v;
			}
			s_wireframe_texture->SetPixel( x, y, value );
			value_x = !value_x;
		}
		value_y = !value_y;
	}
#endif
#endif
	
	// precache tile texture variants/rotations for fast lookups
	struct texture_rule_t {
		uint8_t bitmask;
		uint8_t checked_bits; // some masks don't care about some corners
	};
	
	const std::vector< texture_rule_t > bitmasks = {
		{ B(00000000), B(10101010) }, // 0
		{ B(00001000), B(10101010) }, // 1
		{ B(00101000), B(10111010) }, // 2
		{ B(00111000), B(10111010) }, // 3
		{ B(10001000), B(10101010) }, // 4
		{ B(10101000), B(11111010) }, // 5
		{ B(10111000), B(11111010) }, // 6
		{ B(11101000), B(11111010) }, // 7
		{ B(11111000), B(11111010) }, // 8
		{ B(10101010), B(11111111) }, // 9
		{ B(10101110), B(11111111) }, // 10
		{ B(10111110), B(11111111) }, // 11
		{ B(11101110), B(11111111) }, // 12
		{ B(11111110), B(11111111) }, // 13
		{ B(11111111), B(11111111) }, // 14
		// 15 ???
	};

	for ( uint16_t bitmask = 0 ; bitmask < 256 ; bitmask++ ) {
		ssize_t i = bitmasks.size() - 1;
		while ( i >= 0 ) {
			texture_rule_t rules = bitmasks.at( i );
			if ( ( bitmask & rules.checked_bits ) == ( rules.bitmask & rules.checked_bits ) ) {
				m_texture_variants[ bitmask ] = i;
				break;
			}
			i--;
		}
		if ( i == 0 ) {
			m_texture_variants[ bitmask ] = 0;
		}
	}
	
	// add map modules
	//   order is important!
	Module* m;
	NEW( m, Prepare, this ); m_modules.push_back( m ); // needs to always be first
	NEW( m, LandSurface, this ); m_modules.push_back( m );
	NEW( m, WaterSurface, this ); m_modules.push_back( m );
	NEW( m, Coastlines, this ); m_modules.push_back( m );
	NEW( m, Finalize, this ); m_modules.push_back( m ); // needs to always be last

	m_textures.source = g_engine->GetTextureLoader()->LoadTextureTC( "texture.pcx", Color::RGBA( 125, 0, 128, 255 ) );
}

Map::~Map() {
	if ( m_tile_at_request_id ) {
		m_actors.terrain->CancelDataRequest( m_tile_at_request_id );
	}
	if ( m_actors.terrain ) {
		m_scene->RemoveActor( m_actors.terrain );
		DELETE( m_actors.terrain );
	}
	if ( m_tiles ) {
		DELETE( m_tiles );
	}
	if ( m_textures.terrain ) {
		DELETE( m_textures.terrain );
	}
#if WIREFRAMES
	DELETE( s_wireframe_texture );
	s_wireframe_texture = nullptr;
#endif
	for ( auto& m : m_modules ) {
		DELETE( m );
	}
	if ( m_tile_states ) {
		free( m_tile_states );
	}
}

void Map::SetTiles( Tiles* tiles, bool generate_actors ) {
	ASSERT( !m_tiles, "map tiles already set" );
	m_tiles = tiles;
	if ( generate_actors ) {
		GenerateActors();
	}
}

#ifdef DEBUG
std::vector<actor::Mesh*> Map::GetActors() const {
	return {
		m_actors.terrain,
	};
}
#endif

void Map::GenerateActors() {
	ASSERT( m_tiles, "map tiles not set" );
	m_tiles->Validate();
	
	Log( "Initializing map" );
	
	m_map_state = {};
	
	m_map_state.dimensions.x = m_tiles->GetWidth();
	m_map_state.dimensions.y = m_tiles->GetHeight();
	m_map_state.coord.x = -( Map::s_consts.tile.scale.x * ( m_map_state.dimensions.x + 1 ) / 4 - Map::s_consts.tile.radius.x );
	m_map_state.coord.y = -( Map::s_consts.tile.scale.y * ( m_map_state.dimensions.y + 1 ) / 4 - Map::s_consts.tile.radius.y );
	m_map_state.variables.texture_scaling.x = 1.0f / Map::s_consts.pcx_texture_block.dimensions.x / ( m_map_state.dimensions.x + 1 ); // + 1 for overdraw column
	m_map_state.variables.texture_scaling.y = 1.0f / Map::s_consts.pcx_texture_block.dimensions.y / m_map_state.dimensions.y / LAYER_MAX;
	
	ASSERT( !m_tile_states, "m_tile_states already set" );
	size_t sz = m_map_state.dimensions.y * m_map_state.dimensions.x;
	m_tile_states = (tile_state_t*)malloc( sizeof( tile_state_t ) * sz );
	memset( ptr( m_tile_states, 0, sz ), 0, sz );
	
	if ( m_actors.terrain ) {
		m_scene->RemoveActor( m_actors.terrain );
		DELETE( m_actors.terrain );
	}

	InitTextureAndMesh();
	
	for ( size_t y = 0 ; y < m_map_state.dimensions.y ; y++ ) {
		for ( size_t x = 0 ; x < m_map_state.dimensions.x ; x++ ) {
			if ( y % 2 != x % 2 ) {
				continue;
			}
			
			m_current_tile = m_tiles->At( x, y );
			m_current_ts = GetTileState( x, y );
			
			for ( auto& m : m_modules ) {
				m->GenerateTile( m_current_tile, m_current_ts, &m_map_state );
			}
		}
	}
	
	for ( auto& c : m_map_state.copy_from_after ) {
		m_textures.terrain->AddFrom( m_textures.terrain, c.mode, c.tx1_from, c.ty1_from, c.tx2_from, c.ty2_from, c.tx_to, c.ty_to, c.rotate, c.alpha );
	}
	
	m_mesh_terrain->Finalize();
	m_mesh_terrain_data->Finalize();
	
	// update normals where needed
	for ( auto& cn : m_map_state.copy_normals ) {
		m_mesh_terrain->SetVertexNormal( *cn.second, m_mesh_terrain->GetVertexNormal( *cn.first ) );
	}
	for ( auto& nn : m_map_state.need_normals ) {
		auto* ts = GetTileState( nn.second.first.x, nn.second.first.y );

		m_mesh_terrain->SetVertexNormal( *nn.first, m_mesh_terrain->GetVertexNormal(
			nn.second.second == Texture::AM_ROUND_LEFT ? ts->layers[ LAYER_LAND ].indices.right :
			nn.second.second == Texture::AM_ROUND_TOP ? ts->layers[ LAYER_LAND ].indices.bottom :
			nn.second.second == Texture::AM_ROUND_RIGHT ? ts->layers[ LAYER_LAND ].indices.left :
			nn.second.second == Texture::AM_ROUND_BOTTOM ? ts->layers[ LAYER_LAND ].indices.top : -1
		));
	}
	
	NEW( m_actors.terrain, actor::InstancedMesh, "MapTerrain", m_mesh_terrain );
		m_actors.terrain->SetTexture( m_textures.terrain );
		m_actors.terrain->SetPosition( Map::s_consts.map_position );
		m_actors.terrain->SetAngle( Map::s_consts.map_rotation );
		m_actors.terrain->SetDataMesh( m_mesh_terrain_data );
	m_scene->AddActor( m_actors.terrain );
	
	m_current_tile = nullptr;
	m_current_ts = nullptr;
}

void Map::InitTextureAndMesh() {
	
	if ( m_textures.terrain ) {
		DELETE( m_textures.terrain );
	}
	NEW( m_textures.terrain, Texture, "TerrainTexture",
		( m_map_state.dimensions.x + 1 ) * Map::s_consts.pcx_texture_block.dimensions.x, // + 1 for overdraw_column
		( m_map_state.dimensions.y * LAYER_MAX ) * Map::s_consts.pcx_texture_block.dimensions.y
	);
	
	// not deleting meshes because if they exist - it means they are already linked to actor and are deleted together when needed
	NEW( m_mesh_terrain, types::mesh::Render,
		( m_map_state.dimensions.x * LAYER_MAX + 1 ) * m_map_state.dimensions.y * 5 / 2, // + 1 for overdraw column
		( m_map_state.dimensions.x * LAYER_MAX + 1 ) * m_map_state.dimensions.y * 4 / 2 // + 1 for overdraw column
	);
	NEW( m_mesh_terrain_data, types::mesh::Data, // data mesh has only one layer and no overdraw column
		m_map_state.dimensions.x * m_map_state.dimensions.y * 5 / 2,
		m_map_state.dimensions.x * m_map_state.dimensions.y * 4 / 2
	);
	
	m_map_state.terrain_texture = m_textures.terrain;
}

const Map::tile_texture_info_t Map::GetTileTextureInfo( const Tile* tile, const tile_grouping_criteria_t criteria, const Tile::feature_t feature ) const {
	ASSERT( m_current_ts, "GetTileTextureInfo called outside of tile generation" );
	Map::tile_texture_info_t info;
	
	bool matches[ 16 ];
	uint8_t idx = 0;
	for ( uint8_t i = 0 ; i < 2 ; i++ ) {
		for ( auto& t : tile->neighbours ) {
			switch ( criteria ) {
				case TG_MOISTURE: {
					matches[ idx++ ] = t->moisture == tile->moisture;
					break;
				}
				case TG_FEATURE: {
					matches[ idx++ ] =
						//( t->is_water_tile == tile->is_water_tile ) && // TODO: fix coastline_tint influence on coastlines fungus
						( t->features & feature ) == ( tile->features & feature )
					;
					break;
				}
				default: {
					ASSERT( false, "invalid tile grouping criteria" );
				}
			}
		}
	}
	
	std::vector< uint8_t > possible_rotates = {};
	
	for ( info.rotate_direction = 0 ; info.rotate_direction < 8 ; info.rotate_direction += 2 ) {
		uint8_t bitmask = 0;
		for ( uint8_t i = 0 ; i < 8 ; i++ ) {
			bitmask |= matches[ i + info.rotate_direction ] << i;
		}
		auto it = m_texture_variants.find( bitmask );
		if ( it != m_texture_variants.end() ) {
			info.texture_variant = it->second;
			possible_rotates.push_back( info.rotate_direction );
		}
	}
	
	if ( !possible_rotates.empty() ) {
		info.rotate_direction = possible_rotates[ m_random->GetUInt( 0, possible_rotates.size() - 1 ) ] / 2;
	}
	else {
		ASSERT( false, "could not find texture variant" );
	}
	
	return info;
}

void Map::AddTexture( const tile_layer_type_t tile_layer, const pcx_texture_coordinates_t& tc, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha ) {
	ASSERT( m_current_ts, "AddTexture called outside of tile generation" );
#if WIREFRAMES
	m_textures.terrain->AddFrom( s_wireframe_texture, mode, 0, 0, Map::s_consts.pcx_texture_block.dimensions.x - 1, Map::s_consts.pcx_texture_block.dimensions.y - 1, m_current_ts->tex_coord.x1, tile_layer * m_map_state.dimensions.y * Map::s_consts.pcx_texture_block.dimensions.y + m_current_ts->tex_coord.y1, rotate, alpha );
#else
	m_textures.terrain->AddFrom( m_textures.source, mode, tc.x, tc.y, tc.x + Map::s_consts.pcx_texture_block.dimensions.x - 1, tc.y + Map::s_consts.pcx_texture_block.dimensions.y - 1, m_current_ts->tex_coord.x1, tile_layer * m_map_state.dimensions.y * Map::s_consts.pcx_texture_block.dimensions.y + m_current_ts->tex_coord.y1, rotate, alpha );
#endif
};

void Map::CopyTextureFromLayer( const tile_layer_type_t tile_layer_from, const size_t tx_from, const size_t ty_from, const tile_layer_type_t tile_layer, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha ) {
	ASSERT( m_current_ts, "CopyTextureFromLayer called outside of tile generation" );
	m_textures.terrain->AddFrom( m_textures.terrain, mode, tx_from, tile_layer_from * m_map_state.dimensions.y * Map::s_consts.pcx_texture_block.dimensions.y + ty_from, tx_from + Map::s_consts.pcx_texture_block.dimensions.x - 1, tile_layer_from * m_map_state.dimensions.y * Map::s_consts.pcx_texture_block.dimensions.y + ty_from + Map::s_consts.pcx_texture_block.dimensions.y - 1, m_current_ts->tex_coord.x1, tile_layer * m_map_state.dimensions.y * Map::s_consts.pcx_texture_block.dimensions.y + m_current_ts->tex_coord.y1, rotate, alpha );
};

void Map::CopyTexture( const tile_layer_type_t tile_layer_from, const tile_layer_type_t tile_layer, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha ) {
	ASSERT( m_current_ts, "CopyTexture called outside of tile generation" );
	CopyTextureFromLayer( tile_layer_from, m_current_ts->tex_coord.x1, m_current_ts->tex_coord.y1, tile_layer, mode, rotate, alpha );
};

void Map::CopyTextureDeferred( const tile_layer_type_t tile_layer_from, const size_t tx_from, const size_t ty_from,const tile_layer_type_t tile_layer, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha ) {
	ASSERT( m_current_ts, "CopyTextureDeferred called outside of tile generation" );
	m_map_state.copy_from_after.push_back({ mode, tx_from, tile_layer_from * m_map_state.dimensions.y * Map::s_consts.pcx_texture_block.dimensions.y + ty_from, tx_from + Map::s_consts.pcx_texture_block.dimensions.x - 1, tile_layer_from * m_map_state.dimensions.y * Map::s_consts.pcx_texture_block.dimensions.y + ty_from + Map::s_consts.pcx_texture_block.dimensions.y - 1, (size_t)m_current_ts->tex_coord.x1, tile_layer * m_map_state.dimensions.y * Map::s_consts.pcx_texture_block.dimensions.y + (size_t)m_current_ts->tex_coord.y1, rotate, alpha });
};

Map::tile_state_t* Map::GetTileState( const size_t x, const size_t y ) const {
	ASSERT( x < m_map_state.dimensions.x, "tile state x overflow" );
	ASSERT( y < m_map_state.dimensions.y, "tile state y overflow" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile state axis oddity differs" );
	return &m_tile_states[ y * m_map_state.dimensions.x + x / 2 ];
}

Map::tile_state_t* Map::GetTileState( const Tile* tile ) const {
	return GetTileState( tile->coord.x, tile->coord.y );
}

const Map::map_state_t* Map::GetMapState() const {
	return &m_map_state;
}

const size_t Map::GetWidth() const {
	return m_map_state.dimensions.x;
}

const size_t Map::GetHeight() const {
	return m_map_state.dimensions.y;
}

Random* Map::GetRandom() const {
	return m_random;
}

const Map::tile_info_t Map::GetTileAt( const size_t tile_x, const size_t tile_y ) const {
	ASSERT( m_tiles, "tiles not set" );
	tile_info_t result;
	return { m_tiles->At( tile_x, tile_y ), GetTileState( tile_x, tile_y ), &m_map_state };
}

void Map::CancelTileAtRequest() {
	ASSERT( m_tile_at_request_id, "tileat request not found" );
	m_actors.terrain->CancelDataRequest( m_tile_at_request_id );
	m_tile_at_request_id = 0;
}	

void Map::GetTileAtScreenCoords( const size_t screen_x, const size_t screen_inverse_y ) {
	if ( m_tile_at_request_id ) {
		m_actors.terrain->CancelDataRequest( m_tile_at_request_id );
	}
	m_tile_at_request_id = m_actors.terrain->GetDataAt( screen_x, screen_inverse_y );
}

Map::tile_info_t Map::GetTileAtScreenCoordsResult() {
	if ( m_tile_at_request_id ) {
		auto result = m_actors.terrain->GetDataResponse( m_tile_at_request_id );
		if ( result.first ) {
			m_tile_at_request_id = 0;
			
			if ( result.second ) { // some tile was clicked
				
				result.second--; // we used +1 increment to differentiate 'tile at 0,0' from 'no tiles'
				
				size_t tile_x = result.second % m_map_state.dimensions.x;
				size_t tile_y = result.second / m_map_state.dimensions.x;

				return GetTileAt( tile_x, tile_y );
			}
		}
	}
	
	// no data
	return { nullptr, nullptr, nullptr };
}

const Buffer Map::Serialize() const {
	Buffer buf;
	
	ASSERT( m_tiles, "tiles not set, can't serialize" );
	
	buf.WriteString( m_map_state.Serialize().ToString() );
	
	buf.WriteString( m_tiles->Serialize().ToString() );
	for ( auto y = 0 ; y < m_map_state.dimensions.y ; y++ ) {
		for ( auto x = 0; x < m_map_state.dimensions.x ; x++ ) {
			if ( ( x % 2 ) != ( y % 2 ) ) {
				continue;
			}
			buf.WriteString( GetTileState( x, y )->Serialize().ToString() );
		}
	}
	
	buf.WriteString( m_textures.terrain->Serialize().ToString() );
	buf.WriteString( m_mesh_terrain->Serialize().ToString() );
	buf.WriteString( m_mesh_terrain_data->Serialize().ToString() );
	
	return buf;
}

const Buffer Map::map_state_t::Serialize() const {
	Buffer buf;
	
	buf.WriteFloat( coord.x );
	buf.WriteFloat( coord.y );
	
	buf.WriteInt( dimensions.x );
	buf.WriteInt( dimensions.y );
	
	buf.WriteFloat( variables.texture_scaling.x );
	buf.WriteFloat( variables.texture_scaling.y );
	
	return buf;
}

const Buffer Map::tile_state_t::Serialize() const {
	Buffer buf;
	
	buf.WriteFloat( coord.x );
	buf.WriteFloat( coord.y );
	
	buf.WriteFloat( tex_coord.x );
	buf.WriteFloat( tex_coord.y );
	buf.WriteFloat( tex_coord.x1 );
	buf.WriteFloat( tex_coord.y1 );
	buf.WriteFloat( tex_coord.x2 );
	buf.WriteFloat( tex_coord.y2 );
	
	buf.WriteString( elevations.Serialize().ToString() );
	
	buf.WriteInt( LAYER_MAX );
	for ( auto i = 0 ; i < LAYER_MAX ; i++ ) {
		buf.WriteString( layers[ i ].Serialize().ToString() );
	}
	
	buf.WriteString( overdraw_column.coords.Serialize().ToString() );
	buf.WriteString( overdraw_column.indices.Serialize().ToString() );
	
	buf.WriteString( data_mesh.coords.Serialize().ToString() );
	buf.WriteString( data_mesh.indices.Serialize().ToString() );
	
	buf.WriteBool( is_coastline_corner );
	buf.WriteBool( has_water );
	
	return buf;
}

const Buffer Map::tile_elevations_t::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( center );
	buf.WriteInt( left );
	buf.WriteInt( top );
	buf.WriteInt( right );
	buf.WriteInt( bottom );
	
	return buf;
}

const Buffer Map::tile_layer_t::Serialize() const {
	Buffer buf;
	
	buf.WriteString( indices.Serialize().ToString() );
	buf.WriteString( coords.Serialize().ToString() );
	buf.WriteString( tex_coords.Serialize().ToString() );
	buf.WriteString( colors.Serialize().ToString() );
	
	return buf;
}

const Buffer Map::tile_indices_t::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( center );
	buf.WriteInt( left );
	buf.WriteInt( top );
	buf.WriteInt( right );
	buf.WriteInt( bottom );
	
	return buf;
}

const Buffer Map::tile_vertices_t::Serialize() const {
	Buffer buf;
	
	buf.WriteVec3( center );
	buf.WriteVec3( left );
	buf.WriteVec3( top );
	buf.WriteVec3( right );
	buf.WriteVec3( bottom );
	
	return buf;
}

const Buffer Map::tile_tex_coords_t::Serialize() const {
	Buffer buf;
	
	buf.WriteVec2f( center );
	buf.WriteVec2f( left );
	buf.WriteVec2f( top );
	buf.WriteVec2f( right );
	buf.WriteVec2f( bottom );
	
	return buf;
}

const Buffer Map::tile_colors_t::Serialize() const {
	Buffer buf;
	
	buf.WriteColor( center );
	buf.WriteColor( left );
	buf.WriteColor( top );
	buf.WriteColor( right );
	buf.WriteColor( bottom );
	
	return buf;
}

void Map::Unserialize( Buffer buf ) {
	ASSERT( m_tiles, "tiles not set, can't unserialize" );
	
	m_map_state.Unserialize( Buffer( buf.ReadString() ) );
	
	m_tiles->Unserialize( buf.ReadString() );

	ASSERT( m_tiles->GetWidth() == m_map_state.dimensions.x, "tiles width doesn't match map width ( " + std::to_string( m_tiles->GetWidth() ) + " != " + std::to_string( m_map_state.dimensions.x ) + " )" );
	ASSERT( m_tiles->GetHeight() == m_map_state.dimensions.y, "tiles height doesn't match map height ( " + std::to_string( m_tiles->GetHeight() ) + " != " + std::to_string( m_map_state.dimensions.y ) + " )" );

	if ( m_tile_states ) {
		free( m_tile_states );
	}
	size_t sz = m_map_state.dimensions.y * m_map_state.dimensions.x;
	m_tile_states = (tile_state_t*)malloc( sizeof( tile_state_t ) * sz );
	memset( ptr( m_tile_states, 0, sz ), 0, sz );
	
	for ( auto y = 0 ; y < m_map_state.dimensions.y ; y++ ) {
		for ( auto x = 0; x < m_map_state.dimensions.x ; x++ ) {
			if ( ( x % 2 ) != ( y % 2 ) ) {
				continue;
			}
			GetTileState( x, y )->Unserialize( buf.ReadString() );
		}
	}
	
	InitTextureAndMesh();
	
	m_textures.terrain->Unserialize( buf.ReadString() );
	m_mesh_terrain->Unserialize( buf.ReadString() );
	m_mesh_terrain_data->Unserialize( buf.ReadString() );
	
	NEW( m_actors.terrain, actor::InstancedMesh, "MapTerrain", m_mesh_terrain );
		m_actors.terrain->SetTexture( m_textures.terrain );
		m_actors.terrain->SetPosition( Map::s_consts.map_position );
		m_actors.terrain->SetAngle( Map::s_consts.map_rotation );
		m_actors.terrain->SetDataMesh( m_mesh_terrain_data );
	m_scene->AddActor( m_actors.terrain );
	
}

void Map::map_state_t::Unserialize( Buffer buf ) {
	coord.x = buf.ReadFloat();
	coord.y = buf.ReadFloat();
	
	dimensions.x = buf.ReadInt();
	dimensions.y = buf.ReadInt();
	
	variables.texture_scaling.x = buf.ReadFloat();
	variables.texture_scaling.y = buf.ReadFloat();
}

void Map::tile_state_t::Unserialize( Buffer buf ) {
	coord.x = buf.ReadFloat();
	coord.y = buf.ReadFloat();
	
	tex_coord.x = buf.ReadFloat();
	tex_coord.y = buf.ReadFloat();
	tex_coord.x1 = buf.ReadFloat();
	tex_coord.y1 = buf.ReadFloat();
	tex_coord.x2 = buf.ReadFloat();
	tex_coord.y2 = buf.ReadFloat();
	
	elevations.Unserialize( buf.ReadString() );
	
	if ( (tile_layer_type_t)buf.ReadInt() != LAYER_MAX ) {
		THROW( "LAYER_MAX mismatch" );
	}
	for ( auto i = 0 ; i < LAYER_MAX ; i++ ) {
		layers[i].Unserialize( buf.ReadString() );
	}
	
	overdraw_column.coords.Unserialize( buf.ReadString() );
	overdraw_column.indices.Unserialize( buf.ReadString() );
	
	data_mesh.coords.Unserialize( buf.ReadString() );
	data_mesh.indices.Unserialize( buf.ReadString() );
	
	is_coastline_corner = buf.ReadBool();
	has_water = buf.ReadBool();
}

void Map::tile_elevations_t::Unserialize( Buffer buf ) {
	center = buf.ReadInt();
	left = buf.ReadInt();
	top = buf.ReadInt();
	right = buf.ReadInt();
	bottom = buf.ReadInt();
}

void Map::tile_layer_t::Unserialize( Buffer buf ) {
	indices.Unserialize( buf.ReadString() );
	coords.Unserialize( buf.ReadString() );
	tex_coords.Unserialize( buf.ReadString() );
	colors.Unserialize( buf.ReadString() );
}

void Map::tile_indices_t::Unserialize( Buffer buf ) {
	center = buf.ReadInt();
	left = buf.ReadInt();
	top = buf.ReadInt();
	right = buf.ReadInt();
	bottom = buf.ReadInt();
}

void Map::tile_vertices_t::Unserialize( Buffer buf ) {
	center = buf.ReadVec3();
	left = buf.ReadVec3();
	top = buf.ReadVec3();
	right = buf.ReadVec3();
	bottom = buf.ReadVec3();
}

void Map::tile_tex_coords_t::Unserialize( Buffer buf ) {
	center = buf.ReadVec2f();
	left = buf.ReadVec2f();
	top = buf.ReadVec2f();
	right = buf.ReadVec2f();
	bottom = buf.ReadVec2f();
}

void Map::tile_colors_t::Unserialize( Buffer buf ) {
	center = buf.ReadColor();
	left = buf.ReadColor();
	top = buf.ReadColor();
	right = buf.ReadColor();
	bottom = buf.ReadColor();
}


}
}
}
