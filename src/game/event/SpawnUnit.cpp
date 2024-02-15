#include "SpawnUnit.h"

#include "../Game.h"

namespace game {
namespace event {

SpawnUnit::SpawnUnit( const std::string& unit_def, const size_t pos_x, const size_t pos_y )
	: Event( ET_SPAWN_UNIT )
	, m_unit_def( unit_def )
	, m_pos_x( pos_x )
	, m_pos_y( pos_y ) {
	//
}

const gse::Value SpawnUnit::Apply( game::Game* game ) const {
	const auto* def = game->GetUnitDef( m_unit_def );
	ASSERT_NOLOG( def, "unit def '" + m_unit_def + "' not found" );
	auto* unit = new unit::Unit(
		unit::Unit::GetNextId(),
		def,
		m_pos_x,
		m_pos_y
	);
	game->SpawnUnit(
		unit
	);
	return unit->Wrap();
}

void SpawnUnit::Serialize( types::Buffer& buf, const SpawnUnit* event ) {
	buf.WriteString( event->m_unit_def );
	buf.WriteInt( event->m_pos_x );
	buf.WriteInt( event->m_pos_y );
}

SpawnUnit* SpawnUnit::Unserialize( types::Buffer& buf ) {
	const auto unit_def = buf.ReadString();
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	return new SpawnUnit( unit_def, pos_x, pos_y );
}

}
}
