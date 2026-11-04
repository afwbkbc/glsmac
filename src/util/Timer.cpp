#include "Timer.h"

#include <cstdint>
#include <limits>

namespace util {

static std::chrono::milliseconds GetMonotonicMilliseconds() {
	return std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::steady_clock::now().time_since_epoch()
	);
}

Timer::Timer() {
	m_current = GetMonotonicMilliseconds();
}

void Timer::Tick() {
	const auto current = GetMonotonicMilliseconds();
	if ( m_operation != NONE ) {
		m_elapsed += current - m_current;
	}
	m_current = current;
}

const bool Timer::HasTicked() {
	if ( m_operation != NONE ) {
		Tick();
		if ( m_operation == TIMEOUT || m_operation == INTERVAL ) {
			if ( m_current >= m_target ) {
				if ( m_operation == TIMEOUT ) {
					Stop();
				}
				else if ( m_operation == INTERVAL ) {
					m_target += m_interval;
					m_elapsed = std::chrono::milliseconds::zero();
				}
				return true;
			}
		}
	}
	return false;
}

const bool Timer::IsRunning() const {
	return m_operation != NONE;
}

std::chrono::milliseconds Timer::GetElapsed() {
	Tick();
	return m_elapsed;
}

void Timer::Start() {
	m_operation = TIMER;
	m_elapsed = std::chrono::milliseconds::zero();
}

void Timer::Stop() {
	if ( m_operation != NONE ) {
		m_operation = NONE;
		m_elapsed = std::chrono::milliseconds::zero();
	}
}

void Timer::SetTimeout( const size_t ms ) {
	if ( static_cast< uint64_t >( ms ) > static_cast< uint64_t >( std::numeric_limits< std::chrono::milliseconds::rep >::max() ) ) {
		THROW( "timer duration is too large" );
	}
	Tick();
	Stop();
	m_operation = TIMEOUT;
	m_elapsed = std::chrono::milliseconds::zero();
	m_target = m_current + std::chrono::milliseconds( static_cast< std::chrono::milliseconds::rep >( ms ) );
}

void Timer::SetInterval( const size_t ms ) {
	if ( ms == 0 ) {
		THROW( "timer interval must be greater than zero" );
	}
	if ( static_cast< uint64_t >( ms ) > static_cast< uint64_t >( std::numeric_limits< std::chrono::milliseconds::rep >::max() ) ) {
		THROW( "timer duration is too large" );
	}
	Tick();
	Stop();
	m_operation = INTERVAL;
	m_elapsed = std::chrono::milliseconds::zero();
	m_interval = std::chrono::milliseconds( static_cast< std::chrono::milliseconds::rep >( ms ) );
	m_target = m_current + m_interval;
}

}
