#include "Square.h"

#include "game/Game.h"
#include "game/map/Map.h"
#include "game/map/tile/Tiles.h"
#include "game/map/tile/Tile.h"

namespace game {
namespace map_editor {
namespace brush {

Square::Square( Game* game, const brush_type_t type, const uint16_t width )
	: Brush( game, type )
	, m_width( width ) {
	//
}

const tiles_t Square::Draw( map::tile::Tile* center_tile ) {
	auto* map = m_game->GetMap()->GetTilesPtr();
	tiles_t tiles = {};
	tiles.reserve( m_width * m_width );
	// order is important, start with sides and move towards center for greatest effect
	// TODO: shuffle

	const auto f_add_tile_maybe = [ &tiles, &map ]( const types::Vec2< ssize_t > coords ) -> void {
		if (
			coords.x >= 0 &&
				coords.y >= 0 &&
				coords.x <= map->GetWidth() - 1 &&
				coords.y <= map->GetHeight() - 1
			) {
			tiles.push_back( &map->At( coords.x, coords.y ) );
		}
	};

	for ( int y = -m_width ; y <= m_width ; y++ ) {
		for ( int x = -m_width ; x <= m_width ; x++ ) {
			if ( ( y & 1 ) != ( x & 1 ) ) {
				continue;
			}

			if ( x == 0 && y == 0 ) {
				continue; // center tile will be added last
			}
			if ( abs( x ) + abs( y ) > m_width ) {
				continue; // draw diagonal square instead of full square
			}

			f_add_tile_maybe(
				{
					(ssize_t)center_tile->coord.x + x,
					(ssize_t)center_tile->coord.y - y
				}
			);
			f_add_tile_maybe(
				{
					(ssize_t)center_tile->coord.x - x,
					(ssize_t)center_tile->coord.y + y
				}
			);
			f_add_tile_maybe(
				{
					(ssize_t)center_tile->coord.x + x,
					(ssize_t)center_tile->coord.y + y
				}
			);
			f_add_tile_maybe(
				{
					(ssize_t)center_tile->coord.x - x,
					(ssize_t)center_tile->coord.y - y
				}
			);
		}
	}
	tiles.push_back( center_tile );
	return tiles;
}

}
}
}
