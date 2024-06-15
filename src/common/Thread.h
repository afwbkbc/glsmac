#pragma once

#include <atomic>
#include <thread>

#include "Common.h"
#include "Types.h"

namespace common {

class Module;

CLASS( Thread, Class )

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

	void SetIPS( const float ips );
	void AddModule( Module* module );

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
