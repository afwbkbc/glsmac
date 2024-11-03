#pragma once

#include <vector>

#include "types/Buffer.h"
#include "gse/Value.h"

namespace game {
namespace backend {

class Game;

namespace event {

class Event {
public:
	enum event_type_t {
		ET_NONE,
		ET_RESOURCE_DEFINE,
		ET_ANIMATION_DEFINE,
		ET_UNIT_DEFINE_MORALES,
		ET_UNIT_DEFINE,
		ET_UNIT_SPAWN,
		ET_UNIT_DESPAWN,
		ET_UNIT_MOVE,
		ET_UNIT_ATTACK,
		ET_UNIT_SKIP_TURN,
		ET_BASE_DEFINE_POP,
		ET_BASE_SPAWN,
		ET_COMPLETE_TURN,
		ET_UNCOMPLETE_TURN,
		ET_FINALIZE_TURN,
		ET_TURN_FINALIZED,
		ET_ADVANCE_TURN,
		ET_REQUEST_TILE_LOCKS,
		ET_LOCK_TILES,
		ET_REQUEST_TILE_UNLOCKS,
		ET_UNLOCK_TILES,
	};

	Event( const size_t initiator_slot, const event_type_t type );
	virtual ~Event() = default;

	static const types::Buffer Serialize( const Event* event );
	static Event* Unserialize( types::Buffer& buf );

	static const types::Buffer SerializeMultiple( const std::vector< Event* >& events );
	static void UnserializeMultiple( types::Buffer& buf, std::vector< Event* >& events_out );

	static const bool IsBroadcastable( const event_type_t type );

	const event_type_t m_type;
	const size_t m_initiator_slot;

	void SetDestinationSlot( const uint8_t destination_slot );
	const bool IsProcessableBy( const uint8_t destination_slot ) const;
	const bool IsSendableTo( const uint8_t destination_slot ) const;

	virtual const std::string* Validate( Game* game ) const = 0;
	virtual void Resolve( Game* game ) {};
	virtual const gse::Value Apply( Game* game ) const = 0;
	virtual const std::string ToString( const std::string& prefix = "" ) const = 0;

	bool m_is_validated = false;

protected:
	const std::string* Ok() const;
	const std::string* Error( const std::string& text ) const;

private:
	bool m_is_public = true;
	uint8_t m_destination_slot = 0;
};

}
}
}
