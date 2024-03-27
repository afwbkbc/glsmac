#include "DefineMorales.h"

#include "../Game.h"

namespace game {
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

const gse::Value DefineMorales::Apply( game::Game* game ) const {
	game->DefineMoraleSet( m_moraleset );
	return VALUE( gse::type::Undefined );
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
