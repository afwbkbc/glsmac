#include "AnimationSequence.h"

#include "Animation.h"
#include "AnimationManager.h"
#include "game/backend/Game.h"
#include "game/backend/map/tile/TileManager.h"

namespace game {
namespace backend {
namespace animation {

AnimationSequence::AnimationSequence( gc::Space* const gc_space, AnimationManager* const animation_manager, const size_t sequence_id, const size_t size )
	: gc::Object( gc_space )
	, m_sequence_id( sequence_id )
	, m_expected_size( size )
	, m_am( animation_manager ) {
	m_animations.reserve( size );
}

void AnimationSequence::AddAnimation( Animation* const animation ) {
	ASSERT_NOLOG( !animation->m_sequence, "animation sequence already set" );
	animation->m_sequence = this;
	m_locked_tiles.insert( animation->m_tile );
	m_animations.push_back( animation );
}

void AnimationSequence::Run( GSE_CALLABLE ) {
	ASSERT_NOLOG( !m_is_running, "animation sequence already running" );
	ASSERT_NOLOG( m_animations.size() == m_expected_size, "animation sequence incomplete" );
	m_is_running = true;
	if ( !m_animations.empty() ) {
		ASSERT_NOLOG( !m_locked_tiles.empty(), "locked tiles empty" );
		m_am->m_game->GetTM()->LockTiles( m_am->m_game->GetSlotNum(), m_locked_tiles );
		const auto& first = m_animations.front();
		first->Run( GSE_CALL );
	}
}

void AnimationSequence::Abort() {
	ASSERT_NOLOG( m_is_running, "animation sequence not running" );
	ASSERT_NOLOG( m_current_animation_idx < m_animations.size(), "animation idx overflow" );
	m_animations.at( m_current_animation_idx )->Abort();
	Finish();
}

void AnimationSequence::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	gc::Object::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "AnimationSequence" );

	GC_DEBUG_BEGIN( "animations" );
	for ( const auto& animation : m_animations ) {
		if ( !animation->m_is_finished ) {
			GC_REACHABLE( animation );
		}
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

void AnimationSequence::Finish() {
	ASSERT_NOLOG( m_is_running, "animation sequence not running" );
	ASSERT_NOLOG( !m_locked_tiles.empty(), "locked tiles empty" );
	m_am->m_game->GetTM()->UnlockTiles( m_am->m_game->GetSlotNum(), m_locked_tiles );
	m_am->RemoveAnimationSequence( m_sequence_id );
}

void AnimationSequence::RunNextMaybe( GSE_CALLABLE ) {
	m_current_animation_idx++;
	if ( m_current_animation_idx < m_animations.size() ) {
		m_animations.at( m_current_animation_idx )->Run( GSE_CALL );
	}
	else {
		Finish();
	}
}

}
}
}
