#pragma once

#include <vector>

#include "MiddleAreaPage.h"

#include "game/backend/map_editor/Types.h"

namespace ui::object {
class SimpleButton;
class Label;
}

namespace game {
namespace frontend {
namespace ui {

class CenteredBlock;
class BBSection;

CLASS( Toolbar, MiddleAreaPage )

	Toolbar( Game* game )
		: MiddleAreaPage( game, "Toolbar" ) {}

	void Create() override;
	void Destroy() override;
	void Align() override;

	void UpdateMapFileName();

private:

	CenteredBlock* m_centered_block = nullptr;

	// tools

	static constexpr uint8_t s_tool_button_width = 44; // also includes margin
	static constexpr uint8_t s_tool_button_height = 19; // also includes margin
	static constexpr uint8_t s_tool_buttons_in_row = 5;

	// note: those are also used for button class names
	const std::unordered_map< ::game::backend::map_editor::tool_type_t, std::string > m_tool_names = {
		{ ::game::backend::map_editor::TT_NONE,             "None" },
		{ ::game::backend::map_editor::TT_ELEVATIONS,       "Elevations" },
		{ ::game::backend::map_editor::TT_MOISTURE,         "Moisture" },
		{ ::game::backend::map_editor::TT_FOREST,           "Forest" },
		{ ::game::backend::map_editor::TT_SENSOR_ARRAY,     "Sensor Array" },
		{ ::game::backend::map_editor::TT_MONOLITH,         "Monolith" },
		{ ::game::backend::map_editor::TT_ROCKINESS,        "Rockiness" },
		{ ::game::backend::map_editor::TT_FARM,             "Farm" },
		{ ::game::backend::map_editor::TT_ROAD,             "Road" },
		{ ::game::backend::map_editor::TT_FUNGUS,           "Fungus" },
		{ ::game::backend::map_editor::TT_JUNGLE,           "Jungle" },
		{ ::game::backend::map_editor::TT_RIVERS,           "Rivers" },
		{ ::game::backend::map_editor::TT_SOIL_ENRICHER,    "Soil Enricher" },
		{ ::game::backend::map_editor::TT_MAG_TUBE,         "Mag Tube" },
		{ ::game::backend::map_editor::TT_CONDENSER,        "Condenser" },
		{ ::game::backend::map_editor::TT_DUNES,            "Dunes" },
		{ ::game::backend::map_editor::TT_RESOURCES,        "Resources" },
		{ ::game::backend::map_editor::TT_MINE,             "Mine" },
		{ ::game::backend::map_editor::TT_BUNKER,           "Bunker" },
		{ ::game::backend::map_editor::TT_ECHELON_MIRROR,   "Echelon Mirror" },
		{ ::game::backend::map_editor::TT_URANIUM,          "Uranium" },
		{ ::game::backend::map_editor::TT_UNITY_PODS,       "Unity Pods" },
		{ ::game::backend::map_editor::TT_SOLAR_COLLECTOR,  "Solar Collector" },
		{ ::game::backend::map_editor::TT_AIRBASE,          "Airbase" },
		{ ::game::backend::map_editor::TT_THERMAL_BOREHOLE, "Thermal Borehole" },
		{ ::game::backend::map_editor::TT_GEOTHERMAL,       "Geothermal" },
	};

	std::vector< ::ui::object::SimpleButton* > m_tool_buttons = {};
	::ui::object::SimpleButton* m_active_tool_button = nullptr;

	void SelectTool( ::game::backend::map_editor::tool_type_t tool );

	// brushes

	static constexpr uint8_t s_brush_button_width = 25; // also includes margin
	static constexpr uint8_t s_brush_button_height = 25; // also includes margin
	static constexpr uint8_t s_brush_buttons_in_column = 3;

	// note: those are also used for button class name
	const std::unordered_map< ::game::backend::map_editor::brush_type_t, std::string > m_brush_names = {
		{ ::game::backend::map_editor::BT_NONE,       "None" },
		{ ::game::backend::map_editor::BT_DOT,        "Dot" },
		{ ::game::backend::map_editor::BT_CROSS,      "Cross" },
		{ ::game::backend::map_editor::BT_SQUARE_3_3, "Square 3x3" },
		{ ::game::backend::map_editor::BT_SQUARE_5_5, "Square 5x5" },
		{ ::game::backend::map_editor::BT_SQUARE_7_7, "Square 7x7" },
		{ ::game::backend::map_editor::BT_SQUARE_9_9, "Square 9x9" }
	};

	std::vector< ::ui::object::SimpleButton* > m_brush_buttons = {};
	::ui::object::SimpleButton* m_active_brush_button = nullptr;

	void SelectBrush( ::game::backend::map_editor::brush_type_t brush );

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
