#include "SlidingMenu.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenu::AddStyles() {
	
	AddStyle( "SlidingMenuBlock", SH() {
		s->Set( Style::A_ALIGN, UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
		s->Set( Style::A_WIDTH, 354 );
		s->Set( Style::A_BOTTOM, 16 );
	});

	AddStyle( "SlidingMenuItem", SH() {
		s->Set( Style::A_LEFT, 0 );
		s->Set( Style::A_WIDTH, 300 );
		s->Set( Style::A_HEIGHT, 46 );
		s->Set( Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
	});
	
	AddStyle( "SlidingMenuItemBody", SH() {
		s->Set( Style::A_LEFT, 11 );
		s->Set( Style::A_TOP, 6 );
		s->Set( Style::A_WIDTH, 284 );
		s->Set( Style::A_HEIGHT, 36 );
		s->SetTexture( Style::A_TEXTURE,
			"palette.pcx",
				450, 28, 450, 28,
			TextureLoader::LT_ALPHA,
				0.8
		);
	});
	
	AddStyle( "SlidingMenuItemBottom", SH() {
		s->Set( Style::A_LEFT, 6 );
		s->Set( Style::A_TOP, 40 );
		s->SetTexture( Style::A_TEXTURE,
			"console_x.pcx",
				699, 1, 706, 293,
			TextureLoader::LT_ROTATE |
			TextureLoader::LT_FLIPV |
			TextureLoader::LT_CONTRAST,
				0.2
		);
		s->Set( Style::A_SIZE_FROM_TEXTURE );
	});
	
	AddStyle( "SlidingMenuItemLeft", SH() {
		s->Set( Style::A_LEFT, 6 );
		s->Set( Style::A_TOP, 6);
		s->SetTexture( Style::A_TEXTURE,
			"interface.pcx",
				472, 756, 476, 790,
			TextureLoader::LT_CONTRAST,
				0.3
		);
		s->Set( Style::A_SIZE_FROM_TEXTURE );
	});
	
	AddStyle( "SlidingMenuItemLowerTop", SH() {
		s->Set( Style::A_LEFT, 0 );
		s->Set( Style::A_TOP, -2 );
		s->SetTexture( Style::A_TEXTURE,
			"console_x.pcx",
				708, 1, 715, 354,
			TextureLoader::LT_ROTATE |
			TextureLoader::LT_FLIPV |
			TextureLoader::LT_CONTRAST,
				0.8
		);
		s->Set( Style::A_SIZE_FROM_TEXTURE );
	});
	
	AddStyle( "SlidingMenuItemRight", SH() {
		s->Set( Style::A_LEFT, 294 );
		s->Set( Style::A_TOP, 6 );
		s->SetTexture( Style::A_TEXTURE,
			"interface.pcx",
				299, 756, 303, 790,
			TextureLoader::LT_CONTRAST,
				0.2
		);
		s->Set( Style::A_SIZE_FROM_TEXTURE );

	});
	
	AddStyle( "SlidingMenuItemUpperTop", SH() {
		s->Set( Style::A_LEFT, 200 );
		s->Set( Style::A_TOP, -8 );
		s->SetTexture( Style::A_TEXTURE,
			"console_x.pcx",
				708, 1, 715, 354,
			TextureLoader::LT_ROTATE |
			TextureLoader::LT_FLIPV |
			TextureLoader::LT_CONTRAST,
				1.5
		);
		s->Set( Style::A_SIZE_FROM_TEXTURE );
	});
	
	AddStyle( "SlidingMenuItemText", SH() {
		s->SetFont( Style::A_FONT, "arialnb.ttf", 20 );
	});
	
	AddStyle( "SlidingMenuButtonText", {
		"SlidingMenuItemText"
	}, SH() {
		if ( s->Is( Style::M_SELECTED ) ) {
			s->SetColor( Style::A_TEXTCOLOR, Color::FromRGBA( 193, 240 ,204, 255 ) );
		}
		else {
			s->SetColor( Style::A_TEXTCOLOR, Color::FromRGBA( 94, 162 ,137, 255 ) );
		}
	});
	
	AddStyle( "SlidingMenuTitleText", {
		"SlidingMenuItemText"
	}, SH() {
		s->SetColor( Style::A_TEXTCOLOR, Color::FromRGBA( 230, 243 ,233, 255 ) );
	});
	

}

}
}
}

