#include "MapEditor.h"

#include "types/Vec2.h"

#include "../map/Tile.h"

#include "tool/Elevations.h"

namespace game {
namespace world {
namespace map_editor {

MapEditor::MapEditor( World* world )
	: m_world( world )
{
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

const MapEditor::tiles_to_reload_t MapEditor::Draw( map::Tile* tile, const draw_mode_t mode ) {
	if ( IsEnabled() && mode != DM_NONE && m_active_tool ) {
		Log( "Drawing at " + tile->coord.ToString() );
		return m_active_tool->Draw( tile, mode );
	}
	else {
		return {};
	}
}

void MapEditor::SelectTool( MapEditor::tool_type_t tool ) {
	
	if ( GetActiveTool() != tool ) {
		
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

const MapEditor::tool_type_t MapEditor::GetActiveTool() const {
	if ( m_active_tool ) {
		return m_active_tool->GetType();
	}
	else {
		return TT_NONE;
	}
}

void MapEditor::SelectBrush( MapEditor::brush_type_t brush ) {
	
	if ( m_active_brush != brush ) {
		m_active_brush = brush;
		
		Log( "Selecting brush " + std::to_string( m_active_brush ) );
	}
}

const MapEditor::brush_type_t MapEditor::GetActiveBrush() const {
	return m_active_brush;
}

}
}
}
