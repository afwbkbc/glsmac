#include "DefineUnit.h"

#include "../Game.h"

namespace game {
namespace event {

DefineUnit::DefineUnit( const unit::Def* def )
	: Event( ET_UNIT_DEFINE )
	, m_def( def ) {
	//
}

const gse::Value DefineUnit::Apply( game::Game* game ) const {
	game->DefineUnit( m_def );
	return m_def->Wrap();
}

void DefineUnit::Serialize( types::Buffer& buf, const DefineUnit* event ) {
	buf.WriteString( unit::Def::Serialize( event->m_def ).ToString() );
}

DefineUnit* DefineUnit::Unserialize( types::Buffer& buf ) {
	auto b = types::Buffer( buf.ReadString() );
	return new DefineUnit( unit::Def::Unserialize( b ) );
}

}
}
