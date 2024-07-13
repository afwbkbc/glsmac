#pragma once

#include "Event.h"

namespace game {
namespace backend {
namespace event {

class AdvanceTurn : public Event {
public:
	AdvanceTurn( const size_t initiator_slot, const size_t turn_id );

	const std::string* Validate( Game* game ) const override;
	const gse::Value Apply( Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	const size_t m_turn_id;

	static void Serialize( types::Buffer& buf, const AdvanceTurn* event );
	static AdvanceTurn* Unserialize( types::Buffer& buf, const size_t initiator_slot );

};

}
}
}
