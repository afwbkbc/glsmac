#pragma once

#include <vector>
#include <string>

#include "unit/Unit.h"

namespace game {

class FrontendRequest {
public:

	enum request_type_t {
		FR_NONE,
		FR_QUIT,
		FR_ERROR,
		FR_GLOBAL_MESSAGE,
		FR_TURN_ACTIVE_STATUS,
		FR_TURN_COMPLETE_STATUS,
		FR_SLOT_DEFINE,
		FR_UNIT_DEFINE,
		FR_UNIT_SPAWN,
		FR_UNIT_DESPAWN,
		FR_UNIT_REFRESH,
		FR_UNIT_MOVE
	};
	FrontendRequest( const request_type_t type );
	FrontendRequest( const FrontendRequest& other );
	virtual ~FrontendRequest();

	const request_type_t type = FR_NONE;

	struct slot_define_t {
		size_t slot_index;
		// TODO: name etc
		struct {
			float r;
			float g;
			float b;
			float a;
		} color;
		bool is_progenitor;
	};
	typedef std::vector< slot_define_t > slot_defines_t;

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
			bool is_turn_active;
		} turn_active_status;
		struct {
			bool is_turn_complete;
		} turn_complete_status;
		struct {
			slot_defines_t* slotdefs;
		} slot_define;
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
			unit::Unit::movement_t movement;
			unit::Unit::morale_t morale;
			unit::Unit::health_t health;
		} unit_spawn;
		struct {
			size_t unit_id;
		} unit_despawn;
		struct {
			size_t unit_id;
			unit::Unit::movement_t movement_left;
		} unit_refresh;
		struct {
			size_t unit_id;
			struct {
				size_t x;
				size_t y;
			} tile_coords;
			struct {
				float x;
				float y;
				float z;
			} render_coords;
			unit::Unit::movement_t movement_left;
		} unit_move;
	} data;
};

}
