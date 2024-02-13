#pragma once

#include "types/Serializable.h"

namespace game {
class Game;
namespace event {

CLASS( Event, types::Serializable )
public:
	enum event_type_t {
		ET_SPAWN_UNIT,
	};

	Event( const event_type_t event_type );

	virtual void Apply( Game* game ) const = 0;

protected:
	const event_type_t m_event_type;
};

}
}
