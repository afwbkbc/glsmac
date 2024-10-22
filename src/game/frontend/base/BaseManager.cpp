#include "BaseManager.h"

#include <algorithm>

#include "Base.h"
#include "game/frontend/Game.h"
#include "game/frontend/Slot.h"
#include "game/frontend/unit/UnitManager.h"
#include "game/frontend/tile/TileManager.h"
#include "game/frontend/text/InstancedTextManager.h"
#include "game/frontend/text/InstancedText.h"
#include "game/frontend/faction/Faction.h"
#include "types/mesh/Rectangle.h"
#include "engine/Engine.h"
#include "loader/font/FontLoader.h"
#include "SlotBadges.h"

namespace game {
namespace frontend {
namespace base {

BaseManager::BaseManager( Game* game )
	: m_game( game )
	, m_ism( game->GetISM() )
	, m_name_font( game->GetITM()->GetInstancedFont( g_engine->GetFontLoader()->LoadFont( resource::TTF_ARIALN, 48 ) ) )
	, m_badge_font( game->GetITM()->GetInstancedFont( g_engine->GetFontLoader()->LoadFont( resource::TTF_ARIALNB, 48 ) ) ) {
	//
}

BaseManager::~BaseManager() {
	for ( const auto& it : m_bases ) {
		delete it.second;
	}
}

base::Base* BaseManager::GetBaseById( const size_t id ) const {
	ASSERT( m_bases.find( id ) != m_bases.end(), "base id not found" );
	return m_bases.at( id );
}

void BaseManager::SpawnBase(
	const size_t base_id,
	const size_t slot_index,
	const types::Vec2< size_t >& tile_coords,
	const types::Vec3& render_coords,
	const std::string& name
) {

	ASSERT( m_bases.find( base_id ) == m_bases.end(), "base id already exists" );

	auto* tile = m_game->GetTM()->GetTile( tile_coords );
	auto* slot = m_game->GetSlot( slot_index );
	auto* faction = slot->GetFaction();

	m_bases.insert(
		{
			base_id,
			new base::Base(
				this,
				base_id,
				name,
				slot,
				tile,
				slot_index == m_game->GetMySlotIndex(),
				render_coords,
				m_game->GetITM()->CreateInstancedText(
					name,
					m_name_font,
					faction->m_colors.text,
					faction->m_colors.text_shadow
				)
			)
		}
	);
	auto it = m_faction_base_ids.find( faction );
	if ( it == m_faction_base_ids.end() ) {
		it = m_faction_base_ids.insert(
			{
				faction,
				{}
			}
		).first;
	}
	ASSERT( it->second.find( base_id ) == it->second.end(), "faction base id already exists" );
	it->second.insert( base_id );

	m_game->RenderTile( tile, m_game->GetUM()->GetSelectedUnit() );

}

/* TODO void BaseManager::DespawnBase( const size_t base_id ) {
	const auto& it = m_units.find( unit_id );
	ASSERT( it != m_units.end(), "unit id not found" );

	auto* unit = it->second;

	m_units.erase( it );

	if ( unit->IsOwned() ) {
		RemoveSelectable( unit );
	}

	delete unit;

	m_game->RefreshSelectedTile( m_selected_unit );

}*/

SlotBadges* BaseManager::GetSlotBadges( const size_t slot_index ) const {
	ASSERT( m_slot_badges.find( slot_index ) != m_slot_badges.end(), "slot base badges for index " + std::to_string( slot_index ) + " not defined" );
	return m_slot_badges.at( slot_index );
}

void BaseManager::DefineSlotBadges( const size_t slot_index, const faction::Faction* faction ) {
	ASSERT( m_slot_badges.find( slot_index ) == m_slot_badges.end(), "slot base badges for index " + std::to_string( slot_index ) + " already defined" );
	m_slot_badges.insert(
		{
			slot_index,
			new SlotBadges( this, m_ism, faction )
		}
	);
}

void BaseManager::SelectBase( Base* base ) {
	m_game->OpenBasePopup( base );
}

Base* BaseManager::GetBaseBefore( Base* base ) const {
	const auto& ids_it = m_faction_base_ids.find( base->GetFaction() );
	ASSERT( ids_it != m_faction_base_ids.end(), "faction ids not found" );
	const auto& ids = ids_it->second;
	auto it = ids.find( base->GetId() );
	ASSERT( it != ids.end(), "base not in faction ids" );
	if ( it == ids.begin() ) {
		it = ids.end();
	}
	it--;
	ASSERT( m_bases.find( *it ) != m_bases.end(), "base id not found" );
	return m_bases.at( *it );
}

Base* BaseManager::GetBaseAfter( Base* base ) const {
	const auto& ids_it = m_faction_base_ids.find( base->GetFaction() );
	ASSERT( ids_it != m_faction_base_ids.end(), "faction ids not found" );
	const auto& ids = ids_it->second;
	auto it = ids.find( base->GetId() );
	ASSERT( it != ids.end(), "base not in faction ids" );
	it++;
	if ( it == ids.end() ) {
		it = ids.begin();
	}
	ASSERT( m_bases.find( *it ) != m_bases.end(), "base id not found" );
	return m_bases.at( *it );
}

text::InstancedFont* BaseManager::GetBadgeFont() const {
	return m_badge_font;
}

}
}
}
