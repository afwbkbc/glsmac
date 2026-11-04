#pragma once

#include "Util.h"

#include "Timer.h"

namespace util {

template< class POSITION_TYPE >
CLASS( Scroller, util::Util )

	static constexpr uint16_t SCROLL_STEP_MS = 10;

	void Scroll( const POSITION_TYPE& from, const POSITION_TYPE& to, const size_t duration_ms ) {
		if ( IsRunning() ) {
			Stop();
		}
		m_position = from;
		m_target_position = to;
		m_duration_ms = duration_ms;
		if ( m_duration_ms == 0 ) {
			m_position = m_target_position;
			m_scroll_steps = 0;
			m_steps_left = 0;
			return;
		}
		m_scroll_steps = m_duration_ms / SCROLL_STEP_MS + ( m_duration_ms % SCROLL_STEP_MS ? 1 : 0 );
		m_steps_left = m_scroll_steps;
		m_step = ( m_target_position - m_position ) / static_cast< float >( m_steps_left );
		m_timer.SetInterval( SCROLL_STEP_MS );
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
			if ( m_steps_left > 1 ) {
				m_position += m_step;
				m_steps_left--;
			}
			else {
				m_position = m_target_position;
				m_steps_left = 0;
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

	size_t m_duration_ms = 0;
	size_t m_scroll_steps = 0;

	POSITION_TYPE m_position = {};
	POSITION_TYPE m_step = {};
	POSITION_TYPE m_target_position = {};
	size_t m_steps_left = 0;

};

}
