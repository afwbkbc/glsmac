#include "SpriteRender.h"

namespace game {
namespace unit {

SpriteRender::SpriteRender( const std::string& file, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const uint32_t cx, const uint32_t cy )
	: Render( RT_SPRITE )
	, m_file( file )
	, m_x( x )
	, m_y( y )
	, m_w( w )
	, m_h( h )
	, m_cx( cx )
	, m_cy( cy )
	, m_cshift_x( (float)( cx - x ) / w * 2 - 1.0f )
	, m_cshift_y( (float)( cy - y ) / h * 2 - 1.0f ) {
	//
}

const types::Vec3 SpriteRender::GetSpawnCoords( const float tile_x, const float tile_y, const map::TileState::tile_vertices_t& tile_coords ) const {
	float cx = tile_x;
	if ( m_cshift_x < 0 ) {
		cx -= ( cx - tile_coords.left.x ) * m_cshift_x;
	}
	if ( m_cshift_x > 0 ) {
		cx += ( tile_coords.right.x - cx ) * m_cshift_x;
	}
	float cy = tile_y;
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
	};
}

void SpriteRender::Serialize( types::Buffer& buf, const SpriteRender* render ) {
	buf.WriteString( render->m_file );
	buf.WriteInt( render->m_x );
	buf.WriteInt( render->m_y );
	buf.WriteInt( render->m_w );
	buf.WriteInt( render->m_h );
	buf.WriteInt( render->m_cx );
	buf.WriteInt( render->m_cy );
}

SpriteRender* SpriteRender::Unserialize( types::Buffer& buf ) {
	const auto file = buf.ReadString();
	const auto x = buf.ReadInt();
	const auto y = buf.ReadInt();
	const auto w = buf.ReadInt();
	const auto h = buf.ReadInt();
	const auto cx = buf.ReadInt();
	const auto cy = buf.ReadInt();
	return new SpriteRender( file, x, y, w, h, cx, cy );
}

}
}
