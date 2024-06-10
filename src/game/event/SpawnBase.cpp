#include "SpawnBase.h"

#include "game/Game.h"
#include "game/State.h"
#include "game/slot/Slots.h"
#include "game/map/Map.h"
#include "game/unit/StaticDef.h"
#include "game/unit/Unit.h"
#include "gse/type/Undefined.h"

namespace game {
namespace event {

SpawnBase::SpawnBase(
	const size_t initiator_slot,
	const size_t owner_slot,
	const size_t pos_x,
	const size_t pos_y
)
	: Event( initiator_slot, ET_BASE_SPAWN )
	, m_owner_slot( owner_slot )
	, m_pos_x( pos_x )
	, m_pos_y( pos_y ) {
	//
}

const std::string* SpawnBase::Validate( Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to spawn bases" );
	}
	return Ok();
}

const gse::Value SpawnBase::Apply( game::Game* game ) const {

	auto& owner = game->GetState()->m_slots->GetSlot( m_owner_slot );
	auto* tile = game->GetMap()->GetTile( m_pos_x, m_pos_y );

	auto* base = new base::Base(
		game,
		base::Base::GetNextId(),
		&owner,
		tile
	);
	game->SpawnBase( base );
	return base->Wrap();
}

TS_BEGIN( SpawnBase )
		TS_FUNC_BEGIN( "SpawnBase" ) +
			TS_FUNC_ARG_NUM( "owner_slot", m_owner_slot ) +
			TS_FUNC_ARG_NUM( "pos_x", m_pos_x ) +
			TS_FUNC_ARG_NUM( "pos_y", m_pos_y ) +
		TS_FUNC_END()
TS_END()

void SpawnBase::Serialize( types::Buffer& buf, const SpawnBase* event ) {
	buf.WriteInt( event->m_owner_slot );
	buf.WriteInt( event->m_pos_x );
	buf.WriteInt( event->m_pos_y );
}

SpawnBase* SpawnBase::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const auto owner_slot = buf.ReadInt();
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	return new SpawnBase( initiator_slot, owner_slot, pos_x, pos_y );
}

}
}
