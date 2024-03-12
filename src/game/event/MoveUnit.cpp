#include "MoveUnit.h"

#include "game/Game.h"
#include "game/unit/StaticDef.h"

namespace game {
namespace event {

MoveUnit::MoveUnit( const size_t initiator_slot, const size_t unit_id, const game::map::Tile::direction_t direction )
	: Event( initiator_slot, ET_UNIT_MOVE )
	, m_unit_id( unit_id )
	, m_direction( direction ) {

}

const std::string* MoveUnit::Validate( const Game* game ) const {
	const auto* unit = game->GetUnit( m_unit_id );
	if ( !unit ) {
		return new std::string( "Unit not found" );
	}

	if ( unit->m_owner->GetIndex() != m_initiator_slot ) {
		return new std::string( "Unit can only be moved by it's owner" );
	}

	ASSERT_NOLOG( unit->m_def->m_type == unit::Def::DT_STATIC, "only static units are supported now" );
	const auto* def = (unit::StaticDef*)unit->m_def;

	if ( def->m_movement_type == unit::StaticDef::MT_IMMOVABLE ) {
		return new std::string( "Unit is immovable" );
	}

	if ( !unit->HasMovesLeft() ) {
		return new std::string( "Unit is out of moves" );
	}

	auto* src_tile = unit->m_tile;
	ASSERT_NOLOG( src_tile, "src tile not set" );
	const auto* dst_tile = src_tile->GetNeighbour( m_direction );
	ASSERT_NOLOG( dst_tile, "dst tile not set" );

	if ( src_tile == dst_tile ) {
		return new std::string( "Destination tile is same as source tile" );
	}

	if ( !src_tile->IsAdjactentTo( dst_tile ) ) {
		return new std::string( "Destination tile is not adjactent to source tile" );
	}

	if ( def->m_movement_type == unit::StaticDef::MT_LAND && dst_tile->is_water_tile ) {
		return new std::string( "Land units can't move to sea tile" );
	}
	if ( def->m_movement_type == unit::StaticDef::MT_SEA && !dst_tile->is_water_tile ) {
		return new std::string( "Sea units can't move to land tile" );
	}

	for ( const auto& it : dst_tile->units ) {
		if ( it.second->m_owner != unit->m_owner ) {
			return new std::string( "Destination tile contains foreign units" );
		}
	}

	return nullptr;
}

void MoveUnit::Resolve( Game* game ) {
	const auto* unit = game->GetUnit( m_unit_id );
	ASSERT_NOLOG( unit, "unit not found" );

	auto* src_tile = unit->m_tile;
	ASSERT_NOLOG( src_tile, "src tile not set" );
	const auto* dst_tile = src_tile->GetNeighbour( m_direction );
	ASSERT_NOLOG( dst_tile, "dst tile not set" );

	auto movement_cost = dst_tile->GetMovementCost();

	if ( unit->m_movement >= movement_cost ) {
		m_resolutions.is_move_successful = true;
	}
	else {
		m_resolutions.is_move_successful = game->GetRandom()->GetFloat( 0.0f, movement_cost ) < unit->m_movement;
	}
}

const gse::Value MoveUnit::Apply( game::Game* game ) const {
	auto* unit = game->GetUnit( m_unit_id );
	ASSERT_NOLOG( unit, "unit not found" );
	game->MoveUnit( unit, unit->m_tile->GetNeighbour( m_direction ), m_resolutions.is_move_successful );
	return VALUE( gse::type::Undefined );
}

TS_BEGIN( MoveUnit )
		TS_FUNC_BEGIN( "MoveUnit" ) +
			TS_FUNC_ARG_NUM( "unit_id", m_unit_id ) +
			TS_FUNC_ARG_STR( "direction", map::Tile::GetDirectionString( m_direction ) ) +
		TS_FUNC_END()
TS_END()

void MoveUnit::Serialize( types::Buffer& buf, const MoveUnit* event ) {
	buf.WriteInt( event->m_unit_id );
	buf.WriteInt( event->m_direction );
	buf.WriteBool( event->m_resolutions.is_move_successful );
}

MoveUnit* MoveUnit::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const auto unit_id = buf.ReadInt();
	const auto direction = (map::Tile::direction_t)buf.ReadInt();
	auto* result = new MoveUnit( initiator_slot, unit_id, direction );
	result->m_resolutions.is_move_successful = buf.ReadBool();
	return result;
}

}
}
