#pragma once

#include "Event.h"

#include "game/unit/Unit.h"

namespace game {
namespace event {

class SpawnUnit : public Event {
public:
	SpawnUnit(
		const size_t initiator_slot,
		const std::string& unit_def,
		const size_t owner_slot,
		const size_t pos_x,
		const size_t pos_y,
		const unit::Morale::morale_t morale,
		const unit::Unit::health_t health
	);

	const std::string* Validate( Game* game ) const;
	const gse::Value Apply( game::Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const SpawnUnit* event );
	static SpawnUnit* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	std::string m_unit_def;
	const size_t m_owner_slot;
	const size_t m_pos_x;
	const size_t m_pos_y;
	const unit::Morale::morale_t m_morale;
	const unit::Unit::health_t m_health;
};

}
}
