#include "GC.h"

#include "Space.h"

#if defined( DEBUG ) || defined( FASTDEBUG )
#include "util/LogHelper.h"
#endif

namespace gc {

void GC::Start() {

}

void GC::Stop() {

}

void GC::Iterate() {
	std::lock_guard< std::mutex > guard( m_spaces_mutex );
	for ( const auto& gc_space : m_spaces ) {
		gc_space->Collect();
	}
}

#if defined( DEBUG ) || defined( FASTDEBUG )

thread_local static uint8_t s_gc_debug_offset = 0;
static std::mutex s_gc_debug_mutex;

void GC::DebugLock() {
	s_gc_debug_mutex.lock();
}

void GC::DebugBegin( const std::string& what ) {
	ASSERT_NOLOG( s_gc_debug_offset < 255, "debug offset overflow" );
	util::LogHelper::Println( std::string( s_gc_debug_offset * 2, ' ' ) + what + ":" );
	s_gc_debug_offset++;
}

void GC::DebugEntry( const std::string& what, gc::Object* const object ) {
	util::LogHelper::Println( std::string( s_gc_debug_offset * 2, ' ' ) + what + ": " + std::to_string( (unsigned long)object ) );
}

void GC::DebugEnd() {
	ASSERT_NOLOG( s_gc_debug_offset > 0, "debug offset is zero" );
	s_gc_debug_offset--;
}

void GC::DebugUnlock() {
	s_gc_debug_mutex.unlock();
	ASSERT_NOLOG( s_gc_debug_offset == 0, "debug offset not zero, something's wrong" );
	util::LogHelper::Flush();
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
