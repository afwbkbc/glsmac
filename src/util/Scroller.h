#pragma once

#include "Util.h"
#include "Timer.h"

#include "types/Vec3.h"

namespace util {

template< class POSITION_TYPE >
CLASS( Scroller, util::Util )

	static constexpr uint16_t scroll_time_ms = 100;
	static constexpr uint16_t scroll_step_ms = 10;
	static constexpr uint8_t scroll_steps = scroll_time_ms / scroll_step_ms;

	void Scroll( const POSITION_TYPE& from, const POSITION_TYPE& to ) {
		if ( IsRunning() ) {
			Stop();
		}
		m_position = from;
		m_target_position = to;
		m_steps_left = scroll_steps;
		m_step = ( m_target_position - m_position ) / m_steps_left;
		m_timer.SetInterval( scroll_step_ms );
	}

	void Stop() {
		m_timer.Stop();
	}

	const bool IsRunning() const {
		return m_timer.IsRunning();
	}

	const bool HasTicked() {
		const bool ticked = m_timer.HasTicked();

		if ( ticked ) {
			ASSERT( m_steps_left, "no scrolling steps but timer running" );

			m_position += m_step;

			if ( !--m_steps_left ) {
				Stop();
			}

		}

		return ticked;
	}

	const POSITION_TYPE& GetPosition() const {
		return m_position;
	}
	const POSITION_TYPE& GetTargetPosition() const {
		return m_target_position;
	}

private:

	util::Timer m_timer;

	float m_scroll_speed;

	POSITION_TYPE m_position;
	POSITION_TYPE m_step;
	POSITION_TYPE m_target_position;
	uint8_t m_steps_left;

};

}
