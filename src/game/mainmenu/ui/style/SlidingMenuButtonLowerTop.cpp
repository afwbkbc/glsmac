#include "SlidingMenuButtonLowerTop.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuButtonLowerTop::SetStyle() {
	Set( A_LEFT, 0 );
	Set( A_TOP, -2 );
	SetTexture( A_TEXTURE, g_engine->GetTextureLoader()->LoadTexture(
		"console_x.pcx",
			708, 1, 715, 354,
		loader::texture::TextureLoader::LT_ROTATE |
		loader::texture::TextureLoader::LT_FLIPV |
		loader::texture::TextureLoader::LT_CONTRAST,
			0.8
	) );
	Set( A_SIZE_FROM_TEXTURE );
};


}
}
}
