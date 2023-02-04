#include "Map.h"

#include "engine/Engine.h"

#include "types/Mesh.h"

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

Map::Map( Scene* scene )
	: m_scene( scene )
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
	typedef struct {
		uint8_t bitmask;
		uint8_t checked_bits; // some masks don't care about some corners
	} texture_rule_t;
	
	const vector< texture_rule_t > bitmasks = {
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
}

Map::~Map() {
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

void Map::SetTiles( Tiles* tiles ) {
	ASSERT( !m_tiles, "map tiles already set" );
	m_tiles = tiles;
	GenerateActors();
}

vector<actor::Mesh*> Map::GetActors() const {
	return {
		m_actors.terrain,
	};
}

void Map::GenerateActors() {
	ASSERT( m_tiles, "map tiles not set" );
	m_tiles->Validate();
	
	Log( "Loading map" );
	
	m_map_state = {};
	
	m_map_state.dimensions.x = m_tiles->GetWidth();
	m_map_state.dimensions.y = m_tiles->GetHeight();
	m_map_state.coord.x = -( Map::s_consts.tile.scale.x * m_map_state.dimensions.x / 4 - Map::s_consts.tile.radius.x );
	m_map_state.coord.y = -( Map::s_consts.tile.scale.y * m_map_state.dimensions.y / 4 - Map::s_consts.tile.radius.y );
	m_map_state.variables.texture_scaling.x = 1.0f / Map::s_consts.pcx_texture_block.dimensions.x / m_map_state.dimensions.x;
	m_map_state.variables.texture_scaling.y = 1.0f / Map::s_consts.pcx_texture_block.dimensions.y / m_map_state.dimensions.y / LAYER_MAX;
	
	ASSERT( !m_tile_states, "m_tile_states already set" );
	size_t wh = m_map_state.dimensions.x * m_map_state.dimensions.y;
	m_tile_states = (tile_state_t*)malloc( sizeof( tile_state_t ) * wh );
	memset( ptr( m_tile_states, 0, wh ), 0, wh );
	
	if ( !m_textures.source ) {
		m_textures.source = g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", Color::RGBA( 125, 0, 128, 255 ) );
	}
	if ( m_actors.terrain ) {
		m_scene->RemoveActor( m_actors.terrain );
		DELETE( m_actors.terrain );
	}
	if ( m_textures.terrain ) {
		DELETE( m_textures.terrain );
	}
	
	NEW( m_textures.terrain, Texture, "TerrainTexture", m_map_state.dimensions.x * Map::s_consts.pcx_texture_block.dimensions.x, ( m_map_state.dimensions.y * LAYER_MAX ) * Map::s_consts.pcx_texture_block.dimensions.y );
	NEW( m_mesh_terrain, types::Mesh, m_map_state.dimensions.x * ( m_map_state.dimensions.y * LAYER_MAX ) * 5 / 2, m_map_state.dimensions.x * ( m_map_state.dimensions.y * LAYER_MAX ) * 4 / 2 );
	
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
	
	free( m_tile_states );
	m_tile_states = nullptr;
	
	NEW( m_actors.terrain, actor::Mesh, "MapTerrain", m_mesh_terrain );
		m_actors.terrain->SetTexture( m_textures.terrain );
		m_actors.terrain->SetPosition( Map::s_consts.map_position );
		m_actors.terrain->SetAngle( Map::s_consts.map_rotation );
	m_scene->AddActor( m_actors.terrain );
	
	m_current_tile = nullptr;
	m_current_ts = nullptr;
	m_mesh_terrain = nullptr; // deleted by actor
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
	
	vector< uint8_t > possible_rotates = {};
	
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
		info.rotate_direction = possible_rotates[ rand() % possible_rotates.size() ] / 2;
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
	ASSERT( x < m_tiles->GetWidth(), "tile x overflow" );
	ASSERT( y < m_tiles->GetHeight(), "tile y overflow" );
	return &m_tile_states[ y * m_map_state.dimensions.x + x ];
}


}
}
}
