#pragma once

#include <unordered_map>
#include <vector>

#include "types/Vec3.h"

#include "game/backend/unit/Types.h"

#include "game/frontend/sprite/Sprite.h"

namespace types::texture {
class Texture;
}

namespace game {
namespace frontend {

namespace sprite {
class InstancedSprite;
class InstancedSpriteManager;
}

namespace unit {

class BadgeDefs {
public:

	static const types::Vec3 GetBadgeCoords( const types::Vec3& unit_coords );
	static const types::Vec3 GetBadgeHealthbarCoords( const types::Vec3& unit_coords );

	BadgeDefs( sprite::InstancedSpriteManager* ism );
	~BadgeDefs();

	typedef uint8_t badge_type_t;
	static const badge_type_t BT_NORMAL{ 0 << 0 };
	static const badge_type_t BT_GREYEDOUT{ 1 << 0 };
	static const badge_type_t BT_DEFAULT{ 1 << 1 };
	static const badge_type_t BT_PROGENITOR{ 0 << 1 };

	sprite::InstancedSprite* GetBadgeSprite( const badge_type_t badge_type, const ::game::backend::unit::morale_t morale );
	sprite::InstancedSprite* GetFakeBadgeSprite();
	sprite::Sprite* GetBadgeHealthbarSprite( const float health );
	const size_t GetBadgeBlinkInterval() const;

	const types::Vec3 GetFakeBadgeCoords( const types::Vec3& coords, const uint8_t offset ) const;

private:

	static const struct consts_t {
		const types::Vec2< float > scale = {
			0.25f,
			0.5f
		};
		const types::Vec2< float > offset = {
			-0.25f,
			-0.5
		};
		const struct {
			const uint8_t width = 23;
			const uint8_t height = 30;
			const uint8_t margin = 1;
		} badges;
		const struct {
			const uint8_t resolution = 25;
			const types::Vec2< float > scale = {
				0.04f,
				0.36f
			};
			const types::Vec2< float > offset = {
				-0.292f,
				-0.476f
			};
		} healthbars;
		struct {
			const uint8_t max_per_tile = 10;
			const types::Vec2< float > scale = {
				0.0326f,
				0.283f
			};
			const types::Vec2< float > offset = {
				-0.11f,
				-0.54f
			};
			const float step_x = 0.032f;
		} fake_badges;
		struct {
			const size_t interval_ms = 128;
		} blink;
	} s_consts;

	sprite::InstancedSpriteManager* const m_ism = nullptr;

	typedef std::unordered_map< ::game::backend::unit::morale_t, sprite::InstancedSprite* > unitbadge_spritemap_t;
	typedef std::unordered_map< badge_type_t, unitbadge_spritemap_t > unitbadge_spritemaps_t;
	unitbadge_spritemaps_t m_unitbadge_sprites = {};

	sprite::InstancedSprite* m_fake_badge = nullptr;

	types::texture::Texture* m_badges_texture = nullptr;
	types::texture::Texture* GetBadgesTexture();

	std::unordered_map< uint8_t, types::texture::Texture* > m_healthbar_textures = {};
	std::unordered_map< uint8_t, sprite::Sprite > m_healthbar_sprites = {};

};

}
}
}
