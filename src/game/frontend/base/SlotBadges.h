#pragma once

#include <cstddef>

#include "types/texture/Types.h"

#include "types/Vec3.h"
#include "types/Color.h"

namespace types::texture {
class Texture;
}

namespace game {
namespace frontend {

namespace sprite {
class Sprite;
class InstancedSprite;
class InstancedSpriteManager;
}

namespace faction {
class Faction;
}

namespace base {

class BaseManager;

class SlotBadges {
public:

	static const types::Vec3 GetBadgeCoords( const types::Vec3& unit_coords );

	SlotBadges(
		BaseManager* bm,
		sprite::InstancedSpriteManager* ism,
		const size_t slot_index,
		const faction::Faction* faction
	);
	~SlotBadges();

	sprite::Sprite* GetBaseBadgeSprite( const size_t population, const bool is_guarded );

private:

	BaseManager* m_bm = nullptr;
	sprite::InstancedSpriteManager* const m_ism;
	const std::string m_badges_key;
	const faction::Faction* m_faction = nullptr;

	class slot_sprites_t {
	public:
		sprite::Sprite* unguarded;
		sprite::Sprite* guarded;
	};
	std::unordered_map< size_t, slot_sprites_t > m_per_population_sprites = {};

	static const struct consts_t {
		const types::Vec2< float > scale = {
			0.005f,
			0.0075f
		};
		const types::Vec2< float > offset = {
			-0.5f,
			-0.6f
		};
	} s_consts;

	struct badge_sprite_t {
		types::texture::Texture* texture;
		sprite::InstancedSprite* sprite;
	};
	typedef std::unordered_map< size_t, badge_sprite_t > badge_spritemap_t; // population, sprite
	typedef std::unordered_map< bool, badge_spritemap_t > badge_spritemaps_t; // is_guarded, spritemap
	badge_spritemaps_t m_badge_sprites = {};
	sprite::InstancedSprite* GetBadgeSprite( const size_t population, const bool is_guarded );

};

}
}
}
