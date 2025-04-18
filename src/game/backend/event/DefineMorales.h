#pragma once

#include "LegacyEvent.h"

namespace game {
namespace backend {

namespace unit {
class MoraleSet;
}

namespace event {

class DefineMorales : public LegacyEvent {
public:
	DefineMorales( const size_t initiator_slot, unit::MoraleSet* moraleset );

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class LegacyEvent;

	static void Serialize( types::Buffer& buf, const DefineMorales* event );
	static DefineMorales* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	unit::MoraleSet* m_moraleset;
};

}
}
}
