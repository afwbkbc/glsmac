#include "Render.h"

#include "SpriteRender.h"

namespace game {
namespace unit {

Render::Render( const render_type_t type )
	: m_type( type ) {
	//
}

void Render::Serialize( types::Buffer& buf, const Render* render ) {
	buf.WriteInt( render->m_type );
	switch ( render->m_type ) {
		case RT_SPRITE: {
			SpriteRender::Serialize( buf, (SpriteRender*)render );
			break;
		}
		default:
			THROW( "unknown render type on write: " + std::to_string( render->m_type ) );
	}
}

Render* Render::Unserialize( types::Buffer& buf ) {
	const auto render_type = (render_type_t)buf.ReadInt();
	switch ( render_type ) {
		case RT_SPRITE:
			return SpriteRender::Unserialize( buf );
		default:
			THROW( "unknown render type on read: " + std::to_string( render_type ) );
	}
}

}
}
