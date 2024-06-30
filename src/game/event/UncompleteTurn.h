#pragma once

#include "Event.h"

namespace game {
namespace event {

class UncompleteTurn : public Event {
public:
	UncompleteTurn( const size_t initiator_slot, const size_t turn_id );

	const std::string* Validate( Game* game ) const override;
	const gse::Value Apply( game::Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	const size_t m_turn_id;

	static void Serialize( types::Buffer& buf, const UncompleteTurn* event );
	static UncompleteTurn* Unserialize( types::Buffer& buf, const size_t initiator_slot );

};

}
}
