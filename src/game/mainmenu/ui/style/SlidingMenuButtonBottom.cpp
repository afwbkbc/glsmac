#include "SlidingMenuButtonBottom.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuButtonBottom::SetStyle() {
	Set( A_LEFT, 6 );
	Set( A_TOP, 40 );
	SetTexture( A_TEXTURE, g_engine->GetTextureLoader()->LoadTexture(
		"console_x.pcx",
			699, 1, 706, 293,
		loader::texture::TextureLoader::LT_ROTATE |
		loader::texture::TextureLoader::LT_FLIPV |
		loader::texture::TextureLoader::LT_CONTRAST,
			0.2
	) );
	Set( A_SIZE_FROM_TEXTURE );
};


}
}
}
