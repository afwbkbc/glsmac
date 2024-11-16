#pragma once

#include "Event.h"

namespace game {
namespace backend {

namespace resource {
class Resource;
}

namespace event {

class DefineResource : public Event {
public:
	DefineResource( const size_t initiator_slot, resource::Resource* resource );

	const std::string* Validate( Game* game ) const override;
	const gse::Value Apply( Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const DefineResource* event );
	static DefineResource* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	resource::Resource* m_resource;
};

}
}
}
