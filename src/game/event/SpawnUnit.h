#pragma once

#include "Event.h"

namespace game {
namespace event {

class SpawnUnit : public Event {
public:
	SpawnUnit( const std::string& unit_def, const size_t pos_x, const size_t pos_y );

	void Apply( game::Game* game ) const override;

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

private:
	std::string m_unit_def;
	size_t m_pos_x;
	size_t m_pos_y;
};

}
}
