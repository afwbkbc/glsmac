#include "Faction.h"

#include "types/texture/Texture.h"
#include "gse/type/String.h"
#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "resource/ResourceManager.h"

namespace game {
namespace rules {

const Faction::faction_flag_t Faction::FF_NONE = 0;
const Faction::faction_flag_t Faction::FF_PROGENITOR = 1 << 0;

Faction::Faction() {
	//
}

Faction::Faction( const std::string& id, const std::string& name )
	: m_id( id )
	, m_name( name ) {
	//
}

void Faction::ImportPCX( const std::string& pcx_file ) {
	const auto* texture = g_engine->GetTextureLoader()->LoadCustomTexture( pcx_file );
	m_colors.text = types::Color::FromRGBA( texture->GetPixel( 5, 755 ) );
	m_colors.border = types::Color::FromRGBA( texture->GetPixel( 162, 750 ) );
}

const types::Buffer Faction::Serialize() const {
	types::Buffer buf;

	buf.WriteString( m_id );
	buf.WriteString( m_name );
	buf.WriteColor( m_colors.text );
	buf.WriteColor( m_colors.border );

	return buf;
}

void Faction::Unserialize( types::Buffer buf ) {

	m_id = buf.ReadString();
	m_name = buf.ReadString();
	m_colors.text = buf.ReadColor();
	m_colors.border = buf.ReadColor();

}

WRAPIMPL_BEGIN( Faction, CLASS_FACTION )
	gse::type::object_properties_t color_obj = {
		{
			"text",
			m_colors.text.Wrap()
		},
		{
			"border",
			m_colors.border.Wrap()
		}
	};
	WRAPIMPL_PROPS {
		{
			"id",
			VALUE( gse::type::String, m_id )
		},
		{
			"name",
			VALUE( gse::type::String, m_name )
		},
		{
			"colors",
			VALUE( gse::type::Object, color_obj )
		},
	};
WRAPIMPL_END_PTR( Faction )

UNWRAPIMPL_PTR( Faction )

}
}
