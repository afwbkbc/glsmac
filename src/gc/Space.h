#pragma once

#include <mutex>
#include <atomic>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <thread>

#include "common/Common.h"

#if defined( DEBUG ) || defined( FASTDEBUG )
namespace gse::runner {
class Interpreter;
}
#define CHECKACCUM( _gcspace ) ASSERT_NOLOG( _gcspace->IsAccumulating(), "GC not in accumulation mode" )
#else
#define CHECKACCUM( _gcspace )
#endif

namespace gc {

class GC;
class Object;

CLASS( Space, common::Class )

	Space( Object* const root );
	~Space();

	void Accumulate( const std::function< void() >& f );
	const bool IsAccumulating();

private:
	// if true - it means space is about to be destroyed and doing final cleanups/collects
	std::atomic< bool > m_is_destroying = false;

	// object that is queried for reachability (recursive), not collectable and must be deleted manually after space
	Object* const m_root_object = {};

	// to track accumulating threads
	std::recursive_mutex m_accumulations_mutex; // TODO: make non-recursive
	std::unordered_set< std::thread::id > m_accumulations = {};

	// objects that have been accumulated but won't be collected until accumulator function finishes (that allows for temp values to move and assign where needed)
	std::mutex m_accumulation_mutex;
	std::unordered_set< Object* > m_accumulated_objects = {};

	// objects that are already collectable
	std::mutex m_objects_mutex;
	std::unordered_set< Object* > m_objects = {};

	// thread-safety of collection logic, to make sure only one thread can run collection of this space at any given time
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
