#pragma once

#include "LegacyEvent.h"

namespace game {
namespace backend {

namespace animation {
class Def;
}

namespace event {

class DefineAnimation : public LegacyEvent {
public:
	DefineAnimation( const size_t initiator_slot, animation::Def* def );

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class LegacyEvent;

	static void Serialize( types::Buffer& buf, const DefineAnimation* event );
	static DefineAnimation* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	animation::Def* m_def;
};

}
}
}
