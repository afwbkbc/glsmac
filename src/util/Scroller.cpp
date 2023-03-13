#include "Scroller.h"

namespace util {

const Scroller::consts_t Scroller::s_consts = {};

void Scroller::Scroll( const types::Vec3& from, const types::Vec3& to ) {
	if ( IsRunning() ) {
		Stop();
	}
	m_position = from;
	m_target_position = to;
	//m_scroll_speed = scroll_speed;
	m_steps_left = s_consts.scroll_steps;
	m_step = ( m_target_position - m_position ) / m_steps_left;
	m_timer.SetInterval( s_consts.scroll_step_ms );
}

void Scroller::Stop() {
	m_timer.Stop();
}

const bool Scroller::IsRunning() const {
	return m_timer.IsRunning();
}

const bool Scroller::HasTicked() {
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

const types::Vec3& Scroller::GetPosition() const {
	return m_position;
}

}
