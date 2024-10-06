#pragma once

#include "Event.h"

namespace game {
namespace backend {

namespace animation {
class Def;
}

namespace event {

class DefineAnimation : public Event {
public:
	DefineAnimation( const size_t initiator_slot, animation::Def* def );

	const std::string* Validate( Game* game ) const override;
	const gse::Value Apply( Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const DefineAnimation* event );
	static DefineAnimation* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	animation::Def* m_def;
};

}
}
}
