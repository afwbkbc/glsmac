#include "SpawnUnit.h"

#include "../Game.h"
#include "../unit/Unit.h"

namespace game {
namespace event {

SpawnUnit::SpawnUnit( const std::string& unit_def, const size_t pos_x, const size_t pos_y )
	: Event( ET_SPAWN_UNIT )
	, m_unit_def( unit_def )
	, m_pos_x( pos_x )
	, m_pos_y( pos_y ) {
	//
}

void SpawnUnit::Apply( game::Game* game ) const {
	const auto* def = game->GetUnitDef( m_unit_def );
	ASSERT( def, "unit def '" + m_unit_def + "' not found" );
	game->SpawnUnit(
		new unit::Unit(
			unit::Unit::GetNextId(),
			def,
			m_pos_x,
			m_pos_y
		)
	);
}

const types::Buffer SpawnUnit::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( m_event_type );
	buf.WriteString( m_unit_def );
	buf.WriteInt( m_pos_x );
	buf.WriteInt( m_pos_y );

	return buf;
}

void SpawnUnit::Unserialize( types::Buffer buf ) {

	ASSERT( buf.ReadInt() == m_event_type, "event type mismatch" );
	m_unit_def = buf.ReadString();
	m_pos_x = buf.ReadInt();
	m_pos_y = buf.ReadInt();

}

}
}
