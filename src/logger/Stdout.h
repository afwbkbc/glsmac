#pragma once

#include "Logger.h"

#include <mutex>

namespace logger {

CLASS( Stdout, Logger )
	
#if DEBUG
	void Log( const std::string &text );
	mutex m_log_mutex;
#else
	void Log( const std::string &text ) {}
#endif
	
};

} /* namespace logger */
