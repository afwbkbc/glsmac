#pragma once

#include <mutex>

#include "Logger.h"

namespace logger {

CLASS( Stdout, Logger )

#ifdef DEBUG

	void Start() override;

	void Log( const std::string& text ) override;
	std::mutex m_log_mutex;

#else
	void Log( const std::string &text ) override {}
#endif

#ifdef DEBUG
private:
	bool m_is_quiet;
#endif

};

} /* namespace logger */
