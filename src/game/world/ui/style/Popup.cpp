#include "Popup.h"

namespace game {
namespace world {
namespace ui {
namespace style {

void Popup::AddStyles() {
	
	SetPrefix( "WP" );
	
	AddStyle( "", SH() {
		s->Set( Style::A_ALIGN, UIObject::ALIGN_HCENTER | UIObject::ALIGN_BOTTOM );
		s->Set( Style::A_Z_INDEX, 0.7f );
	});
	
	AddStyle( "SectionOuter", SH() {
		s->Set( ::Style::A_BORDER_SIZE, 1 );
		s->Set( ::Style::A_LEFT, -3 ); // TODO: why are gaps created without it?
		s->Set( ::Style::A_RIGHT, -3 ); // TODO: why are gaps created without it?
		s->SetTexture( ::Style::A_TEXTURE_LEFT, "interface.pcx", 40, 275, 40, 298 );
		s->SetTexture( ::Style::A_TEXTURE_TOP, "interface.pcx", 47, 268, 70, 268 );
		s->SetTexture( ::Style::A_TEXTURE_RIGHT, "interface.pcx", 77, 275, 77, 298 );
		s->SetTexture( ::Style::A_TEXTURE_BOTTOM, "interface.pcx", 47, 305, 70, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BACK, "interface.pcx", 86, 665, 109, 688 );
	});
	
	AddStyle( "SectionInner", SH() {
		s->Set( ::Style::A_BORDER_SIZE, 3 );
		s->Set( ::Style::A_LEFT, 2 );
		s->Set( ::Style::A_TOP, 2 );
		s->Set( ::Style::A_RIGHT, 2 );
		s->Set( ::Style::A_BOTTOM, 2 );
		s->SetTexture( ::Style::A_TEXTURE_LEFT, "interface.pcx", 1, 275, 3, 298 );
		s->SetTexture( ::Style::A_TEXTURE_LEFTTOP, "interface.pcx", 1, 268, 3, 270 );
		s->SetTexture( ::Style::A_TEXTURE_TOP, "interface.pcx", 8, 268, 31, 270 );
		s->SetTexture( ::Style::A_TEXTURE_RIGHTTOP, "interface.pcx", 36, 268, 38, 270 );
		s->SetTexture( ::Style::A_TEXTURE_RIGHT, "interface.pcx", 36, 275, 38, 298 );
		s->SetTexture( ::Style::A_TEXTURE_RIGHTBOTTOM, "interface.pcx", 36, 303, 38, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BOTTOM, "interface.pcx", 8, 303, 31, 305 );
		s->SetTexture( ::Style::A_TEXTURE_LEFTBOTTOM, "interface.pcx", 1, 303, 3, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BACK, "interface.pcx", 86, 548, 109, 571 );
	});
	
	AddStyle( "LeftFrame", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		s->Set( ::Style::A_HEIGHT, 529 );
	});
	
	AddStyle( "LeftLeftFrame", { "LeftFrame" }, SH() {
		s->SetTexture( ::Style::A_TEXTURE, "console_x2_a.pcx", 713, 1, 727, 529 );
		s->Set( ::Style::A_LEFT, -23 );
		s->Set( ::Style::A_WIDTH, 15 );
		s->Set( ::Style::A_TOP, 85 );
	});
	
	AddStyle( "LeftRightFrame", { "LeftFrame" }, SH() {
		s->SetTexture( ::Style::A_TEXTURE, "console_x2_a.pcx", 729, 1, 736, 529 );
		s->Set( ::Style::A_LEFT, -8 );
		s->Set( ::Style::A_WIDTH, 8 );
	});

	AddStyle( "RightFrame", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
		s->Set( ::Style::A_HEIGHT, 529 );
	});
	
	AddStyle( "RightRightFrame", { "RightFrame" }, SH() {
		s->SetTexture( ::Style::A_TEXTURE, "console_x2_a.pcx", 747, 1, 761, 529 );
		s->Set( ::Style::A_RIGHT, -23 );
		s->Set( ::Style::A_WIDTH, 15 );
		s->Set( ::Style::A_TOP, 85 );
	});
	
	AddStyle( "RightLeftFrame", { "RightFrame" }, SH() {
		s->SetTexture( ::Style::A_TEXTURE, "console_x2_a.pcx", 738, 1, 745, 529 );
		s->Set( ::Style::A_RIGHT, -8 );
		s->Set( ::Style::A_WIDTH, 8 );
	});
	
}

}
}
}
}
