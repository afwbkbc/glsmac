#pragma once

#include <vector>

#include "MiddleAreaPage.h"

#include "ui/object/SimpleButton.h"
#include "Section.h"
#include "ui/object/Label.h"

namespace game {
namespace world {
	
namespace ui {
	
CLASS( Toolbar, MiddleAreaPage )
	
	Toolbar( World* world ) : MiddleAreaPage( world, "Toolbar" ) {}

	void Create();
	void Destroy();
	void Align();
	
private:

	// tools
	
	static constexpr uint8_t s_tool_button_width = 44; // also includes margin
	static constexpr uint8_t s_tool_button_height = 19; // also includes margin
	static constexpr uint8_t s_tool_buttons_in_row = 5;

	enum editing_tool_t {
		ET_NONE,
		
		ET_ELEVATIONS,
		ET_MOISTURE,
		ET_FOREST,
		ET_SENSOR_ARRAY,
		ET_MONOLITH,
		ET_ROCKINESS,
		ET_FARM,
		ET_ROAD,
		ET_FUNGUS,
		ET_JUNGLE,
		ET_RIVERS,
		ET_SOIL_ENRICHER,
		ET_MAG_TUBE,
		ET_CONDENSER,
		ET_DUNES,
		ET_RESOURCES,
		ET_MINE,
		ET_BUNKER,
		ET_ECHELON_MIRROR,
		ET_URANIUM,
		ET_UNITY_PODS,
		ET_SOLAR_COLLECTOR,
		ET_AIRBASE,
		ET_THERMAL_BOREHOLE,
		ET_GEOTHERMAL,
			
		ET_MAX
	};
	editing_tool_t m_active_tool = ET_NONE;
	
	// note: those are also used for button class name
	const std::unordered_map< editing_tool_t, std::string > m_tool_names = {
		{ ET_NONE, "None" },
		{ ET_ELEVATIONS, "Elevations" },
		{ ET_MOISTURE, "Moisture" },
		{ ET_FOREST, "Forest" },
		{ ET_SENSOR_ARRAY, "Sensor Array" },
		{ ET_MONOLITH, "Monolith" },
		{ ET_ROCKINESS, "Rockiness" },
		{ ET_FARM, "Farm" },
		{ ET_ROAD, "Road" },
		{ ET_FUNGUS, "Fungus" },
		{ ET_JUNGLE, "Jungle" },
		{ ET_RIVERS, "Rivers" },
		{ ET_SOIL_ENRICHER, "Soil Enricher" },
		{ ET_MAG_TUBE, "Mag Tube" },
		{ ET_CONDENSER, "Condenser" },
		{ ET_DUNES, "Dunes" },
		{ ET_RESOURCES, "Resources" },
		{ ET_MINE, "Mine" },
		{ ET_BUNKER, "Bunker" },
		{ ET_ECHELON_MIRROR, "Echelon Mirror" },
		{ ET_URANIUM, "Uranium" },
		{ ET_UNITY_PODS, "Unity Pods" },
		{ ET_SOLAR_COLLECTOR, "Solar Collector" },
		{ ET_AIRBASE, "Airbase" },
		{ ET_THERMAL_BOREHOLE, "Thermal Borehole" },
		{ ET_GEOTHERMAL, "Geothermal" },
	};

	std::vector< ::ui::object::SimpleButton* > m_tool_buttons = {};
	::ui::object::SimpleButton* m_active_tool_button = nullptr;
	
	void SelectTool( editing_tool_t tool );
	
	// brushes
	
	static constexpr uint8_t s_brush_button_width = 25; // also includes margin
	static constexpr uint8_t s_brush_button_height = 25; // also includes margin
	static constexpr uint8_t s_brush_buttons_in_column = 3;

	enum editing_brush_t {
		EB_NONE,
		
		EB_DOT,
		EB_CROSS,
		EB_SQUARE_3_3,
		EB_SQUARE_5_5,
		EB_SQUARE_7_7,
		EB_SQUARE_9_9,
		
		EB_MAX
	};
	editing_brush_t m_active_brush = EB_NONE;

	// note: those are also used for button class name
	const std::unordered_map< editing_brush_t, std::string > m_brush_names = {
		{ EB_NONE, "None" },
		{ EB_DOT, "Dot" },
		{ EB_CROSS, "Cross" },
		{ EB_SQUARE_3_3, "Square 3x3" },
		{ EB_SQUARE_5_5, "Square 5x5" },
		{ EB_SQUARE_7_7, "Square 7x7" },
		{ EB_SQUARE_9_9, "Square 9x9" }
	};
	
	std::vector< ::ui::object::SimpleButton* > m_brush_buttons = {};
	::ui::object::SimpleButton* m_active_brush_button = nullptr;
	
	void SelectBrush( editing_brush_t brush );

	// tool info

	static constexpr uint8_t s_tool_info_line_height = 14; // also includes margin

	enum tool_info_line_t {
		TI_NONE,
		
		TI_FILE,
		TI_TOOL,
		TI_BRUSH,
		TI_MAP_SIZE,
		TI_MODE,
		
		TI_MAX
	};
	
	struct {
		Section* body = nullptr;
		std::unordered_map< tool_info_line_t, ::ui::object::Label* > labels = {};
	} m_tool_info = {};
	
};

}
}
}
