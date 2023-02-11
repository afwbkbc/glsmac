#pragma once

#include <chrono>

#include "Util.h"

namespace util {

CLASS( Timer, Util )
	
	enum operation_type_t : uint8_t {
		NONE,
		TIMER,
		TIMEOUT,
		INTERVAL,
	};
	
	Timer();

	void Start();
	void Stop();
	
	// use if ( timer.Ticked() ) { ... } to check
	void SetTimeout(const size_t ms);
	
	// use while ( timer.Ticked() ) { ... } to check because it can tick multiple times per iteration ( for example if FPS is low )
	void SetInterval(const size_t ms);

	bool Running();
	std::chrono::milliseconds GetElapsed();
	bool Ticked();
	
protected:

	void Tick();

	operation_type_t m_operation = NONE;
	std::chrono::milliseconds m_current = std::chrono::milliseconds::zero();
	std::chrono::milliseconds m_elapsed = std::chrono::milliseconds::zero();
	std::chrono::milliseconds m_target = std::chrono::milliseconds::zero();
	std::chrono::milliseconds m_interval = std::chrono::milliseconds::zero();
	size_t m_ticks = 0;
	
};

} /* namespace util */
