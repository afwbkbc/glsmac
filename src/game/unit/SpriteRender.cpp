#include "SpriteRender.h"

#include "game/map/tile/Tile.h"
#include "game/map/tile/TileState.h"

namespace game {
namespace unit {

SpriteRender::SpriteRender(
	const std::string& file,
	const uint32_t x,
	const uint32_t y,
	const uint32_t w,
	const uint32_t h,
	const uint32_t cx,
	const uint32_t cy,
	const uint32_t morale_based_xshift
)
	: Render( RT_SPRITE )
	, m_file( file )
	, m_x( x )
	, m_y( y )
	, m_w( w )
	, m_h( h )
	, m_cx( cx )
	, m_cy( cy )
	, m_cshift_x( (float)( cx - x ) / w * 2 - 1.0f )
	, m_cshift_y( (float)( cy - y ) / h * 2 - 1.0f )
	, m_morale_based_xshift( morale_based_xshift ) {
	//
}

const types::Vec3 SpriteRender::GetSpawnCoords( const map::tile::Tile* tile, const map::tile::TileState* ts ) const {
	return ts->layers[
		tile->is_water_tile
			? map::tile::LAYER_WATER
			: map::tile::LAYER_LAND
	].coords.center;

	/*float cx = tile_coords.center.x;
	if ( m_cshift_x < 0 ) {
		cx -= ( cx - tile_coords.left.x ) * m_cshift_x;
	}
	if ( m_cshift_x > 0 ) {
		cx += ( tile_coords.right.x - cx ) * m_cshift_x;
	}
	float cy = tile_coords.center.y;
	if ( m_cshift_y < 0 ) {
		cy += ( cy - tile_coords.top.y ) * m_cshift_y;
	}
	if ( m_cshift_y > 0 ) {
		cy -= ( tile_coords.bottom.y - cy ) * m_cshift_y;
	}
	return {
		cx,
		cy,
		tile_coords.center.z,
	};*/
}

const std::string SpriteRender::ToString( const std::string& prefix ) const {
	return (std::string)
		TS_OBJ_BEGIN( "SpriteRender" ) +
		TS_OBJ_PROP_STR( "file", m_file ) +
		TS_OBJ_PROP_NUM( "x: ", m_x ) +
		TS_OBJ_PROP_NUM( "y: ", m_y ) +
		TS_OBJ_PROP_NUM( "w: ", m_w ) +
		TS_OBJ_PROP_NUM( "h: ", m_w ) +
		TS_OBJ_PROP_NUM( "cx: ", m_cx ) +
		TS_OBJ_PROP_NUM( "cy: ", m_cy ) +
		TS_OBJ_PROP_NUM( "morale_based_xshift", m_morale_based_xshift ) +
		TS_OBJ_END();
}

void SpriteRender::Serialize( types::Buffer& buf, const SpriteRender* render ) {
	buf.WriteString( render->m_file );
	buf.WriteInt( render->m_x );
	buf.WriteInt( render->m_y );
	buf.WriteInt( render->m_w );
	buf.WriteInt( render->m_h );
	buf.WriteInt( render->m_cx );
	buf.WriteInt( render->m_cy );
	buf.WriteInt( render->m_morale_based_xshift );
}

SpriteRender* SpriteRender::Unserialize( types::Buffer& buf ) {
	const auto file = buf.ReadString();
	const auto x = buf.ReadInt();
	const auto y = buf.ReadInt();
	const auto w = buf.ReadInt();
	const auto h = buf.ReadInt();
	const auto cx = buf.ReadInt();
	const auto cy = buf.ReadInt();
	const auto morale_based_xshift = buf.ReadInt();
	return new SpriteRender( file, x, y, w, h, cx, cy, morale_based_xshift );
}

}
}
