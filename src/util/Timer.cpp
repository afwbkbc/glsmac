#include "Timer.h"

#include <ctime>

namespace util {

Timer::Timer() {
	m_current = time(NULL);
}

void Timer::Tick() {
	time_t current = time(NULL);
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
					m_elapsed = 0;
				}
				return true;
			}
		}
	}
	return false;
}

time_t Timer::GetElapsed() {
	Tick();
	return m_elapsed;
}

void Timer::Start() {
	m_operation = TIMER;
	m_elapsed = 0;
}

void Timer::Stop() {
	m_operation = NONE;
	m_elapsed = 0;
}

void Timer::SetTimeout(const size_t seconds) {
	Tick();
	m_operation = TIMEOUT;
	m_elapsed = 0;
	m_target = m_current + seconds;
}

void Timer::SetInterval(const size_t seconds) {
	Tick();
	m_operation = INTERVAL;
	m_elapsed = 0;
	m_interval = seconds;
	m_target = m_current + seconds;
}

}
