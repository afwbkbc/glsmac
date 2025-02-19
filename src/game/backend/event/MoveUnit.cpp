#include "MoveUnit.h"

#include "game/backend/Game.h"
#include "game/backend/unit/UnitManager.h"
#include "game/backend/unit/StaticDef.h"
#include "game/backend/unit/Unit.h"
#include "game/backend/slot/Slot.h"
#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

MoveUnit::MoveUnit( const size_t initiator_slot, const size_t unit_id, const backend::map::tile::direction_t direction )
	: Event( initiator_slot, ET_UNIT_MOVE )
	, m_unit_id( unit_id )
	, m_direction( direction )
	, m_resolutions( VALUE( gse::value::Undefined ) ) {

}

const std::string* MoveUnit::Validate( Game* game ) const {
	auto* unit = game->GetUM()->GetUnit( m_unit_id );
	if ( !unit ) {
		return Error( "Unit not found" );
	}

	if ( unit->m_owner->GetIndex() != m_initiator_slot ) {
		return Error( "Unit can only be moved by it's owner" );
	}

	auto* src_tile = unit->GetTile();
	ASSERT_NOLOG( src_tile, "src tile not set" );
	if ( src_tile->IsLocked() ) {
		return Error( "Source tile is locked" );
	}
	auto* dst_tile = src_tile->GetNeighbour( m_direction );
	ASSERT_NOLOG( dst_tile, "dst tile not set" );
	if ( dst_tile->IsLocked() ) {
		return Error( "Destination tile is locked" );
	}

	return game->GetUM()->MoveUnitValidate( unit, dst_tile );
}

void MoveUnit::Resolve( Game* game ) {
	auto* unit = game->GetUM()->GetUnit( m_unit_id );
	ASSERT_NOLOG( unit, "unit not found" );
	auto* src_tile = unit->GetTile();
	ASSERT_NOLOG( src_tile, "src tile not set" );
	auto* dst_tile = src_tile->GetNeighbour( m_direction );
	ASSERT_NOLOG( dst_tile, "dst tile not set" );

	m_resolutions = game->GetUM()->MoveUnitResolve( unit, dst_tile );
}

gse::Value* const MoveUnit::Apply( Game* game ) const {
	auto* unit = game->GetUM()->GetUnit( m_unit_id );
	ASSERT_NOLOG( unit, "unit not found" );
	game->GetUM()->MoveUnitApply( unit, unit->GetTile()->GetNeighbour( m_direction ), m_resolutions );
	return VALUE( gse::value::Undefined );
}

TS_BEGIN( MoveUnit )
		TS_FUNC_BEGIN( "MoveUnit" ) +
			TS_FUNC_ARG_NUM( "unit_id", m_unit_id ) +
			TS_FUNC_ARG_STR( "direction", map::tile::Tile::GetDirectionString( m_direction ) ) +
		TS_FUNC_END()
TS_END()

void MoveUnit::Serialize( types::Buffer& buf, const MoveUnit* event ) {
	buf.WriteInt( event->m_unit_id );
	buf.WriteInt( event->m_direction );
	types::Buffer b = {};
	gse::Value::Serialize( &b, event->m_resolutions );
	buf.WriteString( b.ToString() );
}

MoveUnit* MoveUnit::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const auto unit_id = buf.ReadInt();
	const auto direction = (map::tile::direction_t)buf.ReadInt();
	auto* result = new MoveUnit( initiator_slot, unit_id, direction );
	types::Buffer b( buf.ReadString() );
	result->m_resolutions = gse::Value::Unserialize( &b );
	return result;
}

}
}
}
