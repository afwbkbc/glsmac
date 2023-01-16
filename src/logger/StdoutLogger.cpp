#include <cstdio>

#include "StdoutLogger.h"

namespace logger {

#if DEBUG
void StdoutLogger::Log( const std::string &text ) const {
	g_debug_stats._mutex.lock();
	if (!g_debug_stats._readonly) { // don't spam from debug overlay
		printf("%s\n", text.c_str());
		fflush( stdout );
	}
	g_debug_stats._mutex.unlock();
}
#endif

} /* namespace logger */
