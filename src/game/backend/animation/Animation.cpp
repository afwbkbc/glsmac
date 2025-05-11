#include "Animation.h"

#include "gse/value/Callable.h"
#include "game/backend/map/tile/Tile.h"

#include "AnimationSequence.h"
#include "AnimationManager.h"

namespace game {
namespace backend {
namespace animation {

Animation::Animation(
	gc::Space* const gc_space,
	map::tile::Tile* const tile,
	const std::string animdef_id,
	gse::value::Callable* const oncomplete
)
	: gc::Object( gc_space )
	, m_tile( tile )
	, m_animdef_id( animdef_id )
	, m_oncomplete( oncomplete ) {}

void Animation::Run( GSE_CALLABLE ) {
	try {
		m_animation_id = m_sequence->m_am->ShowAnimation(
			GSE_CALL,
			m_animdef_id, m_tile, [ this, gc_space, ctx, si, ep ]( const size_t animation_id ) {
				auto ep2 = ep;
				if ( m_oncomplete ) {
					try {
						m_oncomplete->Run( gc_space, ctx, si, ep2, {} );
					}
					catch ( const std::exception& e ) {
						m_sequence->Finish();
						throw;
					}
				}
				m_is_finished = true;
				m_sequence->RunNextMaybe( gc_space, ctx, si, ep2 );
			}
		);
	}
	catch ( std::exception& e ) {
		m_sequence->Finish();
		throw;
	}
}

void Animation::Abort() {
	ASSERT_NOLOG( !m_is_finished, "animation already finished" );
	ASSERT_NOLOG( m_animation_id, "animation id is zero" );
	m_sequence->m_am->AbortAnimation( m_animation_id );
}

void Animation::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	gc::Object::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Animation" );

	if ( m_oncomplete ) {
		GC_DEBUG_BEGIN( "oncomplete" );
		GC_REACHABLE( m_oncomplete );
		GC_DEBUG_END();
	}

	GC_DEBUG_END();
}

}
}
}
