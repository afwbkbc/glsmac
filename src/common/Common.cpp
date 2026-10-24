#include <atomic>

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
static std::atomic< int64_t > s_last_time_ns = 0;
#endif

void Class::Log( const std::string& text ) const {
	if ( g_engine != NULL ) {
#ifdef DEBUG
		const auto time = std::chrono::duration_cast< std::chrono::nanoseconds >(
			std::chrono::steady_clock::now().time_since_epoch()
		).count();
		auto previous = s_last_time_ns.load( std::memory_order_relaxed );
		while (
			previous < time &&
			!s_last_time_ns.compare_exchange_weak( previous, time, std::memory_order_relaxed )
		) {}
		const auto duration = previous > 0 && time > previous
			? time - previous
			: 0;
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

void Class::SetTraceData( const std::string& data ) {
	m_trace_data = data;
}

const std::string& Class::GetTraceData() const {
	return m_trace_data;
}

#endif

}
