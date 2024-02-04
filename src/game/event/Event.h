#pragma once

#include "types/Serializable.h"

namespace game {
class Game;
namespace event {

class Event : public types::Serializable {
public:
	enum event_type_t {

	};

	virtual void Apply( Game* game ) = 0;
};

}
}
