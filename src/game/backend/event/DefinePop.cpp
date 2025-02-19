#include "DefinePop.h"

#include "game/backend/Game.h"
#include "game/backend/base/PopDef.h"
#include "game/backend/base/BaseManager.h"
#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

DefinePop::DefinePop( const size_t initiator_slot, base::PopDef* pop_def )
	: Event( initiator_slot, ET_BASE_DEFINE_POP )
	, m_pop_def( pop_def ) {
	//
}

const std::string* DefinePop::Validate( Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to define units" );
	}
	return Ok();
}

gse::Value* const DefinePop::Apply( Game* game ) const {
	game->GetBM()->DefinePop( m_pop_def );
	return VALUE( gse::value::Undefined );
}

TS_BEGIN( DefinePop )
		TS_FUNC_BEGIN( "DefinePop" ) +
			TS_FUNC_ARG( "def", TS_OF( m_pop_def ) ) +
		TS_FUNC_END()
TS_END()

void DefinePop::Serialize( types::Buffer& buf, const DefinePop* event ) {
	buf.WriteString( base::PopDef::Serialize( event->m_pop_def ).ToString() );
}

DefinePop* DefinePop::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	auto b = types::Buffer( buf.ReadString() );
	return new DefinePop( initiator_slot, base::PopDef::Unserialize( b ) );
}

}
}
}
