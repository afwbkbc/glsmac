#pragma once

#include "Event.h"

namespace game {
namespace backend {
namespace event {

class AttackUnit : public Event {
public:
	AttackUnit( const size_t initiator_slot, const size_t attacker_unit_id, const size_t defender_unit_id );

	const std::string* Validate( Game* game ) const override;
	void Resolve( Game* game ) override;
	const gse::Value Apply( Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const AttackUnit* event );
	static AttackUnit* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	const size_t m_attacker_unit_id;
	const size_t m_defender_unit_id;

	gse::Value m_resolutions;

};

}
}
}
