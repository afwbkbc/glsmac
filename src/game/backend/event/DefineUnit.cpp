#include "DefineUnit.h"

#include "game/backend/Game.h"
#include "game/backend/unit/UnitManager.h"
#include "game/backend/unit/Def.h"

namespace game {
namespace backend {
namespace event {

DefineUnit::DefineUnit( const size_t initiator_slot, unit::Def* def )
	: Event( initiator_slot, ET_UNIT_DEFINE )
	, m_def( def ) {
	//
}

const std::string* DefineUnit::Validate( Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to define units" );
	}
	return Ok();
}

gse::Value* const DefineUnit::Apply( Game* game ) const {
	game->GetUM()->DefineUnit( m_def );
	return m_def->Wrap( game->GetGCSpace() );
}

TS_BEGIN( DefineUnit )
		TS_FUNC_BEGIN( "DefineUnit" ) +
			TS_FUNC_ARG( "def", TS_OF( m_def ) ) +
		TS_FUNC_END()
TS_END()

void DefineUnit::Serialize( types::Buffer& buf, const DefineUnit* event ) {
	buf.WriteString( unit::Def::Serialize( event->m_def ).ToString() );
}

DefineUnit* DefineUnit::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	auto b = types::Buffer( buf.ReadString() );
	return new DefineUnit( initiator_slot, unit::Def::Unserialize( b ) );
}

}
}
}
