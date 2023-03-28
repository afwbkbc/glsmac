#pragma once

#include <vector>

#include "MiddleAreaPage.h"

#include "CenteredBlock.h"
#include "ui/object/SimpleButton.h"
#include "BBSection.h"
#include "ui/object/Label.h"
#include "../../map_editor/MapEditor.h"

namespace game {
namespace world {
	using namespace map_editor;
namespace ui {
	
CLASS( Toolbar, MiddleAreaPage )
	
	Toolbar( World* world ) : MiddleAreaPage( world, "Toolbar" ) {}

	void Create();
	void Destroy();
	void Align();
	
	void UpdateMapFileName();
	
private:

	CenteredBlock* m_centered_block = nullptr;
	
	// tools
	
	static constexpr uint8_t s_tool_button_width = 44; // also includes margin
	static constexpr uint8_t s_tool_button_height = 19; // also includes margin
	static constexpr uint8_t s_tool_buttons_in_row = 5;

	// note: those are also used for button class names
	const std::unordered_map< MapEditor::tool_type_t, std::string > m_tool_names = {
		{ MapEditor::TT_NONE, "None" },
		{ MapEditor::TT_ELEVATIONS, "Elevations" },
		{ MapEditor::TT_MOISTURE, "Moisture" },
		{ MapEditor::TT_FOREST, "Forest" },
		{ MapEditor::TT_SENSOR_ARRAY, "Sensor Array" },
		{ MapEditor::TT_MONOLITH, "Monolith" },
		{ MapEditor::TT_ROCKINESS, "Rockiness" },
		{ MapEditor::TT_FARM, "Farm" },
		{ MapEditor::TT_ROAD, "Road" },
		{ MapEditor::TT_FUNGUS, "Fungus" },
		{ MapEditor::TT_JUNGLE, "Jungle" },
		{ MapEditor::TT_RIVERS, "Rivers" },
		{ MapEditor::TT_SOIL_ENRICHER, "Soil Enricher" },
		{ MapEditor::TT_MAG_TUBE, "Mag Tube" },
		{ MapEditor::TT_CONDENSER, "Condenser" },
		{ MapEditor::TT_DUNES, "Dunes" },
		{ MapEditor::TT_RESOURCES, "Resources" },
		{ MapEditor::TT_MINE, "Mine" },
		{ MapEditor::TT_BUNKER, "Bunker" },
		{ MapEditor::TT_ECHELON_MIRROR, "Echelon Mirror" },
		{ MapEditor::TT_URANIUM, "Uranium" },
		{ MapEditor::TT_UNITY_PODS, "Unity Pods" },
		{ MapEditor::TT_SOLAR_COLLECTOR, "Solar Collector" },
		{ MapEditor::TT_AIRBASE, "Airbase" },
		{ MapEditor::TT_THERMAL_BOREHOLE, "Thermal Borehole" },
		{ MapEditor::TT_GEOTHERMAL, "Geothermal" },
	};

	std::vector< ::ui::object::SimpleButton* > m_tool_buttons = {};
	::ui::object::SimpleButton* m_active_tool_button = nullptr;
	
	void SelectTool( MapEditor::tool_type_t tool );
	
	// brushes
	
	static constexpr uint8_t s_brush_button_width = 25; // also includes margin
	static constexpr uint8_t s_brush_button_height = 25; // also includes margin
	static constexpr uint8_t s_brush_buttons_in_column = 3;

	// note: those are also used for button class name
	const std::unordered_map< MapEditor::brush_type_t, std::string > m_brush_names = {
		{ MapEditor::BT_NONE, "None" },
		{ MapEditor::BT_DOT, "Dot" },
		{ MapEditor::BT_CROSS, "Cross" },
		{ MapEditor::BT_SQUARE_3_3, "Square 3x3" },
		{ MapEditor::BT_SQUARE_5_5, "Square 5x5" },
		{ MapEditor::BT_SQUARE_7_7, "Square 7x7" },
		{ MapEditor::BT_SQUARE_9_9, "Square 9x9" }
	};
	
	std::vector< ::ui::object::SimpleButton* > m_brush_buttons = {};
	::ui::object::SimpleButton* m_active_brush_button = nullptr;
	
	void SelectBrush( MapEditor::brush_type_t brush );

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
		BBSection* body = nullptr;
		std::unordered_map< tool_info_line_t, ::ui::object::Label* > labels = {};
	} m_tool_info = {};
	
};

}
}
}
