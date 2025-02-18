#include "GC.h"

#include "Space.h"

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
