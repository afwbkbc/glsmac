#pragma once

#include "Event.h"

namespace game {
namespace event {

class FinalizeTurn : public Event {
public:
	FinalizeTurn( const size_t initiator_slot );

	const std::string* Validate( Game* game ) const override;
	const gse::Value Apply( game::Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const FinalizeTurn* event );
	static FinalizeTurn* Unserialize( types::Buffer& buf, const size_t initiator_slot );

};

}
}
