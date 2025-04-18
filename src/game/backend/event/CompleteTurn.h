#pragma once

#include "LegacyEvent.h"

namespace game {
namespace backend {
namespace event {

class CompleteTurn : public LegacyEvent {
public:
	CompleteTurn( const size_t initiator_slot, const size_t turn_id );

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class LegacyEvent;

	const size_t m_turn_id;

	static void Serialize( types::Buffer& buf, const CompleteTurn* event );
	static CompleteTurn* Unserialize( types::Buffer& buf, const size_t initiator_slot );

};

}
}
}
