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
	
	AddStyle( "SectionOuter", SH() {
		s->Set( ::Style::A_BORDER_WIDTH, 3 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_LEFT, "interface.pcx", 40, 275, 42, 298 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_LT, "interface.pcx", 40, 268, 42, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_TOP, "interface.pcx", 47, 268, 70, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_RT, "interface.pcx", 75, 268, 77, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_RIGHT, "interface.pcx", 75, 275, 77, 298 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_RB, "interface.pcx", 75, 303, 77, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_BOTTOM, "interface.pcx", 47, 303, 70, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_LB, "interface.pcx", 40, 303, 42, 305 );
	});
	
	AddStyle( "SectionInner", SH() {
		s->Set( ::Style::A_BORDER_WIDTH, 3 );
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
	
	AddStyle( "TilePreviewSprite", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_HCENTER );
		s->Set( ::Style::A_TOP, 25 ); // to compensate for view angle difference with SMAC's
		s->Set( ::Style::A_WIDTH, 100 );
		s->Set( ::Style::A_HEIGHT, 68 );
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

	// stuff at center
	
	AddStyle( "MiddleArea", SH() {
		s->Set( ::Style::A_LEFT, 262 );
		s->Set( ::Style::A_TOP, 68 );
		s->Set( ::Style::A_RIGHT, 262 );
		s->Set( ::Style::A_BOTTOM, 85 );
	});
	
	AddStyle( "MiddleAreaButton", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_RIGHT, 3 );
		s->Set( ::Style::A_WIDTH, 19 );
		s->SetFont( ::Style::A_FONT, "arialnb.ttf", 16 );
		s->SetColor( ::Style::A_TEXTCOLOR, Color::FromRGB( 0, 0, 0 ) );
		if ( s->Is( ::Style::M_HOVER ) || s->Is( ::Style::M_SELECTED ) ) {
			s->SetTexture( ::Style::A_TEXTURE_BORDER_LEFT, "interface.pcx", 40, 587, 40, 610 );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_TOP, "interface.pcx", 47, 580, 70, 580 );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_RIGHT, "interface.pcx", 77, 587, 77, 610 );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_BOTTOM, "interface.pcx", 47, 617, 70, 617 );
		}
		else {
			s->SetTexture( ::Style::A_TEXTURE_BORDER_LEFT, "interface.pcx", 8, 587, 8, 610 );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_TOP, "interface.pcx", 8, 588, 31, 588 );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_RIGHT, "interface.pcx", 31, 587, 31, 610 );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_BOTTOM, "interface.pcx", 8, 610, 31, 610 );
		}
		if ( s->Is( ::Style::M_ACTIVE ) || s->Is( ::Style::M_SELECTED ) ) {
			s->SetTexture( ::Style::A_TEXTURE_BACK, "interface.pcx", 86, 587, 109, 610 );
		}
		else {
			s->SetTexture( ::Style::A_TEXTURE_BACK, "interface.pcx", 47, 587, 70, 610 );
		}
	});
	
	AddStyle( "MiddleAreaPage", SH() {
		s->Set( ::Style::A_LEFT, 2 );
		s->Set( ::Style::A_TOP, 2 );
		s->Set( ::Style::A_RIGHT, 26 );
		s->Set( ::Style::A_BOTTOM, 2 );
	});
	
	// messages page
	
	AddStyle( "MiddleAreaMessages", { "MiddleAreaPage" }, SH() {
		
	});
	
	AddStyle( "MiddleAreaMessagesList", { "SectionInner" }, SH() {
		s->Set( ::Style::A_LEFT, 0 );
		s->Set( ::Style::A_TOP, 0 );
		s->Set( ::Style::A_RIGHT, 0 );
		s->Set( ::Style::A_BOTTOM, 0 );
	});
	
	// toolbar page
	
	AddStyle( "MiddleAreaToolbar", { "MiddleAreaPage" }, SH() {
		
	});
	
	// info panels page
	
	AddStyle( "MiddleAreaInfoPanels", { "MiddleAreaPage" }, SH() {
		
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

	AddStyle( "MinimapTurnCompleteButton", SH() {
		
		const auto f_buttonstyle = [ &s ] ( const size_t ox, const size_t oy ) -> void {
			const std::string t = "console_x2_a.pcx";
			s->SetTexture( ::Style::A_TEXTURE_BORDER_LEFT, t, 0 + ox, 6 + oy, 5 + ox, 18 + oy );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_LT, t, 0 + ox, 0 + oy, 5 + ox, 5 + oy );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_TOP, t, 6 + ox, 0 + oy, 181 + ox, 5 + oy );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_RT, t, 182 + ox, 0 + oy, 187 + ox, 5 + oy );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_RIGHT, t, 182 + ox, 6 + oy, 187 + ox, 18 + oy );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_RB, t, 182 + ox, 19 + oy, 187 + ox, 24 + oy );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_BOTTOM, t, 6 + ox, 19 + oy, 181 + ox, 24 + oy );
			s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_LB, t, 0 + ox, 19 + oy, 5 + ox, 24 + oy );
			s->SetTexture( ::Style::A_TEXTURE_BACK, t, 6 + ox, 6 + oy, 182 + ox, 19 + oy );
		};
	
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_HCENTER );
		s->Set( ::Style::A_TOP, 1 );
		s->Set( ::Style::A_LEFT, 1 );
		s->Set( ::Style::A_RIGHT, 1 );
		s->Set( ::Style::A_HEIGHT, 25 );
		
		s->SetFont( ::Style::A_FONT, "arialnb.ttf", 16 );
		
		s->Set( ::Style::A_BORDER_WIDTH, 6 );
		
		if ( s->Is( ::Style::M_ACTIVE ) ) {
			f_buttonstyle( 193, 55 );
			s->SetColor( ::Style::A_TEXTCOLOR, Color::FromRGB( 0, 0, 0 ) );
		}
		else if ( s->Is( ::Style::M_HOVER ) ) {
			f_buttonstyle( 2, 55 );
			s->SetColor( ::Style::A_TEXTCOLOR, Color::FromRGB( 228, 104, 24 ) );
		}
		else {
			f_buttonstyle( 2, 1 );
			s->SetColor( ::Style::A_TEXTCOLOR, Color::FromRGB( 228, 104, 24 ) );
		}

		s->SetSound( ::Style::A_BUTTON_CLICK_SOUND, "ok.wav" );
		s->Set( ::Style::A_SOUND_VOLUME, 0.5f );
	});
	
	AddStyle( "MinimapImage", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_HCENTER );
		s->Set( ::Style::A_TOP, 29 );
		s->Set( ::Style::A_WIDTH, 224 );
		s->Set( ::Style::A_HEIGHT, 112 );
	});
	
	AddStyle( "MinimapBottomBar", { "SectionInner" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_BOTTOM, -2 );
		s->Set( ::Style::A_LEFT, -2 );
		s->Set( ::Style::A_WIDTH, 118 );
		s->Set( ::Style::A_HEIGHT, 41 );
	});
	
	AddStyle( "MinimapBottomBarLabel", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->SetFont( ::Style::A_FONT, "arialn.ttf", 14 );
		s->SetColor( ::Style::A_TEXTCOLOR, Color::FromRGB( 60, 148, 124 ) );
		s->Set( ::Style::A_LEFT, 5 );
		s->Set( ::Style::A_HEIGHT, 15 );
	});
}

}
}
}
}
