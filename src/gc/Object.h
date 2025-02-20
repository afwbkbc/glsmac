#pragma once

#include <unordered_set>

namespace gc {

class Object {
public:
	Object( const bool is_container = false );
	virtual ~Object() = default;

	const bool m_is_container = false;
};

}
