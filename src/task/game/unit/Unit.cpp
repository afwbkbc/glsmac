#include "Unit.h"

#include "task/game/Game.h"
#include "task/game/tile/Tile.h"
#include "util/String.h"
#include "game/unit/Unit.h"
#include "task/game/Slot.h"
#include "UnitDef.h"
#include "BadgeDefs.h"
#include "SlotBadges.h"
#include "UnitManager.h"
#include "task/game/InstancedSprite.h"
#include "scene/actor/Instanced.h"
#include "types/mesh/Rectangle.h"
#include "scene/actor/Sprite.h"
#include "types/texture/Texture.h"

namespace task {
namespace game {
namespace unit {

Unit::Unit(
	UnitManager* um,
	BadgeDefs* badge_defs,
	const size_t id,
	UnitDef* def,
	Slot* slot,
	tile::Tile* tile,
	const types::Vec3& render_coords,
	const bool is_owned,
	const ::game::unit::movement_t movement,
	const ::game::unit::morale_t morale,
	const std::string& morale_string,
	const ::game::unit::health_t health
)
	: m_um( um )
	, m_badge_defs( badge_defs )
	, m_id( id )
	, m_def( def )
	, m_slot_badges( m_um->GetSlotBadges( slot->GetIndex() ) )
	, m_tile( tile )
	, m_render(
		{
			render_coords,
			false,
			0,
			{},
			{}
		}
	)
	, m_is_owned( is_owned )
	, m_movement( movement )
	, m_morale( morale )
	, m_morale_string( morale_string )
	, m_health( health ) {
	m_is_active = ShouldBeActive();
	m_render.badge.def = m_slot_badges->GetUnitBadgeSprite( m_morale, m_is_active );
	m_render.badge.healthbar.def = m_badge_defs->GetBadgeHealthbarSprite( m_health );
	m_render_data.unit = GetMeshTex( GetSprite()->instanced_sprite );
	m_render_data.badge = GetMeshTex( GetBadgeSprite()->instanced_sprite );
	m_render_data.healthbar = GetMeshTex( GetBadgeHealthbarSprite()->instanced_sprite );
	m_tile->AddUnit( this );
}

Unit::~Unit() {
	Hide();
	m_tile->RemoveUnit( this );
}

const size_t Unit::GetId() const {
	return m_id;
}

const bool Unit::IsOwned() const {
	return m_is_owned;
}

const bool Unit::IsActive() const {
	return m_is_active;
}

tile::Tile* Unit::GetTile() const {
	return m_tile;
}

const size_t Unit::GetSelectionWeight() const {
	size_t weight = 0;

	// active units have priority
	if ( m_is_active ) {
		weight += 200;
	}

	// non-artillery units have priority
	// TODO: use real unit properties
	if ( m_def->IsArtillery() ) {
		weight += 60;
	}

	// higher morale has priority
	weight += m_morale * 2;

	// health is also somewhat important
	weight += m_health * 8;

	return weight;
}

Sprite* Unit::GetSprite() const {
	return m_def->GetSprite( m_morale );
}

Sprite* Unit::GetBadgeSprite() const {
	return m_render.badge.def;
}

Sprite* Unit::GetBadgeHealthbarSprite() const {
	return m_render.badge.healthbar.def;
}

const std::string Unit::GetNameString() const {
	return m_def->GetNameString();
}

const std::string Unit::GetStatsString() const {
	return m_def->GetStatsString();
}

const std::string& Unit::GetMoraleString() const {
	return m_morale_string;
}

const std::string Unit::GetMovesString() const {
	if ( m_def->IsImmovable() ) {
		return "";
	}
	return "Moves: " + util::String::ApproximateFloat( m_movement );
}

void Unit::Iterate() {
	while ( m_render.badge.blink.timer.HasTicked() ) {
		if ( IsBadgeVisible() ) {
			HideBadge();
		}
		else {
			ShowBadge();
		}
	}
	while ( m_mover.HasTicked() ) {
		SetRenderCoords( m_mover.GetPosition() );
	}
}

void Unit::SetActiveOnTile() {
	m_tile->SetActiveUnit( this );
}

void Unit::Show() {
	if ( !m_render.is_rendered ) {
		const auto& c = m_render.coords;

		auto* sprite = m_def->GetSprite( m_morale );

		if ( !m_render.instance_id ) {
			m_render.instance_id = sprite->next_instance_id++;
		}
		sprite->instanced_sprite->actor->SetInstance(
			m_render.instance_id, {
				c.x,
				c.y,
				c.z
			}
		);

		if ( !m_render.badge.instance_id ) {
			m_render.badge.instance_id = m_render.badge.def->next_instance_id++;
		}
		if ( !m_render.badge.healthbar.instance_id ) {
			m_render.badge.healthbar.instance_id = m_render.badge.healthbar.def->next_instance_id++;
		}

		ShowBadge();

		m_render.is_rendered = true;
	}
}

void Unit::Hide() {
	if ( m_render.is_rendered ) {
		m_def->GetSprite( m_morale )->instanced_sprite->actor->RemoveInstance( m_render.instance_id );
		StopBadgeBlink( false );
		m_render.is_rendered = false;
	}
}

const bool Unit::IsBadgeVisible() const {
	return m_render.badge.def->instanced_sprite->actor->HasInstance( m_render.badge.instance_id );
}

void Unit::ShowBadge() {
	m_render.badge.def->instanced_sprite->actor->SetInstance( m_render.badge.instance_id, BadgeDefs::GetBadgeCoords( m_render.coords ) );
	m_render.badge.healthbar.def->instanced_sprite->actor->SetInstance( m_render.badge.healthbar.instance_id, BadgeDefs::GetBadgeHealthbarCoords( m_render.coords ) );
}

void Unit::HideBadge() {
	m_render.badge.def->instanced_sprite->actor->RemoveInstance( m_render.badge.instance_id );
	m_render.badge.healthbar.def->instanced_sprite->actor->RemoveInstance( m_render.badge.healthbar.instance_id );
}

void Unit::ShowFakeBadge( const uint8_t offset ) {
	if ( !m_render.fake_badge.instance_id || m_fake_badge_offset != offset ) {
		if ( m_render.fake_badge.instance_id ) {
			m_slot_badges->HideFakeBadge( !m_render.fake_badge.instance_id );
		}
		m_render.fake_badge.instance_id = m_slot_badges->ShowFakeBadge( m_render.coords, offset );
		m_fake_badge_offset = offset;
	}
}

void Unit::HideFakeBadge() {
	if ( m_render.fake_badge.instance_id ) {
		m_slot_badges->HideFakeBadge( m_render.fake_badge.instance_id );
		m_render.fake_badge.instance_id = 0;
		m_fake_badge_offset = 0;
	}
}

void Unit::StartBadgeBlink() {
	HideBadge();
	m_render.badge.blink.timer.SetInterval( m_badge_defs->GetBadgeBlinkInterval() );
}

void Unit::StopBadgeBlink( const bool is_badge_shown ) {
	m_render.badge.blink.timer.Stop();
	if ( is_badge_shown ) {
		ShowBadge();
	}
	else {
		HideBadge();
	}
}

void Unit::Refresh() {
	if ( m_need_refresh ) {
		m_need_refresh = false;
		m_is_active = ShouldBeActive();
		if ( !m_is_active ) {
			StopBadgeBlink( false );
		}
		const bool is_badge_visible = IsBadgeVisible();
		if ( is_badge_visible ) {
			HideBadge();
		}
		{
			auto* newdef = m_slot_badges->GetUnitBadgeSprite( m_morale, m_is_active );
			if ( m_render.badge.def != newdef ) {
				if ( m_render.badge.instance_id ) {
					m_render.badge.def->instanced_sprite->actor->RemoveInstance( m_render.badge.instance_id );
					m_render.badge.instance_id = 0;
				}
				m_render.badge.def = newdef;
			}
			if ( !m_render.badge.instance_id ) {
				m_render.badge.instance_id = m_render.badge.def->next_instance_id++;
			}
		}
		{
			auto* newdef = m_badge_defs->GetBadgeHealthbarSprite( m_health );
			if ( m_render.badge.healthbar.def != newdef ) {
				if ( m_render.badge.healthbar.instance_id ) {
					m_render.badge.healthbar.def->instanced_sprite->actor->RemoveInstance( m_render.badge.healthbar.instance_id );
					m_render.badge.healthbar.instance_id = 0;
				}
				m_render.badge.healthbar.def = newdef;
			}
			if ( !m_render.badge.healthbar.instance_id ) {
				m_render.badge.healthbar.instance_id = m_render.badge.healthbar.def->next_instance_id++;
			}
		}
		// TODO: cache/optimize
		m_render_data.unit = GetMeshTex( GetSprite()->instanced_sprite );
		m_render_data.badge = GetMeshTex( GetBadgeSprite()->instanced_sprite );
		m_render_data.healthbar = GetMeshTex( GetBadgeHealthbarSprite()->instanced_sprite );
		if ( is_badge_visible ) {
			ShowBadge();
		}
	}
}

void Unit::SetMovement( const ::game::unit::movement_t movement ) {
	if ( movement != m_movement ) {
		m_movement = movement;
		m_need_refresh = true;
	}
}

void Unit::SetHealth( const ::game::unit::health_t health ) {
	if ( health != m_health ) {
		m_health = health;
		m_need_refresh = true;
	}
}

const bool Unit::CanMove() const {
	return m_movement >= ::game::unit::Unit::MINIMUM_MOVEMENT_TO_KEEP;
}

void Unit::SetTile( tile::Tile* dst_tile ) {
	ASSERT_NOLOG( m_tile, "source tile not set" );
	ASSERT_NOLOG( dst_tile, "destination tile not set" );

	m_tile->RemoveUnit( this );

	m_tile = dst_tile;

	m_tile->AddUnit( this );

	UpdateFromTile();
}

void Unit::MoveToTile( tile::Tile* dst_tile ) {
	ASSERT_NOLOG( !m_mover.IsRunning(), "unit already moving" );
	ASSERT_NOLOG( m_tile != dst_tile, "can't move to same tile" );
	ASSERT_NOLOG( m_tile, "source tile not set" );
	ASSERT_NOLOG( dst_tile, "destination tile not set" );

	StopBadgeBlink( true );
	HideFakeBadge();
	Show();

	auto from = m_tile->GetRenderData().coords.InvertY();
	auto to = dst_tile->GetRenderData().coords.InvertY();
	m_mover.Scroll( from, m_um->GetCloserCoords( to, from ), MOVE_DURATION_MS );
}

void Unit::UpdateFromTile() {
	ASSERT_NOLOG( m_tile, "tile not set" );
	SetRenderCoords( m_tile->GetRenderData().coords.InvertY() );
}

const bool Unit::IsMoving() const {
	return m_mover.IsRunning();
}

const Unit::render_data_t& Unit::GetRenderData() const {
	return m_render_data;
}

const bool Unit::ShouldBeActive() const {
	return m_is_owned && CanMove();
}

Unit::meshtex_t Unit::GetMeshTex( const InstancedSprite* sprite ) {
	auto* texture = sprite->actor->GetSpriteActor()->GetTexture();
	NEWV( mesh, types::mesh::Rectangle );
	mesh->SetCoords(
		{
			0.0f,
			0.0f
		},
		{
			1.0f,
			1.0f
		},
		{
			sprite->xy.x,
			sprite->xy.y
		},
		{
			sprite->xy.x + sprite->wh.x,
			sprite->xy.y + sprite->wh.y
		},
		{
			texture->m_width,
			texture->m_height
		},
		0.8f
	);
	return {
		mesh,
		texture,
	};
}

void Unit::SetRenderCoords( const types::Vec3& coords ) {
	m_render.coords = coords;
	m_def->GetSprite( m_morale )->instanced_sprite->actor->UpdateInstance(
		m_render.instance_id, {
			coords.x,
			coords.y,
			coords.z
		}
	);
	m_render.badge.def->instanced_sprite->actor->UpdateInstance(
		m_render.badge.instance_id,
		BadgeDefs::GetBadgeCoords( m_render.coords )
	);
	m_render.badge.healthbar.def->instanced_sprite->actor->UpdateInstance(
		m_render.badge.healthbar.instance_id,
		BadgeDefs::GetBadgeHealthbarCoords( m_render.coords )
	);
	if ( m_render.fake_badge.instance_id ) {
		m_slot_badges->HideFakeBadge( m_render.fake_badge.instance_id );
		m_render.fake_badge.instance_id = m_slot_badges->ShowFakeBadge( m_render.coords, m_fake_badge_offset );
	}
	m_need_refresh = true;
}

}
}
}
