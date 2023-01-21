#pragma once

#include "Logger.h"

#if DEBUG
#include "util/Timer.h"
#endif

namespace logger {

class StdoutLogger : public Logger {
public:
	
#if DEBUG
	void Log( const std::string &text );
	void Iterate();
	
	string m_line_buffer = "";
	util::Timer m_eoln_timer;
	bool m_is_anything_new = false;
#else
	void Log( const std::string &text ) const {}
#endif
	
};

} /* namespace logger */
