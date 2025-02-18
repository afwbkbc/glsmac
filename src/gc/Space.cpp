#include "Space.h"

#include "engine/Engine.h"
#include "GC.h"
#include "Object.h"

namespace gc {

Space::Space() {
	g_engine->GetGC()->AddSpace( this );
}

Space::~Space() {
	g_engine->GetGC()->RemoveSpace( this );
	while ( Collect() ) {}
#if defined( DEBUG ) || defined( FASTDEBUG )
	{
		std::lock_guard< std::mutex > guard( m_objects_mutex );
		if ( !m_objects.empty() ) {
			Log( "WARNING: " + std::to_string( m_objects.size() ) + " objects are still reachable!" );
		}
		else {
			Log( "All objects have been destroyed." );
		}
	}
#endif
}

void Space::Add( Object* object ) {
	std::lock_guard< std::mutex > guard( m_objects_mutex );
	Log( "Adding object: " + std::to_string( (unsigned long)object ) );
	ASSERT( m_objects.find( object ) == m_objects.end(), "object already in gc space" );
	m_objects.insert( object );
}

void Space::Remove( Object* object ) {
	std::lock_guard< std::mutex > guard( m_objects_mutex );
	Log( "Removing object: " + std::to_string( (unsigned long)object ) );
	ASSERT( m_objects.find( object ) != m_objects.end(), "object not in gc space" );
	ASSERT( m_objects_to_remove.find( object ) == m_objects_to_remove.end(), "object already pending removal" );
	m_objects_to_remove.insert( object );
}

const bool Space::Collect() {
	std::lock_guard< std::mutex > guard( m_collect_mutex ); // allow only one collection at same space at same time
	m_objects_mutex.lock();

	if ( m_objects.empty() ) {
		ASSERT( m_objects_to_remove.empty(), "objects empty but removals pending" );
		m_objects_mutex.unlock();
		return false;
	}

	// make copies to release global lock asap
	const auto objects = m_objects;
	const auto objects_to_remove = m_objects_to_remove;
	m_objects_to_remove.clear();

	m_objects_mutex.unlock();

	ASSERT( m_reachable_objects_tmp.empty(), "reachable objects tmp not empty" );

#if defined( DEBUG ) || defined( FASTDEBUG )
	for ( const auto& object_to_remove : objects_to_remove ) {
		ASSERT( objects.find( object_to_remove ) != objects.end(), "object " + std::to_string( (unsigned long)object_to_remove ) + " is pending removal but not found" );
	}
#endif

	Log( "Collecting from " + std::to_string( objects.size() ) + " objects" );
	for ( const auto& object : objects ) {
		if ( objects_to_remove.find( object ) == objects_to_remove.end() ) {
			object->CollectActiveObjects( m_reachable_objects_tmp );
		}
	}
	Log( "Found " + std::to_string( m_reachable_objects_tmp.size() ) + " reachable objects, " + std::to_string( objects.size() - m_reachable_objects_tmp.size() ) + " unreachable" );

#if defined( DEBUG ) || defined( FASTDEBUG )
	for ( const auto& reachable_object : m_reachable_objects_tmp ) {
		ASSERT( objects.find( reachable_object ) != objects.end(), "reachable object " + std::to_string( (unsigned long)reachable_object ) + " not found" );
	}
#endif

	bool anything_removed = false;
	std::unordered_set< Object* > kept_objects = {};
	for ( const auto& object : objects ) {
		const auto& it = m_reachable_objects_tmp.find( object );
		if ( it == m_reachable_objects_tmp.end() ) {
			Log( "Destroying unreachable object: " + std::to_string( (unsigned long)object ) );
			delete object;
			anything_removed = true;
		}
		else {
			kept_objects.insert( object );
		}
	}

	m_reachable_objects_tmp.clear();

	if ( anything_removed ) {
		std::lock_guard< std::mutex > guard2( m_objects_mutex );
		m_objects = kept_objects; // copy back to update
	}

	return anything_removed;
}

}
