#include "PopupMenuFrame.h"

#include "ui/object/UIObject.h"

#include "engine/Engine.h"

namespace game {
namespace mainmenu {
namespace style {

void PopupMenuFrame::SetStyle() {
	Set( A_ALIGN, ui::object::UIObject::ALIGN_CENTER );
	Set( A_WIDTH, 500 );
	Set( A_HEIGHT, 200 );
	
	const unordered_map<Style::attribute_type_t,vector<size_t>> textures = {
		{ A_TEXTURE_BACK, { 86, 548, 109, 571 } },
		{ A_TEXTURE_BORDER_LEFT, { 79, 431, 79, 454 } },
		{ A_TEXTURE_BORDER_TOP, { 86, 307, 109, 307 } },
		{ A_TEXTURE_BORDER_RIGHT, { 116, 431, 116, 454 } },
		{ A_TEXTURE_BORDER_BOTTOM, { 86, 461, 109, 461 } },
	};

	for ( auto& texture : textures ) {
		SetObject( texture.first, g_engine->GetTextureLoader()->LoadTexture( "interface.pcx", texture.second[0] , texture.second[1], texture.second[2], texture.second[3] ) );
	}
	
}

}
}
}
