#pragma once

#include "types/Buffer.h"

namespace game {
class Game;
namespace event {

class Event {
public:
	enum event_type_t {
		ET_SPAWN_UNIT,
	};

	Event( const event_type_t type );
	virtual ~Event() = default;

	static const types::Buffer Serialize( const Event* event );
	static Event* Unserialize( types::Buffer& buf );

	const event_type_t m_type;

	virtual void Apply( Game* game ) const = 0;

protected:
};

}
}
