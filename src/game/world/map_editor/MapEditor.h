#pragma once

#include <vector>
#include <unordered_map>

#include "base/Base.h"

namespace game {
namespace world {
	class World;
namespace map {
	class Tile;
}
	
namespace map_editor {

namespace tool {
	class Tool;
}

CLASS( MapEditor, base::Base )

	// tools
	enum tool_type_t {
		TT_NONE,
		
		TT_ELEVATIONS,
		TT_MOISTURE,
		TT_FOREST,
		TT_SENSOR_ARRAY,
		TT_MONOLITH,
		TT_ROCKINESS,
		TT_FARM,
		TT_ROAD,
		TT_FUNGUS,
		TT_JUNGLE,
		TT_RIVERS,
		TT_SOIL_ENRICHER,
		TT_MAG_TUBE,
		TT_CONDENSER,
		TT_DUNES,
		TT_RESOURCES,
		TT_MINE,
		TT_BUNKER,
		TT_ECHELON_MIRROR,
		TT_URANIUM,
		TT_UNITY_PODS,
		TT_SOLAR_COLLECTOR,
		TT_AIRBASE,
		TT_THERMAL_BOREHOLE,
		TT_GEOTHERMAL,
			
		TT_MAX
	};
	std::unordered_map< tool_type_t, tool::Tool* > m_tools = {};
	tool::Tool* m_active_tool = nullptr;
	void SelectTool( tool_type_t tool );
	const tool_type_t GetActiveTool() const;

	// brushes
	enum brush_type_t {
		BT_NONE,
		
		BT_DOT,
		BT_CROSS,
		BT_SQUARE_3_3,
		BT_SQUARE_5_5,
		BT_SQUARE_7_7,
		BT_SQUARE_9_9,
		
		BT_MAX
	};
	brush_type_t m_active_brush = BT_NONE;
	void SelectBrush( MapEditor::brush_type_t brush );
	const brush_type_t GetActiveBrush() const;
	
	MapEditor( World* world );
	~MapEditor();
	
	const bool IsEnabled() const;
	
	enum draw_mode_t {
		DM_NONE,
		DM_DEC, // remove / decrease intensity
		DM_INC // add / increase intensity
	};
	
	typedef std::vector< map::Tile* > tiles_to_reload_t;
	const tiles_to_reload_t Draw( map::Tile* tile, const draw_mode_t mode ); // returns tiles that need reload
	
private:
	World* m_world = nullptr;
	
};

}
}
}
