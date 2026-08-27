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
#include "game/backend/base/PopDef.h"
#include "PopDef.h"
#include "types/mesh/Rectangle.h"
#include "engine/Engine.h"
#include "loader/font/FontLoader.h"
#include "SlotBadges.h"
#include "game/backend/Game.h"
#include "game/backend/base/BaseManager.h"
#include "game/backend/base/Base.h"
#include "game/backend/map/Map.h"

namespace game {
namespace frontend {
namespace base {

BaseManager::BaseManager( Game* game )
	: m_game( game )
	, m_ism( game->GetISM() )
	, m_name_font( game->GetITM()->GetInstancedFont( g_engine->GetFontLoader()->LoadFont( ::resource::TTF_ARIALN, 48 ) ) )
	, m_badge_font( game->GetITM()->GetInstancedFont( g_engine->GetFontLoader()->LoadFont( ::resource::TTF_ARIALNB, 48 ) ) ) {
	//
}

BaseManager::~BaseManager() {
	for ( const auto& it : m_bases ) {
		delete it.second;
	}
	for ( const auto& it : m_popdefs ) {
		delete it.second;
	}
	for ( const auto& it : m_slot_badges ) {
		delete it.second;
	}
}

base::Base* BaseManager::GetBaseById( const size_t id ) const {
	const auto& it = m_bases.find( id );
	return it != m_bases.end()
		? it->second
		: nullptr;
}

void BaseManager::DefinePop( const backend::base::PopDef* def ) {
	ASSERT( m_popdefs.find( def->m_id ) == m_popdefs.end(), "popdef already defined: " + def->m_id );
	m_popdefs_order.push_back( def->m_id );
	m_popdefs.insert(
		{
			def->m_id,
			new PopDef(
				def->m_name,
				def->m_renders_human,
				def->m_renders_progenitor
			)
		}
	);
}

void BaseManager::UndefinePop( const std::string& id ) {
	const auto& it = m_popdefs.find( id );
	ASSERT( it != m_popdefs.end(), "popdef does not exist: " + id );
	delete it->second;
	m_popdefs.erase( it );
	for ( auto it = m_popdefs_order.begin() ; it != m_popdefs_order.end() ; it++ ) {
		if ( ( *it ) == id ) {
			m_popdefs_order.erase( it );
			break;
		}
	}
}

const std::vector< std::string >& BaseManager::GetPopDefOrder() const {
	return m_popdefs_order;
}

PopDef* BaseManager::GetPopDef( const std::string& id ) const {
	ASSERT( m_popdefs.find( id ) != m_popdefs.end(), "popdef not defined: " + id );
	return m_popdefs.at( id );
}

void BaseManager::SpawnBase(
	const size_t base_id,
	const size_t slot_index,
	faction::Faction* faction,
	const types::Vec2< size_t >& tile_coords,
	const types::Vec3& render_coords,
	const std::string& name
) {

	ASSERT( m_bases.find( base_id ) == m_bases.end(), "base id already exists" );

	auto* tile = m_game->GetTM()->GetTile( tile_coords );
	auto* slot = m_game->GetSlot( slot_index );
	auto* owner_faction = slot->GetFaction();

	auto* base = new base::Base(
		this,
		base_id,
		name,
		slot,
		faction,
		tile,
		slot_index == m_game->GetMySlotIndex(),
		render_coords,
		CreateNameText( name, owner_faction )
	);

	m_bases.insert(
		{
			base_id,
			base
		}
	);

	RefreshBase( base );
}

void BaseManager::UpdateBase(
	Base* base,
	const size_t slot_index,
	faction::Faction* faction,
	const std::string& name
) {
	ASSERT( base, "base is null" );
	auto* const owner = m_game->GetSlot( slot_index );
	const bool is_owned = slot_index == m_game->GetMySlotIndex();
	base->SetState( name, owner, faction, is_owned );
}

void BaseManager::DespawnBase( const size_t base_id ) {
	const auto& it = m_bases.find( base_id );
	ASSERT( it != m_bases.end(), "base id not found" );

	auto* base = it->second;

	m_bases.erase( it );
	m_game->UpdateRelatedWidgets( ui::WT_BASE_PREVIEW, base_id, nullptr );

	delete base;

	m_game->RefreshSelectedTile();
}

void BaseManager::RefreshBase( Base* base ) {
	m_game->RenderTile( base->GetTile(), m_game->GetUM()->GetSelectedUnit() );
	m_game->UpdateRelatedWidgets( ui::WT_BASE_PREVIEW, base->GetId(), base );
}
SlotBadges* BaseManager::GetSlotBadges( const size_t slot_index ) const {
	ASSERT( m_slot_badges.find( slot_index ) != m_slot_badges.end(), "slot base badges for index " + std::to_string( slot_index ) + " not defined" );
	return m_slot_badges.at( slot_index );
}

void BaseManager::DefineSlotBadges( const size_t slot_index, const faction::Faction* faction ) {
	ASSERT( m_slot_badges.find( slot_index ) == m_slot_badges.end(), "slot base badges for index " + std::to_string( slot_index ) + " already defined" );
	m_slot_badges.insert(
		{
			slot_index,
			new SlotBadges( this, m_ism, slot_index, faction )
		}
	);
}

void BaseManager::SelectBase( Base* base ) {
	m_game->UpdateTilePreview( base->GetTile() );
	auto* game = m_game->GetGame();
	auto* const b = game->GetBM()->GetBase( base->GetId() );
	m_game->Trigger(
		game->GetMap(), "base_preview", ARGS_F( &b ) {
			{
				"base",
				b->Wrap( GSE_CALL )
			},
		}; }
	);
	m_game->Trigger(
		game, "base_select", ARGS_F( &b ) {
			{
				"base",
				b->Wrap( GSE_CALL )
			},
		}; }
	);
}

Base* BaseManager::GetBaseBefore( Base* base ) const {
	ASSERT( base, "base is null" );
	const auto* const faction = base->GetOwner()->GetFaction();
	Base* previous = nullptr;
	Base* last = nullptr;
	bool found = false;
	for ( const auto& it : m_bases ) {
		auto* const candidate = it.second;
		if ( candidate->GetOwner()->GetFaction() != faction ) {
			continue;
		}
		found = found || candidate == base;
		if ( !last || candidate->GetId() > last->GetId() ) {
			last = candidate;
		}
		if (
			candidate->GetId() < base->GetId()
			&& ( !previous || candidate->GetId() > previous->GetId() )
		) {
			previous = candidate;
		}
	}
	ASSERT( found, "base not found for owner" );
	ASSERT( last, "owner has no bases" );
	return previous ? previous : last;
}

Base* BaseManager::GetBaseAfter( Base* base ) const {
	ASSERT( base, "base is null" );
	const auto* const faction = base->GetOwner()->GetFaction();
	Base* next = nullptr;
	Base* first = nullptr;
	bool found = false;
	for ( const auto& it : m_bases ) {
		auto* const candidate = it.second;
		if ( candidate->GetOwner()->GetFaction() != faction ) {
			continue;
		}
		found = found || candidate == base;
		if ( !first || candidate->GetId() < first->GetId() ) {
			first = candidate;
		}
		if (
			candidate->GetId() > base->GetId()
			&& ( !next || candidate->GetId() < next->GetId() )
		) {
			next = candidate;
		}
	}
	ASSERT( found, "base not found for owner" );
	ASSERT( first, "owner has no bases" );
	return next ? next : first;
}

text::InstancedFont* BaseManager::GetBadgeFont() const {
	return m_badge_font;
}

text::InstancedText* BaseManager::CreateNameText( const std::string& name, const faction::Faction* faction ) const {
	ASSERT( faction, "base owner faction is null" );
	return m_game->GetITM()->CreateInstancedText(
		name,
		m_name_font,
		faction->m_colors.text,
		faction->m_colors.text_shadow
	);
}

}
}
}
