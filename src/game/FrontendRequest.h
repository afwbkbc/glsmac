#pragma once

#include <vector>
#include <string>

#include "unit/Types.h"
#include "game/turn/Types.h"

namespace game {

namespace map::tile {
class Tile;
class TileState;
}

namespace rules {
class Faction;
}

class FrontendRequest {
public:

	enum request_type_t {
		FR_NONE,
		FR_QUIT,
		FR_ERROR,
		FR_GLOBAL_MESSAGE,
		FR_UPDATE_TILES,
		FR_TURN_STATUS,
		FR_TURN_ADVANCE,
		FR_FACTION_DEFINE,
		FR_SLOT_DEFINE,
		FR_ANIMATION_DEFINE,
		FR_ANIMATION_SHOW,
		FR_UNIT_DEFINE,
		FR_UNIT_SPAWN,
		FR_UNIT_DESPAWN,
		FR_UNIT_UPDATE,
		FR_UNIT_MOVE,
	};
	FrontendRequest( const request_type_t type );
	FrontendRequest( const FrontendRequest& other );
	virtual ~FrontendRequest();

	const request_type_t type = FR_NONE;

	typedef std::vector< const game::rules::Faction* > faction_defines_t;

	struct slot_define_t {
		size_t slot_index;
		std::string faction_id;
	};
	typedef std::vector< slot_define_t > slot_defines_t;

	typedef std::vector< std::pair< map::tile::Tile*, map::tile::TileState* > > tile_updates_t;

	union {
		struct {
			const std::string* reason;
		} quit;
		struct {
			const std::string* what;
			const std::string* stacktrace;
		} error;
		struct {
			const std::string* message;
		} global_message;
		struct {
			const tile_updates_t* tile_updates;
		} update_tiles;
		struct {
			turn::turn_status_t status;
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
			size_t running_animation_id;
			struct {
				float x;
				float y;
				float z;
			} render_coords;
		} animation_show;
		struct {
			const std::string* serialized_unitdef; // can be optimized
		} unit_define;
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
			unit::movement_t movement;
			unit::morale_t morale;
			const std::string* morale_string;
			unit::health_t health;
		} unit_spawn;
		struct {
			size_t unit_id;
		} unit_despawn;
		struct {
			size_t unit_id;
			unit::movement_t movement;
			unit::health_t health;
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
	} data;
};

}
