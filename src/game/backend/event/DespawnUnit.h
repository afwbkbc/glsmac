#pragma once

#include "LegacyEvent.h"

namespace game {
namespace backend {
namespace event {

class DespawnUnit : public LegacyEvent {
public:
	DespawnUnit( const size_t initiator_slot, const size_t unit_id );

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class LegacyEvent;

	static void Serialize( types::Buffer& buf, const DespawnUnit* event );
	static DespawnUnit* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	const size_t m_unit_id;
};

}
}
}
