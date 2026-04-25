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
	std::lock_guard guard( m_animations_mutex );
	m_animations.reserve( size );
}

void AnimationSequence::AddAnimation( Animation* const animation ) {
	ASSERT( !animation->m_sequence, "animation sequence already set" );
	animation->m_sequence = this;
	m_locked_tiles.insert( animation->m_tile );
	{
		std::lock_guard guard( m_animations_mutex );
		m_animations.push_back( animation );
	}
}

void AnimationSequence::Run( GSE_CALLABLE ) {
	std::lock_guard guard( m_animations_mutex );
	ASSERT( !m_is_running, "animation sequence already running" );
	ASSERT( m_animations.size() == m_expected_size, "animation sequence incomplete" );
	m_is_running = true;
	if ( !m_animations.empty() ) {
		ASSERT( !m_locked_tiles.empty(), "locked tiles empty" );
		m_am->m_game->GetTM()->LockTiles( m_am->m_game->GetSlotNum(), m_locked_tiles );
		const auto& first = m_animations.front();
		first->Run( GSE_CALL );
	}
}

void AnimationSequence::Abort() {
	std::lock_guard guard( m_animations_mutex );
	ASSERT( m_is_running, "animation sequence not running" );
	ASSERT( m_current_animation_idx < m_animations.size(), "animation idx overflow" );
	m_animations.at( m_current_animation_idx )->Abort();
	Finish();
}

void AnimationSequence::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	gc::Object::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "AnimationSequence" );

	GC_DEBUG_BEGIN( "animations" );
	{
		std::lock_guard guard( m_animations_mutex );
		for ( size_t i = 0 ; i < m_animations.size() ; i++ ) {
			auto* const animation = m_animations[ i ];
			if ( animation ) {
				if ( !animation->m_is_finished ) {
					GC_REACHABLE( animation );
				}
				else {
					m_animations[ i ] = nullptr;
				}
			}
		}
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

void AnimationSequence::Finish() {
	ASSERT( m_is_running, "animation sequence not running" );
	ASSERT( !m_locked_tiles.empty(), "locked tiles empty" );
	m_am->m_game->GetTM()->UnlockTiles( m_am->m_game->GetSlotNum(), m_locked_tiles );
	m_am->RemoveAnimationSequence( m_sequence_id );
}

void AnimationSequence::RunNextMaybe( GSE_CALLABLE ) {
	m_current_animation_idx++;
	{
		std::lock_guard guard( m_animations_mutex );
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
}
