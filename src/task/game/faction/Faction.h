#pragma once

#include "types/Color.h"

#include "task/game/Sprite.h"

namespace game::rules {
class Faction;
}

namespace task {
namespace game {

class InstancedSpriteManager;

namespace faction {

class Faction {
public:
	Faction( const ::game::rules::Faction* def, InstancedSpriteManager* ism );

	const types::Color m_border_color;
	const bool m_is_progenitor;

private:
	Sprite m_base_sprites[6][4] = {};

};

}
}
}
