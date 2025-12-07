#include "Pop.h"

#include "game/backend/Game.h"
#include "game/backend/base/Base.h"
#include "game/backend/base/PopDef.h"
#include "game/backend/base/BaseManager.h"
#include "gse/value/String.h"
#include "gse/value/Int.h"
#include "gse/value/Undefined.h"
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
	ASSERT( m_base, "pop base is null" );
	ASSERT( m_def, "pop def is null" );

	buf.WriteInt( m_base->m_id );
	buf.WriteString( m_def->m_name );
	buf.WriteInt( m_variant );
}

void Pop::Deserialize( types::Buffer& buf, Game* game ) {
	ASSERT( !m_base, "pop base is not null" );
	ASSERT( !m_def, "pop def is not null" );

	auto* bm = game->GetBM();
	ASSERT( bm, "bm is null" );

	m_base = bm->GetBase( buf.ReadInt() );
	ASSERT( m_base, "base not found" );
	m_def = bm->GetPopDef( buf.ReadString() );
	ASSERT( m_def, "pop def not found" );
	m_variant = buf.ReadInt();
}

WRAPIMPL_BEGIN( Pop )
	WRAPIMPL_PROPS
		{
			"type",
			VALUE( gse::value::String,, m_def->m_id )
		},
		{
			"variant",
			VALUE( gse::value::Int,, m_variant )
		},
		{
			"get_base",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				ASSERT( m_base, "pop has no base" );
				return m_base->Wrap( GSE_CALL, gc_space );
			} )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Pop )

}
}
}
