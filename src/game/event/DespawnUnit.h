#pragma once

#include "Event.h"

namespace game {
namespace event {

class DespawnUnit : public Event {
public:
	DespawnUnit( const size_t unit_id );

	const gse::Value Apply( game::Game* game ) const override;

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const DespawnUnit* event );
	static DespawnUnit* Unserialize( types::Buffer& buf );

private:
	size_t m_unit_id;
};

}
}
