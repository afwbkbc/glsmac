#include "Space.h"

#include <thread>
#include <chrono>

#include "engine/Engine.h"
#include "GC.h"
#include "Object.h"
#include "util/Time.h"

namespace gc {

Space::Space() {
	g_engine->GetGC()->AddSpace( this );
}

Space::~Space() {
	g_engine->GetGC()->RemoveSpace( this );
	m_collect_mutex.lock(); // wait for any ongoing collects to finish
	m_is_destroying = true;
	m_collect_mutex.unlock();
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
		if ( !m_pending_objects.empty() ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( GC::NEW_OBJECT_PROTECTION_TIME_MS ) );
		}
	}
	Log( "All objects have been destroyed." );
}

void Space::Add( Object* object ) {
	ASSERT_NOLOG( !m_is_destroying, "space is destroying" );
	std::lock_guard< std::mutex > guard( m_objects_to_add_mutex );
	//Log( "Adding object: " + std::to_string( (unsigned long)object ) );
	ASSERT( m_objects.find( object ) == m_objects.end(), "object " + std::to_string( (unsigned long)object ) + " already exists" );
	ASSERT( m_objects_to_add.find( object ) == m_objects_to_add.end(), "object " + std::to_string( (unsigned long)object ) + " already pending addition" );
	m_objects_to_add.insert( object );
}

void Space::AddRoot( Object* object ) {
	//std::lock_guard< std::mutex > guard2( m_collect_mutex );
	ASSERT_NOLOG( !m_is_destroying, "space is destroying" );
	{
		std::lock_guard< std::mutex > guard( m_root_objects_mutex );
		Log( "Adding root object: " + std::to_string( (unsigned long)object ) );
		ASSERT( m_root_objects.find( object ) == m_root_objects.end(), "root object " + std::to_string( (unsigned long)object ) + " already exists" );
		m_root_objects.insert( object );
	}
}

void Space::RemoveRoot( Object* object ) {
	//std::lock_guard< std::mutex > guard2( m_collect_mutex );
	if ( !m_is_destroying ) { // if destroying then all root objects will be removed anyway
		std::lock_guard< std::mutex > guard( m_root_objects_mutex );
		Log( "Removing root object: " + std::to_string( (unsigned long)object ) );
		ASSERT( m_root_objects.find( object ) != m_root_objects.end(), "root object " + std::to_string( (unsigned long)object ) + " not found" );
		m_root_objects.erase( object );
	}
}

void Space::ReplaceRoot( Object* from, Object* to ) {
	ASSERT_NOLOG( !m_is_destroying, "space is destroying" );
	std::lock_guard< std::mutex > guard( m_root_objects_mutex );
	Log( "Replacing root object: " + std::to_string( (unsigned long)from ) + " -> " + std::to_string( (unsigned long)to ) );
	ASSERT( m_root_objects.find( from ) != m_root_objects.end(), "root object " + std::to_string( (unsigned long)from ) + " not found" );
	ASSERT( m_root_objects.find( to ) == m_root_objects.end(), "root object " + std::to_string( (unsigned long)to ) + " already exists" );
	m_root_objects.erase( from );
	m_root_objects.insert( to );
}

const bool Space::Collect() {
	std::lock_guard< std::mutex > guard( m_collect_mutex ); // allow only one collection at same space at same time
	{
		std::lock_guard< std::mutex > guard2( m_root_objects_mutex );
		ASSERT( m_reachable_objects_tmp.empty(), "reachable objects tmp not empty" );
		GC_DEBUG_LOCK();
		Log( "Collecting from " + std::to_string( m_root_objects.size() ) + " root objects" );
		for ( const auto& object : m_root_objects ) {
			ASSERT_NOLOG( m_reachable_objects_tmp.find( object ) == m_reachable_objects_tmp.end(), "root already added" );
			GC_DEBUG_BEGIN( "Root" );
			m_reachable_objects_tmp.insert( object );
			object->GetReachableObjects( m_reachable_objects_tmp );
			GC_DEBUG_END();
		}
		GC_DEBUG_UNLOCK();
	}

	bool anything_removed = false;
	std::unordered_set< Object* > removed_objects = {};
	for ( const auto& object : m_objects ) {
		const auto& it = m_reachable_objects_tmp.find( object );
		if ( it == m_reachable_objects_tmp.end() ) {
			ASSERT( removed_objects.find( object ) == removed_objects.end(), "object " + std::to_string( (unsigned long)object ) + " was already removed" );
			Log( "Destroying unreachable object: " + std::to_string( (unsigned long)object ) );
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
		}

	}

	// add pending objects to collectable objects
	const auto now = util::Time::Now();
	if ( !m_pending_objects.empty() ) {
		const auto pending_objects = m_pending_objects;
		for ( const auto& it : pending_objects ) {
			ASSERT_NOLOG( removed_objects.find( it.first ) == removed_objects.end(), "pending object already removed" );
			if ( now - it.second >= GC::NEW_OBJECT_PROTECTION_TIME_MS ) {
				m_objects.insert( it.first );
				m_pending_objects.erase( it.first );
			}
		}
	}

	// add new objects to pending objects
	{
		std::lock_guard< std::mutex > guard2( m_objects_to_add_mutex );
		if ( !m_objects_to_add.empty() ) {
#if defined( DEBUG ) || defined( FASTDEBUG )
			for ( const auto& object : m_objects_to_add ) {
				ASSERT( m_objects.find( object ) == m_objects.end(), "object " + std::to_string( (unsigned long)object ) + " already added" );
			}
#endif
			for ( const auto& object : m_objects_to_add ) {
				ASSERT_NOLOG( removed_objects.find( object ) == removed_objects.end(), "object to be added already removed" );
				m_pending_objects.insert(
					{
						object,
						now
					}
				);
			}
			m_objects_to_add.clear();
		}
	}

	return anything_removed;
}

}
