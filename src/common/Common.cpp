#include <atomic>
#include <mutex>

#ifdef DEBUG
#include <chrono>
#endif

#include "Common.h"

#include "engine/Engine.h"
#include "logger/Logger.h"

#ifdef DEBUG
debug_stats_t g_debug_stats = {};
#endif

namespace common {

std::atomic< size_t > g_next_object_id;

Class::Class()
	: m_object_id( g_next_object_id++ ) {
	//
}

const std::string Class::GetNamespace() const {
	return "";
}

const std::string Class::GetName() const {
	if ( !m_name.empty() ) {
		return GetNamespace() + "(" + m_name + ")#" + std::to_string( m_object_id );
	}
	else {
		return GetNamespace() + "#" + std::to_string( m_object_id );
	}
}

const std::string& Class::GetLocalName() const {
	return m_name;
}

#ifdef DEBUG
static uint64_t last_time = 0;
static std::mutex last_time_mutex;
#endif

void Class::Log( const std::string& text ) const {
	if ( g_engine != NULL ) {
#ifdef DEBUG
		const auto time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		uint64_t duration = 0;
		{
			std::lock_guard<std::mutex> guard( last_time_mutex );
			duration = last_time
				? time - last_time
				: 0;
			last_time = time;
		}
#endif
		g_engine->Log(
#ifdef DEBUG
			"[+" + std::to_string( duration ) + "ns] " +
#endif
				"<" + GetName() + "> " + text
		);
	}
}

#ifdef DEBUG

void Class::SetTesting( const bool testing ) {
	m_is_testing = testing;
}

const bool Class::IsTesting() const {
	return m_is_testing;
}

void Class::TestBreakpoint() {
	if ( IsTesting() ) {
		int a = 5;
		/**** put gdb breakpoint here ****/
		a++;
	}
}

#endif

}
