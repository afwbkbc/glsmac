#include "Coastlines1.h"

#include "game/map/Map.h"
#include "game/map/MapState.h"
#include "game/map/Consts.h"
#include "game/map/tile/Tile.h"
#include "game/map/tile/TileState.h"
#include "util/random/Random.h"
#include "util/Perlin.h"

namespace game {
namespace map {
namespace module {

Coastlines1::Coastlines1( Map* const map )
	: Coastlines( map ) {
	NEW( m_perlin, util::Perlin, map->GetRandom()->GetUInt( 0, UINT32_MAX - 1 ) );
}

Coastlines1::~Coastlines1() {
	DELETE( m_perlin );
}

void Coastlines1::GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) {

	// it is what it is

	float cw = 0.06f;
	float cwc = 1.5f;
	float tcw = cw * 2.0f;
	float tcwc = cwc * 0.85f;
	float tcww = tcw * s_consts.tc.texture_pcx.dimensions.x;
	float tcwh = tcw * s_consts.tc.texture_pcx.dimensions.y;
	const types::texture::add_flag_t coastline_mode = types::texture::AM_MERGE | types::texture::AM_INVERT;

	std::vector< coastline_corner_t > coastline_corners = {};
	coastline_corner_t coastline_corner_tmp = {};

	if ( !tile->is_water_tile ) {

		if ( tile->W->is_water_tile || tile->NW->is_water_tile || tile->SW->is_water_tile ) {
			//ts->layers[ tile::LAYER_LAND ].colors.left = s_consts.coastlines.coastline_tint;
			if ( tile->W->is_water_tile && ( tile->NW->is_water_tile || tile->SW->is_water_tile ) ) {
				ts->layers[ tile::LAYER_LAND ].coords.left.x += cw;
				if ( !tile->NW->is_water_tile ) {
					ts->layers[ tile::LAYER_LAND ].coords.left.y -= cw;
					ts->layers[ tile::LAYER_LAND ].tex_coords.left.y -= tcwh;
				}
				else if ( !tile->SW->is_water_tile ) {
					ts->layers[ tile::LAYER_LAND ].coords.left.y += cw;
					ts->layers[ tile::LAYER_LAND ].tex_coords.left.x += tcww;
				}
				else {
					ts->layers[ tile::LAYER_LAND ].coords.left.x += cw * cwc;
					ts->layers[ tile::LAYER_LAND ].tex_coords.left.y -= tcwh * tcwc;
					ts->layers[ tile::LAYER_LAND ].tex_coords.left.x += tcww * tcwc;
				}
			}
		}
		if ( tile->N->is_water_tile || tile->NW->is_water_tile || tile->NE->is_water_tile ) {
			//ts->layers[ tile::LAYER_LAND ].colors.top = s_consts.coastlines.coastline_tint;
			if ( tile->N->is_water_tile && ( tile->NW->is_water_tile || tile->NE->is_water_tile ) ) {
				ts->layers[ tile::LAYER_LAND ].coords.top.y += cw;
				if ( !tile->NW->is_water_tile ) {
					ts->layers[ tile::LAYER_LAND ].coords.top.x -= cw;
					ts->layers[ tile::LAYER_LAND ].tex_coords.top.y += tcwh;
				}
				else if ( !tile->NE->is_water_tile ) {
					ts->layers[ tile::LAYER_LAND ].coords.top.x += cw;
					ts->layers[ tile::LAYER_LAND ].tex_coords.top.x += tcww;
				}
				else {
					ts->layers[ tile::LAYER_LAND ].coords.top.y += cw * cwc;
					ts->layers[ tile::LAYER_LAND ].tex_coords.top.x += tcww * tcwc;
					ts->layers[ tile::LAYER_LAND ].tex_coords.top.y += tcwh * tcwc;
				}
			}
		}
		if ( tile->E->is_water_tile || tile->NE->is_water_tile || tile->SE->is_water_tile ) {
			//ts->layers[ tile::LAYER_LAND ].colors.right = s_consts.coastlines.coastline_tint;
			if ( tile->E->is_water_tile && ( tile->NE->is_water_tile || tile->SE->is_water_tile ) ) {
				ts->layers[ tile::LAYER_LAND ].coords.right.x -= cw;
				if ( !tile->NE->is_water_tile ) {
					ts->layers[ tile::LAYER_LAND ].coords.right.y -= cw;
					ts->layers[ tile::LAYER_LAND ].tex_coords.right.x -= tcww;
				}
				else if ( !tile->SE->is_water_tile ) {
					ts->layers[ tile::LAYER_LAND ].coords.right.y += cw;
					ts->layers[ tile::LAYER_LAND ].tex_coords.right.y += tcwh;
				}
				else {
					ts->layers[ tile::LAYER_LAND ].coords.right.x -= cw * cwc;
					ts->layers[ tile::LAYER_LAND ].tex_coords.right.x -= tcww * tcwc;
					ts->layers[ tile::LAYER_LAND ].tex_coords.right.y += tcwh * tcwc;
				}
			}
		}
		if ( tile->S->is_water_tile || tile->SW->is_water_tile || tile->SE->is_water_tile ) {
			//ts->layers[ tile::LAYER_LAND ].colors.bottom = s_consts.coastlines.coastline_tint;
			if ( tile->S->is_water_tile && ( tile->SW->is_water_tile || tile->SE->is_water_tile ) ) {
				ts->layers[ tile::LAYER_LAND ].coords.bottom.y -= cw;
				if ( !tile->SW->is_water_tile ) {
					ts->layers[ tile::LAYER_LAND ].coords.bottom.x -= cw;
					ts->layers[ tile::LAYER_LAND ].tex_coords.bottom.x -= tcww;
				}
				else if ( !tile->SE->is_water_tile ) {
					ts->layers[ tile::LAYER_LAND ].coords.bottom.x += cw;
					ts->layers[ tile::LAYER_LAND ].tex_coords.bottom.y -= tcwh;
				}
				else {
					ts->layers[ tile::LAYER_LAND ].coords.bottom.y -= cw * cwc;
					ts->layers[ tile::LAYER_LAND ].tex_coords.bottom.x -= tcww * tcwc;
					ts->layers[ tile::LAYER_LAND ].tex_coords.bottom.y -= tcwh * tcwc;
				}
			}
		}
		ts->layers[ tile::LAYER_LAND ].tex_coords.center.x = ( ts->layers[ tile::LAYER_LAND ].tex_coords.left.x + ts->layers[ tile::LAYER_LAND ].tex_coords.top.x + ts->layers[ tile::LAYER_LAND ].tex_coords.right.x + ts->layers[ tile::LAYER_LAND ].tex_coords.bottom.x ) / 4;
		ts->layers[ tile::LAYER_LAND ].tex_coords.center.y = ( ts->layers[ tile::LAYER_LAND ].tex_coords.left.y + ts->layers[ tile::LAYER_LAND ].tex_coords.top.y + ts->layers[ tile::LAYER_LAND ].tex_coords.right.y + ts->layers[ tile::LAYER_LAND ].tex_coords.bottom.y ) / 4;

	}
	else { // is_water_tile

	}

	ts->layers[ tile::LAYER_LAND ].coords.center = {
		( ts->layers[ tile::LAYER_LAND ].coords.left.x + ts->layers[ tile::LAYER_LAND ].coords.right.x ) / 2,
		( ts->layers[ tile::LAYER_LAND ].coords.top.y + ts->layers[ tile::LAYER_LAND ].coords.bottom.y ) / 2,
		s_consts.tile.elevation_to_vertex_z.Clamp( ts->elevations.center )
	};

	// coast water texture
	if ( !tile->is_water_tile && (
		tile->W->is_water_tile ||
			tile->NW->is_water_tile ||
			tile->N->is_water_tile ||
			tile->NE->is_water_tile ||
			tile->E->is_water_tile ||
			tile->SE->is_water_tile ||
			tile->S->is_water_tile ||
			tile->SW->is_water_tile
	) ) {
		/*m_map->AddTexture(
			tile::LAYER_WATER_SURFACE,
			s_consts.tc.texture_pcx.water[ 0 ],
			AM_DEFAULT,
			RandomRotate(),
			s_consts.coastlines.coast_water_alpha
		);*/

		ts->layers[ tile::LAYER_WATER_SURFACE ].colors.center =
			ts->layers[ tile::LAYER_WATER_SURFACE ].colors.left =
				ts->layers[ tile::LAYER_WATER_SURFACE ].colors.top =
					ts->layers[ tile::LAYER_WATER_SURFACE ].colors.right =
						ts->layers[ tile::LAYER_WATER_SURFACE ].colors.bottom =
							s_consts.coastlines.coastline_tint;
	}

	if ( tile->is_water_tile && (
		!tile->W->is_water_tile ||
			!tile->NW->is_water_tile ||
			!tile->N->is_water_tile ||
			!tile->NE->is_water_tile ||
			!tile->E->is_water_tile ||
			!tile->SE->is_water_tile ||
			!tile->S->is_water_tile ||
			!tile->SW->is_water_tile
	) ) {

		ts->layers[ tile::LAYER_WATER_SURFACE_EXTRA ].colors.center.value.alpha = s_consts.coastlines.coast_water_center_alpha;

		if ( tile->W->is_water_tile && tile->NW->is_water_tile && tile->SW->is_water_tile ) {
			ts->layers[ tile::LAYER_WATER_SURFACE_EXTRA ].colors.left.value.alpha = 0.0f;
		}
		else {
			ts->layers[ tile::LAYER_WATER_SURFACE_EXTRA ].colors.left = s_consts.coastlines.coastline_tint;
		}

		if ( tile->N->is_water_tile && tile->NW->is_water_tile && tile->NE->is_water_tile ) {
			ts->layers[ tile::LAYER_WATER_SURFACE_EXTRA ].colors.top.value.alpha = 0.0f;
		}
		else {
			ts->layers[ tile::LAYER_WATER_SURFACE_EXTRA ].colors.top = s_consts.coastlines.coastline_tint;
		}

		if ( tile->E->is_water_tile && tile->NE->is_water_tile && tile->SE->is_water_tile ) {
			ts->layers[ tile::LAYER_WATER_SURFACE_EXTRA ].colors.right.value.alpha = 0.0f;
		}
		else {
			ts->layers[ tile::LAYER_WATER_SURFACE_EXTRA ].colors.right = s_consts.coastlines.coastline_tint;
		}

		if ( tile->S->is_water_tile && tile->SW->is_water_tile && tile->SE->is_water_tile ) {
			ts->layers[ tile::LAYER_WATER_SURFACE_EXTRA ].colors.bottom.value.alpha = 0.0f;
		}
		else {
			ts->layers[ tile::LAYER_WATER_SURFACE_EXTRA ].colors.bottom = s_consts.coastlines.coastline_tint;
		}

		m_map->AddTexture(
			tile::LAYER_WATER_SURFACE_EXTRA,
			s_consts.tc.texture_pcx.water[ 0 ],
			types::texture::AM_DEFAULT,
			RandomRotate(),
			s_consts.coastlines.coast_water_alpha
		);
	}

	if ( ts->has_water ) {

		// corners on land tiles
		if ( ts->is_coastline_corner ) {

			m_map->AddTexture(
				tile::LAYER_WATER_SURFACE_EXTRA,
				s_consts.tc.texture_pcx.water[ 0 ],
				types::texture::AM_DEFAULT,
				RandomRotate(),
				s_consts.coastlines.coast_water_alpha
			);

			// TODO: refactor?
			auto add_flags = types::texture::AM_MERGE | types::texture::AM_COASTLINE_BORDER;
			if (
				tile->W->is_water_tile &&
					( tile->SW->is_water_tile || tile->coord.y == ms->dimensions.y - 1 ) &&
					( tile->NW->is_water_tile || tile->coord.y == 0 )
				) {
				add_flags |= types::texture::AM_ROUND_LEFT;
			}
			if (
				( tile->N->is_water_tile || tile->coord.y <= 1 ) &&
					( tile->NW->is_water_tile || tile->coord.y == 0 ) &&
					( tile->NE->is_water_tile || tile->coord.y == 0 )
				) {
				add_flags |= types::texture::AM_ROUND_TOP;
			}
			if (
				tile->E->is_water_tile &&
					( tile->SE->is_water_tile || tile->coord.y == ms->dimensions.y - 1 ) &&
					( tile->NE->is_water_tile || tile->coord.y == 0 )
				) {
				add_flags |= types::texture::AM_ROUND_RIGHT;
			}
			if (
				( tile->S->is_water_tile || tile->coord.y <= ms->dimensions.y - 2 ) &&
					( tile->SE->is_water_tile || tile->coord.y == ms->dimensions.y - 1 ) &&
					( tile->SW->is_water_tile || tile->coord.y == ms->dimensions.y - 1 )
				) {
				add_flags |= types::texture::AM_ROUND_BOTTOM;
			}

			// coastline tint
/*			if ( tile->W->is_water_tile || tile->SW->is_water_tile || tile->NW->is_water_tile ) {
				ts->layers[ tile::LAYER_WATER ].colors.left = s_consts.coastlines.coastline_tint;
			}
			if ( tile->N->is_water_tile || tile->NW->is_water_tile || tile->NE->is_water_tile ) {
				ts->layers[ tile::LAYER_WATER ].colors.top = s_consts.coastlines.coastline_tint;
			}
			if ( tile->E->is_water_tile || tile->SE->is_water_tile || tile->NE->is_water_tile ) {
				ts->layers[ tile::LAYER_WATER ].colors.right = s_consts.coastlines.coastline_tint;
			}
			if ( tile->S->is_water_tile || tile->SW->is_water_tile || tile->SE->is_water_tile ) {
				ts->layers[ tile::LAYER_WATER ].colors.bottom = s_consts.coastlines.coastline_tint;
			}*/

			// copy texture from land
			m_map->CopyTexture( tile::LAYER_LAND, tile::LAYER_WATER, add_flags, 0, 1.0f, m_perlin );

		}

		coastline_corners.clear();

		// corners on water tiles
		if ( tile->is_water_tile ) {

			if ( !tile->SW->is_water_tile && !tile->NW->is_water_tile ) {
				coastline_corner_tmp = {};
				coastline_corner_tmp.flags = types::texture::AM_ROUND_LEFT;
				//ts->layers[ tile::LAYER_WATER ].colors.left = s_consts.coastlines.coastline_tint;
				if ( !tile->W->is_water_tile ) {
					coastline_corner_tmp.can_mirror = true;
					if ( tile->coord.x >= 2 ) {
						coastline_corner_tmp.msx = tile->coord.x - 2;
					}
					else {
						coastline_corner_tmp.msx = ms->dimensions.x - 2 + tile->coord.x;
					}
					coastline_corner_tmp.msy = tile->coord.y;
				}
				else {
					coastline_corner_tmp.maybe_mirror_sw = true;
					coastline_corner_tmp.maybe_mirror_nw = true;
				}
				coastline_corners.push_back( coastline_corner_tmp );
			}

			if ( !tile->NW->is_water_tile && !tile->NE->is_water_tile ) {
				coastline_corner_tmp = {};
				coastline_corner_tmp.flags = types::texture::AM_ROUND_TOP;
				//ts->layers[ tile::LAYER_WATER ].colors.top = s_consts.coastlines.coastline_tint;
				if ( !tile->N->is_water_tile ) {
					if ( tile->coord.y >= 2 ) {
						coastline_corner_tmp.can_mirror = true;
						coastline_corner_tmp.msx = tile->coord.x;
						coastline_corner_tmp.msy = tile->coord.y - 2;
					}
				}
				else {
					coastline_corner_tmp.maybe_mirror_nw = true;
					coastline_corner_tmp.maybe_mirror_ne = true;
				}
				coastline_corners.push_back( coastline_corner_tmp );
			}
			if ( !tile->SE->is_water_tile && !tile->NE->is_water_tile ) {
				coastline_corner_tmp = {};
				coastline_corner_tmp.flags = types::texture::AM_ROUND_RIGHT;
				//ts->layers[ tile::LAYER_WATER ].colors.right = s_consts.coastlines.coastline_tint;
				if ( !tile->E->is_water_tile ) {
					coastline_corner_tmp.can_mirror = true;
					if ( tile->coord.x < ms->dimensions.x - 2 ) {
						coastline_corner_tmp.msx = tile->coord.x + 2;
					}
					else {
						coastline_corner_tmp.msx = tile->coord.x % 2;
					}
					coastline_corner_tmp.msy = tile->coord.y;
				}
				else {
					coastline_corner_tmp.maybe_mirror_se = true;
					coastline_corner_tmp.maybe_mirror_ne = true;
				}
				coastline_corners.push_back( coastline_corner_tmp );
			}
			if ( !tile->SE->is_water_tile && !tile->SW->is_water_tile ) {
				coastline_corner_tmp = {};
				coastline_corner_tmp.flags = types::texture::AM_ROUND_BOTTOM;
				//ts->layers[ tile::LAYER_WATER ].colors.bottom = s_consts.coastlines.coastline_tint;
				if ( !tile->S->is_water_tile ) {
					if ( tile->coord.y < ms->dimensions.y - 2 ) {
						coastline_corner_tmp.can_mirror = true;
						coastline_corner_tmp.msx = tile->coord.x;
						coastline_corner_tmp.msy = tile->coord.y + 2;
					}
				}
				else {
					coastline_corner_tmp.maybe_mirror_se = true;
					coastline_corner_tmp.maybe_mirror_sw = true;
				}
				coastline_corners.push_back( coastline_corner_tmp );
			}

			for ( auto& c : coastline_corners ) {

				if ( c.can_mirror ) {
					c.mirror_mode = types::texture::AM_MIRROR_X | types::texture::AM_MIRROR_Y;
				}

				if ( !c.can_mirror && tile->coord.y >= 1 ) {
					c.msy = tile->coord.y - 1;
					if ( !c.can_mirror && coastline_corner_tmp.maybe_mirror_nw ) {
						if ( tile->coord.x >= 1 ) {
							c.msx = tile->coord.x - 1;
						}
						else {
							c.msx = ms->dimensions.x - 1;
						}
						c.mirror_mode = types::texture::AM_MIRROR_X;
						c.can_mirror = true;
					}
					if ( !c.can_mirror && coastline_corner_tmp.maybe_mirror_ne ) {
						if ( tile->coord.x < ms->dimensions.x - 1 ) {
							c.msx = tile->coord.x + 1;
						}
						else {
							c.msx = 1;
						}
						c.mirror_mode = types::texture::AM_MIRROR_Y;
						c.can_mirror = true;
					}
				}
				if ( !c.can_mirror && tile->coord.y < ms->dimensions.y - 1 ) {
					c.msy = tile->coord.y + 1;
					if ( !c.can_mirror && coastline_corner_tmp.maybe_mirror_sw ) {
						if ( tile->coord.x >= 1 ) {
							c.msx = tile->coord.x - 1;
						}
						else {
							c.msx = ms->dimensions.x - 1;
						}
						c.mirror_mode = types::texture::AM_MIRROR_X;
						c.can_mirror = true;
					}
					if ( !c.can_mirror && coastline_corner_tmp.maybe_mirror_se ) {
						if ( tile->coord.x < ms->dimensions.x - 1 ) {
							c.msx = tile->coord.x + 1;
						}
						else {
							c.msx = 1;
						}
						c.mirror_mode = types::texture::AM_MIRROR_Y;
						c.can_mirror = true;
					}
				}

				if ( c.can_mirror ) {
					// mirror opposite tile
					m_map->CopyTextureDeferred(
						tile::LAYER_LAND,
						c.msx * s_consts.tc.texture_pcx.dimensions.x,
						c.msy * s_consts.tc.texture_pcx.dimensions.y,
						tile::LAYER_WATER,
						coastline_mode | c.flags | c.mirror_mode,
						0
					);
				}
				else {
					// use default texture
					m_map->AddTexture(
						tile::LAYER_WATER,
						s_consts.tc.texture_pcx.arid[ 0 ],
						coastline_mode | c.flags,
						RandomRotate()
					);
				}
			}
		}
	}
}

}
}
}
