#pragma once

#include <atomic>
#include <thread>

#include "base/Base.h"
#include "base/Types.h"

namespace base {

class Module;

CLASS( Thread, Base )

	enum thread_state_t {
		STATE_INACTIVE,
		STATE_STARTING,
		STATE_ACTIVE,
		STATE_STOPPING,
	};

	enum thread_command_t {
		COMMAND_NONE,
		COMMAND_STOP,
	};

	Thread( const std::string& thread_name );
	~Thread();
	void SetIPS( const float ips ) {
		m_ips = ips;
	}
	void AddModule( Module* module ) {
		m_modules.push_back( module );
	}
	void T_Start();
	bool T_IsRunning();
	void T_Stop();

	const std::string& GetThreadName() const;

protected:
	const std::string m_thread_name = "";

	void Run();
	std::thread* m_thread = nullptr;

	std::atomic< thread_state_t > m_state = STATE_INACTIVE;
	std::atomic< thread_command_t > m_command = COMMAND_NONE;
	modules_t m_modules = {};
	float m_ips = 10;

#ifdef DEBUG

	size_t m_icounter;

#endif
};

}
