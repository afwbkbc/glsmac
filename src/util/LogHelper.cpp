#include "LogHelper.h"

#include "common/Mutex.h"
#include "common/Assert.h"

#include <iostream>

namespace util {

static common::Mutex* s_mutex = nullptr;

void LogHelper::Init() {
	ASSERT( !s_mutex, "mutex already initialized" );
	s_mutex = new common::Mutex();
}

void LogHelper::Print( const std::string& text ) {
	ASSERT( s_mutex, "mutex not initialized" );
	std::lock_guard guard( *s_mutex );
	std::cout << text;
}

void LogHelper::Println( const std::string& text, const bool nomutex ) {
	if ( !nomutex ) {
		ASSERT( s_mutex, "mutex not initialized" );
		s_mutex->lock();
	}
	std::cout << text << std::endl;
	if ( !nomutex ) {
		s_mutex->unlock();
	}
}

void LogHelper::Flush() {
	ASSERT( s_mutex, "mutex not initialized" );
	std::lock_guard guard( *s_mutex );
	std::cout << std::flush;
}

}
