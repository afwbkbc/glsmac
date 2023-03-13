#pragma once

#include "Util.h"
#include "Timer.h"

#include "types/Vec3.h"

namespace util {

CLASS( Scroller, util::Util )
	
	struct consts_t {
		const uint16_t scroll_time_ms = 100;
		const uint16_t scroll_step_ms = 2;
		const uint8_t scroll_steps = scroll_time_ms / scroll_step_ms;
	};
	static const consts_t s_consts;
	
	void Scroll( const types::Vec3& from, const types::Vec3& to );
	void Stop();
	const bool IsRunning() const;
	const bool HasTicked();
	
	const types::Vec3& GetPosition() const;
	
private:
	
	util::Timer m_timer;

	float m_scroll_speed;
	
	types::Vec3 m_step;
	types::Vec3 m_target_position;
	uint8_t m_steps_left;

	types::Vec3 m_position;
};

}
