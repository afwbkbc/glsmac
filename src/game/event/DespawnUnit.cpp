#include "DespawnUnit.h"

#include "../Game.h"

#include "gse/type/Undefined.h"

namespace game {
namespace event {

DespawnUnit::DespawnUnit( const size_t initiator_slot, const size_t unit_id )
	: Event( initiator_slot, ET_UNIT_DESPAWN )
	, m_unit_id( unit_id ) {
	//
}

const std::string* DespawnUnit::Validate( const Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to despawn units" );
	}
	return Ok();
}

const gse::Value DespawnUnit::Apply( game::Game* game ) const {
	game->DespawnUnit( m_unit_id );
	return VALUE( gse::type::Undefined );
}

TS_BEGIN( DespawnUnit )
		TS_FUNC_BEGIN( "DespawnUnit" ) +
			TS_FUNC_ARG_NUM( "unit_id", m_unit_id ) +
		TS_FUNC_END()
TS_END()

void DespawnUnit::Serialize( types::Buffer& buf, const DespawnUnit* event ) {
	buf.WriteInt( event->m_unit_id );
}

DespawnUnit* DespawnUnit::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	const auto unit_id = buf.ReadInt();
	return new DespawnUnit( initiator_slot, unit_id );
}

}
}
