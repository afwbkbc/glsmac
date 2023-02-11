#pragma once

#include <atomic>
#include <thread>

#include "base/Base.h"
#include "base/Module.h"

namespace base {

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

	Thread( const string& thread_name );
	~Thread();
	void SetIPS( const float ips ) {
		m_ips = ips;
	}
	void AddModule( base::Module *module ) {
		m_modules.push_back( module );
	}
	void T_Start();
	bool T_IsRunning();
	void T_Stop();
	
	const string& GetThreadName() const;
	
	void SetCommand( const thread_command_t command );
protected:
	const string m_thread_name = "";
	
	void Run();
	thread* m_thread = nullptr;
	
	atomic<thread_state_t> m_state = STATE_INACTIVE;
	atomic<thread_command_t> m_command = COMMAND_NONE;
	base::modules_t m_modules = {};
	float m_ips = 10;
#ifdef DEBUG
	size_t m_icounter;
#endif
};

} /* namespace base */
