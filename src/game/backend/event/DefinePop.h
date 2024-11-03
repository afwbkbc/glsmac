#pragma once

#include "Event.h"

namespace game {
namespace backend {

namespace base {
class PopDef;
}

namespace event {

class DefinePop : public Event {
public:
	DefinePop( const size_t initiator_slot, base::PopDef* pop_def );

	const std::string* Validate( Game* game ) const override;
	const gse::Value Apply( Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const DefinePop* event );
	static DefinePop* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	base::PopDef* m_pop_def;
};

}
}
}
