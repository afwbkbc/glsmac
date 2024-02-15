#pragma once

#include "types/Buffer.h"

#include "gse/Value.h"

namespace game {
class Game;
namespace event {

class Event {
public:
	enum event_type_t {
		ET_NONE,
		ET_UNIT_DEFINE,
		ET_UNIT_SPAWN,
		ET_UNIT_DESPAWN,
	};

	Event( const event_type_t type );
	virtual ~Event() = default;

	static const types::Buffer Serialize( const Event* event );
	static Event* Unserialize( types::Buffer& buf );

	static const types::Buffer SerializeMultiple( const std::vector< const Event* >& events );
	static void UnserializeMultiple( types::Buffer& buf, std::vector< const Event* >& events_out );

	const event_type_t m_type;

	virtual const gse::Value Apply( Game* game ) const = 0;

protected:
};

}
}
