#pragma once

#include "types/Color.h"

namespace game::rules {
class Faction;
}

namespace task {
namespace game {
namespace faction {

class Faction {
public:
	Faction( const ::game::rules::Faction* def );

	const types::Color m_border_color;
	const bool m_is_progenitor;
};

}
}
}
