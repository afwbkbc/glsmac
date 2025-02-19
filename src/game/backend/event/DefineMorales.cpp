#include "DefineMorales.h"

#include "game/backend/Game.h"
#include "game/backend/unit/UnitManager.h"
#include "game/backend/unit/MoraleSet.h"
#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

DefineMorales::DefineMorales( const size_t initiator_slot, unit::MoraleSet* moraleset )
	: Event( initiator_slot, ET_UNIT_DEFINE_MORALES )
	, m_moraleset( moraleset ) {
	//
}

const std::string* DefineMorales::Validate( Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to define units" );
	}
	return Ok();
}

gse::Value* const DefineMorales::Apply( Game* game ) const {
	game->GetUM()->DefineMoraleSet( m_moraleset );
	return VALUE( gse::value::Undefined );
}

TS_BEGIN( DefineMorales )
		TS_FUNC_BEGIN( "DefineMorales" ) +
			TS_FUNC_ARG( "def", TS_OF( m_moraleset ) ) +
		TS_FUNC_END()
TS_END()

void DefineMorales::Serialize( types::Buffer& buf, const DefineMorales* event ) {
	buf.WriteString( unit::MoraleSet::Serialize( event->m_moraleset ).ToString() );
}

DefineMorales* DefineMorales::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	auto b = types::Buffer( buf.ReadString() );
	return new DefineMorales( initiator_slot, unit::MoraleSet::Unserialize( b ) );
}

}
}
}
