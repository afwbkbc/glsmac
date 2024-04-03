#pragma once

#include "AnimationDef.h"

#include "util/Timer.h"

namespace task {
namespace game {

class Animation {
public:
	Animation( const size_t id, AnimationDef* def, const Vec3& render_coords );
	~Animation();

	const size_t GetId() const;
	const bool IsFinished() const;

	void Iterate();

private:

	util::Timer m_timer;
	size_t m_id = 0;
	AnimationDef* m_def = nullptr;
	Vec3 m_render_coords = {};
	size_t frame_index = 0;
	size_t instance_id = 0;

};

}
}
