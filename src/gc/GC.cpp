#include "GC.h"

#include "Space.h"

#if defined( DEBUG ) || defined( FASTDEBUG )
#include "util/LogHelper.h"
#include "engine/Engine.h"
#include "config/Config.h"
#endif

namespace gc {

void GC::Start() {

}

void GC::Stop() {

}

void GC::Iterate() {
#if defined( DEBUG ) || defined( FASTDEBUG )
	ASSERT_NOLOG( g_engine, "engine not set" );
	static const bool s_no_gc = !g_engine->GetConfig()->HasDebugFlag( config::Config::DF_NO_GC );
	if ( s_no_gc ) {
		return;
	}
#endif
	std::lock_guard< std::mutex > guard( m_spaces_mutex );
	for ( const auto& gc_space : m_spaces ) {
		gc_space->Collect();
	}
}

#if defined( DEBUG ) || defined( FASTDEBUG )

thread_local static uint8_t s_gc_debug_offset = 0;
static std::mutex s_gc_debug_mutex;

const bool GC::IsDebugEnabled() {
	ASSERT_NOLOG( g_engine, "engine not set" );
	static const bool s_is_gc_debug_enabled = g_engine->GetConfig()->HasDebugFlag( config::Config::DF_VERBOSE_GC );
	return s_is_gc_debug_enabled;
}

void GC::DebugLock() {
	if ( IsDebugEnabled() ) {
		s_gc_debug_mutex.lock();
	}
}

void GC::DebugBegin( const std::string& what ) {
	if ( IsDebugEnabled() ) {
		ASSERT_NOLOG( s_gc_debug_offset < 255, "debug offset overflow" );
		util::LogHelper::Println( std::string( s_gc_debug_offset * 2, ' ' ) + what + ":" );
		s_gc_debug_offset++;
	}
}

void GC::DebugEntry( const std::string& what, gc::Object* const object ) {
	if ( IsDebugEnabled() ) {
		util::LogHelper::Println( std::string( s_gc_debug_offset * 2, ' ' ) + what + ": " + std::to_string( (unsigned long)object ) );
	}
}

void GC::DebugEnd() {
	if ( IsDebugEnabled() ) {
		ASSERT_NOLOG( s_gc_debug_offset > 0, "debug offset is zero" );
		s_gc_debug_offset--;
	}
}

void GC::DebugUnlock() {
	if ( IsDebugEnabled() ) {
		s_gc_debug_mutex.unlock();
		ASSERT_NOLOG( s_gc_debug_offset == 0, "debug offset not zero, something's wrong" );
		util::LogHelper::Flush();
	}
}

#endif

void GC::AddSpace( Space* const gc_space ) {
	std::lock_guard< std::mutex > guard( m_spaces_mutex );
	ASSERT( m_spaces.find( gc_space ) == m_spaces.end(), "gc space already added" );
	m_spaces.insert( gc_space );
}

void GC::RemoveSpace( Space* const gc_space ) {
	std::lock_guard< std::mutex > guard( m_spaces_mutex );
	ASSERT( m_spaces.find( gc_space ) != m_spaces.end(), "gc space not found" );
	m_spaces.erase( gc_space );
}

}
