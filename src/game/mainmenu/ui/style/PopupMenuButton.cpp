#include "PopupMenuButton.h"

#include "engine/Engine.h"

namespace game {
namespace mainmenu {
namespace style {

void PopupMenuButton::SetStyle() {
	Set( A_WIDTH, 234 );
	Set( A_HEIGHT, 20 );
	
	unordered_map<Style::attribute_type_t,vector<size_t>> textures;
	
	if ( m_modifier == M_HOVER ) {
		textures = {
			{ A_TEXTURE_BORDER_LEFT, { 64, 149, 64, 166 } },
			{ A_TEXTURE_BORDER_TOP, { 68, 148, 134, 148 } },
			{ A_TEXTURE_BORDER_RIGHT, { 138, 149, 138, 166 } },
			{ A_TEXTURE_BORDER_BOTTOM, { 68, 167, 134, 167 } },
		};
	}
	else {
		/*textures = {
			{ A_TEXTURE_BORDER_LEFT, { 84, 149, 84, 166 } },
			{ A_TEXTURE_BORDER_TOP, { 88, 148, 154, 148 } },
			{ A_TEXTURE_BORDER_RIGHT, { 158, 149, 158, 166 } },
			{ A_TEXTURE_BORDER_BOTTOM, { 88, 167, 154, 167 } },
		};*/
	}
	textures[ A_TEXTURE_BACK ] = { 68, 149, 134, 166 };
	
	for ( auto& texture : textures ) {
		SetObject( texture.first, g_engine->GetTextureLoader()->LoadTexture( "interface.pcx", texture.second[0] , texture.second[1], texture.second[2], texture.second[3] ) );
	}
}

}
}
}
