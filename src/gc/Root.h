#pragma once

#include "Object.h"

namespace gc {

class Root : public Object {
public:
	Root()
		: Object( nullptr ) {}

	virtual void GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) = 0;

};

}
