#include "Pop.h"

#include "game/backend/Game.h"
#include "game/backend/base/Base.h"
#include "game/backend/base/PopDef.h"
#include "game/backend/base/BaseManager.h"
#include "gse/type/String.h"
#include "gse/type/Int.h"
#include "gse/type/Undefined.h"
#include "gse/callable/Native.h"

namespace game {
namespace backend {
namespace base {

Pop::Pop( Base* const base, PopDef* def, const uint8_t variant )
	: m_base( base )
	, m_def( def )
	, m_variant( variant ) {
	//
}

void Pop::Serialize( types::Buffer& buf ) const {
	ASSERT_NOLOG( m_base, "pop base is null" );
	ASSERT_NOLOG( m_def, "pop def is null" );

	buf.WriteInt( m_base->m_id );
	buf.WriteString( m_def->m_name );
	buf.WriteInt( m_variant );
}

void Pop::Unserialize( types::Buffer& buf, Game* game ) {
	ASSERT_NOLOG( !m_base, "pop base is not null" );
	ASSERT_NOLOG( !m_def, "pop def is not null" );

	m_base = game->GetBM()->GetBase( buf.ReadInt() );
	ASSERT_NOLOG( m_base, "base not found" );
	m_def = game->GetBM()->GetPopDef( buf.ReadString() );
	ASSERT_NOLOG( m_def, "pop def not found" );
	m_variant = buf.ReadInt();
}

WRAPIMPL_BEGIN( Pop, CLASS_BASE_POP )
	WRAPIMPL_PROPS
		{
			"type",
			VALUE( gse::type::String, m_def->m_id )
		},
		{
			"variant",
			VALUE( gse::type::Int, m_variant )
		},
		{
			"get_base",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				ASSERT_NOLOG( m_base, "pop has no base" );
				return m_base->Wrap();
			} )
		},
	};
WRAPIMPL_END_PTR( Pop )

UNWRAPIMPL_PTR( Pop )

}
}
}
