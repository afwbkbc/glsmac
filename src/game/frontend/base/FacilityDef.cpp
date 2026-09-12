#include "FacilityDef.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "resource/ResourceManager.h"

namespace game {
namespace frontend {
namespace base {

FacilityDef::FacilityDef(
	const std::string& name,
	const backend::base::render_info_t& render
)
	: m_name( name ) {
	auto* tl = g_engine->GetTextureLoader();
	auto* rm = g_engine->GetResourceManager();
	m_texture = tl->LoadCustomTexture( render.file, render.x, render.y, render.x + render.width, render.y + render.height );
}

types::texture::Texture* FacilityDef::GetTexture() const {
	return m_texture;
}

}
}
}