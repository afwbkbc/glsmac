#pragma once

#include "Event.h"

namespace game {
namespace event {

class SpawnUnit : public Event {
public:
	SpawnUnit( const std::string& unit_def, const size_t pos_x, const size_t pos_y );

	const gse::Value Apply( game::Game* game ) const override;

private:
	friend class Event;

	static void Serialize( types::Buffer& buf, const SpawnUnit* event );
	static SpawnUnit* Unserialize( types::Buffer& buf );

private:
	std::string m_unit_def;
	size_t m_pos_x;
	size_t m_pos_y;
};

}
}
