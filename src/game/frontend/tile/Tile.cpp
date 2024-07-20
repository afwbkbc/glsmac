#include "Tile.h"

#include "game/frontend/unit/Unit.h"
#include "game/frontend/base/Base.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/map/tile/TileState.h"
#include "types/mesh/Render.h"

namespace game {
namespace frontend {
namespace tile {

std::vector< size_t > Tile::GetUnitsOrder( const std::unordered_map< size_t, unit::Unit* >& units ) {
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

const bool Tile::IsWater() const {
	return m_is_water;
}

const types::Vec2< size_t >& Tile::GetCoords() const {
	return m_coords;
}

void Tile::AddUnit( unit::Unit* unit ) {
	if ( m_units.find( unit->GetId() ) != m_units.end() ) {
		return; // already on tile
	}
	m_units.insert(
		{
			unit->GetId(),
			unit
		}
	);
	m_is_units_reorder_needed = true;
	if ( m_base ) {
		m_base->Update();
	}
	Render();
}

void Tile::RemoveUnit( unit::Unit* unit ) {
	if ( m_units.find( unit->GetId() ) == m_units.end() ) {
		return; // not on tile
	}
	if ( m_render.currently_rendered_unit == unit ) {
		SetActiveUnit( nullptr );
	}
	m_units.erase( unit->GetId() );
	m_is_units_reorder_needed = true;
	if ( m_base ) {
		m_base->Update();
	}
	Render();
}

void Tile::SetActiveUnit( unit::Unit* unit ) {
	if ( m_render.currently_rendered_unit && m_render.currently_rendered_unit != unit ) {
		m_render.currently_rendered_unit->Hide();
		m_render.currently_rendered_unit = unit;
	}
}

void Tile::SetBase( base::Base* base ) {
	ASSERT_NOLOG( !m_base, "base already set" );
	m_base = base;
	Render();
}

void Tile::UnsetBase( base::Base* base ) {
	ASSERT_NOLOG( m_base == base, "different or no base set" );
	m_base->Hide();
	m_base = nullptr;
	Render();
}

void Tile::Render( size_t selected_unit_id ) {

	if ( m_base ) {
		m_base->Hide();
	}

	SetActiveUnit( nullptr );

	for ( const auto& unit : m_render.currently_rendered_fake_badges ) {
		unit->HideFakeBadge();
	}
	m_render.currently_rendered_fake_badges.clear();

	bool should_show_units = !m_units.empty();
	if ( m_base ) {
		m_base->Show();
		should_show_units = false;
		for ( const auto& it : m_units ) {
			if ( it.second->GetId() == selected_unit_id ) {
				should_show_units = true;
				break;
			}
		}
	}
	if ( should_show_units ) {
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
			const auto id = m_render.currently_rendered_unit->GetId();
			m_render.currently_rendered_unit->Show();
			if ( id == most_important_unit_id ) {
				m_render.currently_rendered_unit->ShowBadge();
			}
			if ( id == selected_unit_id && m_render.currently_rendered_unit->IsActive() ) {
				m_render.currently_rendered_unit->StartBadgeBlink();
			}
		}
		size_t idx;
		const auto& fake_badges = m_render.currently_rendered_fake_badges;
		for ( size_t i = 0 ; i < fake_badges.size() ; i++ ) { // order is important
			idx = fake_badges.size() - i - 1;
			fake_badges.at( idx )->ShowFakeBadge( i );
		}
	}
	else {
		for ( const auto& it : m_units ) {
			it.second->Hide();
			it.second->HideFakeBadge();
			it.second->HideBadge();
		}
	}

}

const std::unordered_map< size_t, unit::Unit* >& Tile::GetUnits() const {
	return m_units;
}

const std::vector< unit::Unit* >& Tile::GetOrderedUnits() {
	if ( m_is_units_reorder_needed ) {
		m_ordered_units.clear();
		m_ordered_units.reserve( m_units.size() );
		const auto order = GetUnitsOrder( m_units );
		for ( const auto& it : order ) {
			m_ordered_units.push_back( m_units.at( it ) );
		}
		m_is_units_reorder_needed = false;
	}
	return m_ordered_units;
}

unit::Unit* Tile::GetMostImportantUnit() {
	if ( m_units.empty() ) {
		return nullptr;
	}
	else {
		return GetOrderedUnits().front();
	}
}

Tile* Tile::GetNeighbour( const backend::map::tile::direction_t direction ) {
	switch ( direction ) {
		case backend::map::tile::D_NONE:
			return this;
		case backend::map::tile::D_W:
			return W;
		case backend::map::tile::D_NW:
			return NW;
		case backend::map::tile::D_N:
			return N;
		case backend::map::tile::D_NE:
			return NE;
		case backend::map::tile::D_E:
			return E;
		case backend::map::tile::D_SE:
			return SE;
		case backend::map::tile::D_S:
			return S;
		case backend::map::tile::D_SW:
			return SW;
		default:
			THROW( "unknown tile direction: " + std::to_string( direction ) );
	}
}

const Tile::render_data_t& Tile::GetRenderData() const {
	return m_render_data;
}

void Tile::Update( const backend::map::tile::Tile& tile, const backend::map::tile::TileState& ts ) {

	m_is_water = tile.is_water_tile;

	backend::map::tile::tile_layer_type_t lt = ( tile.is_water_tile
		? backend::map::tile::LAYER_WATER
		: backend::map::tile::LAYER_LAND
	);
	const auto& layer = ts.layers[ lt ];

	backend::map::tile::tile_vertices_t selection_coords = {};
	backend::map::tile::tile_vertices_t preview_coords = {};

#define x( _k ) selection_coords._k = layer.coords._k
	x( center );
	x( left );
	x( top );
	x( right );
	x( bottom );
#undef x

	if ( !tile.is_water_tile && ts.is_coastline_corner ) {
		if ( tile.W->is_water_tile ) {
			selection_coords.left = ts.layers[ backend::map::tile::LAYER_WATER ].coords.left;
		}
		if ( tile.N->is_water_tile ) {
			selection_coords.top = ts.layers[ backend::map::tile::LAYER_WATER ].coords.top;
		}
		if ( tile.E->is_water_tile ) {
			selection_coords.right = ts.layers[ backend::map::tile::LAYER_WATER ].coords.right;
		}
		if ( tile.S->is_water_tile ) {
			selection_coords.bottom = ts.layers[ backend::map::tile::LAYER_WATER ].coords.bottom;
		}
	}

	lt = ( ( tile.is_water_tile || ts.is_coastline_corner )
		? backend::map::tile::LAYER_WATER
		: backend::map::tile::LAYER_LAND
	);
#define x( _k ) preview_coords._k = layer.coords._k
	x( center );
	x( left );
	x( top );
	x( right );
	x( bottom );
#undef x
	// absolute coords to relative
#define x( _k ) preview_coords._k -= preview_coords.center
	x( left );
	x( top );
	x( right );
	x( bottom );
#undef x
	preview_coords.center = {
		0.0f,
		0.0f,
		0.0f
	};

	std::vector< backend::map::tile::tile_layer_type_t > layers = {};
	if ( tile.is_water_tile ) {
		layers.push_back( backend::map::tile::LAYER_LAND );
		layers.push_back( backend::map::tile::LAYER_WATER_SURFACE );
		layers.push_back( backend::map::tile::LAYER_WATER_SURFACE_EXTRA ); // TODO: only near coastlines?
		layers.push_back( backend::map::tile::LAYER_WATER );
	}
	else {
		if ( ts.is_coastline_corner ) {
			layers.push_back( backend::map::tile::LAYER_WATER_SURFACE );
			layers.push_back( backend::map::tile::LAYER_WATER_SURFACE_EXTRA );
			layers.push_back( backend::map::tile::LAYER_WATER );
		}
		else {
			layers.push_back( backend::map::tile::LAYER_LAND );
		}
	}

	// looks a bit too bright without lighting otherwise
	const float tint_modifier = 0.7f;

	std::vector< types::mesh::Render* > preview_meshes = {};

	preview_meshes.reserve( layers.size() );
	for ( auto i = 0 ; i < layers.size() ; i++ ) {
		lt = layers[ i ];

		NEWV( mesh, types::mesh::Render, 5, 4 );

		const auto& l = ts.layers[ lt ];

		auto tint = l.colors;

		//Log( "Coords = " + preview_coords.center.ToString() + " " + preview_coords.left.ToString() + " " + preview_coords.top.ToString() + " " + preview_coords.right.ToString() + " " + preview_coords.bottom.ToString() );

#define x( _k ) auto _k = mesh->AddVertex( preview_coords._k, l.tex_coords._k, tint._k * tint_modifier )
		x( center );
		x( left );
		x( top );
		x( right );
		x( bottom );
#undef x

#define x( _a, _b, _c ) mesh->AddSurface( { _a, _b, _c } )
		x( center, left, top );
		x( center, top, right );
		x( center, right, bottom );
		x( center, bottom, left );
#undef x

		mesh->Finalize();

		preview_meshes.push_back( mesh );
	}

	std::vector< std::string > sprites = {};
	for ( auto& s : ts.sprites ) {
		sprites.push_back( s.actor );
	}

	std::vector< std::string > info_lines = {};

	auto e = *tile.elevation.center;
	if ( tile.is_water_tile ) {
		if ( e < backend::map::tile::ELEVATION_LEVEL_TRENCH ) {
			info_lines.push_back( "Ocean Trench" );
		}
		else if ( e < backend::map::tile::ELEVATION_LEVEL_OCEAN ) {
			info_lines.push_back( "Ocean" );
		}
		else {
			info_lines.push_back( "Ocean Shelf" );
		}
		info_lines.push_back( "Depth: " + std::to_string( -e ) + "m" );
	}
	else {
		info_lines.push_back( "Elev: " + std::to_string( e ) + "m" );
		std::string tilestr = "";
		switch ( tile.rockiness ) {
			case backend::map::tile::ROCKINESS_FLAT: {
				tilestr += "Flat";
				break;
			}
			case backend::map::tile::ROCKINESS_ROLLING: {
				tilestr += "Rolling";
				break;
			}
			case backend::map::tile::ROCKINESS_ROCKY: {
				tilestr += "Rocky";
				break;
			}
		}
		tilestr += " & ";
		switch ( tile.moisture ) {
			case backend::map::tile::MOISTURE_ARID: {
				tilestr += "Arid";
				break;
			}
			case backend::map::tile::MOISTURE_MOIST: {
				tilestr += "Moist";
				break;
			}
			case backend::map::tile::MOISTURE_RAINY: {
				tilestr += "Rainy";
				break;
			}
		}
		info_lines.push_back( tilestr );
	}

#define FEATURE( _feature, _line ) \
            if ( tile.features & backend::map::tile::_feature ) { \
                info_lines.push_back( _line ); \
            }

	if ( tile.is_water_tile ) {
		FEATURE( FEATURE_XENOFUNGUS, "Sea Fungus" )
	}
	else {
		FEATURE( FEATURE_XENOFUNGUS, "Xenofungus" )
	}

	switch ( tile.bonus ) {
		case backend::map::tile::BONUS_NUTRIENT: {
			info_lines.push_back( "Nutrient bonus" );
			break;
		}
		case backend::map::tile::BONUS_ENERGY: {
			info_lines.push_back( "Energy bonus" );
			break;
		}
		case backend::map::tile::BONUS_MINERALS: {
			info_lines.push_back( "Minerals bonus" );
			break;
		}
		default: {
			// nothing
		}
	}

	if ( tile.is_water_tile ) {
		FEATURE( FEATURE_GEOTHERMAL, "Geothermal" )
	}
	else {
		FEATURE( FEATURE_RIVER, "River" )
		FEATURE( FEATURE_JUNGLE, "Jungle" )
		FEATURE( FEATURE_DUNES, "Dunes" )
		FEATURE( FEATURE_URANIUM, "Uranium" )
	}
	FEATURE( FEATURE_MONOLITH, "Monolith" )

#undef FEATURE

#define TERRAFORMING( _terraforming, _line ) \
            if ( tile.terraforming & backend::map::tile::_terraforming ) { \
                info_lines.push_back( _line ); \
            }

	if ( tile.is_water_tile ) {
		TERRAFORMING( TERRAFORMING_FARM, "Kelp Farm" );
		TERRAFORMING( TERRAFORMING_SOLAR, "Tidal Harness" );
		TERRAFORMING( TERRAFORMING_MINE, "Mining Platform" );

		TERRAFORMING( TERRAFORMING_SENSOR, "Sensor Buoy" );
	}
	else {
		TERRAFORMING( TERRAFORMING_FOREST, "Forest" );
		TERRAFORMING( TERRAFORMING_FARM, "Farm" );
		TERRAFORMING( TERRAFORMING_SOIL_ENRICHER, "Soil Enricher" );
		TERRAFORMING( TERRAFORMING_MINE, "Mine" );
		TERRAFORMING( TERRAFORMING_SOLAR, "Solar Collector" );

		TERRAFORMING( TERRAFORMING_CONDENSER, "Condenser" );
		TERRAFORMING( TERRAFORMING_MIRROR, "Echelon Mirror" );
		TERRAFORMING( TERRAFORMING_BOREHOLE, "Thermal Borehole" );

		TERRAFORMING( TERRAFORMING_ROAD, "Road" );
		TERRAFORMING( TERRAFORMING_MAG_TUBE, "Mag Tube" );

		TERRAFORMING( TERRAFORMING_SENSOR, "Sensor Array" );
		TERRAFORMING( TERRAFORMING_BUNKER, "Bunker" );
		TERRAFORMING( TERRAFORMING_AIRBASE, "Airbase" );
	}

#undef TERRAFORMING

	// combine into printable lines
	std::string info_line = "";
	std::string info_line_new = "";
	constexpr size_t max_length = 16; // TODO: determine width from actual text because different symbols are different

	std::vector< std::string > preview_lines = {};
	for ( auto& line : info_lines ) {
		info_line_new = info_line + ( info_line.empty()
			? ""
			: ", "
		) + line;
		if ( info_line_new.size() > max_length ) {
			preview_lines.push_back( info_line );
			info_line = line;
		}
		else {
			info_line = info_line_new;
		}
	}
	if ( !info_line.empty() ) {
		preview_lines.push_back( info_line );
	}

	m_render_data.coords = layer.coords.center;
	m_render_data.selection_coords = selection_coords;
	m_render_data.preview_meshes = preview_meshes;
	m_render_data.preview_lines = preview_lines;
	m_render_data.sprites = sprites;

	for ( const auto& it : m_units ) {
		it.second->UpdateFromTile();
	}
	if ( m_base ) {
		m_base->UpdateFromTile();
	}
}

}
}
}
