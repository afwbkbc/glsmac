#include "PopDef.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "resource/ResourceManager.h"

namespace game {
namespace frontend {
namespace base {

PopDef::PopDef(
	const std::string& name,
	const backend::base::pop_render_infos_t& renders_human,
	const backend::base::pop_render_infos_t& renders_progenitor
)
	: m_name( name ) {
	auto* tl = g_engine->GetTextureLoader();
	auto* rm = g_engine->GetResourceManager();
#define X( _r ) \
    for ( const auto& r : renders_##_r ) { \
        m_textures_##_r.push_back( tl->LoadTexture( rm->GetResource( r.file ), r.x, r.y, r.x + r.width, r.y + r.height ) ); \
    }
	X( human )
	X( progenitor )
#undef X
}

types::texture::Texture* PopDef::GetTexture( const uint8_t variant, const bool is_progenitor ) const {
	const auto& textures = is_progenitor
		? m_textures_progenitor
		: m_textures_human;
	return textures.at( std::min( (size_t)variant, textures.size() - 1 ) );
}

}
}
}