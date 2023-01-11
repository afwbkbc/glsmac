#include "Timer.h"

#include <ctime>

using namespace std::chrono;

namespace util {

Timer::Timer() {
	m_current = duration_cast< milliseconds >( system_clock::now().time_since_epoch() );
}

void Timer::Tick() {
	auto current = duration_cast< milliseconds >( system_clock::now().time_since_epoch() );
	if (m_operation != NONE) {
		m_elapsed += current - m_current;
	}
	m_current = current;
}

bool Timer::Ticked() {
	if (m_operation != NONE) {
		Tick();
		if (m_operation == TIMEOUT || m_operation == INTERVAL) {
			if (m_current >= m_target) {
				if (m_operation == TIMEOUT) {
					Stop();
				}
				else if (m_operation == INTERVAL) {
					m_target += m_interval;
					m_elapsed = milliseconds::zero();
				}
				return true;
			}
		}
	}
	return false;
}

bool Timer::Running() {
	return m_operation != NONE;
}

milliseconds Timer::GetElapsed() {
	Tick();
	return m_elapsed;
}

void Timer::Start() {
	m_operation = TIMER;
	m_elapsed = milliseconds::zero();
}

void Timer::Stop() {
	m_operation = NONE;
	m_elapsed = milliseconds::zero();
}

void Timer::SetTimeout(const size_t ms) {
	Tick();
	m_operation = TIMEOUT;
	m_elapsed = milliseconds::zero();
	m_target = m_current + milliseconds( ms );
}

void Timer::SetInterval(const size_t ms) {
	Tick();
	m_operation = INTERVAL;
	m_elapsed = milliseconds::zero();
	m_interval = milliseconds( ms );
	m_target = m_current + m_interval;
}

}
