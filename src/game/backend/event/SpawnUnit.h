#pragma once

#include "LegacyEvent.h"

#include "game/backend/unit/Types.h"

namespace game {
namespace backend {
namespace event {

class SpawnUnit : public LegacyEvent {
public:
	SpawnUnit(
		const size_t initiator_slot,
		const std::string& unit_def,
		const size_t owner_slot,
		const size_t pos_x,
		const size_t pos_y,
		const unit::morale_t morale,
		const unit::health_t health
	);

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class LegacyEvent;

	static void Serialize( types::Buffer& buf, const SpawnUnit* event );
	static SpawnUnit* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	std::string m_unit_def;
	const size_t m_owner_slot;
	const size_t m_pos_x;
	const size_t m_pos_y;
	const unit::morale_t m_morale;
	const unit::health_t m_health;
};

}
}
}
