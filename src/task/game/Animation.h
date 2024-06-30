#pragma once

#include "Types.h"

#include "util/Timer.h"
#include "types/Vec3.h"

namespace scene::actor {
class Sound;
}

namespace task {
namespace game {

class AnimationDef;

class Animation {
public:
	Animation( const size_t animation_id, AnimationDef* def, const types::Vec3& render_coords );
	~Animation();

	const bool IsFinished() const;

	void Iterate();

private:

	void ShowNextFrame();

	util::Timer m_timer;
	scene::actor::Sound* m_sound = nullptr;
	AnimationDef* m_def = nullptr;
	types::Vec3 m_render_coords = {};
	const instanced_sprites_t& m_frames;
	size_t m_frame_index = 0;
	size_t m_instance_id = 0;

};

}
}
