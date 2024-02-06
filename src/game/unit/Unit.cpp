#include "Unit.h"

namespace game {
namespace unit {

static size_t next_id = 1;
const size_t Unit::GetNextId() {
	return next_id;
}

Unit::Unit( const size_t id, const Def* def, const size_t pos_x, const size_t pos_y )
	: m_id( id )
	, m_def( def )
	, m_pos_x( pos_x )
	, m_pos_y( pos_y ) {
	if ( next_id <= id ) {
		next_id = id + 1;
	}
}

const size_t Unit::GetId() const {
	return m_id;
}

const Def* Unit::GetDef() const {
	return m_def;
}

const size_t Unit::GetPosX() const {
	return m_pos_x;
}

const size_t Unit::GetPosY() const {
	return m_pos_y;
}

}
}
