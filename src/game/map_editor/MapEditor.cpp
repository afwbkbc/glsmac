#include <unordered_set>

#include "MapEditor.h"

#include "../world/Tile.h"

#include "brush/Dot.h"
#include "brush/Cross.h"
#include "brush/Square.h"

#include "tool/Elevations.h"
#include "tool/Moisture.h"
#include "tool/Rockiness.h"
#include "tool/Resource.h"
#include "tool/Feature.h"
#include "tool/Terraforming.h"

namespace game {
namespace map_editor {

MapEditor::MapEditor( Game* game )
	: m_game( game ) {
	NEW( m_brushes[ BT_DOT ], brush::Dot, m_game );
	NEW( m_brushes[ BT_CROSS ], brush::Cross, m_game );
#define x( _bt, _w ) NEW( m_brushes[ _bt ], brush::Square, m_game, _bt, _w )
	x( BT_SQUARE_3_3, 3 );
	x( BT_SQUARE_5_5, 5 );
	x( BT_SQUARE_7_7, 7 );
	x( BT_SQUARE_9_9, 9 );
#undef x

	NEW( m_tools[ TT_ELEVATIONS ], tool::Elevations, m_game );
	NEW( m_tools[ TT_MOISTURE ], tool::Moisture, m_game );
	NEW( m_tools[ TT_ROCKINESS ], tool::Rockiness, m_game );

	NEW( m_tools[ TT_RESOURCES ], tool::Resource, m_game );

#define x( _tt, _f ) NEW( m_tools[ _tt ], tool::Feature, m_game, _tt, world::Tile::_f )
	x( TT_MONOLITH, F_MONOLITH );
	x( TT_FUNGUS, F_XENOFUNGUS );
	x( TT_JUNGLE, F_JUNGLE );
	x( TT_RIVERS, F_RIVER );
	x( TT_DUNES, F_DUNES );
	x( TT_URANIUM, F_URANIUM );
	x( TT_GEOTHERMAL, F_GEOTHERMAL );
	x( TT_UNITY_PODS, F_UNITY_POD );
#undef x

#define x( _tt, _t ) NEW( m_tools[ _tt ], tool::Terraforming, m_game, _tt, world::Tile::_t )
	x( TT_FOREST, T_FOREST );
	x( TT_FARM, T_FARM );
	x( TT_SOLAR_COLLECTOR, T_SOLAR );
	x( TT_MINE, T_MINE );
	x( TT_SOIL_ENRICHER, T_SOIL_ENRICHER );
	x( TT_CONDENSER, T_CONDENSER );
	x( TT_ECHELON_MIRROR, T_MIRROR );
	x( TT_THERMAL_BOREHOLE, T_BOREHOLE );
	x( TT_SENSOR_ARRAY, T_SENSOR );
	x( TT_BUNKER, T_BUNKER );
	x( TT_AIRBASE, T_AIRBASE );
	x( TT_ROAD, T_ROAD );
	x( TT_MAG_TUBE, T_MAG_TUBE );
#undef x
}

MapEditor::~MapEditor() {
	for ( auto& tool : m_tools ) {
		DELETE( tool.second );
	}
}

const bool MapEditor::IsEnabled() const {
	// always enabled for now
	return true;
}

const MapEditor::tiles_t MapEditor::Draw( world::Tile* tile, const draw_mode_t mode ) {
	if ( IsEnabled() && mode != DM_NONE && m_active_tool && m_active_brush ) {
		Log( "Drawing at " + tile->coord.ToString() + " with brush " + std::to_string( GetActiveBrushType() ) + " tool " + std::to_string( GetActiveToolType() ) );
		tiles_t tiles_to_reload = {};
		const tiles_t tiles_to_draw = GetUniqueTiles( m_active_brush->Draw( tile ) );
		for ( auto& t : tiles_to_draw ) {
			const tiles_t tiles = m_active_tool->Draw( t, mode );
			tiles_to_reload.insert( tiles_to_reload.end(), tiles.begin(), tiles.end() );
		}
		return GetUniqueTiles( tiles_to_reload );
	}
	else {
		return {};
	}
}

void MapEditor::SelectTool( MapEditor::tool_type_t tool ) {

	if ( GetActiveToolType() != tool ) {
		auto it = m_tools.find( tool );
		if ( it != m_tools.end() ) {

			Log( "Selecting tool " + std::to_string( tool ) );

			m_active_tool = it->second;
		}
		else {
			Log( "Tool " + std::to_string( tool ) + " not implemented yet" );

			m_active_tool = nullptr;
		}
	}
}

const MapEditor::tool_type_t MapEditor::GetActiveToolType() const {
	if ( m_active_tool ) {
		return m_active_tool->GetType();
	}
	else {
		return TT_NONE;
	}
}

void MapEditor::SelectBrush( MapEditor::brush_type_t brush ) {

	if ( GetActiveBrushType() != brush ) {
		auto it = m_brushes.find( brush );
		if ( it != m_brushes.end() ) {

			Log( "Selecting brush " + std::to_string( brush ) );

			m_active_brush = it->second;
		}
		else {
			Log( "Brush " + std::to_string( brush ) + " not implemented yet" );

			m_active_brush = nullptr;
		}
	}
}

const MapEditor::brush_type_t MapEditor::GetActiveBrushType() const {
	if ( m_active_brush ) {
		return m_active_brush->GetType();
	}
	else {
		return BT_NONE;
	}
}

const MapEditor::tiles_t MapEditor::GetUniqueTiles( const tiles_t& tiles ) const {
	std::unordered_set< world::Tile* > added_tiles = {};
	added_tiles.reserve( tiles.size() );
	tiles_t result = {};
	result.reserve( tiles.size() );
	for ( auto& tile : tiles ) {
		if ( added_tiles.find( tile ) == added_tiles.end() ) {
			added_tiles.insert( tile );
			result.push_back( tile );
		}
	}
	return result;
}

}
}
