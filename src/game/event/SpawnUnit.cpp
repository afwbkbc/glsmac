#include "SpawnUnit.h"

#include "game/Game.h"
#include "game/State.h"

namespace game {
namespace event {

SpawnUnit::SpawnUnit( const std::string& unit_def, const size_t owner_index, const size_t pos_x, const size_t pos_y, const unit::Unit::morale_t morale )
	: Event( ET_UNIT_SPAWN )
	, m_unit_def( unit_def )
	, m_owner_index( owner_index )
	, m_pos_x( pos_x )
	, m_pos_y( pos_y )
	, m_morale( morale ) {
	//
}

const gse::Value SpawnUnit::Apply( game::Game* game ) const {
	const auto* def = game->GetUnitDef( m_unit_def );
	ASSERT_NOLOG( def, "unit def '" + m_unit_def + "' not found" );
	auto& owner = game->GetState()->m_slots.GetSlot( m_owner_index );
	auto* tile = game->GetMap()->GetTile( m_pos_x, m_pos_y );
	auto* unit = new unit::Unit(
		unit::Unit::GetNextId(),
		def,
		&owner,
		tile,
		m_morale
	);
	game->SpawnUnit(
		unit
	);
	return unit->Wrap();
}

void SpawnUnit::Serialize( types::Buffer& buf, const SpawnUnit* event ) {
	buf.WriteString( event->m_unit_def );
	buf.WriteInt( event->m_owner_index );
	buf.WriteInt( event->m_pos_x );
	buf.WriteInt( event->m_pos_y );
	buf.WriteInt( event->m_morale );
}

SpawnUnit* SpawnUnit::Unserialize( types::Buffer& buf ) {
	const auto unit_def = buf.ReadString();
	const auto owner_index = buf.ReadInt();
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	const auto morale = (unit::Unit::morale_t)buf.ReadInt();
	return new SpawnUnit( unit_def, owner_index, pos_x, pos_y, morale );
}

}
}
