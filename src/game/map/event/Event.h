#pragma once

#include "types/Serializable.h"

namespace game {
namespace world {
class World;
namespace event {

class Event : public types::Serializable {
public:
	enum event_type_t {
		
	};

	virtual void Apply( World* world ) = 0;
};

}
}
}
