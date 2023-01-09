#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include "Thread.h"

namespace base {

Thread::Thread() {
	m_state = STATE_INACTIVE;
	m_command = COMMAND_NONE;
}

Thread::~Thread() {
	if (m_state != STATE_INACTIVE) {
		throw ThreadError( "thread was not shutdown properly" );
	}
}

void Thread::Run() {
	m_state = STATE_STARTING;

#ifdef DEBUG
	m_icounter = 0;
#endif

	for ( modules_t::iterator it = m_modules.begin() ; it < m_modules.end() ; ++it )
		(*it)->Start();

	float step_diff = 0.0f;
	float step_len;
	size_t step_len_rounded;

#ifdef DEBUG
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

			auto start = boost::chrono::high_resolution_clock::now();

#ifdef DEBUG
			auto mstart = start;
#endif

			for ( modules_t::iterator it = m_modules.begin() ; it < m_modules.end() ; ++it ) {
				//Log( "Iterating [" + (*it)->GetName() + "]" );
				(*it)->Iterate();
#ifdef DEBUG
				auto mfinish = boost::chrono::high_resolution_clock::now();
				modulensdiff[ it - m_modules.begin() ] = boost::chrono::duration_cast<boost::chrono::nanoseconds>(mfinish - mstart).count();
				mstart = mfinish;
#endif
			}

			auto finish = boost::chrono::high_resolution_clock::now();

			auto nsdiff = boost::chrono::duration_cast<boost::chrono::nanoseconds>(finish - start).count();

			step_len = 1000000000 / m_ips - step_diff;
			if ( nsdiff > step_len ) {
#ifdef DEBUG
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

#ifdef DEBUG
				m_icounter++;
/*				Log( "frame " + std::to_string( m_icounter ) + ": sleeping " + std::to_string( step_len_rounded) + "ns (nsdiff: " + std::to_string( nsdiff ) + ", stepdiff: " + std::to_string( step_diff ) + ")" );
				for ( modules_t::iterator it = m_modules.begin() ; it != m_modules.end() ; ++it ) {
					Log( (*it)->GetName() + " " + std::to_string( modulensdiff[ it - m_modules.begin() ] ) + "ns ( " + std::to_string( (float) modulensdiff[ it - m_modules.begin() ] * 100 / step_len ) + "%)");
				}*/
#endif


				boost::this_thread::sleep_for( boost::chrono::nanoseconds(step_len_rounded) );
			}
		}

		m_state = STATE_STOPPING;
		for ( modules_t::iterator it = m_modules.end() - 1 ; it >= m_modules.begin() ; --it )
			(*it)->Stop();
		m_state = STATE_INACTIVE;

	} catch ( Error &e ) {

		try {
			m_state = STATE_STOPPING;
			for ( modules_t::iterator it = m_modules.end() - 1 ; it >= m_modules.begin() ; --it )
				(*it)->Stop();
			m_state = STATE_INACTIVE;
		} catch ( Error &e ) {};

		throw e;
	}
}

void Thread::SetCommand( const thread_command_t command ) {
	if ( m_command != COMMAND_NONE )
		throw ThreadError( "thread command overflow" );
	m_command = command;
}

} /* namespace base */
