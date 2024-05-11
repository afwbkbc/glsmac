#include "Animation.h"

#include "engine/Engine.h"
#include "AnimationDef.h"
#include "scene/actor/Sound.h"
#include "audio/Audio.h"
#include "InstancedSprite.h"
#include "scene/actor/Instanced.h"

namespace task {
namespace game {

Animation::Animation( const size_t animation_id, AnimationDef* def, const types::Vec3& render_coords )
	: m_def( def )
	, m_render_coords( render_coords )
	, m_frames( def->GetSprites() ) {
	ASSERT_NOLOG( !m_def->GetSprites().empty(), "animation has no sprites defined" );
	m_sound = new scene::actor::Sound( "Animation_Sound_" + std::to_string( animation_id ), def->GetSound() );
	g_engine->GetAudio()->AddActor( m_sound );
	m_timer.SetInterval( m_def->GetDurationMs() / m_frames.size() );
	m_sound->Play();
}

Animation::~Animation() {
	m_timer.Stop();
	if ( m_instance_id ) {
		// clear animation
		m_frames.at( m_frame_index )->actor->RemoveInstance( m_instance_id );
	}
	g_engine->GetAudio()->RemoveAndDeleteActor( m_sound );
}

const bool Animation::IsFinished() const {
	return !m_timer.IsRunning();
}

void Animation::Iterate() {
	while ( m_timer.HasTicked() ) {
		if ( m_frame_index ) {
			// clear previous frame
			m_frames.at( m_frame_index - 1 )->actor->RemoveInstance( m_instance_id );
		}
		if ( m_frame_index++ < m_frames.size() ) {
			// show next frame
			m_instance_id = m_frames.at( m_frame_index - 1 )->actor->AddInstance( m_render_coords );
		}
		else {
			// no frames left
			m_instance_id = 0;
			m_timer.Stop();
		}
	}
}

}
}
