#pragma once

#include <unordered_set>
#include <mutex>
#include <string>

#define REACHABLE_EXT( _var, _method ) \
    if ( reachable_objects.find( _var ) == reachable_objects.end() ) { \
        (_var)->_method( reachable_objects ); \
    } \
    else { \
        GC_DEBUG( "ref", _var ); \
}
#define GC_REACHABLE( _var ) REACHABLE_EXT( _var, GetReachableObjects )

#if ( defined( DEBUG ) || defined( FASTDEBUG ) )
#include "GC.h"
#define GC_LOG( _what ) \
    if ( GC::IsDebugEnabled() ) { \
        Log( _what ); \
    }
#define GC_DEBUG_LOCK() gc::GC::DebugLock()
#define GC_DEBUG_BEGIN( _what ) gc::GC::DebugBegin( _what )
#define GC_DEBUG( _what, _object ) gc::GC::DebugEntry( _what, _object )
#define GC_DEBUG_END() gc::GC::DebugEnd()
#define GC_DEBUG_UNLOCK() gc::GC::DebugUnlock()
#else
#define GC_LOG( _what )
#define GC_DEBUG_LOCK()
#define GC_DEBUG_BEGIN( _what )
#define GC_DEBUG( _what, _object )
#define GC_DEBUG_END()
#define GC_DEBUG_UNLOCK()
#endif

namespace gc {

class Space;

class Object {
public:
	Object( gc::Space* const gc_space );
	virtual ~Object() = default;

	virtual void GetReachableObjects( std::unordered_set< Object* >& reachable_objects );

protected:
	std::recursive_mutex m_gc_mutex; // always use this when changing or collecting reachables

};

}
