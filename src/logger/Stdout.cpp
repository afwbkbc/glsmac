#include <cstdio>

#include "Stdout.h"

namespace logger {

#ifdef DEBUG
void Stdout::Log( const std::string &text ) {
	g_debug_stats._mutex.lock();
	if (!g_debug_stats._readonly) { // don't spam from debug overlay
		m_log_mutex.lock();
		printf("%s\n", text.c_str());
		fflush( stdout ); // we want to flush to have everything printed in case of crash
		m_log_mutex.unlock();
	}
	g_debug_stats._mutex.unlock();
}

#endif

} /* namespace logger */
