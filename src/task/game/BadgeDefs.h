#pragma once

#include <unordered_map>

#include "game/unit/Unit.h"
#include "types/Texture.h"

#include "InstancedSpriteManager.h"
#include "InstancedSprite.h"
#include "Sprite.h"

namespace task {
namespace game {

class BadgeDefs {
public:

	static const Vec3 GetBadgeCoords( const Vec3& unit_coords );
	static const Vec3 GetBadgeHealthbarCoords( const Vec3& unit_coords );

	BadgeDefs( InstancedSpriteManager* ism );
	~BadgeDefs();

	typedef uint8_t badge_type_t;
	static const badge_type_t BT_NORMAL{ 0 << 0 };
	static const badge_type_t BT_GREYEDOUT{ 1 << 0 };
	static const badge_type_t BT_DEFAULT{ 1 << 1 };
	static const badge_type_t BT_PROGENITOR{ 0 << 1 };

	InstancedSprite* GetBadgeSprite( const badge_type_t badge_type, const ::game::unit::Unit::morale_t morale ) const;
	InstancedSprite* GetFakeBadgeSprite() const;
	Sprite* GetBadgeHealthbarSprite( const float health );
	const size_t GetBadgeBlinkInterval() const;

	const Vec3 GetFakeBadgeCoords( const Vec3& coords, const uint8_t offset ) const;

private:

	static const struct consts_t {
		const Vec2< float > scale = {
			0.25f,
			0.5f
		};
		const Vec2< float > offset = {
			-0.25f,
			-0.5
		};
		const struct {
			const uint8_t resolution = 25;
			const Vec2< float > scale = {
				0.04f,
				0.36f
			};
			const Vec2< float > offset = {
				-0.292f,
				-0.476f
			};
		} healthbars;
		struct {
			const uint8_t max_per_tile = 10;
			const Vec2< float > scale = {
				0.0326f,
				0.283f
			};
			const Vec2< float > offset = {
				-0.11f,
				-0.54f
			};
			const float step_x = 0.032f;
		} fake_badges;
		struct {
			const size_t interval_ms = 128;
		} blink;
	} s_consts;

	InstancedSpriteManager* const m_ism = nullptr;

	typedef std::unordered_map< ::game::unit::Unit::morale_t, InstancedSprite* > unitbadge_spritemap_t;
	typedef std::unordered_map< badge_type_t, unitbadge_spritemap_t > unitbadge_spritemaps_t;
	unitbadge_spritemaps_t m_unitbadge_sprites = {};

	InstancedSprite* m_fake_badge = nullptr;

	std::vector< types::Texture* > m_healthbar_textures = {};
	std::vector< Sprite > m_healthbar_sprites = {};

};

}
}
