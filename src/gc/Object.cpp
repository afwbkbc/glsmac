#include "Object.h"

#include "Space.h"

namespace gc {

Object::Object( gc::Space* const gc_space ) {
	if ( gc_space ) {
		gc_space->Add( this );
	}
}

void Object::GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) {
	GC_DEBUG_BEGIN( "gc::Object" );

	GC_DEBUG( "this", this );
	reachable_objects.insert( this );

	{
		if ( !m_persisted_objects.empty() ) {
			GC_DEBUG_BEGIN( "persisted_objects" );
			for ( const auto& obj : m_persisted_objects ) {
				GC_REACHABLE( obj );
			}
			GC_DEBUG_END();
		}
	}

	GC_DEBUG_END();
}

void Object::Persist( Object* const obj ) {
	ASSERT_NOLOG( m_persisted_objects.find( obj ) == m_persisted_objects.end(), "object already persisted" );
	m_persisted_objects.insert( obj );
}

void Object::Unpersist( Object* const obj ) {
	ASSERT_NOLOG( m_persisted_objects.find( obj ) != m_persisted_objects.end(), "object not persisted" );
	m_persisted_objects.erase( obj );
}

}
