#include "Style.h"

namespace game {
namespace world {
namespace ui {

void Style::AddStyles() {
	
	const pair< size_t, size_t > split_bottom_frame_at = { 356, 520 };
	
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
		//s->SetTexture( ::Style::A_TEXTURE, "interface.pcx", 86, 548, 109, 571 );
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
		
	});
	
	
	AddStyle( "MapBottomBarUnitPreview", { "MapBottomBarSection" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_WIDTH, 135 );
		s->Set( ::Style::A_HEIGHT, 196 );
		s->Set( ::Style::A_LEFT, 5 );
		s->Set( ::Style::A_BOTTOM, 5 );
	});
}

}
}
}
