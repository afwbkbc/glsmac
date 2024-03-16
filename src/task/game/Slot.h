#pragma once

#include <cstddef>

#include "InstancedSpriteManager.h"
#include "Sprite.h"
#include "SlotBadges.h"
#include "BadgeDefs.h"

#include "types/Color.h"

namespace task {
namespace game {

class Slot {
public:

	typedef uint8_t slot_flags_t;
	static const slot_flags_t SF_NONE{ 0 };
	static const slot_flags_t SF_PROGENITOR{ 1 << 0 };

	Slot(
		BadgeDefs* badge_defs,
		InstancedSpriteManager* ism,
		const size_t slot_index, const types::Color& color, const slot_flags_t flags
	);
	~Slot();

	Sprite* GetUnitBadgeSprite( const ::game::unit::Unit::morale_t morale, const bool is_active );

	const size_t ShowFakeBadge( const Vec3& coords, const uint8_t offset );
	void HideFakeBadge( const size_t instance_id );

private:

	BadgeDefs* const m_badge_defs;
	InstancedSpriteManager* const m_ism;
	const std::string m_badges_key;
	const size_t m_slot_index;
	const types::Color m_color;
	const slot_flags_t m_flags;

	std::unordered_map< ::game::unit::Unit::morale_t, SlotBadges > badges = {};
	Sprite fake_badge = {};
};

}
}
