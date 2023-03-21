#include <unordered_set>

#include "MapEditor.h"

#include "types/Vec2.h"

#include "../map/Tile.h"

#include "brush/Dot.h"
#include "brush/Cross.h"
#include "brush/Square.h"

#include "tool/Elevations.h"

namespace game {
namespace world {
namespace map_editor {

MapEditor::MapEditor( World* world )
	: m_world( world )
{
	NEW( m_brushes[ BT_DOT ], brush::Dot, m_world );
	NEW( m_brushes[ BT_CROSS ], brush::Cross, m_world );
	NEW( m_brushes[ BT_SQUARE_3_3 ], brush::Square, m_world, BT_SQUARE_3_3, 3 );
	NEW( m_brushes[ BT_SQUARE_5_5 ], brush::Square, m_world, BT_SQUARE_5_5, 5 );
	NEW( m_brushes[ BT_SQUARE_7_7 ], brush::Square, m_world, BT_SQUARE_7_7, 7 );
	NEW( m_brushes[ BT_SQUARE_9_9 ], brush::Square, m_world, BT_SQUARE_9_9, 9 );
	
	NEW( m_tools[ TT_ELEVATIONS ], tool::Elevations, m_world );
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

const MapEditor::tiles_t MapEditor::Draw( map::Tile* tile, const draw_mode_t mode ) {
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
	std::unordered_set< map::Tile* > added_tiles = {};
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
}
