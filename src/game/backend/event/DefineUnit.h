#pragma once

#include "Event.h"

namespace game {
namespace backend {

namespace unit {
class Def;
}

namespace event {

class DefineUnit : public Event {
public:
	DefineUnit( const size_t initiator_slot, unit::Def* def );

	const std::string* Validate( Game* game ) const override;
	gse::Value* const Apply( Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const DefineUnit* event );
	static DefineUnit* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	unit::Def* m_def;
};

}
}
}
