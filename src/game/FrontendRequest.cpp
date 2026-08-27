#include "FrontendRequest.h"

#include <cstring>

#include "common/Common.h"
#include "backend/map/tile/Tile.h"
#include "backend/map/tile/TileState.h"

namespace game {

tile_render_snapshot_t::tile_render_snapshot_t(
	const backend::map::tile::Tile& tile,
	const backend::map::tile::TileState& tile_state
) {
	coords = tile.coord;
	is_water = tile.is_water_tile;
	west_is_water = tile.W->is_water_tile;
	north_is_water = tile.N->is_water_tile;
	east_is_water = tile.E->is_water_tile;
	south_is_water = tile.S->is_water_tile;
	is_coastline_corner = tile_state.is_coastline_corner;
	elevation = *tile.elevation.center;
	moisture = tile.moisture;
	rockiness = tile.rockiness;
	bonus = tile.bonus;
	features = tile.features;
	terraforming = tile.terraforming;
	for ( size_t layer = 0 ; layer < backend::map::tile::LAYER_MAX ; layer++ ) {
		layers[ layer ].coords = tile_state.layers[ layer ].coords;
		layers[ layer ].tex_coords = tile_state.layers[ layer ].tex_coords;
		layers[ layer ].colors.Set( tile_state.layers[ layer ].colors );
	}
	sprites.reserve( tile_state.sprites.size() );
	for ( const auto& sprite : tile_state.sprites ) {
		sprites.push_back( sprite.actor );
	}
}

FrontendRequest::FrontendRequest( const request_type_t type )
	: type( type ) {
	memset( &data, 0, sizeof( data ) );
}

FrontendRequest::FrontendRequest( const FrontendRequest& other )
	: type( other.type ) {

	data = other.data;

	switch ( type ) {
		case FR_QUIT: {
			NEW( data.quit.reason, std::string, *other.data.quit.reason );
			break;
		}
		case FR_ERROR: {
			NEW( data.error.what, std::string, *other.data.error.what );
			if ( other.data.error.stacktrace ) {
				NEW( data.error.stacktrace, std::string, *other.data.error.stacktrace );
			}
			break;
		}
		case FR_UPDATE_TILES: {
			NEW( data.update_tiles.tile_updates, tile_updates_t, *other.data.update_tiles.tile_updates );
			NEW( data.update_tiles.sprite_actors, tile_sprite_actors_t, *other.data.update_tiles.sprite_actors );
			NEW( data.update_tiles.sprite_removals, tile_sprite_removals_t, *other.data.update_tiles.sprite_removals );
			NEW( data.update_tiles.sprite_additions, tile_sprite_additions_t, *other.data.update_tiles.sprite_additions );
			NEW(
				data.update_tiles.serialized_terrain_texture_patch,
				std::string,
				*other.data.update_tiles.serialized_terrain_texture_patch
			);
			if ( other.data.update_tiles.serialized_terrain_mesh ) {
				NEW(
					data.update_tiles.serialized_terrain_mesh,
					std::string,
					*other.data.update_tiles.serialized_terrain_mesh
				);
			}
			if ( other.data.update_tiles.serialized_terrain_data_mesh ) {
				NEW(
					data.update_tiles.serialized_terrain_data_mesh,
					std::string,
					*other.data.update_tiles.serialized_terrain_data_mesh
				);
			}
			break;
		}
		case FR_FACTION_DEFINE: {
			NEW( data.faction_define.factiondefs, faction_defines_t, *other.data.faction_define.factiondefs );
			break;
		}
		case FR_SLOT_DEFINE: {
			NEW( data.slot_define.slotdefs, slot_defines_t, *other.data.slot_define.slotdefs );
			break;
		}
		case FR_ANIMATION_DEFINE: {
			NEW( data.animation_define.serialized_animation, std::string, *other.data.animation_define.serialized_animation );
			break;
		}
		case FR_ANIMATION_UNDEFINE: {
			NEW( data.animation_undefine.animation_id, std::string, *other.data.animation_undefine.animation_id );
			break;
		}
		case FR_ANIMATION_SHOW: {
			NEW( data.animation_show.animation_id, std::string, *other.data.animation_show.animation_id );
			break;
		}
		case FR_UNIT_DEFINE: {
			NEW( data.unit_define.serialized_unitdef, std::string, *other.data.unit_define.serialized_unitdef );
			break;
		}
		case FR_UNIT_UNDEFINE: {
			NEW( data.unit_undefine.id, std::string, *other.data.unit_undefine.id );
			break;
		}
		case FR_UNIT_SPAWN: {
			NEW( data.unit_spawn.unitdef_id, std::string, *other.data.unit_spawn.unitdef_id );
			NEW( data.unit_spawn.morale_string, std::string, *other.data.unit_spawn.morale_string );
			break;
		}
		case FR_UNIT_UPDATE: {
			NEW( data.unit_update.morale_string, std::string, *other.data.unit_update.morale_string );
			break;
		}
		case FR_BASE_POP_DEFINE: {
			NEW( data.base_pop_define.serialized_popdef, std::string, *other.data.base_pop_define.serialized_popdef );
			break;
		}
		case FR_BASE_POP_UNDEFINE: {
			NEW( data.base_pop_undefine.id, std::string, *other.data.base_pop_undefine.id );
			break;
		}
		case FR_BASE_SPAWN: {
			NEW( data.base_spawn.name, std::string, *other.data.base_spawn.name );
			NEW( data.base_spawn.faction_id, std::string, *other.data.base_spawn.faction_id );
			break;
		}
		case FR_BASE_UPDATE: {
			NEW( data.base_update.name, std::string, *other.data.base_update.name );
			NEW( data.base_update.faction_id, std::string, *other.data.base_update.faction_id );
			NEW( data.base_update.pops, base_pops_t, *other.data.base_update.pops );
			break;
		}
		case FR_RESOURCE_DEFINE: {
			NEW( data.resource_define.serialized_resourcedef, std::string, *other.data.resource_define.serialized_resourcedef );
			break;
		}
		case FR_RESOURCE_UNDEFINE: {
			NEW( data.resource_undefine.id, std::string, *other.data.resource_undefine.id );
			break;
		}
		case FR_NORESOURCE_DEFINE: {
			NEW( data.noresource_define.serialized_noresource, std::string, *other.data.noresource_define.serialized_noresource );
			break;
		}
		case FR_LOADER_SHOW:
		case FR_LOADER_TEXT: {
			NEW( data.loader.text, std::string, *other.data.loader.text );
			break;
		}

		default: {
			//
		}
	}
}

FrontendRequest::~FrontendRequest() {
	switch ( type ) {
		case FR_QUIT: {
			DELETE( data.quit.reason );
			break;
		}
		case FR_ERROR: {
			DELETE( data.error.what );
			if ( data.error.stacktrace ) {
				DELETE( data.error.stacktrace );
			}
			break;
		}
		case FR_UPDATE_TILES: {
			DELETE( data.update_tiles.tile_updates );
			DELETE( data.update_tiles.sprite_actors );
			DELETE( data.update_tiles.sprite_removals );
			DELETE( data.update_tiles.sprite_additions );
			DELETE( data.update_tiles.serialized_terrain_texture_patch );
			if ( data.update_tiles.serialized_terrain_mesh ) {
				DELETE( data.update_tiles.serialized_terrain_mesh );
			}
			if ( data.update_tiles.serialized_terrain_data_mesh ) {
				DELETE( data.update_tiles.serialized_terrain_data_mesh );
			}
			break;
		}
		case FR_FACTION_DEFINE: {
			DELETE( data.faction_define.factiondefs );
			break;
		}
		case FR_SLOT_DEFINE: {
			DELETE( data.slot_define.slotdefs );
			break;
		}
		case FR_ANIMATION_DEFINE: {
			DELETE( data.animation_define.serialized_animation );
			break;
		}
		case FR_ANIMATION_UNDEFINE: {
			DELETE( data.animation_undefine.animation_id );
			break;
		}
		case FR_ANIMATION_SHOW: {
			DELETE( data.animation_show.animation_id );
			break;
		}
		case FR_UNIT_DEFINE: {
			DELETE( data.unit_define.serialized_unitdef );
			break;
		}
		case FR_UNIT_UNDEFINE: {
			DELETE( data.unit_undefine.id );
			break;
		}
		case FR_UNIT_SPAWN: {
			DELETE( data.unit_spawn.unitdef_id );
			DELETE( data.unit_spawn.morale_string );
			break;
		}
		case FR_UNIT_UPDATE: {
			DELETE( data.unit_update.morale_string );
			break;
		}
		case FR_BASE_POP_DEFINE: {
			DELETE( data.base_pop_define.serialized_popdef );
			break;
		}
		case FR_BASE_POP_UNDEFINE: {
			DELETE( data.base_pop_undefine.id );
			break;
		}
		case FR_BASE_SPAWN: {
			DELETE( data.base_spawn.name );
			DELETE( data.base_spawn.faction_id );
			break;
		}
		case FR_BASE_UPDATE: {
			DELETE( data.base_update.name );
			DELETE( data.base_update.faction_id );
			DELETE( data.base_update.pops );
			break;
		}
		case FR_RESOURCE_DEFINE: {
			DELETE( data.resource_define.serialized_resourcedef );
			break;
		}
		case FR_RESOURCE_UNDEFINE: {
			DELETE( data.resource_undefine.id );
			break;
		}
		case FR_NORESOURCE_DEFINE: {
			DELETE( data.noresource_define.serialized_noresource );
			break;
		}
		case FR_LOADER_TEXT:
		case FR_LOADER_SHOW: {
			DELETE( data.loader.text );
			break;
		}
		default: {
			//
		}
	}
}

}
