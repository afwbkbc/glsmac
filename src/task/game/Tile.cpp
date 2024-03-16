#include "Tile.h"

namespace task {
namespace game {

std::vector< size_t > Tile::GetUnitsOrder( const std::unordered_map< size_t, Unit* >& units ) {
	std::map< size_t, std::vector< size_t > > weights; // { weight, units }

	for ( auto& it : units ) {
		const auto unit_id = it.first;
		const auto* unit = it.second;
		size_t weight = unit->GetSelectionWeight();
		weights[ -weight ].push_back( unit_id ); // negative because we need reverse order
	}

	std::vector< size_t > result = {};
	for ( const auto& it : weights ) {
		for ( const auto& unit_id : it.second ) {
			result.push_back( unit_id );
		}
	}

	return result;
}

Tile::Tile( const types::Vec2< size_t >& coords )
	: m_coords( coords ) {

}

const types::Vec2< size_t >& Tile::GetCoords() const {
	return m_coords;
}

void Tile::AddUnit( Unit* unit ) {
	ASSERT_NOLOG( m_units.find( unit->GetId() ) == m_units.end(), "unit already added" );
	m_units.insert(
		{
			unit->GetId(),
			unit
		}
	);
}

void Tile::RemoveUnit( Unit* unit ) {
	ASSERT_NOLOG( m_units.find( unit->GetId() ) != m_units.end(), "unit not found" );
	if ( m_render.currently_rendered_unit == unit ) {
		SetActiveUnit( nullptr );
	}
	m_units.erase( unit->GetId() );
}

void Tile::SetActiveUnit( Unit* unit ) {
	if ( m_render.currently_rendered_unit ) {
		m_render.currently_rendered_unit->Hide();
		m_render.currently_rendered_unit = unit;
	}
}

void Tile::Render( size_t selected_unit_id ) {

	SetActiveUnit( nullptr );

	for ( const auto& unit : m_render.currently_rendered_fake_badges ) {
		unit->HideFakeBadge();
	}
	m_render.currently_rendered_fake_badges.clear();

	// is needed?
	/*if ( m_selected_unit_state == tile_state->render.currently_rendered_unit ) {
		m_selected_unit_state = nullptr;
	}*/

	if ( !m_units.empty() ) {
		const auto units_order = GetUnitsOrder( m_units );
		ASSERT_NOLOG( !units_order.empty(), "units order is empty" );

		const auto most_important_unit_id = units_order.front();

		// also display badges from stacked units that are not visible themselves
		// TODO: refactor
		ASSERT_NOLOG( m_render.currently_rendered_fake_badges.empty(), "orphan badges already rendered" );
		size_t fake_badge_idx = 1;
		for ( const auto& unit_id : units_order ) {
			const auto& unit = m_units.at( unit_id );
			if ( !m_render.currently_rendered_unit && unit_id == most_important_unit_id ) {
				// choose first by default
				m_render.currently_rendered_unit = unit;
			}
			else if ( unit_id == selected_unit_id ) {
				// override default is found
				if ( m_render.currently_rendered_unit ) {
					m_render.currently_rendered_fake_badges.push_back( m_render.currently_rendered_unit );
				}
				m_render.currently_rendered_unit = unit;
			}
			else {
				// render fake badge
				m_render.currently_rendered_fake_badges.push_back( unit );
				if ( fake_badge_idx++ > 10 ) {
					break;
				}
			}
		}
		if ( m_render.currently_rendered_unit ) {
			m_render.currently_rendered_unit->Show();
		}
		size_t idx;
		const auto& fake_badges = m_render.currently_rendered_fake_badges;
		for ( size_t i = 0 ; i < fake_badges.size() ; i++ ) { // order is important
			idx = fake_badges.size() - i - 1;
			fake_badges.at( idx )->ShowFakeBadge( i );
		}
	}

}

}
}
