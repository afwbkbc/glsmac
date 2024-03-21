#include "DefineUnit.h"

#include "../Game.h"

namespace game {
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

const gse::Value DefineUnit::Apply( game::Game* game ) const {
	game->DefineUnit( m_def );
	return m_def->Wrap();
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
