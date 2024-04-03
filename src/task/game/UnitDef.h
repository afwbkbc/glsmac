#pragma once

#include <string>

#include "InstancedSpriteManager.h"

#include "game/unit/Def.h"

#include "Sprite.h"

namespace task {
namespace game {

class UnitDef {
public:
	UnitDef( InstancedSpriteManager* ism, const ::game::unit::Def* unitdef );
	~UnitDef();

	const bool IsArtillery() const;

	Sprite* GetSprite( const ::game::unit::Morale::morale_t morale );

	const bool IsImmovable() const;

	const std::string GetNameString() const;
	const std::string GetStatsString() const;

private:

	InstancedSpriteManager* const m_ism;

	std::string m_id;
	std::string m_name;
	::game::unit::Def::def_type_t m_type;

	typedef std::unordered_map< ::game::unit::Morale::morale_t, Sprite > morale_based_sprites_t;

	// TODO: get rid of union
	union {
		struct {
			::game::unit::Def::movement_type_t movement_type;
			::game::unit::Unit::movement_t movement_per_turn;
			struct {
				bool is_sprite;
				uint32_t morale_based_xshift;
				union {
					Sprite sprite;
					morale_based_sprites_t* morale_based_sprites;
				};
			} render;
		} static_;
	};
};

}
}
