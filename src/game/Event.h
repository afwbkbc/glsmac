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
		ET_GLOBAL_MESSAGE,
		ET_SPAWN_UNIT,
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
			std::string* message;
		} global_message;
		struct {
			std::string* serialized_unit;
			struct {
				float x;
				float y;
				float z;
			} coords;
		} spawn_unit;
	} data;
};

typedef std::vector< Event > game_events_t;

}
