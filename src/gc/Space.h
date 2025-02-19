#pragma once

#include <mutex>
#include <unordered_set>

#include "common/Common.h"

namespace gc {

class GC;
class Object;

CLASS( Space, common::Class )

	Space();
	~Space();

	void Add( Object* object );
	void Remove( Object* object );

private:
	std::unordered_set< Object* > m_objects = {};

	std::mutex m_objects_to_add_mutex;
	std::unordered_set< Object* > m_objects_to_add = {};
	
	std::mutex m_objects_to_remove_mutex;
	std::unordered_set< Object* > m_objects_to_remove = {};

	std::mutex m_collect_mutex;

	std::unordered_set< Object* > m_reachable_objects_tmp = {}; // keep it here to avoid reallocations

private:
	friend class GC;
	const bool Collect(); // true if anything was gced, false otherwise

};

}
