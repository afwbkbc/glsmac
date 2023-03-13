#include <ctime>

#include "Timer.h"

namespace util {

Timer::Timer() {
	m_current = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now().time_since_epoch() );
}

void Timer::Tick() {
	auto current = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now().time_since_epoch() );
	if (m_operation != NONE) {
		m_elapsed += current - m_current;
	}
	m_current = current;
}

const bool Timer::HasTicked() {
	if (m_operation != NONE) {
		Tick();
		if (m_operation == TIMEOUT || m_operation == INTERVAL) {
			if (m_current >= m_target) {
				if (m_operation == TIMEOUT) {
					Stop();
				}
				else if (m_operation == INTERVAL) {
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

void Timer::SetTimeout(const size_t ms) {
	Tick();
	Stop();
	m_operation = TIMEOUT;
	m_elapsed = std::chrono::milliseconds::zero();
	m_target = m_current + std::chrono::milliseconds( ms );
}

void Timer::SetInterval(const size_t ms) {
	Tick();
	Stop();
	m_operation = INTERVAL;
	m_elapsed = std::chrono::milliseconds::zero();
	m_interval = std::chrono::milliseconds( ms );
	m_target = m_current + m_interval;
}

}
