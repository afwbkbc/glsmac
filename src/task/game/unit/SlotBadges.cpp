#include "SlotBadges.h"

#include "BadgeDefs.h"
#include "task/game/InstancedSpriteManager.h"
#include "task/game/InstancedSprite.h"
#include "task/game/faction/Faction.h"
#include "scene/actor/Instanced.h"

namespace task {
namespace game {
namespace unit {

SlotBadges::SlotBadges(
	BadgeDefs* badge_defs,
	InstancedSpriteManager* ism,
	const size_t slot_index,
	const faction::Faction* faction
)
	: m_badge_defs( badge_defs )
	, m_ism( ism )
	, m_badges_key( "Badge_" + std::to_string( slot_index ) ) {

	const auto& c = faction->m_border_color.value;
	const types::texture::repaint_rules_t& rules = {
		{ // active
			types::Color::RGB( 252, 0, 0 ),
			types::Color( c.red, c.green, c.blue ).GetRGBA()
		},
		{ // greyed out
			types::Color::RGB( 125, 0, 0 ),
			types::Color( ( 0.5f + c.red / 2 ) / 2, ( 0.5f + c.green / 2 ) / 2, ( 0.5f + c.blue / 2 ) / 2 ).GetRGBA()
		}
	};

	// fake badges should be slightly grayer
	auto fake_badge_rules = rules;
	for ( auto& rule : fake_badge_rules ) {
		rule.second = types::Color( ( 0.6f + c.red / 2.5 ) / 2, ( 0.6f + c.green / 2.5 ) / 2, ( 0.6f + c.blue / 2.5 ) / 2 ).GetRGBA();
	}

	const BadgeDefs::badge_type_t badge_type = faction->m_is_progenitor
		? BadgeDefs::BT_PROGENITOR
		: BadgeDefs::BT_DEFAULT;
	for ( auto morale = ::game::unit::MORALE_MIN ; morale <= ::game::unit::MORALE_MAX ; morale++ ) {
		auto& badgedef = m_per_morale_sprites[ morale ];
		NEW( badgedef.normal, Sprite, {
			m_ism->GetRepaintedInstancedSprite(
				m_badges_key + "_" + std::to_string( morale ) + "_NORMAL",
				m_badge_defs->GetBadgeSprite( badge_type | BadgeDefs::BT_NORMAL, morale ),
				rules
			),
			1
		} );
		NEW( badgedef.greyedout, Sprite, {
			m_ism->GetRepaintedInstancedSprite(
				m_badges_key + "_" + std::to_string( morale ) + "_GRAYEDOUT",
				m_badge_defs->GetBadgeSprite( badge_type | BadgeDefs::BT_GREYEDOUT, morale ),
				rules
			),
			1
		} );
	}
	NEW( m_fake_badge, Sprite, {
		m_ism->GetRepaintedInstancedSprite(
			m_badges_key + "_FAKE",
			m_badge_defs->GetFakeBadgeSprite(),
			fake_badge_rules
		),
		1
	} );
}

SlotBadges::~SlotBadges() {
	for ( auto morale = ::game::unit::MORALE_MIN ; morale <= ::game::unit::MORALE_MAX ; morale++ ) {
		m_ism->RemoveRepaintedInstancedSpriteByKey( m_badges_key + "_" + std::to_string( morale ) + "_NORMAL" );
		m_ism->RemoveRepaintedInstancedSpriteByKey( m_badges_key + "_" + std::to_string( morale ) + "_GRAYEDOUT" );
	}
	m_ism->RemoveRepaintedInstancedSpriteByKey( m_badges_key + "_FAKE" );
	for ( auto morale = ::game::unit::MORALE_MIN ; morale <= ::game::unit::MORALE_MAX ; morale++ ) {
		auto& badgedef = m_per_morale_sprites[ morale ];
		DELETE( badgedef.normal );
		DELETE( badgedef.greyedout );
	}
	DELETE( m_fake_badge );
}

Sprite* SlotBadges::GetUnitBadgeSprite( const ::game::unit::morale_t morale, const bool is_active ) {
	return is_active
		? m_per_morale_sprites.at( morale ).normal
		: m_per_morale_sprites.at( morale ).greyedout;
}

const size_t SlotBadges::ShowFakeBadge( const types::Vec3& coords, const uint8_t offset ) {
	const size_t instance_id = m_fake_badge->next_instance_id++;
	m_fake_badge->instanced_sprite->actor->SetInstance( instance_id, m_badge_defs->GetFakeBadgeCoords( coords, offset ) );
	return instance_id;
}

void SlotBadges::HideFakeBadge( const size_t instance_id ) {
	m_fake_badge->instanced_sprite->actor->RemoveInstance( instance_id );
}

}
}
}
