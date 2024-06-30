#pragma once

#include <vector>

namespace game {

namespace map::tile {
class Tile;
}

namespace map_editor {

typedef std::vector< map::tile::Tile* > tiles_t;

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

enum draw_mode_t {
	DM_NONE,
	DM_DEC, // remove / decrease intensity
	DM_INC // add / increase intensity
};

}
}