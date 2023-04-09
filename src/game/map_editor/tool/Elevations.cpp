#include "Elevations.h"

#include "../../Game.h"

namespace game {
namespace map_editor {
namespace tool {

Elevations::Elevations( Game* game )
	: Tool( game, MapEditor::TT_ELEVATIONS )
{
	//
}

const MapEditor::tiles_t Elevations::Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) {
	MapEditor::tiles_t tiles_to_reload = {};
	
	if ( tile->coord.y > 1 && tile->coord.y < m_game->GetMap()->GetHeight() - 2 ) { // editing poles will screw things up
	
		map::Tile::elevation_t elevation, change;
		
		// prevent extreme slopes
		const auto el = map::s_consts.tile.maximum_allowed_slope_elevation;
		const auto f_change_corner =
			[ this, &tile, &mode, &elevation, &change, &el ]
			( map::Tile::elevation_t* corner )
		-> void {
			elevation = *corner;
			change = (map::Tile::elevation_t)m_game->GetRandom()->GetUInt( elevation_change_min, elevation_change_max );
			if ( mode == MapEditor::DM_DEC ) {
				elevation = std::max< map::Tile::elevation_t >( map::Tile::ELEVATION_MIN, elevation - change );
			}
			else if ( mode == MapEditor::DM_INC ) {
				elevation = std::min< map::Tile::elevation_t >( map::Tile::ELEVATION_MAX, elevation + change );
			}
			for ( auto& n : tile->neighbours ) {
				elevation = std::min< map::Tile::elevation_t >( elevation, *n->elevation.center + el );
				elevation = std::max< map::Tile::elevation_t >( elevation, *n->elevation.center - el );
			}
			if ( mode == MapEditor::DM_DEC ) {
				*corner = std::min< map::Tile::elevation_t >( *corner, elevation );
			}
			else if ( mode == MapEditor::DM_INC ) {
				*corner = std::max< map::Tile::elevation_t >( *corner, elevation );
			}
		};
		
		f_change_corner( tile->elevation.left );
		f_change_corner( tile->elevation.top );
		f_change_corner( tile->elevation.right );
		f_change_corner( tile->elevation.bottom );
		
		tile->Update();
		
		// tile can be either full-underwater or full-land
		for ( auto& corner : tile->elevation.corners ) {
			if ( *tile->elevation.center > 0 != *corner > 0 ) {
				*corner = -*corner;
			}
		}
		tile->Update();

		// update neighbour tiles because they share some corners
		for ( auto& n : tile->neighbours ) {
			n->Update();
		}

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
