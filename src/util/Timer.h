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

	virtual void Start();
	void Stop();

	// use if ( timer.HasTicked() ) { ... } to check
	virtual void SetTimeout( const size_t ms );

	// use while ( timer.HasTicked() ) { ... } to check because it can tick multiple times per iteration ( for example if FPS is low )
	virtual void SetInterval( const size_t ms );

	const bool IsRunning() const;
	std::chrono::milliseconds GetElapsed();
	virtual const bool HasTicked();

protected:

	void Tick();

	operation_type_t m_operation = NONE;
	std::chrono::milliseconds m_current = std::chrono::milliseconds::zero();
	std::chrono::milliseconds m_elapsed = std::chrono::milliseconds::zero();
	std::chrono::milliseconds m_target = std::chrono::milliseconds::zero();
	std::chrono::milliseconds m_interval = std::chrono::milliseconds::zero();

};

}
