#pragma once

#include <unordered_set>

#include "Object.h"

namespace gc {

class Container : public Object {
public:
	Container();
	virtual ~Container() = default;

	virtual void CollectActiveObjects( std::unordered_set< Object* >& active_objects ) = 0;

};

}
