#pragma once

#include <string>

#include "LegacyEvent.h"

namespace game {
namespace backend {
namespace event {

class SpawnBase : public LegacyEvent {
public:
	SpawnBase(
		const size_t initiator_slot,
		const size_t owner_slot,
		const size_t pos_x,
		const size_t pos_y,
		const std::string& name
	);

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class LegacyEvent;

	static void Serialize( types::Buffer& buf, const SpawnBase* event );
	static SpawnBase* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	const size_t m_owner_slot;
	const size_t m_pos_x;
	const size_t m_pos_y;
	const std::string m_name;
};

}
}
}
