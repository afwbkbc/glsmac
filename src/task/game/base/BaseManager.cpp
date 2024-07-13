#include "BaseManager.h"

#include <algorithm>

#include "Base.h"
#include "task/game/Game.h"
#include "task/game/Slot.h"
#include "task/game/unit/UnitManager.h"
#include "task/game/tile/TileManager.h"
#include "task/game/text/InstancedTextManager.h"
#include "task/game/text/InstancedText.h"
#include "task/game/faction/Faction.h"
#include "types/mesh/Rectangle.h"
#include "engine/Engine.h"
#include "loader/font/FontLoader.h"

namespace task {
namespace game {
namespace base {

BaseManager::BaseManager( Game* game )
	: m_game( game )
	, m_name_font( game->GetITM()->GetInstancedFont( g_engine->GetFontLoader()->LoadFont( resource::TTF_ARIALN, 48 ) ) ) {

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
	const ::game::backend::base::BaseData& data
) {

	ASSERT( m_bases.find( base_id ) == m_bases.end(), "base id already exists" );

	auto* tile = m_game->GetTM()->GetTile( tile_coords );
	auto* slot = m_game->GetSlot( slot_index );
	auto* faction = slot->GetFaction();

	m_bases.insert(
		{
			base_id,
			new base::Base(
				base_id,
				slot,
				tile,
				slot_index == m_game->GetMySlotIndex(),
				render_coords,
				m_game->GetITM()->CreateInstancedText(
					data.name,
					m_name_font,
					faction->m_colors.text,
					faction->m_colors.text_shadow
				),
				data.population
			)
		}
	);

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

}
}
}
