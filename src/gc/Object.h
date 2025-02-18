#pragma once

#include <unordered_set>

namespace gc {

class Object {
public:

	virtual ~Object() = default;
	
	virtual void CollectActiveObjects( std::unordered_set< Object* >& active_objects ) = 0;

};

}
