#include "SlidingMenuButtonLeft.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuButtonLeft::SetStyle() {
	Set( A_LEFT, 6 );
	Set( A_TOP, 6);
	SetTexture( A_TEXTURE, g_engine->GetTextureLoader()->LoadTexture(
		"interface.pcx",
			472, 756, 476, 790,
		loader::texture::TextureLoader::LT_CONTRAST,
			0.3
	) );
	Set( A_SIZE_FROM_TEXTURE );
};


}
}
}
