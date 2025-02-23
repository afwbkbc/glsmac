#pragma once

#include "Event.h"

namespace game {
namespace backend {
namespace event {

class CompleteTurn : public Event {
public:
	CompleteTurn( const size_t initiator_slot, const size_t turn_id );

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	const size_t m_turn_id;

	static void Serialize( types::Buffer& buf, const CompleteTurn* event );
	static CompleteTurn* Unserialize( types::Buffer& buf, const size_t initiator_slot );

};

}
}
}
