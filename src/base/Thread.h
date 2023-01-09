#pragma once

#include "base/Base.h"
#include "base/Module.h"

namespace base {

MAJOR_CLASS( Thread, Base )

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

	Thread();
	~Thread();
	void SetIPS( const float ips ) {
		m_ips = ips;
	}
	void AddModule( base::Module *module ) {
		m_modules.push_back( module );
	}
	void Run();
	void SetCommand( const thread_command_t command );
protected:
	thread_state_t m_state;
	thread_command_t m_command;
	base::modules_t m_modules;
	float m_ips;
#ifdef DEBUG
	size_t m_icounter;
#endif
};

} /* namespace base */
