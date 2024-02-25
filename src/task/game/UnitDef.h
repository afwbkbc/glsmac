#pragma once

#include <string>

#include "game/unit/Def.h"
#include "game/unit/StaticDef.h"

#include "Sprite.h"

namespace task {
namespace game {

class UnitDef {
public:
	std::string m_id;
	std::string m_name;
	::game::unit::Def::def_type_t m_type;
	union {
		struct {
			::game::unit::StaticDef::movement_type_t movement_type;
			::game::unit::Unit::movement_t movement_per_turn;
			struct {
				bool is_sprite;
				uint32_t morale_based_xshift;
				union {
					Sprite sprite;
					std::unordered_map< ::game::unit::Unit::morale_t, Sprite >* morale_based_sprites;
				};
			} render;
		} static_;
	};
};

}
}
