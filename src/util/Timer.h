#pragma once

#include "Util.h"

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
	void SetTimeout(const size_t seconds);
	void SetInterval(const size_t seconds);

	time_t GetElapsed();
	bool Ticked();
	
protected:

	void Tick();

	operation_type_t m_operation = NONE;
	time_t m_current = 0;
	time_t m_elapsed = 0;
	time_t m_target = 0;
	time_t m_interval = 0;
	size_t m_ticks = 0;
	
};

} /* namespace util */
