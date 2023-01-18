#include <chrono>
#include <thread>
#include <cstring>
#include <cmath>

#include "Thread.h"

namespace base {

Thread::Thread() {
	m_state = STATE_INACTIVE;
	m_command = COMMAND_NONE;
}

Thread::~Thread() {
	if (m_state != STATE_INACTIVE) {
		Log( "WARNING: thread was not shutdown properly!" );
	}
}

void Thread::Run() {
	m_state = STATE_STARTING;

#if DEBUG
	m_icounter = 0;
#endif

	for ( modules_t::iterator it = m_modules.begin() ; it < m_modules.end() ; ++it )
		(*it)->Start();

	float step_diff = 0.0f;
	float step_len;
	size_t step_len_rounded;

#if DEBUG
	size_t modulensdiff[ m_modules.size() ];
	memset( modulensdiff, 0, sizeof(modulensdiff ));
#endif

	m_state = STATE_ACTIVE;

	try {

		while ( m_state == STATE_ACTIVE ) {

			if ( m_command != COMMAND_NONE ) {

				switch ( m_command ) {
					case COMMAND_STOP:
						m_state = STATE_STOPPING;
						break;
					default:;
				}

				m_command = COMMAND_NONE;
				continue;
			}

			auto start = std::chrono::high_resolution_clock::now();

#if DEBUG
			auto mstart = start;
#endif

			for ( modules_t::iterator it = m_modules.begin() ; it < m_modules.end() ; ++it ) {
				//Log( "Iterating [" + (*it)->GetName() + "]" );
				(*it)->Iterate();
#if DEBUG
				auto mfinish = std::chrono::high_resolution_clock::now();
				modulensdiff[ it - m_modules.begin() ] = std::chrono::duration_cast<std::chrono::nanoseconds>(mfinish - mstart).count();
				mstart = mfinish;
#endif
			}

			auto finish = std::chrono::high_resolution_clock::now();

			auto nsdiff = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();

			step_len = 1000000000 / m_ips - step_diff;
			if ( nsdiff > step_len ) {
#if DEBUG
				/*
				Log( "Lag detected!" );
				for ( modules_t::iterator it = m_modules.begin() ; it != m_modules.end() ; ++it ) {
					Log( (*it)->GetName() + " " + std::to_string( modulensdiff[ it - m_modules.begin() ] ) + "ns ( " + std::to_string( (float) modulensdiff[ it - m_modules.begin() ] * 100 / step_len ) + "%)");
				}
				*/
#endif
				step_diff = 0.0f;
				// TODO: change ips?
			}
			else {
				step_len -= nsdiff;
				step_len_rounded = ceil( step_len );
				step_diff = step_len_rounded - step_len;

#if DEBUG
				m_icounter++;
/*				Log( "frame " + std::to_string( m_icounter ) + ": sleeping " + std::to_string( step_len_rounded) + "ns (nsdiff: " + std::to_string( nsdiff ) + ", stepdiff: " + std::to_string( step_diff ) + ")" );
				for ( modules_t::iterator it = m_modules.begin() ; it != m_modules.end() ; ++it ) {
					Log( (*it)->GetName() + " " + std::to_string( modulensdiff[ it - m_modules.begin() ] ) + "ns ( " + std::to_string( (float) modulensdiff[ it - m_modules.begin() ] * 100 / step_len ) + "%)");
				}*/
#endif


				std::this_thread::sleep_for( std::chrono::nanoseconds(step_len_rounded) );
			}
		}

		m_state = STATE_STOPPING;
		for ( modules_t::iterator it = m_modules.end() - 1 ; it >= m_modules.begin() ; --it )
			(*it)->Stop();
		m_state = STATE_INACTIVE;

	} catch ( runtime_error &e ) {

		try {
			m_state = STATE_STOPPING;
			for ( modules_t::iterator it = m_modules.end() - 1 ; it >= m_modules.begin() ; --it )
				(*it)->Stop();
			m_state = STATE_INACTIVE;
		} catch ( runtime_error &e ) {};

		throw e;
	}
}

void Thread::SetCommand( const thread_command_t command ) {
	ASSERT( m_command == COMMAND_NONE, "thread command overlap" );
	m_command = command;
}

} /* namespace base */
