#include "SlidingMenuItemRight.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuItemRight::SetStyle() {
	Set( A_LEFT, 294 );
	Set( A_TOP, 6 );
	SetObject( A_TEXTURE, g_engine->GetTextureLoader()->LoadTexture(
		"interface.pcx",
			299, 756, 303, 790,
		loader::texture::TextureLoader::LT_CONTRAST,
			0.2
	) );
	Set( A_SIZE_FROM_TEXTURE );
};


}
}
}
