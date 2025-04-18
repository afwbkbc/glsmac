#pragma once

#include "LegacyEvent.h"

namespace game {
namespace backend {

namespace unit {
class Def;
}

namespace event {

class DefineUnit : public LegacyEvent {
public:
	DefineUnit( const size_t initiator_slot, unit::Def* def );

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class LegacyEvent;

	static void Serialize( types::Buffer& buf, const DefineUnit* event );
	static DefineUnit* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	unit::Def* m_def;
};

}
}
}
