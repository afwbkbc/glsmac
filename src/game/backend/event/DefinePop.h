#pragma once

#include "LegacyEvent.h"

namespace game {
namespace backend {

namespace base {
class PopDef;
}

namespace event {

class DefinePop : public LegacyEvent {
public:
	DefinePop( const size_t initiator_slot, base::PopDef* pop_def );

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class LegacyEvent;

	static void Serialize( types::Buffer& buf, const DefinePop* event );
	static DefinePop* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	base::PopDef* m_pop_def;
};

}
}
}
