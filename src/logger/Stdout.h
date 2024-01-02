#pragma once

#include <mutex>

#include "Logger.h"

namespace logger {

CLASS( Stdout, Logger )

	void Log( const std::string& text ) override;
	std::mutex m_log_mutex;

};

}
