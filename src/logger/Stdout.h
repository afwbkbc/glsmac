#pragma once

#include <mutex>
#include <atomic>

#include "Logger.h"

namespace logger {

// hacky but ok
extern std::atomic< bool > g_is_muted;

CLASS( Stdout, Logger )

	void Log( const std::string& text ) override;

private:
	std::mutex m_log_mutex;

};

}
