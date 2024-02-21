#include "DespawnUnit.h"

#include "../Game.h"

#include "gse/type/Undefined.h"

namespace game {
namespace event {

DespawnUnit::DespawnUnit( const size_t unit_id )
	: Event( ET_UNIT_DESPAWN )
	, m_unit_id( unit_id ) {
	//
}

const gse::Value DespawnUnit::Apply( game::Game* game ) const {
	game->DespawnUnit( m_unit_id );
	return VALUE( gse::type::Undefined );
}

void DespawnUnit::Serialize( types::Buffer& buf, const DespawnUnit* event ) {
	buf.WriteInt( event->m_unit_id );
}

DespawnUnit* DespawnUnit::Unserialize( types::Buffer& buf ) {
	const auto unit_id = buf.ReadInt();
	return new DespawnUnit( unit_id );
}

}
}
