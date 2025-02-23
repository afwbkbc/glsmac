#include "GC.h"

#include "Space.h"

namespace gc {

void GC::Start() {

}

void GC::Stop() {

}

void GC::Iterate() {
	std::lock_guard< std::mutex > guard( m_spaces_mutex );
	Log( "GC ITERATE BEGIN " + std::to_string( m_spaces.size() ) );
	for ( const auto& gc_space : m_spaces ) {
		gc_space->Collect();
	}
	Log( "GC ITERATE END" );
}

void GC::AddSpace( Space* const gc_space ) {
	std::lock_guard< std::mutex > guard( m_spaces_mutex );
	ASSERT( m_spaces.find( gc_space ) == m_spaces.end(), "gc space already added" );
	m_spaces.insert( gc_space );
	Log( "GC SPACE ADD " + std::to_string( m_spaces.size() ) );
}

void GC::RemoveSpace( Space* const gc_space ) {
	std::lock_guard< std::mutex > guard( m_spaces_mutex );
	ASSERT( m_spaces.find( gc_space ) != m_spaces.end(), "gc space not found" );
	m_spaces.erase( gc_space );
	Log( "GC SPACE REMOVE " + std::to_string( m_spaces.size() ) );
}

}
