#pragma once

#include "Util.h"

#include <chrono>

using namespace std::chrono;

namespace util {

CHILD_CLASS( Timer, Util )
	
	enum operation_type_t : uint8_t {
		NONE,
		TIMER,
		TIMEOUT,
		INTERVAL,
	};
	
	Timer();

	void Start();
	void Stop();
	void SetTimeout(const size_t ms);
	void SetInterval(const size_t ms);

	bool Running();
	milliseconds GetElapsed();
	bool Ticked();
	
protected:

	void Tick();

	operation_type_t m_operation = NONE;
	milliseconds m_current = milliseconds::zero();
	milliseconds m_elapsed = milliseconds::zero();
	milliseconds m_target = milliseconds::zero();
	milliseconds m_interval = milliseconds::zero();
	size_t m_ticks = 0;
	
};

} /* namespace util */
