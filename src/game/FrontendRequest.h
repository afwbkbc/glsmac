#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "backend/unit/Types.h"
#include "backend/turn/Types.h"
#include "backend/map/Types.h"
#include "backend/map/tile/Types.h"
#include "types/Vec3.h"

namespace game {

namespace backend::map::tile {
class Tile;
class TileState;
}

struct tile_render_layer_t {
	backend::map::tile::tile_vertices_t coords = {};
	backend::map::tile::tile_tex_coords_t tex_coords = {};
	backend::map::tile::tile_colors_t colors = {};
};

struct tile_render_snapshot_t {
	tile_render_snapshot_t() = default;
	tile_render_snapshot_t(
		const backend::map::tile::Tile& tile,
		const backend::map::tile::TileState& tile_state
	);

	backend::map::tile::coords_t coords = {};
	bool is_water = false;
	bool west_is_water = false;
	bool north_is_water = false;
	bool east_is_water = false;
	bool south_is_water = false;
	bool is_coastline_corner = false;
	backend::map::tile::elevation_t elevation = 0;
	backend::map::tile::moisture_t moisture = backend::map::tile::MOISTURE_NONE;
	backend::map::tile::rockiness_t rockiness = backend::map::tile::ROCKINESS_NONE;
	backend::map::tile::bonus_t bonus = backend::map::tile::BONUS_NONE;
	backend::map::tile::feature_t features = backend::map::tile::FEATURE_NONE;
	backend::map::tile::terraforming_t terraforming = backend::map::tile::TERRAFORMING_NONE;
	tile_render_layer_t layers[ backend::map::tile::LAYER_MAX ] = {};
	std::vector< std::string > sprites = {};
};

namespace backend {
namespace faction {
class Faction;
}
}

class FrontendRequest {
public:

	enum request_type_t {
		FR_NONE,
		FR_QUIT,
		FR_ERROR,
		FR_UPDATE_TILES,
		FR_TURN_STATUS,
		FR_TURN_ADVANCE,
		FR_FACTION_DEFINE,
		FR_SLOT_DEFINE,
		FR_ANIMATION_DEFINE,
		FR_ANIMATION_UNDEFINE,
		FR_ANIMATION_SHOW,
		FR_ANIMATION_ABORT,
		FR_TILE_SELECT,
		FR_UNIT_SELECT,
		FR_BASE_SELECT,
		FR_UNIT_DEFINE,
		FR_UNIT_UNDEFINE,
		FR_UNIT_SPAWN,
		FR_UNIT_DESPAWN,
		FR_UNIT_UPDATE,
		FR_UNIT_MOVE,
		FR_BASE_POP_DEFINE,
		FR_BASE_POP_UNDEFINE,
		FR_BASE_SPAWN,
		FR_BASE_DESPAWN,
		FR_BASE_UPDATE,
		FR_RESOURCE_DEFINE,
		FR_RESOURCE_UNDEFINE,
		FR_NORESOURCE_DEFINE,
		FR_NORESOURCE_UNDEFINE,
		FR_LOADER_SHOW,
		FR_LOADER_TEXT,
		FR_LOADER_HIDE,
		FR_UNIT_TELEPORT,
	};
	FrontendRequest( const request_type_t type );
	FrontendRequest( const FrontendRequest& other );
	virtual ~FrontendRequest();

	const request_type_t type = FR_NONE;

	typedef std::vector< const backend::faction::Faction* > faction_defines_t;

	struct slot_define_t {
		size_t slot_index;
		std::string faction_id;
	};
	typedef std::vector< slot_define_t > slot_defines_t;

	typedef std::vector< tile_render_snapshot_t > tile_updates_t;
	typedef std::unordered_map< std::string, backend::map::sprite_actor_t > tile_sprite_actors_t;
	typedef std::unordered_map< size_t, std::string > tile_sprite_removals_t;
	typedef std::unordered_map< size_t, std::pair< std::string, types::Vec3 > > tile_sprite_additions_t;

	struct base_pop_t {
		std::string type;
		uint8_t variant;
	};
	typedef std::vector< base_pop_t > base_pops_t;

