#include "Space.h"

#include <thread>
#include <chrono>

#include "engine/Engine.h"
#include "GC.h"
#include "util/Time.h"
#include "util/String.h"
#include "util/LogHelper.h"
#include "graphics/Graphics.h"

namespace gc {

Space::Space( Object* const root_object )
	: m_root_object( root_object ) {
	ASSERT_NOLOG( root_object, "root object is null" );
	g_engine->GetGC()->AddSpace( this );
}

Space::~Space() {
	g_engine->GetGC()->RemoveSpace( this );
	m_collect_mutex.lock(); // wait for any ongoing collects to finish
	m_is_destroying = true;
	m_collect_mutex.unlock();

	// collect until there's nothing to collect
	GC_LOG( "Destroying remaining objects" );
	{
		m_objects_mutex.lock();
		while ( !m_objects.empty() ) {
			GC_LOG( std::to_string( m_objects.size() ) + " objects are still reachable" );
			m_objects_mutex.unlock();
			{
				std::lock_guard guard( m_accumulations_mutex );
				if ( !m_accumulations.empty() ) {
					Log( "WARNING: space is destroying but still accumulating in " + std::to_string( m_accumulations.size() ) + " thread(s)" );
				}
			}
			while ( Collect() ) {}
			m_objects_mutex.lock();
			if ( !m_objects.empty() ) {
				Log( "WARNING: collect finished but objects still not empty" );
			}
		}
		m_objects_mutex.unlock();
	}
	GC_LOG( "All objects have been destroyed." );
}

void Space::Add( Object* object ) {
	ASSERT_NOLOG( !m_is_destroying, "space is destroying" );
	ASSERT_NOLOG( IsAccumulating(), "GC not in accumulation mode" );
	//GC_LOG( "Adding object: " + std::to_string( (unsigned long long)object ) );
	ASSERT( m_accumulated_objects.find( object ) == m_accumulated_objects.end(), "object " + std::to_string( (unsigned long long)object ) + " already exists" );
	m_accumulated_objects.insert( object );
}

void Space::Accumulate( const std::function< void() >& f ) {
	bool is_accumulating;
	const auto tid = std::this_thread::get_id();
	{
		std::lock_guard guard( m_accumulations_mutex );
		is_accumulating = m_accumulations.find( tid ) != m_accumulations.end();
	}
	if ( is_accumulating ) { // recursive accumulate, nothing to do
		f();
	}
	else { // top accumulate, need to do full logic
		std::lock_guard guard2( m_accumulation_mutex );
		std::lock_guard guard( m_collect_mutex ); // do not accumulate during collect or vice versa // TODO: optimize to reduce lock times
		const auto& commit = [ this, &tid ]() {
			{
				std::lock_guard guard( m_accumulations_mutex );
				ASSERT_NOLOG( m_accumulations.find( tid ) != m_accumulations.end(), "accumulations thread not found" );
				m_accumulations.erase( tid );
			}
			if ( !m_accumulated_objects.empty() ) {
				std::lock_guard guard( m_objects_mutex );
				GC_LOG( "Accumulated " + std::to_string( m_accumulated_objects.size() ) + " objects" );
				m_objects.insert( m_accumulated_objects.begin(), m_accumulated_objects.end() );
				m_accumulated_objects.clear();
			}
		};
		ASSERT_NOLOG( m_accumulated_objects.empty(), "accumulated objects not empty" );
		{
			std::lock_guard guard3( m_accumulations_mutex );
			ASSERT_NOLOG( m_accumulations.find( tid ) == m_accumulations.end(), "accumulations thread already exists" );
			m_accumulations.insert( tid );
		}
		try {
			f();
		}
		catch ( const gse::Exception& e ) {
			commit();
			throw;
		}
		commit();
	}
}

const bool Space::IsAccumulating() {
	std::lock_guard guard( m_accumulations_mutex );
	return m_accumulations.find( std::this_thread::get_id() ) != m_accumulations.end();
}

const bool Space::Collect() {
	std::lock_guard guard( m_collect_mutex ); // allow only one collection at same space at same time
	ASSERT( m_reachable_objects_tmp.empty(), "reachable objects tmp not empty" );

	GC_DEBUG_LOCK();
	GC_DEBUG_BEGIN( "Root" );
	m_root_object->GetReachableObjects( m_reachable_objects_tmp );
	GC_DEBUG_END();
	GC_DEBUG_UNLOCK();

	std::unordered_set< Object* > removed_objects = {};
	{
		std::lock_guard guard3( m_accumulations_mutex ); // prevent collection during accumulation // TODO: improve

		g_engine->GetGraphics()->NoRender( // tmp: prevent race conditions with render thread
			[ this, &removed_objects ]() {
				std::lock_guard guard2( m_objects_mutex );
				for ( const auto& object : m_objects ) {
					const auto& it = m_reachable_objects_tmp.find( object );
					if ( it == m_reachable_objects_tmp.end() ) {
						ASSERT( removed_objects.find( object ) == removed_objects.end(), "object " + std::to_string( (unsigned long long)object ) + " was already removed" );
#if defined( DEBUG ) || defined( FASTDEBUG )
						GC_LOG( "Destroying unreachable object: " + util::String::ToHexString( (unsigned long long)object ) /* TODO + "[ " + object->ToString() + " ]"*/ );
#endif
						delete object;
						removed_objects.insert( object );
					}
				}
				GC_LOG( "Kept " + std::to_string( m_reachable_objects_tmp.size() ) + " reachable objects, removed " + std::to_string( removed_objects.size() ) + " unreachable" );
			}
		);

		m_reachable_objects_tmp.clear();

		{
			std::lock_guard guard2( m_objects_mutex );
			for ( const auto& object : removed_objects ) {
				ASSERT( m_objects.find( object ) != m_objects.end(), "object to be removed not found" );
				m_objects.erase( object );
			}
		}
	}

	return !removed_objects.empty();
}

}
