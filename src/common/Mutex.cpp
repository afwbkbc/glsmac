#include "Mutex.h"
#include "Assert.h"

#include <atomic>

namespace common {

static std::atomic< mutex_mode_t > s_mutex_mode = MM_UNKNOWN;

void Mutex::SetMutexMode( const mutex_mode_t mutex_mode ) {
	ASSERT( s_mutex_mode == MM_UNKNOWN, "mutex mode already set" );
	s_mutex_mode = mutex_mode;
}

Mutex::Mutex() {
	ASSERT( s_mutex_mode != MM_UNKNOWN, "mutex mode not set" );
	switch ( s_mutex_mode ) {
		case MM_DEFAULT: {
			m_mutex = new std::shared_mutex;
			break;
		}
		default: {
		}
	}
}

Mutex::~Mutex() {
	switch ( s_mutex_mode ) {
		case MM_DEFAULT: {
			delete m_mutex;
			break;
		}
		default: {
		}
	}
}

void Mutex::lock() {
	switch ( s_mutex_mode ) {
		case MM_DEFAULT: {
			m_mutex->lock();
			break;
		}
		default: {
		}
	}
}

void Mutex::unlock() {
	switch ( s_mutex_mode ) {
		case MM_DEFAULT: {
			m_mutex->unlock();
			break;
		}
		default: {
		}
	}
}

}
