#include "SlotBadges.h"

#include "BaseManager.h"
#include "game/frontend/sprite/InstancedSpriteManager.h"
#include "game/frontend/sprite/InstancedSprite.h"
#include "game/frontend/text/InstancedFont.h"
#include "game/frontend/faction/Faction.h"
#include "game/backend/map/Consts.h"
#include "scene/actor/Instanced.h"
#include "types/texture/Texture.h"

namespace game {
namespace frontend {
namespace base {

const SlotBadges::consts_t SlotBadges::s_consts = {};

const types::Vec3 SlotBadges::GetBadgeCoords( const types::Vec3& unit_coords ) {
	return {
		unit_coords.x + backend::map::s_consts.tile.scale.x * s_consts.offset.x,
		unit_coords.y - backend::map::s_consts.tile.scale.y * s_consts.offset.y * backend::map::s_consts.sprite.y_scale,
		unit_coords.z
	};
}

SlotBadges::SlotBadges(
	BaseManager* bm,
	sprite::InstancedSpriteManager* ism,
	const size_t slot_index,
	const faction::Faction* faction
)
	: m_bm( bm )
	, m_ism( ism )
	, m_slot_index( slot_index )
	, m_faction( faction ) {
	//
}

SlotBadges::~SlotBadges() {
	for ( const auto& it : m_per_population_sprites ) {
		if ( it.second.unguarded ) {
			DELETE( it.second.unguarded );
		}
		if ( it.second.guarded ) {
			DELETE( it.second.guarded );
		}
	}
	for ( const auto& spritemaps : m_badge_sprites ) {
		for ( const auto& it : spritemaps.second ) {
			m_ism->RemoveInstancedSpriteByKey( it.second.sprite->key );
			DELETE( it.second.texture );
		}
	}
}

sprite::Sprite* SlotBadges::GetBaseBadgeSprite( const size_t population, const bool is_guarded ) {
	auto it = m_per_population_sprites.find( population );
	if ( it == m_per_population_sprites.end() ) {
		it = m_per_population_sprites.insert(
			{
				population,
				{
					nullptr,
					nullptr
				}
			}
		).first;
	}
	sprite::Sprite** sprite_ptr = is_guarded
		? &it->second.guarded
		: &it->second.unguarded;
	if ( !*sprite_ptr ) {
		NEW( *sprite_ptr, sprite::Sprite, {
			GetBadgeSprite(
				population,
				is_guarded
			),
			1
		} );
	}
	return *sprite_ptr;
}

sprite::InstancedSprite* SlotBadges::GetBadgeSprite( const size_t population, const bool is_guarded ) {

	auto it1 = m_badge_sprites.find( is_guarded );
	if ( it1 == m_badge_sprites.end() ) {
		it1 = m_badge_sprites.insert(
			{
				is_guarded,
				{}
			}
		).first;
	}
	auto it2 = it1->second.find( population );
	if ( it2 == it1->second.end() ) {

		const auto& c = m_faction->m_colors;
		const auto transparent = types::Color::FromRGBA( 0, 0, 0, 0 );
		const auto black = types::Color::FromRGB( 0, 0, 0 );

		types::Color background, border, text, text_shadow;
		if ( is_guarded ) {
			background = c.text;
			border = black;
			text = black;
			text_shadow = transparent;
		}
		else {
			background = transparent;
			border = black;
			text = c.text;
			text_shadow = black;
		}

		auto* texture = m_bm->GetBadgeFont()->GetTextTexture(
			std::to_string( population ),
			background,
			text,
			text_shadow,
			3
		);

		// add border
		const auto w = texture->GetWidth() - 1;
		const auto h = texture->GetHeight() - 1;
		const auto b = 2;
		texture->Fill( 0, 0, w, b, border );
		texture->Fill( 0, h - b, w, h, border );
		texture->Fill( 0, 0, b, h, border );
		texture->Fill( w - b, 0, w, h, border );

		const uint32_t tw = texture->GetWidth();
		const uint32_t th = texture->GetHeight();

		it2 = it1->second.insert(
			{
				population,
				{
					texture,
					m_ism->GetInstancedSprite(
						"BaseBadge_" + std::to_string( m_slot_index ) + "_" + std::to_string( population ) + ( is_guarded
							? "_guarded"
							: "_unguarded"
						),
						texture,
						{
							0,
							0,
						},
						{
							tw,
							th,
						},
						{
							(uint32_t)std::round( tw / 2 ),
							(uint32_t)std::round( th / 2 ),
						},
						{
							backend::map::s_consts.tile.scale.x * s_consts.scale.x * tw,
							backend::map::s_consts.tile.scale.y * s_consts.scale.y * backend::map::s_consts.sprite.y_scale * th
						},
						ZL_TERRAIN_TEXT,
						0.008f
					)
				}
			}
		).first;
	}
	return it2->second.sprite;
}

}
}
}
