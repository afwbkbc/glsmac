#pragma once

#include <cstddef>

#include "game/unit/Types.h"

#include "types/Vec3.h"
#include "types/Color.h"

namespace task {
namespace game {

class Sprite;
class InstancedSpriteManager;

namespace faction {
class Faction;
}

namespace unit {

class BadgeDefs;

class SlotBadges {
public:

	SlotBadges(
		BadgeDefs* badge_defs,
		InstancedSpriteManager* ism,
		const size_t slot_index,
		const faction::Faction* faction
	);
	~SlotBadges();

	Sprite* GetUnitBadgeSprite( const ::game::unit::morale_t morale, const bool is_active );

	const size_t ShowFakeBadge( const types::Vec3& coords, const uint8_t offset );
	void HideFakeBadge( const size_t instance_id );

private:

	BadgeDefs* const m_badge_defs;
	InstancedSpriteManager* const m_ism;
	const std::string m_badges_key;

	class slot_sprites_t {
	public:
		Sprite* normal;
		Sprite* greyedout;
	};
	std::unordered_map< ::game::unit::morale_t, slot_sprites_t > m_per_morale_sprites = {};
	Sprite* m_fake_badge;
};

}
}
}
