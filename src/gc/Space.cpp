#include "Space.h"

#include "engine/Engine.h"
#include "GC.h"
#include "Object.h"

namespace gc {

Space::Space() {
	g_engine->GetGC()->AddSpace( this );
}

Space::~Space() {
	Log( "SPACE BEFORE REMOVE" );
	g_engine->GetGC()->RemoveSpace( this );
	Log( "SPACE AFTER REMOVE" );
	m_collect_mutex.lock(); // wait for any ongoing collects to finish
	m_is_destroying = true;
	m_collect_mutex.unlock();
	Log( "SPACE AFTER REMOVE WAIT" );
	{
		// remove root objects
		std::lock_guard< std::mutex > guard( m_root_objects_mutex );
		//if ( !m_root_objects.empty() ) {
		//m_objects.insert( m_root_objects.begin(), m_root_objects.end() );
		//}
		m_root_objects.clear();
	}
	// collect until there's nothing to collect
	while ( !m_objects.empty() || !m_pending_objects.empty() || !m_objects_to_add.empty() ) {
		Log( std::to_string( m_objects.size() ) + " objects are still reachable" );
		while ( Collect() ) {}
	}
	Log( "All objects have been destroyed." );
}

void Space::Add( Object* object ) {
	ASSERT_NOLOG( !m_is_destroying, "space is destroying" );
	std::lock_guard< std::mutex > guard( m_objects_to_add_mutex );
	Log( "Adding object: " + object->GCString() );
	ASSERT( m_objects_to_add.find( object ) == m_objects_to_add.end(), "object " + object->GCString() + " already pending addition" );
	m_objects_to_add.insert( object );
}

void Space::AddRoot( Object* object ) {
	//std::lock_guard< std::mutex > guard2( m_collect_mutex );
	ASSERT_NOLOG( !m_is_destroying, "space is destroying" );
	{
		std::lock_guard< std::mutex > guard( m_root_objects_mutex );
		Log( "Adding root object: " + object->GCString() );
		ASSERT( m_root_objects.find( object ) == m_root_objects.end(), "root object " + object->GCString() + " already exists" );
		m_root_objects.insert( object );
	}
}

void Space::RemoveRoot( Object* object ) {
	//std::lock_guard< std::mutex > guard2( m_collect_mutex );
	if ( !m_is_destroying ) { // if destroying then all root objects will be removed anyway
		std::lock_guard< std::mutex > guard( m_root_objects_mutex );
		Log( "Removing root object: " + object->GCString() );
		ASSERT( m_root_objects.find( object ) != m_root_objects.end(), "root object " + object->GCString() + " not found" );
		m_root_objects.erase( object );
	}
}

void Space::ReplaceRoot( Object* from, Object* to ) {
	ASSERT_NOLOG( !m_is_destroying, "space is destroying" );
	std::lock_guard< std::mutex > guard( m_root_objects_mutex );
	Log( "Replacing root object: " + from->GCString() + " -> " + to->GCString() );
	ASSERT( m_root_objects.find( from ) != m_root_objects.end(), "root object " + from->GCString() + " not found" );
	ASSERT( m_root_objects.find( to ) == m_root_objects.end(), "root object " + to->GCString() + " already exists" );
	m_root_objects.erase( from );
	m_root_objects.insert( to );
}

const bool Space::Collect() {
	std::lock_guard< std::mutex > guard( m_collect_mutex ); // allow only one collection at same space at same time
	{
		std::lock_guard< std::mutex > guard2( m_root_objects_mutex );
		ASSERT( m_reachable_objects_tmp.empty(), "reachable objects tmp not empty" );
		for ( const auto& object : m_root_objects ) { // prevent deletion of root objects
			m_reachable_objects_tmp.insert( object );
		}
		Log( "Collecting from " + std::to_string( m_root_objects.size() ) + " root objects" );
		for ( const auto& object : m_root_objects ) {
			object->GetReachableObjects( m_reachable_objects_tmp );
		}
	}

	// TMP
	for ( const auto& object : m_objects ) {
		Log( "OBJECT PTR: " + std::to_string( (unsigned long)object ) );
		Log( "OBJECT STRING: " + object->GCString() );
	}

	bool anything_removed = false;
	std::unordered_set< Object* > removed_objects = {};
	for ( const auto& object : m_objects ) {
		const auto& it = m_reachable_objects_tmp.find( object );
		if ( it == m_reachable_objects_tmp.end() ) {
			ASSERT( removed_objects.find( object ) == removed_objects.end(), "object " + std::to_string( (unsigned long)object ) + " was already removed" );
			Log( "Destroying unreachable object: " + std::to_string( (unsigned long)object ) );
			Log( "Object GCString(): " + object->GCString() );
			delete object;
			anything_removed = true;
			removed_objects.insert( object );
		}
	}
	Log( "Kept " + std::to_string( m_reachable_objects_tmp.size() ) + " reachable objects, removed " + std::to_string( removed_objects.size() ) + " unreachable" );

	m_reachable_objects_tmp.clear();

	if ( anything_removed ) {
		std::lock_guard< std::mutex > guard2( m_objects_to_add_mutex );

		for ( const auto& object : removed_objects ) {
			ASSERT( m_objects.find( object ) != m_objects.end(), "object to be removed not found" );
			m_objects.erase( object );
			//m_pending_objects.erase( object );
			//m_objects_to_add.erase( object );
			//m_root_objects.erase( object );
		}

	}

	// add pending objects to collectable objects
	if ( !m_pending_objects.empty() ) {
		for ( const auto& object : m_pending_objects ) {
			if ( removed_objects.find( object ) == removed_objects.end() ) {
				m_objects.insert( object );
			}
		}
		m_pending_objects.clear();
	}

	// add new objects to pending objects
	{
		std::lock_guard< std::mutex > guard2( m_objects_to_add_mutex );
		if ( !m_objects_to_add.empty() ) {
#if defined( DEBUG ) || defined( FASTDEBUG )
			for ( const auto& object : m_objects_to_add ) {
				ASSERT( m_objects.find( object ) == m_objects.end(), "object " + object->GCString() + " already added" );
			}
#endif
			for ( const auto& object : m_objects_to_add ) {
				if ( removed_objects.find( object ) == removed_objects.end() ) {
					m_pending_objects.insert( object );
				}
			}
			m_objects_to_add.clear();
		}
	}

	return anything_removed;
}

}
