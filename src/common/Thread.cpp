#include <chrono>
#include <thread>
#include <cstring>
#include <cmath>
#include <exception>

#include "Thread.h"
#include "common/Module.h"

namespace common {

Thread::Thread( const std::string& thread_name )
	: m_thread_name( thread_name ) {
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

void Thread::SetIPS( const float ips ) {
	m_ips = ips;
}
void Thread::AddModule( Module* module ) {
	ASSERT( module, "null module added" );
	m_modules.push_back( module );
}

void Thread::T_Start() {
	ASSERT( !m_thread, "thread object already initialized" );
	m_state = STATE_STARTING;
	try {
		m_thread = new std::thread( [ this ]() { Run(); } );
	}
	catch ( ... ) {
		m_state = STATE_INACTIVE;
		throw;
	}
}

void Thread::RunOnCurrentThread( const std::function< bool() >& should_stop, const std::function< void() >& before_stop ) {
	ASSERT( !m_thread && m_state == STATE_INACTIVE, "thread already running" );
	m_state = STATE_STARTING;
	Run( should_stop, before_stop );
}

bool Thread::T_IsRunning() {
	if ( m_state != STATE_INACTIVE ) {
		return true;
	}
	else {
		if ( m_thread ) {
			// INACTIVE means module cleanup finished; wait for Run (including
			// its final log) to return before the owner can destroy the engine.
			m_thread->join();
			delete m_thread;
			m_thread = nullptr;
		}
		return false;
	}
}

void Thread::T_Stop() {
	ASSERT( m_command == COMMAND_NONE, "thread command overlap" );
	m_command = Thread::COMMAND_STOP;
	Log( "Sent STOP command" );
}

void Thread::Run( const std::function< bool() >& should_stop, const std::function< void() >& before_stop ) {

	ASSERT( m_state == STATE_STARTING, "starting thread from invalid state" );
	size_t started_modules = 0;
	std::exception_ptr error;
	try {

		Log( "Starting thread" );

#ifdef DEBUG
		m_icounter = 0;
#endif

		for ( modules_t::iterator it = m_modules.begin() ; it < m_modules.end() ; ++it ) {
			( *it )->Start();
			++started_modules;
		}

		float step_diff = 0.0f;
		float step_len;
		size_t step_len_rounded;

#ifdef DEBUG
	/*std::vector< size_t > modulensdiff = {};
	modulensdiff.resize( m_modules.size() );*/
#endif

		m_state = STATE_ACTIVE;

		Log( "Thread started, entering main loop" );

		while ( m_state == STATE_ACTIVE ) {
			if ( should_stop && should_stop() ) {
				break;
			}

			auto start = std::chrono::high_resolution_clock::now();

#ifdef DEBUG
			//auto mstart = start;
#endif

			for ( modules_t::iterator it = m_modules.begin() ; it < m_modules.end() ; ++it ) {
				//Log( "Iterating [" + (*it)->GetName() + "]" );
				( *it )->Iterate();
#ifdef DEBUG
			/*auto mfinish = std::chrono::high_resolution_clock::now();
			modulensdiff[ it - m_modules.begin() ] = std::chrono::duration_cast< std::chrono::nanoseconds >( mfinish - mstart ).count();
			mstart = mfinish;*/
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
					THROW( "unknown thread command " + std::to_string( m_command ) );
			}
		}

		Log( "Stopping thread" );
	}
	catch ( ... ) {
		error = std::current_exception();
	}

	m_state = STATE_STOPPING;
	try {
		if ( before_stop ) {
			before_stop();
		}
	}
	catch ( ... ) {
		if ( !error ) {
			error = std::current_exception();
		}
	}
	// A failed Start must clean up its own partial initialization. Stop only
	// modules whose Start returned, and continue even if one Stop fails.
	while ( started_modules ) {
		try {
			m_modules[ --started_modules ]->Stop();
		}
		catch ( ... ) {
			if ( !error ) {
				error = std::current_exception();
			}
		}
	}

	m_state = STATE_INACTIVE;
	if ( error ) {
		std::rethrow_exception( error );
	}
	Log( "Thread stopped" );
}

const std::string& Thread::GetThreadName() const {
	return m_thread_name;
}

}
