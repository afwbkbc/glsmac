#include "Elevations.h"

#include "../../World.h"

namespace game {
namespace world {
namespace map_editor {
namespace tool {

Elevations::Elevations( World* world )
	: Tool( world, MapEditor::TT_ELEVATIONS )
{
	//
}

const MapEditor::tiles_to_reload_t Elevations::Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) {
	MapEditor::tiles_to_reload_t tiles_to_reload = {};
	
	if ( tile->coord.y > 1 && tile->coord.y < m_world->GetMap()->GetHeight() - 2 ) { // editing poles will screw things up
	
		for ( auto& corner : tile->elevation.corners ) {
			const auto change = (Tile::elevation_t)m_world->GetRandom()->GetUInt( elevation_change_min, elevation_change_max );
			if ( mode == MapEditor::DM_DEC ) {
				*corner = std::max< Tile::elevation_t >( Tile::ELEVATION_MIN, *corner - change );
			}
			else if ( mode == MapEditor::DM_INC ) {
				*corner = std::min< Tile::elevation_t >( Tile::ELEVATION_MAX, *corner + change );
			}
		}
		
		// prevent extreme slopes
		const auto el = Map::s_consts.tile.maximum_allowed_slope_elevation / 2; // / 2 because relative to center
		#define x( _s, _d ) \
			*tile->elevation._s = std::min< Tile::elevation_t >( *tile->elevation._s, *tile->_d->elevation.center + el ); \
			*tile->elevation._s = std::max< Tile::elevation_t >( *tile->elevation._s, *tile->_d->elevation.center - el ); \
			tile->_d->Update()
			x( left, W );
			x( top, N );
			x( right, E );
			x( bottom, S );
		#undef x
		
		tile->Update();

		// tile can be either full-underwater or full-land
		for ( auto& corner : tile->elevation.corners ) {
			if ( *tile->elevation.center > 0 != *corner > 0 ) {
				*corner = -*corner;
			}
		}
		tile->Update();

		// we need to reload quite a few tiles because coastlines 2 tiles away may need to be redrawn or removed
		// TODO: reduce based on some conditions
		tiles_to_reload = {
			tile,
			tile->W,
				tile->W->SW,
				tile->W->W,
				tile->W->NW,
			tile->NW,
				tile->NW->NW,
			tile->N,
				tile->N->NW,
				tile->N->N,
				tile->N->NE,
			tile->NE,
				tile->NE->NE,
			tile->E,
				tile->E->NE,
				tile->E->E,
				tile->E->SE,
			tile->SE,
				tile->SE->SE,
			tile->S,
				tile->S->SE,
				tile->S->S,
				tile->S->SW,
			tile->SW,
				tile->SW->SW
		};
	}
	
	return tiles_to_reload;
}
	
}
}
}
}
