#pragma once

#include <string>
#include <unordered_map>

#include "game/unit/Types.h"

#include "types/Vec3.h"
// TODO: remove?
#include "task/game/Sprite.h"

namespace game::unit {
class Def;
}

namespace task {
namespace game {

class InstancedSpriteManager;

namespace unit {

class UnitDef {
public:
	UnitDef( InstancedSpriteManager* ism, const ::game::unit::Def* unitdef );
	~UnitDef();

	const bool IsArtillery() const;

	Sprite* GetSprite( const ::game::unit::morale_t morale );
	const types::Vec3 GetUnitCoordsOnTile( const types::Vec3& tile_coords );

	const bool IsImmovable() const;

	const std::string GetNameString() const;
	const std::string GetStatsString() const;

private:

	InstancedSpriteManager* const m_ism;

	std::string m_id;
	std::string m_name;
	::game::unit::def_type_t m_type;

	typedef std::unordered_map< ::game::unit::morale_t, Sprite > morale_based_sprites_t;

	// TODO: get rid of union
	union {
		struct {
			::game::unit::movement_type_t movement_type;
			::game::unit::movement_t movement_per_turn;
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
}
