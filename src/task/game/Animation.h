#pragma once

#include "AnimationDef.h"

#include "util/Timer.h"

namespace task {
namespace game {

class Animation {
public:
	Animation( AnimationDef* def, const Vec3& render_coords );
	~Animation();

	const bool IsFinished() const;

	void Iterate();

private:

	util::Timer m_timer;
	AnimationDef* m_def = nullptr;
	Vec3 m_render_coords = {};
	const AnimationDef::sprites_t& m_frames;
	size_t m_frame_index = 0;
	size_t m_instance_id = 0;

};

}
}
