#pragma once

#include <cstddef>

#include "Def.h"

namespace game {
namespace unit {

class Unit {
public:

	static const size_t GetNextId();

	Unit( const size_t id, const Def* def, const size_t pos_x, const size_t pos_y );

	const size_t GetId() const;
	const Def* GetDef() const;
	const size_t GetPosX() const;
	const size_t GetPosY() const;

private:
	const size_t m_id;
	const Def* m_def;
	size_t m_pos_x;
	size_t m_pos_y;
};

}
}
