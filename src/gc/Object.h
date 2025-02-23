#pragma once

#include <unordered_set>
#include <mutex>
#include <string>

namespace gc {

class Space;

class Object {
public:
	Object( gc::Space* const gc_space );
	virtual ~Object() = default;

	virtual void GetReachableObjects( std::unordered_set< Object* >& active_objects );

protected:
	std::mutex m_gc_mutex; // always use this when changing or collecting reachables

};

}
