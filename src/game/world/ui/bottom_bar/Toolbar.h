#pragma once

#include <vector>

#include "MiddleAreaPage.h"

#include "ui/object/SimpleButton.h"

namespace game {
namespace world {
	
namespace ui {
	
CLASS( Toolbar, MiddleAreaPage )
	
	static constexpr uint8_t s_tool_button_width = 44; // also includes margin
	static constexpr uint8_t s_tool_button_height = 19; // also includes margin
	static constexpr uint8_t s_tool_buttons_in_row = 5;

	Toolbar( World* world ) : MiddleAreaPage( world, "Toolbar" ) {}

	void Create();
	void Destroy();
	void Align();
	
private:

	// tools
	
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
	
};

}
}
}
