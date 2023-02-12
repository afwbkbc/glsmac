#include "Style.h"

namespace game {
namespace world {
namespace ui {

void Style::AddStyles() {
	
	AddStyle( "MapBottomBar", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM );
		s->Set( ::Style::A_HEIGHT, 256 );
	});
	
	AddStyle( "MapBottomBarFrame", SH() {
		
	});
	
	AddStyle( "MapBottomBarFrameLeft", { "MapBottomBarFrame" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_WIDTH, 356 );
		s->SetTextureTC( ::Style::A_TEXTURE, "console2_A.pcx", 0, 0, 356, 256, Color::RGB( 100, 16, 156 ) );
	});
	
	AddStyle( "MapBottomBarFrameMiddle", { "MapBottomBarFrame" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM );
		s->Set( ::Style::A_LEFT, 356 );
		s->Set( ::Style::A_RIGHT, 520 );
		s->SetTextureTC( ::Style::A_TEXTURE, "console2_A.pcx", 357, 0, ( 1024 - 521 ), 256, Color::RGB( 100, 16, 156 ) );
	});
	
	AddStyle( "MapBottomBarFrameRight", { "MapBottomBarFrame" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_WIDTH, 520 );
		s->SetTextureTC( ::Style::A_TEXTURE, "console2_A.pcx", ( 1024 - 520 ), 0, 1023, 256, Color::RGB( 100, 16, 156 ) );
	});
	
	AddStyle( "MapBottomBarFrameBackground", SH() {
		s->SetTexture( ::Style::A_TEXTURE, "interface.pcx", 86, 665, 109, 688 );
	});
	
	AddStyle( "MapBottomBarButton", SH() {
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
	
	AddStyle( "MapBottomBarButtonMenu", { "MapBottomBarButton" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_LEFT, 10 );
		s->Set( ::Style::A_TOP, 19 );
		s->SetSound( ::Style::A_BUTTON_CLICK_SOUND, "amenu2.wav" );
		s->Set( ::Style::A_SOUND_VOLUME, 0.4 );
	});
	
	AddStyle( "MapBottomBarButtonCommlink", { "MapBottomBarButton" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_RIGHT, 10 );
		s->Set( ::Style::A_TOP, 21 );
		s->SetSound( ::Style::A_BUTTON_CLICK_SOUND, "mmenu.wav" );
		s->Set( ::Style::A_SOUND_VOLUME, 0.2 );
	});
	
	AddStyle( "MapBottomBarSection", SH() {
		s->Set( ::Style::A_BORDER_WIDTH, 3 );
	});
	
	AddStyle( "MapBottomBarSectionOuter", { "MapBottomBarSection" }, SH() {
		s->SetTexture( ::Style::A_TEXTURE_BORDER_LEFT, "interface.pcx", 40, 275, 42, 298 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_LT, "interface.pcx", 40, 268, 42, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_TOP, "interface.pcx", 47, 268, 70, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_RT, "interface.pcx", 75, 268, 77, 270 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_RIGHT, "interface.pcx", 75, 275, 77, 298 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_RB, "interface.pcx", 75, 303, 77, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_BOTTOM, "interface.pcx", 47, 303, 70, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BORDER_CORNER_LB, "interface.pcx", 40, 303, 42, 305 );
	});
	
	AddStyle( "MapBottomBarSectionInner", { "MapBottomBarSection" }, SH() {
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
	
	AddStyle( "MapBottomBarTilePreview", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_LEFT, 140 );
		s->Set( ::Style::A_TOP, 58 );
		s->Set( ::Style::A_WIDTH, 103 );
		s->Set( ::Style::A_HEIGHT, 134 );
		
	});

	AddStyle( "MapBottomBarTilePreviewImage", SH() {
		s->Set( ::Style::A_HEIGHT, 60 );
	});
	
	AddStyle( "MapBottomBarUnitPreview", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_LEFT, 5 );
		s->Set( ::Style::A_BOTTOM, 5 );
		s->Set( ::Style::A_WIDTH, 135 );
		s->Set( ::Style::A_HEIGHT, 196 );
	});
	
}

}
}
}
