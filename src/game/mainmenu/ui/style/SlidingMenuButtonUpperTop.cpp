#include "SlidingMenuButtonUpperTop.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuButtonUpperTop::SetStyle() {
	Set( A_LEFT, 200 );
	Set( A_TOP, -8 );
	SetTexture( A_TEXTURE, g_engine->GetTextureLoader()->LoadTexture(
		"console_x.pcx",
			708, 1, 715, 354,
		loader::texture::TextureLoader::LT_ROTATE |
		loader::texture::TextureLoader::LT_FLIPV |
		loader::texture::TextureLoader::LT_CONTRAST,
			1.5
	) );
	Set( A_SIZE_FROM_TEXTURE );
};


}
}
}
