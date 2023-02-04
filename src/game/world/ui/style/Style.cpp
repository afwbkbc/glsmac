#include "Style.h"

namespace game {
namespace world {
namespace ui {

void Style::AddStyles() {
	
	// TODO: multilevel includes	
	AddStyle( "MapBottomBar", SH() {
/*		// frame
		{
			const unordered_map<Style::attribute_type_t,vector<size_t>> textures = {
				{ Style::A_TEXTURE_BACK, { 86, 353, 109, 376 } },
				{ Style::A_TEXTURE_BORDER_LEFT, { 79, 431, 79, 454 } },
				{ Style::A_TEXTURE_BORDER_TOP, { 86, 307, 109, 307 } },
				{ Style::A_TEXTURE_BORDER_RIGHT, { 116, 431, 116, 454 } },
				{ Style::A_TEXTURE_BORDER_BOTTOM, { 86, 461, 109, 461 } },
			};
			for ( auto& texture : textures ) {
				s->SetTexture( texture.first, "interface.pcx", texture.second[0] , texture.second[1], texture.second[2], texture.second[3] );
			}
		}*/
	});
	
	AddStyle( "MapBottomBarFrame", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM );
		s->Set( ::Style::A_WIDTH, 1024 );
		s->Set( ::Style::A_HEIGHT, 257 );
		s->SetTexture( ::Style::A_TEXTURE, "console2_A.pcx.png", Color::RGB( 100, 16, 156 ) ); // what's console_A.pcx.png then ?
});
	
}

}
}
}
