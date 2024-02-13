#include "SpriteRender.h"

namespace game {
namespace unit {

SpriteRender::SpriteRender( const std::string& file, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h )
	: Render( RT_SPRITE )
	, m_file( file )
	, m_x( x )
	, m_y( y )
	, m_w( w )
	, m_h( h ) {
	//
}

void SpriteRender::Serialize( types::Buffer& buf, const SpriteRender* render ) {
	buf.WriteString( render->m_file );
	buf.WriteInt( render->m_x );
	buf.WriteInt( render->m_y );
	buf.WriteInt( render->m_w );
	buf.WriteInt( render->m_h );
}

SpriteRender* SpriteRender::Unserialize( types::Buffer& buf ) {
	const auto file = buf.ReadString();
	const auto x = buf.ReadInt();
	const auto y = buf.ReadInt();
	const auto w = buf.ReadInt();
	const auto h = buf.ReadInt();
	return new SpriteRender( file, x, y, w, h );
}

}
}
