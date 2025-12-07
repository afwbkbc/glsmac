#include "SpriteRender.h"

#include "game/backend/map/tile/Tile.h"
#include "game/backend/map/tile/TileState.h"

namespace game {
namespace backend {
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
	, m_render(
		{
			file,
			x,
			y,
			w,
			h,
			cx,
			cy,
			morale_based_xshift,
		}
	) {
	//
}

const std::string SpriteRender::ToString( const std::string& prefix ) const {
	return (std::string)
		TS_OBJ_BEGIN( "SpriteRender" ) +
		TS_OBJ_PROP_STR( "file", m_render.file ) +
		TS_OBJ_PROP_NUM( "x: ", m_render.x ) +
		TS_OBJ_PROP_NUM( "y: ", m_render.y ) +
		TS_OBJ_PROP_NUM( "w: ", m_render.w ) +
		TS_OBJ_PROP_NUM( "h: ", m_render.w ) +
		TS_OBJ_PROP_NUM( "cx: ", m_render.cx ) +
		TS_OBJ_PROP_NUM( "cy: ", m_render.cy ) +
		TS_OBJ_PROP_NUM( "morale_based_xshift", m_render.morale_based_xshift ) +
		TS_OBJ_END();
}

void SpriteRender::Serialize( types::Buffer& buf, const SpriteRender* render ) {
	buf.WriteString( render->m_render.file );
	buf.WriteInt( render->m_render.x );
	buf.WriteInt( render->m_render.y );
	buf.WriteInt( render->m_render.w );
	buf.WriteInt( render->m_render.h );
	buf.WriteInt( render->m_render.cx );
	buf.WriteInt( render->m_render.cy );
	buf.WriteInt( render->m_render.morale_based_xshift );
}

SpriteRender* SpriteRender::Deserialize( types::Buffer& buf ) {
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
}
