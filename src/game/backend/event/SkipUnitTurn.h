#pragma once

#include "Event.h"

namespace game {
namespace backend {
namespace event {

class SkipUnitTurn : public Event {
public:
	SkipUnitTurn( const size_t initiator_slot, const size_t unit_id );

	const std::string* Validate( Game* game ) const override;
	gse::Value* const Apply( Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const SkipUnitTurn* event );
	static SkipUnitTurn* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	const size_t m_unit_id;
};

}
}
}
