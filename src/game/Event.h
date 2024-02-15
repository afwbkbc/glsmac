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
		ET_UNIT_SPAWN,
		ET_UNIT_DESPAWN,
	};
	Event( const event_type_t type );
	Event( const Event& other );
	virtual ~Event();

	const event_type_t type = ET_NONE;

	union {
		struct {
			std::string* reason;
		} quit;
		struct {
			std::string* what;
			std::string* stacktrace;
		} error;
		struct {
			std::string* message;
		} global_message;
		struct {
			bool is_turn_complete;
		} turn_complete_status;
		struct {
			std::string* serialized_unit;
			struct {
				float x;
				float y;
				float z;
			} coords;
		} unit_spawn;
		struct {
			size_t unit_id;
		} unit_despawn;
	} data;
};

typedef std::vector< Event > game_events_t;

}
