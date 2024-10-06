#pragma once

#include <string>

#include "Event.h"

#include "game/backend/base/BaseData.h"

namespace game {
namespace backend {
namespace event {

class SpawnBase : public Event {
public:
	SpawnBase(
		const size_t initiator_slot,
		const size_t owner_slot,
		const size_t pos_x,
		const size_t pos_y,
		const base::BaseData& data
	);

	const std::string* Validate( Game* game ) const override;
	const gse::Value Apply( Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const SpawnBase* event );
	static SpawnBase* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	const size_t m_owner_slot;
	const size_t m_pos_x;
	const size_t m_pos_y;
	const base::BaseData m_data;
};

}
}
}
