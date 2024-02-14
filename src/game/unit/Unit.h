#pragma once

#include <cstddef>

#include "Def.h"

namespace game {
namespace unit {

class Unit {
public:

	static const size_t GetNextId();

	Unit( const size_t id, const Def* def, const size_t pos_x, const size_t pos_y );
	virtual ~Unit() = default;

	const size_t m_id;
	const Def* m_def;

	size_t m_pos_x;
	size_t m_pos_y;

	static const types::Buffer Serialize( const Unit* unit );
	static Unit* Unserialize( types::Buffer& buf );

};

}
}
