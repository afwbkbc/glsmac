#pragma once

#include "Event.h"

namespace game {
namespace backend {

namespace unit {
class MoraleSet;
}

namespace event {

class DefineMorales : public Event {
public:
	DefineMorales( const size_t initiator_slot, unit::MoraleSet* moraleset );

	const std::string* Validate( Game* game ) const override;
	const gse::Value Apply( Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const DefineMorales* event );
	static DefineMorales* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	unit::MoraleSet* m_moraleset;
};

}
}
}
