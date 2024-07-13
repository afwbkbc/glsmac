#include "SlotBadges.h"

#include "BadgeDefs.h"
#include "game/frontend/sprite/InstancedSpriteManager.h"
#include "game/frontend/sprite/InstancedSprite.h"
#include "game/frontend/faction/Faction.h"
#include "scene/actor/Instanced.h"

namespace game {
namespace frontend {
namespace unit {

SlotBadges::SlotBadges(
	BadgeDefs* badge_defs,
	sprite::InstancedSpriteManager* ism,
	const size_t slot_index,
	const faction::Faction* faction
)
	: m_badge_defs( badge_defs )
	, m_ism( ism )
	, m_badges_key( "Badge_" + std::to_string( slot_index ) )
	, m_faction( faction ) {

	const auto& c = faction->m_colors.border.value;
	m_repaint_rules = {
		{ // active
			types::Color::RGB( 252, 0, 0 ),
			types::Color( c.red, c.green, c.blue ).GetRGBA()
		},
		{ // greyed out
			types::Color::RGB( 125, 0, 0 ),
			types::Color( ( 0.5f + c.red / 2 ) / 2, ( 0.5f + c.green / 2 ) / 2, ( 0.5f + c.blue / 2 ) / 2 ).GetRGBA()
		}
	};

	// fake badges should be slightly greyer
	m_fake_badge_repaint_rules = m_repaint_rules;
	for ( auto& rule : m_fake_badge_repaint_rules ) {
		rule.second = types::Color( ( 0.6f + c.red / 2.5 ) / 2, ( 0.6f + c.green / 2.5 ) / 2, ( 0.6f + c.blue / 2.5 ) / 2 ).GetRGBA();
	}

}

SlotBadges::~SlotBadges() {
	for ( const auto& it : m_per_morale_sprites ) {
		if ( it.second.normal ) {
			m_ism->RemoveRepaintedInstancedSpriteByKey( it.second.normal->instanced_sprite->key );
			DELETE( it.second.normal );
		}
		if ( it.second.greyedout ) {
			m_ism->RemoveRepaintedInstancedSpriteByKey( it.second.greyedout->instanced_sprite->key );
			DELETE( it.second.greyedout );
		}
	}
	if ( m_fake_badge ) {
		m_ism->RemoveRepaintedInstancedSpriteByKey( m_fake_badge->instanced_sprite->key );
		DELETE( m_fake_badge );
	}
}

sprite::Sprite* SlotBadges::GetUnitBadgeSprite( const ::game::backend::unit::morale_t morale, const bool is_active ) {
	auto it = m_per_morale_sprites.find( morale );
	if ( it == m_per_morale_sprites.end() ) {
		it = m_per_morale_sprites.insert(
			{
				morale,
				{
					nullptr,
					nullptr
				}
			}
		).first;
	}
	sprite::Sprite** sprite_ptr = is_active
		? &it->second.normal
		: &it->second.greyedout;
	if ( !*sprite_ptr ) {
		NEW( *sprite_ptr, sprite::Sprite, {
			m_ism->GetRepaintedInstancedSprite(
				m_badges_key + "_" + std::to_string( morale ) + ( is_active
					? "_NORMAL"
					: "_GREYEDOUT"
				),
				m_badge_defs->GetBadgeSprite(
					( m_faction->m_is_progenitor
						? BadgeDefs::BT_PROGENITOR
						: BadgeDefs::BT_DEFAULT
					) | ( is_active
						? BadgeDefs::BT_NORMAL
						: BadgeDefs::BT_GREYEDOUT
					), morale
				),
				m_repaint_rules
			),
			1
		} );
	}
	return *sprite_ptr;
}

const size_t SlotBadges::ShowFakeBadge( const types::Vec3& coords, const uint8_t offset ) {
	auto* fake_badge = GetFakeBadge();
	const size_t instance_id = fake_badge->next_instance_id++;
	fake_badge->instanced_sprite->actor->SetInstance( instance_id, m_badge_defs->GetFakeBadgeCoords( coords, offset ) );
	return instance_id;
}

void SlotBadges::HideFakeBadge( const size_t instance_id ) {
	GetFakeBadge()->instanced_sprite->actor->RemoveInstance( instance_id );
}

sprite::Sprite* SlotBadges::GetFakeBadge() {
	if ( !m_fake_badge ) {
		NEW( m_fake_badge, sprite::Sprite, {
			m_ism->GetRepaintedInstancedSprite(
				m_badges_key + "_FAKE",
				m_badge_defs->GetFakeBadgeSprite(),
				m_fake_badge_repaint_rules
			),
			1
		} );
	}
	return m_fake_badge;
}

}
}
}
