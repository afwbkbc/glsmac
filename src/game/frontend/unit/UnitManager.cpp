#include "UnitManager.h"

#include <algorithm>

#include "Unit.h"
#include "UnitDef.h"
#include "SlotBadges.h"
#include "game/backend/unit/Def.h"
#include "game/frontend/Game.h"
#include "game/frontend/tile/TileManager.h"
#include "BadgeDefs.h"
#include "types/mesh/Rectangle.h"

namespace game {
namespace frontend {
namespace unit {

UnitManager::UnitManager( Game* game )
	: m_game( game )
	, m_ism( game->GetISM() ) {
	NEW( m_badge_defs, BadgeDefs, m_ism );
}

UnitManager::~UnitManager() {
	for ( const auto& it : m_units ) {
		delete it.second;
	}
	for ( const auto& it : m_unitdefs ) {
		delete it.second;
	}
	for ( const auto& it : m_slot_badges ) {
		delete it.second;
	}
	DELETE( m_badge_defs );
}

void UnitManager::Iterate() {
	if ( m_selected_unit ) {
		m_selected_unit->Iterate();
	}
	auto it = m_moving_units.begin();
	while ( it != m_moving_units.end() ) {
		const auto& unit = it->first;
		if ( unit != m_selected_unit ) { // already iterated
			unit->Iterate();
		}
		if ( !unit->IsMoving() ) {
			const auto& tile = it->second.tile;
			if ( unit == m_selected_unit ) {
				m_game->SetSelectedTile( tile );
			}
			unit->SetTile( tile );
			m_game->RefreshSelectedTileIf( tile, m_selected_unit );
			m_game->SendAnimationFinished( it->second.animation_id );
			it = m_moving_units.erase( it );
		}
		else {
			it++;
		}
	}
}

Unit* UnitManager::GetUnitById( const size_t id ) const {
	ASSERT( m_units.find( id ) != m_units.end(), "unit id not found" );
	return m_units.at( id );
}

void UnitManager::DefineUnit( const backend::unit::Def* def ) {
	ASSERT( m_unitdefs.find( def->m_id ) == m_unitdefs.end(), "unit def already exists" );

	Log( "Defining unit: " + def->m_id );

	m_unitdefs.insert(
		{
			def->m_id,
			new UnitDef(
				m_ism,
				def
			)
		}
	);
}

void UnitManager::UndefineUnit( const std::string& id ) {
	ASSERT( m_unitdefs.find( id ) != m_unitdefs.end(), "unit def not found" );

	Log( "Undefining unit: " + id );

	m_unitdefs.erase( id );
}

void UnitManager::SpawnUnit(
	const size_t unit_id,
	const std::string& unitdef_id,
	const size_t slot_index,
	const types::Vec2< size_t >& tile_coords,
	const types::Vec3& render_coords,
	const backend::unit::movement_t movement,
	const backend::unit::morale_t morale,
	const std::string& morale_string,
	const backend::unit::health_t health
) {

	ASSERT( m_unitdefs.find( unitdef_id ) != m_unitdefs.end(), "unitdef not found" );

	if ( m_units.find( unit_id ) != m_units.end() ) {
		// maybe unit was spawned, then despawned and then despawn rolled back
		return;
	}

	auto* unitdef = m_unitdefs.at( unitdef_id );
	auto* slot = m_game->GetSlot( slot_index );
	auto* tile = m_game->GetTM()->GetTile( tile_coords );

	auto* unit = m_units.insert(
		{
			unit_id,
			new Unit(
				this,
				m_badge_defs,
				unit_id,
				unitdef,
				slot,
				tile,
				{
					render_coords.x,
					render_coords.y,
					render_coords.z
				},
				slot_index == m_game->GetMySlotIndex(),
				movement,
				morale,
				morale_string,
				health
			)
		}
	).first->second;

	types::mesh::Rectangle* mesh = nullptr;
	types::texture::Texture* texture = nullptr;

	m_game->RenderTile( tile, m_selected_unit );

	if ( unit->IsActive() ) {
		const bool was_selectables_empty = m_selectable_units.empty();
		AddSelectable( unit );
		if ( was_selectables_empty ) {
			SelectNextUnitMaybe();
		}
	}
}

void UnitManager::DespawnUnit( const size_t unit_id ) {
	const auto& it = m_units.find( unit_id );
	ASSERT( it != m_units.end(), "unit id not found" );

	auto* unit = it->second;

	m_units.erase( it );

	if ( unit->IsOwned() ) {
		RemoveSelectable( unit );
	}

	ASSERT( m_selected_unit != unit, "unit still selected" );

	delete unit;

	m_game->RefreshSelectedTile( m_selected_unit );

}

void UnitManager::RefreshUnit( Unit* unit ) {
	const auto was_active = unit->IsActive();
	unit->Refresh();
	UpdateSelectable( unit );
	m_game->RenderTile( unit->GetTile(), m_selected_unit );
	if ( m_selected_unit == unit && was_active ) {
		if ( !unit->IsActive() ) {
			SelectNextUnitOrSwitchToTileSelection();
		}
		else {
			unit->StartBadgeBlink();
		}
	}
}

void UnitManager::MoveUnit( Unit* unit, tile::Tile* dst_tile, const size_t animation_id ) {
	auto* src_tile = unit->GetTile();
	const auto& it = m_moving_units.find( unit );
	if ( it != m_moving_units.end() ) {
		const auto& tile = it->second.tile;
		if ( unit == m_selected_unit ) {
			m_game->SetSelectedTile( tile );
		}
		unit->SetTile( tile );
		m_game->RefreshSelectedTileIf( tile, m_selected_unit );
		m_game->SendAnimationFinished( it->second.animation_id );
		m_moving_units.erase( it );
	}
	m_moving_units.insert(
		{
			unit,
			{
				dst_tile,
				animation_id
			}
		}
	);
	src_tile->RemoveUnit( unit );
	m_game->RefreshSelectedTileIf( src_tile, m_selected_unit );
	m_game->SetSelectedTile( dst_tile );
	unit->MoveToTile( dst_tile );
}

void UnitManager::MoveUnit_deprecated( Unit* unit, tile::Tile* dst_tile, const types::Vec3& dst_render_coords ) {

	auto* src_tile = unit->GetTile();

	unit->SetTile( dst_tile );

	// TODO: animation

	if ( m_selected_unit == unit ) {
		m_game->SetSelectedTile( m_selected_unit->GetTile() );
	}

	// update old tile
	m_game->RenderTile( src_tile, m_selected_unit );

	// update unit
	RefreshUnit( unit );

	// update new tile
	m_game->RenderTile( dst_tile, m_selected_unit );

	if ( m_selected_unit == unit ) {
		m_game->ScrollToSelectedTile( false );
	}

}

Unit* UnitManager::GetSelectedUnit() const {
	return m_selected_unit;
}

void UnitManager::SelectUnit( Unit* unit, const bool actually_select_unit ) {
	Log( "Selecting unit " + std::to_string( unit->GetId() ) );
	//m_tile_at_query_purpose = backend::TQP_UNIT_SELECT; // ???
	m_game->DeselectTileOrUnit();
	if ( m_selected_unit != unit ) {

		unit->SetActiveOnTile();

		m_selected_unit = unit;
		m_selected_unit->Show();

		m_game->SetSelectedTile( m_selected_unit->GetTile() );
		m_game->RefreshSelectedTile( m_selected_unit );
	}
	if ( actually_select_unit && m_selected_unit->IsActive() && m_game->IsTurnActive() ) {
		auto* most_important_unit = m_game->GetSelectedTileMostImportantUnit();
		if ( most_important_unit && m_selected_unit != most_important_unit ) {
			most_important_unit->Hide();
		}
		m_selected_unit->StartBadgeBlink();
		Log( "Selected unit " + std::to_string( m_selected_unit->GetId() ) );
	}
}

void UnitManager::DeselectUnit() {
	m_previously_deselected_unit = m_selected_unit;
	if ( m_selected_unit ) {
		auto* most_important_unit = m_game->GetSelectedTileMostImportantUnit();
		if ( !most_important_unit || m_selected_unit == most_important_unit ) {
			m_selected_unit->StopBadgeBlink( true );
		}
		else if ( most_important_unit ) {
			m_selected_unit->Hide();
			most_important_unit->Show();
		}
		m_selected_unit = nullptr;
	}
}

Unit* UnitManager::GetPreviouslyDeselectedUnit() const {
	return m_previously_deselected_unit;
}

SlotBadges* UnitManager::GetSlotBadges( const size_t slot_index ) const {
	ASSERT( m_slot_badges.find( slot_index ) != m_slot_badges.end(), "slot unit badges for index " + std::to_string( slot_index ) + " not defined" );
	return m_slot_badges.at( slot_index );
}

void UnitManager::DefineSlotBadges( const size_t slot_index, const faction::Faction* faction ) {
	ASSERT( m_slot_badges.find( slot_index ) == m_slot_badges.end(), "slot unit badges for index " + std::to_string( slot_index ) + " already defined" );
	m_slot_badges.insert(
		{
			slot_index,
			new SlotBadges( m_badge_defs, m_ism, slot_index, faction )
		}
	);
}

const types::Vec3 UnitManager::GetCloserCoords( const types::Vec3& coords, const types::Vec3& ref_coords ) const {
	return m_game->GetCloserCoords( coords, ref_coords );
}

void UnitManager::AddSelectable( Unit* unit ) {
	const auto& it = std::find( m_selectable_units.begin(), m_selectable_units.end(), unit );
	if ( it == m_selectable_units.end() ) {
		m_selectable_units.push_back( unit );
	}
}

void UnitManager::RemoveSelectable( Unit* unit ) {
	const auto& it = std::find( m_selectable_units.begin(), m_selectable_units.end(), unit );
	if ( it != m_selectable_units.end() ) {
		m_selectable_units.erase( it );
		if ( m_previously_deselected_unit == unit ) {
			m_previously_deselected_unit = nullptr;
		}
		if ( m_selected_unit == unit ) {
			SelectNextUnitOrSwitchToTileSelection();
		}
	}
}

void UnitManager::UpdateSelectable( Unit* unit ) {
	if ( unit->IsOwned() ) {
		if ( unit->IsActive() ) {
			AddSelectable( unit );
		}
		else {
			RemoveSelectable( unit );
			if ( m_selected_unit == unit ) {
				SelectNextUnitOrSwitchToTileSelection();
			}
		}
	}
}

Unit* UnitManager::GetNextSelectable() {
	if ( !m_selectable_units.empty() ) {
		if ( !m_selected_unit ) {
			return m_selectable_units.front();
		}
		Unit* selected_unit = nullptr;
		float selected_distance = 0;
		// can be optimized, but probably no need
		for ( const auto& unit : m_selectable_units ) {
			if ( unit == m_selected_unit ) {
				continue;
			}
			const auto& a = unit->GetTile()->GetCoords();
			const auto& b = m_selected_unit->GetTile()->GetCoords();
			float distance = sqrtf( pow( (float)a.x - b.x, 2 ) + pow( (float)a.y - b.y, 2 ) );
			if ( !selected_unit || distance < selected_distance ) {
				selected_unit = unit;
				selected_distance = distance;
			}
		}
		if ( !selected_unit ) {
			selected_unit = m_selected_unit; // the only unit left
		}
		else {
			auto* tile = selected_unit->GetTile();
			if ( tile == m_selected_unit->GetTile() ) {
				const auto tile_units = tile->GetOrderedUnits();
				ASSERT( !tile_units.empty(), "tile units empty" );
				bool searching = false;
				bool found = false;
				Unit* first_good_candidate = nullptr;
				for ( size_t i = 0 ; i < tile_units.size() ; i++ ) {
					if ( tile_units.at( i ) == m_selected_unit ) {
						searching = true;
					}
					else {
						auto* tile_unit = tile_units.at( i );
						if ( searching ) {
							if ( tile_unit->IsActive() ) {
								selected_unit = tile_unit;
								found = true;
								break;
							}
						}
						else if ( !first_good_candidate && tile_unit->IsActive() ) {
							first_good_candidate = tile_unit;
						}
					}
				}
				if ( searching && !found ) {
					selected_unit = first_good_candidate;
				}
			}
		}
		return selected_unit;
	}
	return nullptr;
}

const bool UnitManager::SelectNextUnitMaybe() {
	if ( !m_game->IsTurnActive() ) {
		return false;
	}
	auto* selected_unit = GetNextSelectable();
	if ( selected_unit != m_selected_unit ) {
		if ( selected_unit ) {
			SelectUnit( selected_unit, true );
			m_game->ScrollToTile( m_selected_unit->GetTile(), true );
			return true;
		}
		else {
			m_selected_unit = nullptr;
			return false;
		}
	}
	return false;
}

void UnitManager::SelectNextUnitOrSwitchToTileSelection() {
	if ( !SelectNextUnitMaybe() ) {
		m_game->SelectUnitOrSelectedTile( m_selected_unit );
		m_selected_unit = nullptr;
	}
}

}
}
}
