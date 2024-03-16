#pragma once

#include "Event.h"

namespace game {
namespace event {

class CompleteTurn : public Event {
public:
	CompleteTurn( const size_t initiator_slot );

	const std::string* Validate( const Game* game ) const override;
	const gse::Value Apply( game::Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const CompleteTurn* event );
	static CompleteTurn* Unserialize( types::Buffer& buf, const size_t initiator_slot );

};

}
}
