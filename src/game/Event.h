#pragma once

#include <vector>
#include <string>

namespace game {

class Event {
public:

	enum event_type_t {
		ET_NONE,
		ET_QUIT,
	};
	Event( const event_type_t type );
	Event( const Event& other );
	virtual ~Event();

	const event_type_t type = ET_NONE;

	union {
		struct {
			std::string* reason;
		} quit;
	} data;
};

typedef std::vector< Event > game_events_t;

}
