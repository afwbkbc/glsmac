#include "Slot.h"

#include "BadgeDefs.h"
#include "types/texture/Types.h"
#include "InstancedSpriteManager.h"
#include "InstancedSprite.h"
#include "scene/actor/Instanced.h"

namespace task {
namespace game {

Slot::Slot(
	BadgeDefs* badge_defs,
	InstancedSpriteManager* ism,
	const size_t slot_index, const types::Color& color, const slot_flags_t flags
)
	: m_badge_defs( badge_defs )
	, m_ism( ism )
	, m_badges_key( "Badge_" + std::to_string( slot_index ) )
	, m_slot_index( slot_index )
	, m_color( color )
	, m_flags( flags ) {

	const auto& c = m_color.value;
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

	const BadgeDefs::badge_type_t badge_type = ( m_flags & SF_PROGENITOR )
		? BadgeDefs::BT_PROGENITOR
		: BadgeDefs::BT_DEFAULT;
	for ( auto morale = ::game::unit::MORALE_MIN ; morale <= ::game::unit::MORALE_MAX ; morale++ ) {
		auto& badgedef = badges[ morale ];
		badgedef.normal.instanced_sprite = m_ism->GetRepaintedInstancedSprite(
			m_badges_key + "_" + std::to_string( morale ) + "_NORMAL",
			m_badge_defs->GetBadgeSprite( badge_type | BadgeDefs::BT_NORMAL, morale ),
			rules
		);
		badgedef.normal.next_instance_id = 1;
		badgedef.greyedout.instanced_sprite = m_ism->GetRepaintedInstancedSprite(
			m_badges_key + "_" + std::to_string( morale ) + "_GRAYEDOUT",
			m_badge_defs->GetBadgeSprite( badge_type | BadgeDefs::BT_GREYEDOUT, morale ),
			rules
		);
		badgedef.greyedout.next_instance_id = 1;
	}
	fake_badge.instanced_sprite = m_ism->GetRepaintedInstancedSprite(
		m_badges_key + "_FAKE",
		m_badge_defs->GetFakeBadgeSprite(),
		fake_badge_rules
	);
	fake_badge.next_instance_id = 1;
}

Slot::~Slot() {
	for ( auto morale = ::game::unit::MORALE_MIN ; morale <= ::game::unit::MORALE_MAX ; morale++ ) {
		m_ism->RemoveRepaintedInstancedSpriteByKey( m_badges_key + "_" + std::to_string( morale ) + "_NORMAL" );
		m_ism->RemoveRepaintedInstancedSpriteByKey( m_badges_key + "_" + std::to_string( morale ) + "_GRAYEDOUT" );
	}
	m_ism->RemoveRepaintedInstancedSpriteByKey( m_badges_key + "_FAKE" );
}

Sprite* Slot::GetUnitBadgeSprite( const ::game::unit::morale_t morale, const bool is_active ) {
	return is_active
		? &badges.at( morale ).normal
		: &badges.at( morale ).greyedout;
}

const size_t Slot::ShowFakeBadge( const types::Vec3& coords, const uint8_t offset ) {
	const size_t instance_id = fake_badge.next_instance_id++;
	fake_badge.instanced_sprite->actor->SetInstance( instance_id, m_badge_defs->GetFakeBadgeCoords( coords, offset ) );
	return instance_id;
}

void Slot::HideFakeBadge( const size_t instance_id ) {
	fake_badge.instanced_sprite->actor->RemoveInstance( instance_id );
}

}
}
