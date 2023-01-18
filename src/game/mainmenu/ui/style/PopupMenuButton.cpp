#include "PopupMenuButton.h"

#include "engine/Engine.h"

namespace game {
namespace mainmenu {
namespace style {

void PopupMenuButton::SetStyle() {
	Set( A_WIDTH, 234 );
	Set( A_HEIGHT, 20 );
	
	unordered_map<Style::attribute_type_t,vector<size_t>> textures = {};
	
	// borders
	if ( Is( M_ACTIVE ) ) {
		textures = {
			{ A_TEXTURE_BACK, { 68, 170, 134, 187 } },
			{ A_TEXTURE_BORDER_LEFT, { 64, 170, 64, 187 } },
			{ A_TEXTURE_BORDER_TOP, { 68, 169, 134, 169 } },
			{ A_TEXTURE_BORDER_RIGHT, { 138, 170, 138, 187 } },
			{ A_TEXTURE_BORDER_BOTTOM, { 68, 188, 134, 188 } },
		};
	}
	else { 
		if ( Is( M_HOVER ) ) {
			textures = {
				{ A_TEXTURE_BORDER_LEFT, { 64, 149, 64, 166 } },
				{ A_TEXTURE_BORDER_TOP, { 68, 148, 134, 148 } },
				{ A_TEXTURE_BORDER_RIGHT, { 138, 149, 138, 166 } },
				{ A_TEXTURE_BORDER_BOTTOM, { 68, 167, 134, 167 } },
			};
		}
		else {
			textures = {
				{ A_TEXTURE_BORDER_LEFT, { 64, 128, 64, 145 } },
				{ A_TEXTURE_BORDER_TOP, { 68, 127, 134, 127 } },
				{ A_TEXTURE_BORDER_RIGHT, { 138, 128, 138, 145 } },
				{ A_TEXTURE_BORDER_BOTTOM, { 68, 146, 134, 146 } },
			};
		}
		textures[ A_TEXTURE_BACK ] = { 68, 149, 134, 166 };
	}
	
	for ( auto& texture : textures ) {
		SetObject( texture.first, g_engine->GetTextureLoader()->LoadTexture( "interface.pcx", texture.second[0] , texture.second[1], texture.second[2], texture.second[3] ) );
	}

	SetObject( A_FONT, g_engine->GetFontLoader()->LoadFont( "arialnb.ttf", 18 ) );
	if ( Is( M_HOVER ) && !Is( M_ACTIVE ) ) {
		SetColor( A_TEXTCOLOR, Color::FromRGB( 164, 176, 232 ) );
	}
	else {
		SetColor( A_TEXTCOLOR, Color::FromRGB( 13, 23, 36 ) );
	}

}

}
}
}
