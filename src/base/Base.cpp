#include <atomic>

#include "Base.h"

#include "engine/Engine.h"

#ifdef DEBUG
debug_stats_t g_debug_stats = {};
#endif

namespace base {

std::atomic< size_t > g_next_object_id;

Base::Base()
	: m_object_id( g_next_object_id++ ) {
	//
}

const std::string Base::GetNamespace() const {
	return "";
}

const std::string Base::GetName() const {
	if ( !m_name.empty() ) {
		return GetNamespace() + "(" + m_name + ")#" + std::to_string( m_object_id );
	}
	else {
		return GetNamespace() + "#" + std::to_string( m_object_id );
	}
}

const std::string& Base::GetLocalName() const {
	return m_name;
}

void Base::Log( const std::string& text ) const {
	if ( g_engine != NULL ) {
		g_engine->GetLogger()->Log( "<" + GetName() + "> " + text );
	}
}

#ifdef DEBUG

void Base::SetTesting( const bool testing ) {
	m_is_testing = testing;
}

const bool Base::IsTesting() const {
	return m_is_testing;
}

void Base::TestBreakpoint() {
	if ( IsTesting() ) {
		int a = 5;
		/**** put gdb breakpoint here ****/
		a++;
	}
}

#endif

} /* namespace base */
