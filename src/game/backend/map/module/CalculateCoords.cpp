#include "CalculateCoords.h"

#include "game/backend/map/Map.h"
#include "game/backend/map/Consts.h"
#include "game/backend/map/tile/TileState.h"
#include "util/random/Random.h"

namespace game {
namespace backend {
namespace map {
namespace module {

void CalculateCoords::GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) {

	for ( auto lt = 0 ; lt < tile::LAYER_MAX ; lt++ ) {

		// vertex coodinates for tile corners ( everything but land is flat at water level )
#define vcz( _lt, _val ) ( ( _lt == tile::LAYER_LAND ) ? s_consts.tile.elevation_to_vertex_z.Clamp( _val ) : s_consts.tile.water_level_z )

		ts->layers[ lt ].coords.left = {
			ts->coord.x - s_consts.tile.radius.x,
			ts->coord.y,
			vcz( lt, ts->elevations.left )
		};
		ts->layers[ lt ].coords.top = {
			ts->coord.x,
			ts->coord.y - s_consts.tile.radius.y,
			vcz( lt, ts->elevations.top )
		};
		ts->layers[ lt ].coords.right = {
			ts->coord.x + s_consts.tile.radius.x,
			ts->coord.y,
			vcz( lt, ts->elevations.right )
		};
		ts->layers[ lt ].coords.bottom = {
			ts->coord.x,
			ts->coord.y + s_consts.tile.radius.y,
			vcz( lt, ts->elevations.bottom )
		};
		ts->layers[ lt ].coords.center = {
			( ts->layers[ lt ].coords.left.x + ts->layers[ tile::LAYER_LAND ].coords.right.x ) / 2,
			( ts->layers[ tile::LAYER_LAND ].coords.top.y + ts->layers[ tile::LAYER_LAND ].coords.bottom.y ) / 2,
			vcz( lt, ts->elevations.center )
		};

		float ofs = 1.0f; // remove lines between tiles

		ts->layers[ lt ].tex_coords = {
			{
				ts->tex_coord.x + m_map->GetRandom()->GetFloat(
					-s_consts.tile.random.texture_center_stretch,
					s_consts.tile.random.texture_center_stretch
				),
										ts->tex_coord.y + m_map->GetRandom()->GetFloat(
											-s_consts.tile.random.texture_center_stretch,
											s_consts.tile.random.texture_center_stretch
										)
			}, // center
			{   ts->tex_coord.x1 + ofs, ts->tex_coord.y2 - ofs }, // left
			{   ts->tex_coord.x1 + ofs, ts->tex_coord.y1 + ofs }, // top
			{   ts->tex_coord.x2 - ofs, ts->tex_coord.y1 + ofs }, // right
			{   ts->tex_coord.x2 - ofs, ts->tex_coord.y2 - ofs } // bottom
		};

	}
}

}
}
}
}
