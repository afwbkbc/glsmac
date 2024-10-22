#include "Pop.h"

#include "game/backend/Game.h"
#include "game/backend/base/Base.h"
#include "game/backend/base/PopDef.h"

namespace game {
namespace backend {
namespace base {

Pop::Pop( Base* base, PopDef* def )
	: m_base( base )
	, m_def( def ) {
	//
}

void Pop::Serialize( types::Buffer& buf ) const {
	ASSERT_NOLOG( m_base, "pop base is null" );
	ASSERT_NOLOG( m_def, "pop def is null" );

	buf.WriteInt( m_base->m_id );
	buf.WriteString( m_def->m_name );
}

void Pop::Unserialize( types::Buffer& buf, Game* game ) {
	ASSERT_NOLOG( !m_base, "pop base is not null" );
	ASSERT_NOLOG( !m_def, "pop def is not null" );

	m_base = game->GetBase( buf.ReadInt() );
	ASSERT_NOLOG( m_base, "base not found" );
	m_def = game->GetPopDef( buf.ReadString() );
	ASSERT_NOLOG( m_def, "pop def not found" );
}

}
}
}
