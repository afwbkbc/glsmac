#include "Animation.h"

namespace task {
namespace game {

Animation::Animation( const size_t id, AnimationDef* def, const Vec3& render_coords )
	: m_id( id )
	, m_def( def )
	, m_render_coords( render_coords ) {

	ASSERT_NOLOG( !m_def->GetSprites().empty(), "animation has no sprites defined" );
}

Animation::~Animation() {
	//
}

const size_t Animation::GetId() const {
	return m_id;
}

const bool Animation::IsFinished() const {
	return !m_timer.IsRunning();
}

void Animation::Iterate() {
	while ( m_timer.HasTicked() ) {
		
	}
}

}
}
