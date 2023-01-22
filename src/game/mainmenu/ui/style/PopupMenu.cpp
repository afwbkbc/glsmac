#include "PopupMenu.h"

namespace game {
namespace mainmenu {
namespace style {

void PopupMenu::AddStyles() {

	AddStyle( "PopupMenuFrame", SH() {
		// frame
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
		}
		
		// header
		{
			const unordered_map<Style::attribute_type_t,vector<size_t>> textures = {
				{ Style::A_HEADER_TEXTURE_BACK, { 86, 314, 109, 337 } },
				{ Style::A_HEADER_TEXTURE_BORDER_LEFT, { 79, 431, 79, 454 } },
				{ Style::A_HEADER_TEXTURE_BORDER_TOP, { 86, 307, 109, 307 } },
				{ Style::A_HEADER_TEXTURE_BORDER_RIGHT, { 116, 431, 116, 454 } },
				{ Style::A_HEADER_TEXTURE_BORDER_BOTTOM, { 86, 461, 109, 461 } },
			};
			for ( auto& texture : textures ) {
				s->SetTexture( texture.first, "interface.pcx", texture.second[0] , texture.second[1], texture.second[2], texture.second[3] );
			}
		}
		s->SetColor( Style::A_HEADER_TEXTCOLOR, Color::FromRGB( 109, 126, 178 ) );
	});
	
	AddStyle( "PopupMenuWindow", { "PopupMenuFrame" }, SH() {
		s->SetFont( Style::A_HEADER_FONT, "arialnb.ttf", 18 );
		s->Set( Style::A_HEADER_HEIGHT, 22 );
	});
	
	AddStyle( "PopupMenuSection", { "PopupMenuFrame" }, SH() {
		s->SetFont( Style::A_HEADER_FONT, "arialnb.ttf", 16 );
		s->Set( Style::A_HEADER_HEIGHT, 20 );
	});
	
	AddStyle( "PopupMenuButton", SH() {
		unordered_map<Style::attribute_type_t,vector<size_t>> textures = {};

		// borders
		if ( s->Is( Style::M_ACTIVE ) || s->Is( Style::M_SELECTED ) ) {
			textures = {
				{ Style::A_TEXTURE_BACK, { 68, 170, 134, 187 } },
				{ Style::A_TEXTURE_BORDER_LEFT, { 64, 170, 64, 187 } },
				{ Style::A_TEXTURE_BORDER_TOP, { 68, 169, 134, 169 } },
				{ Style::A_TEXTURE_BORDER_RIGHT, { 138, 170, 138, 187 } },
				{ Style::A_TEXTURE_BORDER_BOTTOM, { 68, 188, 134, 188 } },
			};
		}
		else { 
			if ( s->Is( Style::M_HOVER ) ) {
				textures = {
					{ Style::A_TEXTURE_BORDER_LEFT, { 64, 149, 64, 166 } },
					{ Style::A_TEXTURE_BORDER_TOP, { 68, 148, 134, 148 } },
					{ Style::A_TEXTURE_BORDER_RIGHT, { 138, 149, 138, 166 } },
					{ Style::A_TEXTURE_BORDER_BOTTOM, { 68, 167, 134, 167 } },
				};
			}
			else {
				textures = {
					{ Style::A_TEXTURE_BORDER_LEFT, { 64, 128, 64, 145 } },
					{ Style::A_TEXTURE_BORDER_TOP, { 68, 127, 134, 127 } },
					{ Style::A_TEXTURE_BORDER_RIGHT, { 138, 128, 138, 145 } },
					{ Style::A_TEXTURE_BORDER_BOTTOM, { 68, 146, 134, 146 } },
				};
			}
			textures[ Style::A_TEXTURE_BACK ] = { 68, 149, 134, 166 };
		}

		for ( auto& texture : textures ) {
			s->SetTexture( texture.first, "interface.pcx", texture.second[0] , texture.second[1], texture.second[2], texture.second[3] );
		}

		if ( s->Is( Style::M_HOVER ) && !s->Is( Style::M_ACTIVE ) && !s->Is( Style::M_SELECTED) ) {
			s->SetColor( Style::A_TEXTCOLOR, Color::FromRGB( 164, 176, 232 ) );
		}
		else {
			s->SetColor( Style::A_TEXTCOLOR, Color::FromRGB( 13, 23, 36 ) );
		}
	});

	AddStyle( "PopupMenuButtonList", {
		"PopupMenuButton"
	}, SH() {
		s->Set( Style::A_LEFT, 3 );
		s->Set( Style::A_RIGHT, 3 );
		//s->Set( Style::A_TOP, 3 );
		s->Set( Style::A_BOTTOM, 3 );

		s->SetFont( Style::A_FONT, "arialnb.ttf", 16 );
		s->Set( Style::A_TEXTALIGN, UIObject::ALIGN_LEFT );
	});
	
	AddStyle( "PopupMenuButtonOkCancel", {
		"PopupMenuButton"
	}, SH() {
		s->Set( Style::A_WIDTH, 234 );
		s->Set( Style::A_HEIGHT, 20 );

		s->SetFont( Style::A_FONT, "arialnb.ttf", 18 );
		s->SetSound( Style::A_BUTTON_CLICK_SOUND, "ok.wav" );
		s->Set( Style::A_SOUND_VOLUME, 0.5 );
	});
	
	AddStyle( "PopupMenuText", SH() {
		s->SetFont( Style::A_FONT, "arialnb.ttf", 18 );
		s->SetColor( Style::A_TEXTCOLOR, Color::FromRGB( 129, 146, 198 ) );
	});
	
	
}

}
}
}

