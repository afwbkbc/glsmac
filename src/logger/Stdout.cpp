#include "Stdout.h"

#include "util/LogHelper.h"

namespace logger {

std::atomic< bool > g_is_muted = false;

void Stdout::Log( const std::string& text ) {
	if ( !g_is_muted ) {
#ifdef DEBUG
		g_debug_stats._mutex.lock();
		if ( !g_debug_stats._readonly ) { // don't spam from debug overlay
#endif
			m_log_mutex.lock();
			util::LogHelper::Println( text );
#ifdef DEBUG
			util::LogHelper::Flush(); // we want to flush to have everything printed in case of crash
#endif
			m_log_mutex.unlock();
#ifdef DEBUG
		}
		g_debug_stats._mutex.unlock();
#endif
	}
}

}
