#pragma once

#include <mutex>
#include <atomic>
#include <unordered_set>
#include <unordered_map>
#include <chrono>

#include "common/Common.h"

#if defined( DEBUG ) || defined( FASTDEBUG )
namespace gse::runner {
class Interpreter;
}
#endif

namespace gc {

class GC;
class Object;

CLASS( Space, common::Class )

	Space();
	~Space();

	void AddRoot( Object* object );
	void RemoveRoot( Object* object );
	void ReplaceRoot( Object* from, Object* to ); // atomic

private:
	std::atomic< bool > m_is_destroying = false;

	// objects to add to pending
	std::mutex m_objects_to_add_mutex;
	std::unordered_set< Object* > m_objects_to_add = {};

	// objects that were just created and have temporary protection because they need time to be assigned somewhere to become reachable
	std::unordered_map< Object*, uint64_t > m_pending_objects = {};

	// objects that are already collectable
	std::unordered_set< Object* > m_objects = {};

	// objects that are queried for reachability (recursive), they return sets of reachable objects. not collectable until manually removed
	std::mutex m_root_objects_mutex;
	std::unordered_set< Object* > m_root_objects = {};

	// thread-safety of collection logic
	std::mutex m_collect_mutex;

	// to avoid reallocations
	std::unordered_set< Object* > m_reachable_objects_tmp = {};

private:
	friend class GC;
#if defined( DEBUG ) || defined( FASTDEBUG )
	friend class gse::runner::Interpreter;
#endif
	const bool Collect(); // true if anything was gced, false otherwise

private:
	friend class Object;
	void Add( Object* object );

};

}
