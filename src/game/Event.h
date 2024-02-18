#pragma once

#include <vector>
#include <string>

#include "unit/Unit.h"

namespace game {

class Event {
public:

	enum event_type_t {
		ET_NONE,
		ET_QUIT,
		ET_ERROR,
		ET_GLOBAL_MESSAGE,
		ET_TURN_COMPLETE_STATUS,
		ET_SLOT_DEFINE,
		ET_UNIT_DEFINE,
		ET_UNIT_SPAWN,
		ET_UNIT_DESPAWN,
	};
	Event( const event_type_t type );
	Event( const Event& other );
	virtual ~Event();

	const event_type_t type = ET_NONE;

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
			const std::string* unitdef_name;
			size_t slot_index;
			struct {
				float x;
				float y;
				float z;
			} coords;
			bool is_active;
			unit::Unit::morale_t morale;
		} unit_spawn;
		struct {
			size_t unit_id;
		} unit_despawn;
	} data;
};

typedef std::vector< Event > game_events_t;

}
