#include "Faction.h"

#include "types/texture/Texture.h"
#include "gse/value/String.h"
#include "gse/value/Bool.h"
#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "resource/ResourceManager.h"
#include "util/String.h"

namespace game {
namespace backend {
namespace faction {

Faction::Faction() {
	//
}

Faction::Faction( const std::string& id, const std::string& name )
	: m_id( id )
	, m_name( name ) {
	//
}

const types::Buffer Faction::Serialize() const {
	types::Buffer buf;

	buf.WriteString( m_id );
	buf.WriteString( m_name );

	buf.WriteColor( m_colors.text );
	buf.WriteColor( m_colors.text_shadow );
	buf.WriteColor( m_colors.border );

	buf.WriteString( m_bases_render.file );
	buf.WriteInt( m_bases_render.grid_x );
	buf.WriteInt( m_bases_render.grid_y );
	buf.WriteInt( m_bases_render.cell_width );
	buf.WriteInt( m_bases_render.cell_height );
	buf.WriteInt( m_bases_render.cell_cx );
	buf.WriteInt( m_bases_render.cell_cy );
	buf.WriteInt( m_bases_render.cell_padding );
	buf.WriteFloat( m_bases_render.scale_x );
	buf.WriteFloat( m_bases_render.scale_y );

	return buf;
}

void Faction::Unserialize( types::Buffer buf ) {

	m_id = buf.ReadString();
	m_name = buf.ReadString();

	m_colors.text = buf.ReadColor();
	m_colors.text_shadow = buf.ReadColor();
	m_colors.border = buf.ReadColor();

	m_bases_render.file = buf.ReadString();
	m_bases_render.grid_x = buf.ReadInt();
	m_bases_render.grid_y = buf.ReadInt();
	m_bases_render.cell_width = buf.ReadInt();
	m_bases_render.cell_height = buf.ReadInt();
	m_bases_render.cell_cx = buf.ReadInt();
	m_bases_render.cell_cy = buf.ReadInt();
	m_bases_render.cell_padding = buf.ReadInt();
	m_bases_render.scale_x = buf.ReadFloat();
	m_bases_render.scale_y = buf.ReadFloat();

}

WRAPIMPL_BEGIN( Faction )
	WRAPIMPL_PROPS
			{
				"id",
				VALUE( gse::value::String, , m_id )
			},
			{
				"name",
				VALUE( gse::value::String, , m_name )
			},
			{
				"text_color",
				m_colors.text.Wrap( GSE_CALL ),
			},
			{
				"is_naval",
				VALUE( gse::value::Bool, , m_flags & Faction::FF_NAVAL )
			},
			{
				"is_progenitor",
				VALUE( gse::value::Bool, , m_flags & Faction::FF_PROGENITOR )
			},
		};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Faction )

}
}
}
