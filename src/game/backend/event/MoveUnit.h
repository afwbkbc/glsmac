#pragma once

#include "Event.h"

#include "game/backend/map/tile/Types.h"

namespace game {
namespace backend {
namespace event {

class MoveUnit : public Event {
public:
	MoveUnit( const size_t initiator_slot, const size_t unit_id, const backend::map::tile::direction_t direction );

	const std::string* Validate( GSE_CALLABLE, Game* game ) const override;
	void Resolve( GSE_CALLABLE, Game* game ) override;
	gse::Value* const Apply( GSE_CALLABLE, Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const MoveUnit* event );
	static MoveUnit* Unserialize( GSE_CALLABLE, types::Buffer& buf, const size_t initiator_slot );

private:
	const size_t m_unit_id;
	const backend::map::tile::direction_t m_direction;

	gse::Value* m_resolutions = nullptr;

};

}
}
}
