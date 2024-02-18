#pragma once

#include "Event.h"

#include "game/unit/Unit.h"

namespace game {
namespace event {

class SpawnUnit : public Event {
public:
	SpawnUnit(
		const std::string& unit_def,
		const size_t owner_index,
		const size_t pos_x,
		const size_t pos_y,
		const unit::Unit::morale_t morale,
		const unit::Unit::health_t health
	);

	const gse::Value Apply( game::Game* game ) const override;

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const SpawnUnit* event );
	static SpawnUnit* Unserialize( types::Buffer& buf );

private:
	std::string m_unit_def;
	const size_t m_owner_index;
	const size_t m_pos_x;
	const size_t m_pos_y;
	const unit::Unit::morale_t m_morale;
	const unit::Unit::health_t m_health;
};

}
}
