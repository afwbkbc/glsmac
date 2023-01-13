#include "SlidingMenuItemBody.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuItemBody::SetStyle() {
	Set( A_LEFT, 11 );
	Set( A_TOP, 6 );
	Set( A_WIDTH, 284 );
	Set( A_HEIGHT, 36 );
	SetObject( A_TEXTURE, g_engine->GetTextureLoader()->LoadTexture(
		"palette.pcx",
			450, 28, 450, 28,
		loader::texture::TextureLoader::LT_ALPHA,
			0.8
	) );
};


}
}
}
