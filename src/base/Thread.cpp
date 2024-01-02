#include <chrono>
#include <thread>
#include <cstring>
#include <cmath>

#include "Thread.h"

using namespace std;

namespace base {

Thread::Thread( const string& thread_name )
	: Base()
	, m_thread_name( thread_name ) {
	m_state = STATE_INACTIVE;
	m_command = COMMAND_NONE;
	m_name = thread_name;
}

Thread::~Thread() {
	if ( m_state != STATE_INACTIVE ) {
		Log( "WARNING: thread " + GetThreadName() + " was not shutdown properly!" );
	}
	else {
		//delete m_thread;
	}
}

void Thread::T_Start() {
	ASSERT( !m_thread, "thread object already initialized" );
	m_state = STATE_STARTING;
	m_thread = new thread( &Thread::Run, this );
}

bool Thread::T_IsRunning() {
	return m_state != STATE_INACTIVE;
}

void Thread::T_Stop() {
	ASSERT( m_command == COMMAND_NONE, "thread command overlap" );
	Log( "Sent STOP command" );
	m_command = Thread::COMMAND_STOP;
}

void Thread::Run() {

	ASSERT( m_state == STATE_STARTING, "starting thread from invalid state" );

	Log( "Starting thread" );

#ifdef DEBUG
	m_icounter = 0;
#endif

	for ( modules_t::iterator it = m_modules.begin() ; it < m_modules.end() ; ++it ) {
		( *it )->Start();
	}

	float step_diff = 0.0f;
	float step_len;
	size_t step_len_rounded;

#ifdef DEBUG
	size_t modulensdiff[m_modules.size()];
	memset( modulensdiff, 0, sizeof( modulensdiff ) );
#endif

	m_state = STATE_ACTIVE;

	Log( "Thread started, entering main loop" );

//	try {

	while ( m_state == STATE_ACTIVE ) {

		auto start = std::chrono::high_resolution_clock::now();

#ifdef DEBUG
		auto mstart = start;
#endif

		for ( modules_t::iterator it = m_modules.begin() ; it < m_modules.end() ; ++it ) {
			//Log( "Iterating [" + (*it)->GetName() + "]" );
			( *it )->Iterate();
#ifdef DEBUG
			auto mfinish = std::chrono::high_resolution_clock::now();
			modulensdiff[ it - m_modules.begin() ] = std::chrono::duration_cast< std::chrono::nanoseconds >( mfinish - mstart ).count();
			mstart = mfinish;
#endif
		}

		auto finish = std::chrono::high_resolution_clock::now();

		auto nsdiff = std::chrono::duration_cast< std::chrono::nanoseconds >( finish - start ).count();

		step_len = 1000000000 / m_ips - step_diff;
		if ( nsdiff > step_len ) {
#ifdef DEBUG
/*	TODO: fix and add stats to debug overlay			
				Log( "Thread lag detected!" );
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

			std::this_thread::sleep_for( std::chrono::nanoseconds( step_len_rounded ) );
		}

		switch ( m_command ) {
			case COMMAND_NONE:
				// nothing
				break;
			case COMMAND_STOP:
				Log( "Received STOP command" );
				m_state = STATE_STOPPING;
				break;
			default:
				THROW( "unknown thread command " + to_string( m_command ) );
		}
	}

	Log( "Stopping thread" );

	m_state = STATE_STOPPING;
	for ( modules_t::iterator it = m_modules.end() - 1 ; it >= m_modules.begin() ; --it ) {
		( *it )->Stop();
	}

	Log( "Thread stopped" );

	m_state = STATE_INACTIVE;

/*	} catch ( runtime_error &e ) {

		try {
			m_state = STATE_STOPPING;
			for ( modules_t::iterator it = m_modules.end() - 1 ; it >= m_modules.begin() ; --it )
				(*it)->Stop();
			m_state = STATE_INACTIVE;
		} catch ( runtime_error &e ) {};

		throw e;
	}*/
}

const string& Thread::GetThreadName() const {
	return m_thread_name;
}

} /* namespace base */
