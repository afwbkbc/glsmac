#pragma once

#include "Event.h"

#include "gse/type/Undefined.h"

#include "game/map/Tile.h"

namespace game {
namespace event {

class MoveUnit : public Event {
public:
	MoveUnit( const size_t initiator_slot, const size_t unit_id, const game::map::Tile::direction_t direction );

	const std::string* Validate( Game* game ) const override;
	void Resolve( Game* game ) override;
	const gse::Value Apply( game::Game* game ) const override;
	TS_DEF()

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const MoveUnit* event );
	static MoveUnit* Unserialize( types::Buffer& buf, const size_t initiator_slot );

private:
	const size_t m_unit_id;
	const game::map::Tile::direction_t m_direction;

	gse::Value m_resolutions = VALUE( gse::type::Undefined );

};

}
}
