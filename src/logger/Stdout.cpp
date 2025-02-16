#include <cstdio>

#include "Stdout.h"

namespace logger {

std::atomic< bool > g_is_muted = false;

void Stdout::Log( const std::string& text ) {
	if ( !g_is_muted ) {
#ifdef DEBUG
		g_debug_stats._mutex.lock();
		if ( !g_debug_stats._readonly ) { // don't spam from debug overlay
#endif
			m_log_mutex.lock();
			printf( "%s\n", text.c_str() );
			fflush( stdout ); // we want to flush to have everything printed in case of crash
			m_log_mutex.unlock();
#ifdef DEBUG
		}
		g_debug_stats._mutex.unlock();
#endif
	}
}

}