	typedef std::vector< std::pair< std::string, size_t > > tile_yields_t;

	union {
		struct {
			const std::string* reason;
		} quit;
		struct {
			const std::string* what;
			const std::string* stacktrace;
		} error;
		struct {
			const tile_updates_t* tile_updates;
			const tile_sprite_actors_t* sprite_actors;
			const tile_sprite_removals_t* sprite_removals;
			const tile_sprite_additions_t* sprite_additions;
			const std::string* serialized_terrain_texture_patch;
			const std::string* serialized_terrain_mesh;
			const std::string* serialized_terrain_data_mesh;
			size_t terrain_texture_x;
			size_t terrain_texture_y;
			size_t terrain_texture_width;
			size_t terrain_texture_height;
		} update_tiles;
		struct {
			size_t tile_x;
			size_t tile_y;
			const tile_yields_t* tile_yields;
		} tile_data;
		struct {
			backend::turn::turn_status_t status;
		} turn_status;
		struct {
			size_t turn_id;
		} turn_advance;
		struct {
			faction_defines_t* factiondefs;
		} faction_define;
		struct {
			slot_defines_t* slotdefs;
		} slot_define;
		struct {
			const std::string* serialized_animation; // can be optimized
		} animation_define;
		struct {
			const std::string* animation_id;
		} animation_undefine;
		struct {
			const std::string* animation_id;
			size_t running_animation_id;
			struct {
				float x;
				float y;
				float z;
			} render_coords;
		} animation_show;
		struct {
			size_t running_animation_id;
		} animation_abort;
		struct {
			size_t x;
			size_t y;
		} tile_select;
		struct {
			size_t unit_id;
		} unit_select;
		struct {
			size_t base_id;
		} base_select;
		struct {
			const std::string* serialized_unitdef; // can be optimized
		} unit_define;
		struct {
			const std::string* id;
		} unit_undefine;
		struct {
			size_t unit_id;
			const std::string* unitdef_id;
			size_t slot_index;
			struct {
				size_t x;
				size_t y;
			} tile_coords;
			struct {
				float x;
				float y;
				float z;
			} render_coords;
			backend::unit::movement_t movement;
			backend::unit::morale_t morale;
			const std::string* morale_string;
			backend::unit::health_t health;
			bool embarked;
		} unit_spawn;
		struct {
			size_t unit_id;
		} unit_despawn;
		struct {
			size_t unit_id;
			backend::unit::movement_t movement;
			backend::unit::morale_t morale;
			const std::string* morale_string;
			backend::unit::health_t health;
			bool embarked;
			struct {
				size_t x;
				size_t y;
			} tile_coords;
			struct {
				float x;
				float y;
				float z;
			} render_coords; // TODO: store render coords of tiles on frontend
		} unit_update;
		struct {
			size_t unit_id;
			size_t running_animation_id;
			struct {
				size_t x;
				size_t y;
			} dst_tile_coords;
		} unit_move;
		struct {
			size_t unit_id;
			struct {
				size_t x;
				size_t y;
			} dst_tile_coords;
		} unit_teleport;
		struct {
			const std::string* serialized_popdef;
		} base_pop_define;
		struct {
			const std::string* id;
		} base_pop_undefine;
		struct {
			size_t base_id;
			size_t slot_index;
			const std::string* faction_id;
			struct {
				size_t x;
				size_t y;
			} tile_coords;
			struct {
				float x;
				float y;
				float z;
			} render_coords;
			const std::string* name;
		} base_spawn;
		struct {
			size_t base_id;
		} base_despawn;
		struct {
			size_t base_id;
			size_t slot_index;
			const std::string* faction_id;
			const std::string* name;
			base_pops_t* pops;
		} base_update;
		struct {
			const std::string* serialized_resourcedef; // can be optimized
		} resource_define;
		struct {
			const std::string* id;
		} resource_undefine;
		struct {
			const std::string* serialized_noresource; // can be optimized
		} noresource_define;
		struct {
			const std::string* text;
		} loader;
	} data;
};

}
