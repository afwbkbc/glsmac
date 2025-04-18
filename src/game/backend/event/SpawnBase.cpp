#include "SpawnBase.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/map/Map.h"
#include "game/backend/unit/StaticDef.h"
#include "game/backend/unit/Unit.h"
#include "game/backend/base/Base.h"
#include "game/backend/base/Pop.h"
#include "game/backend/base/BaseManager.h"
#include "gse/value/Undefined.h"
#include "game/backend/Player.h"

namespace game {
namespace backend {
namespace event {

SpawnBase::SpawnBase(
	const size_t initiator_slot,
	const size_t owner_slot,
	const size_t pos_x,
	const size_t pos_y,
	const std::string& name
)
	: LegacyEvent( initiator_slot, ET_BASE_SPAWN )
	, m_owner_slot( owner_slot )
	, m_pos_x( pos_x )
	, m_pos_y( pos_y )
	, m_name( name ) {
	//
}

const std::string* SpawnBase::Validate( GSE_CALLABLE, Game* game ) const {
	if ( !game->GetBM() ) {
		return Error( "Game is not initialized properly" );
	}
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to spawn bases" );
	}
	return Ok();
}

gse::Value* const SpawnBase::Apply( GSE_CALLABLE, Game* game ) const {

	auto& owner = game->GetState()->m_slots->GetSlot( m_owner_slot );
	auto* tile = game->GetMap()->GetTile( m_pos_x, m_pos_y );

	auto* base = new base::Base(
		game,
		base::Base::GetNextId(),
		&owner,
		owner.GetPlayer()->GetFaction(),
		tile,
		m_name,
		{} // will be added later
	);

	auto* bm = game->GetBM();
	ASSERT_NOLOG( bm, "bm is null" );
	bm->SpawnBase( GSE_CALL, base );

	return base->Wrap( GSE_CALL, game->GetGCSpace() );
}

TS_BEGIN( SpawnBase )
		TS_FUNC_BEGIN( "SpawnBase" ) +
			TS_FUNC_ARG_NUM( "owner_slot", m_owner_slot ) +
			TS_FUNC_ARG_NUM( "pos_x", m_pos_x ) +
			TS_FUNC_ARG_NUM( "pos_y", m_pos_y ) +
			TS_FUNC_ARG_STR( "name", m_name ) +
		TS_FUNC_END()
TS_END()

void SpawnBase::Serialize( types::Buffer& buf, const SpawnBase* event ) {
	buf.WriteInt( event->m_owner_slot );
	buf.WriteInt( event->m_pos_x );
	buf.WriteInt( event->m_pos_y );
	buf.WriteString( event->m_name );
}

SpawnBase* SpawnBase::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const auto owner_slot = buf.ReadInt();
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	const auto name = buf.ReadString();
	return new SpawnBase( initiator_slot, owner_slot, pos_x, pos_y, name );
}

}
}
}
