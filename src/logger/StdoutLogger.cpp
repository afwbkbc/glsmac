#include <cstdio>

#include "StdoutLogger.h"

namespace logger {

#if DEBUG
void StdoutLogger::Log( const std::string &text ) {
	g_debug_stats._mutex.lock();
	if (!g_debug_stats._readonly) { // don't spam from debug overlay
		printf("%s\n", text.c_str());
		fflush( stdout ); // we want to flush to have everything printed in case of crash
		
		m_line_buffer += text + "\n";
		
		// make space in output every second, to make it easier to read what is useful while testing something
		//m_eoln_timer.SetTimeout( 1000 );
	}
	g_debug_stats._mutex.unlock();
}

void StdoutLogger::Iterate() {
	if ( m_eoln_timer.Ticked() ) {
		printf( "\n\n%s", m_line_buffer.c_str() );
		fflush( stdout );
		m_line_buffer.clear();
	}
}

#endif

} /* namespace logger */
