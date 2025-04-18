#pragma once

#include "LegacyEvent.h"

namespace game {
namespace backend {
namespace event {

class FinalizeTurn : public LegacyEvent {
public:
	FinalizeTurn( const size_t initiator_slot );

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class LegacyEvent;

	static void Serialize( types::Buffer& buf, const FinalizeTurn* event );
	static FinalizeTurn* Unserialize( types::Buffer& buf, const size_t initiator_slot );

};

}
}
}
