#include "BottomBar.h"

namespace game {
namespace world {
namespace ui {
namespace style {

void BottomBar::AddStyles() {
	
	SetPrefix( "MapBottomBar" );
	
	// frame
	
	AddStyle( "", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM );
		s->Set( ::Style::A_HEIGHT, 256 );
	});
	
	AddStyle( "Frame", SH() {
		
	});
	
	AddStyle( "FrameLeft", { "Frame" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_WIDTH, 356 );
		s->SetTextureTC( ::Style::A_TEXTURE, "console2_A.pcx", 0, 0, 356, 256, Color::RGB( 100, 16, 156 ) );
	});
	
	AddStyle( "FrameMiddle", { "Frame" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM );
		s->Set( ::Style::A_LEFT, 356 );
		s->Set( ::Style::A_RIGHT, 520 );
		s->SetTextureTC( ::Style::A_TEXTURE, "console2_A.pcx", 357, 0, ( 1024 - 521 ), 256, Color::RGB( 100, 16, 156 ) );
	});
	
	AddStyle( "FrameRight", { "Frame" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_WIDTH, 520 );
		s->SetTextureTC( ::Style::A_TEXTURE, "console2_A.pcx", ( 1024 - 520 ), 0, 1023, 256, Color::RGB( 100, 16, 156 ) );
	});
	
	AddStyle( "FrameBackground", SH() {
		s->SetTexture( ::Style::A_TEXTURE, "interface.pcx", 86, 665, 109, 688 );
	});
	
	AddStyle( "Section", SH() {
		s->Set( ::Style::A_BORDER_WIDTH, 3 );
	});
	
	AddStyle( "SectionOuter", { "Section" }, SH() {
		s->SetTexture( ::Style::A_TEXTURE_BORDER_LEFT, "interface.pcx", 40, 275, 42, 298 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_LT, "interface.pcx", 40, 268, 42, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_TOP, "interface.pcx", 47, 268, 70, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_RT, "interface.pcx", 75, 268, 77, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_RIGHT, "interface.pcx", 75, 275, 77, 298 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_RB, "interface.pcx", 75, 303, 77, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_BOTTOM, "interface.pcx", 47, 303, 70, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_LB, "interface.pcx", 40, 303, 42, 305 );
	});
	
	AddStyle( "SectionInner", { "Section" }, SH() {
		s->SetTexture( ::Style::A_TEXTURE_BORDER_LEFT, "interface.pcx", 1, 275, 3, 298 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_LT, "interface.pcx", 1, 268, 3, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_TOP, "interface.pcx", 8, 268, 31, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_RT, "interface.pcx", 36, 268, 38, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_RIGHT, "interface.pcx", 36, 275, 38, 298 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_RB, "interface.pcx", 36, 303, 38, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_BOTTOM, "interface.pcx", 8, 303, 31, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_LB, "interface.pcx", 1, 303, 3, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BACK, "interface.pcx", 86, 548, 109, 571 );
	});
	
	// buttons
	
	AddStyle( "Button", SH() {
		s->Set( ::Style::A_WIDTH, 108 );
		s->Set( ::Style::A_HEIGHT, 16 );
		
		s->SetFont( ::Style::A_FONT, "arialnb.ttf", 16 );
		
		if ( s->Is( ::Style::M_ACTIVE ) ) {
			s->SetTexture( ::Style::A_TEXTURE_BACK, "console_x2_a.pcx", 1, 247, 106, 260 );
			s->SetColor( ::Style::A_TEXTCOLOR, Color::FromRGB( 0, 0, 0 ) );
		}
		else if ( s->Is( ::Style::M_HOVER ) ) {
			s->SetTexture( ::Style::A_TEXTURE_BACK, "console_x2_a.pcx", 1, 232, 106, 245 );
			s->SetColor( ::Style::A_TEXTCOLOR, Color::FromRGB( 0, 0, 0 ) );
		}
		else {
			s->SetTexture( ::Style::A_TEXTURE_BACK, "console_x2_a.pcx", 1, 217, 106, 230 );
			s->SetColor( ::Style::A_TEXTCOLOR, Color::FromRGB( 228, 104, 24 ) );
		}

	});
	
	AddStyle( "ButtonMenu", { "Button" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_LEFT, 10 );
		s->Set( ::Style::A_TOP, 19 );
		s->SetSound( ::Style::A_BUTTON_CLICK_SOUND, "amenu2.wav" );
		s->Set( ::Style::A_SOUND_VOLUME, 0.4 );
	});
	
	AddStyle( "ButtonCommlink", { "Button" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_RIGHT, 10 );
		s->Set( ::Style::A_TOP, 21 );
		s->SetSound( ::Style::A_BUTTON_CLICK_SOUND, "mmenu.wav" );
		s->Set( ::Style::A_SOUND_VOLUME, 0.2 );
	});
	
	// unit preview
	
	AddStyle( "UnitPreview", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_TOP, 58 );
		s->Set( ::Style::A_BOTTOM, 6 );
		s->Set( ::Style::A_LEFT, 5 );
		s->Set( ::Style::A_WIDTH, 136 );
	});
	
	// tile preview
	
	AddStyle( "TilePreview", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_TOP, 58 );
		s->Set( ::Style::A_BOTTOM, 6 );
		s->Set( ::Style::A_LEFT, 139 );
		s->Set( ::Style::A_WIDTH, 105 );
	});

	AddStyle( "TilePreviewImage", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_HCENTER );
		s->Set( ::Style::A_TOP, 2 );
		
		/* classic */
		/*s->Set( ::Style::A_WIDTH, 84 );
		s->Set( ::Style::A_HEIGHT, 52 );*/
		
		/* topdown */
		s->Set( ::Style::A_WIDTH, 84 );
		s->Set( ::Style::A_HEIGHT, 84 );
	});
	
	AddStyle( "TilePreviewText", SH() {
		s->SetFont( ::Style::A_FONT, "arialn.ttf", 14 );
		s->SetColor( ::Style::A_TEXTCOLOR, Color::FromRGB( 116, 156, 56 ) );
		s->Set( ::Style::A_HEIGHT, 15 );
	});
	
	AddStyle( "TilePreviewTextLine", { "TilePreviewText" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_LEFT, 3 );
	});
	
	AddStyle( "TilePreviewTextFooter", { "TilePreviewText" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_RIGHT, 2 );
		s->Set( ::Style::A_BOTTOM, 6 );
	});

	// info panels at center
	AddStyle( "InfoPanels", SH() {
		s->Set( ::Style::A_LEFT, 262 );
		s->Set( ::Style::A_TOP, 68 );
		s->Set( ::Style::A_RIGHT, 262 );
		s->Set( ::Style::A_BOTTOM, 85 );
	});
	
	// units list at bottom
	
	AddStyle( "UnitsList", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM );
		s->Set( ::Style::A_BOTTOM, 6 );
		s->Set( ::Style::A_LEFT, 242 );
		s->Set( ::Style::A_RIGHT, 257 );
		s->Set( ::Style::A_HEIGHT, 60 );
	});
	
	// minimap
	
	AddStyle( "MiniMap", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_TOP, 58 );
		s->Set( ::Style::A_BOTTOM, 6 );
		s->Set( ::Style::A_RIGHT, 6 );
		s->Set( ::Style::A_WIDTH, 238 );
	});

}

}
}
}
}
