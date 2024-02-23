#pragma once

#include <vector>

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

	Event( const size_t initiator_slot, const event_type_t type );
	virtual ~Event() = default;

	static const types::Buffer Serialize( const Event* event );
	static Event* Unserialize( types::Buffer& buf );

	static const types::Buffer SerializeMultiple( const std::vector< Event* >& events );
	static void UnserializeMultiple( types::Buffer& buf, std::vector< Event* >& events_out );

	const event_type_t m_type;
	const size_t m_initiator_slot;

	virtual const std::string* Validate( const Game* game ) const = 0;
	virtual const gse::Value Apply( Game* game ) const = 0;
	virtual const std::string ToString( const std::string& prefix = "" ) const = 0;

	bool m_is_validated = false;

protected:
};

}
}
