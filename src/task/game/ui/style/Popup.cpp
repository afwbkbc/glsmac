#include "Popup.h"

namespace task {
namespace game {
namespace ui {
namespace style {

void Popup::AddStyles() {

	SetPrefix( "WP" );

	// general style

	AddStyle(
		"", SH() {
			s->Set( Style::A_ALIGN, UIObject::ALIGN_HCENTER | UIObject::ALIGN_BOTTOM );
			s->Set( Style::A_Z_INDEX, 0.7f );
		}
	);

	// TODO: multilevel inheritance
	AddStyle(
		"SectionOuter", SH() {
			s->Set( Style::A_BORDER_SIZE, 1 );
			s->Set( Style::A_LEFT, -3 ); // TODO: why are gaps created without it?
			s->Set( Style::A_RIGHT, -3 ); // TODO: why are gaps created without it?
			s->SetTexture( Style::A_TEXTURE_LEFT, "interface.pcx", 40, 275, 40, 298 );
			s->SetTexture( Style::A_TEXTURE_TOP, "interface.pcx", 47, 268, 70, 268 );
			s->SetTexture( Style::A_TEXTURE_RIGHT, "interface.pcx", 77, 275, 77, 298 );
			s->SetTexture( Style::A_TEXTURE_BOTTOM, "interface.pcx", 47, 305, 70, 305 );
			s->SetTexture( Style::A_TEXTURE_BACK, "interface.pcx", 86, 665, 109, 688 );
		}
	);

	AddStyle(
		"Frame", SH() {
			s->Set( ::Style::A_BORDER_SIZE, 3 );
			s->SetTexture( ::Style::A_TEXTURE_LEFT, "interface.pcx", 1, 275, 3, 298 );
			s->SetTexture( ::Style::A_TEXTURE_LEFTTOP, "interface.pcx", 1, 268, 3, 270 );
			s->SetTexture( ::Style::A_TEXTURE_TOP, "interface.pcx", 8, 268, 31, 270 );
			s->SetTexture( ::Style::A_TEXTURE_RIGHTTOP, "interface.pcx", 36, 268, 38, 270 );
			s->SetTexture( ::Style::A_TEXTURE_RIGHT, "interface.pcx", 36, 275, 38, 298 );
			s->SetTexture( ::Style::A_TEXTURE_RIGHTBOTTOM, "interface.pcx", 36, 303, 38, 305 );
			s->SetTexture( ::Style::A_TEXTURE_BOTTOM, "interface.pcx", 8, 303, 31, 305 );
			s->SetTexture( ::Style::A_TEXTURE_LEFTBOTTOM, "interface.pcx", 1, 303, 3, 305 );
			s->SetTexture( ::Style::A_TEXTURE_BACK, "interface.pcx", 86, 548, 109, 571 );
		}
	);

	AddStyle(
		"InnerFrame", { "Frame" }, SH() {
			s->Set( Style::A_Z_INDEX, 0.6f );
		}
	);

	AddStyle(
		"SectionInner", { "Frame" }, SH() {
			s->Set( Style::A_LEFT, 1 );
			s->Set( Style::A_TOP, 1 );
			s->Set( Style::A_RIGHT, 1 );
			s->Set( Style::A_BOTTOM, 1 );

			s->SetFont( Style::A_HEADER_FONT, "arialnb.ttf", 18 );
			s->Set( Style::A_HEADER_HEIGHT, 22 );
			s->SetColor( Style::A_HEADER_TEXTCOLOR, Color::FromRGB( 71, 144, 161 ) );
			s->SetTexture( Style::A_HEADER_TEXTURE_LEFT, "interface.pcx", 40, 275, 40, 298 );
			s->SetTexture( Style::A_HEADER_TEXTURE_TOP, "interface.pcx", 47, 268, 70, 268 );
			s->SetTexture( Style::A_HEADER_TEXTURE_RIGHT, "interface.pcx", 77, 275, 77, 298 );
			s->SetTexture( Style::A_HEADER_TEXTURE_BOTTOM, "interface.pcx", 47, 305, 70, 305 );
		}
	);

	AddStyle(
		"LeftFrame", SH() {
			s->Set( Style::A_ALIGN, UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
			s->Set( Style::A_HEIGHT, 529 );
		}
	);

	AddStyle(
		"LeftLeftFrame", { "LeftFrame" }, SH() {
			s->SetTexture( Style::A_TEXTURE, "console_x2_a.pcx", 713, 1, 727, 529 );
			s->Set( Style::A_LEFT, -23 );
			s->Set( Style::A_WIDTH, 15 );
			s->Set( Style::A_TOP, 85 );
		}
	);

	AddStyle(
		"LeftRightFrame", { "LeftFrame" }, SH() {
			s->SetTexture( Style::A_TEXTURE, "console_x2_a.pcx", 729, 1, 736, 529 );
			s->Set( Style::A_LEFT, -8 );
			s->Set( Style::A_WIDTH, 8 );
		}
	);

	AddStyle(
		"RightFrame", SH() {
			s->Set( Style::A_ALIGN, UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
			s->Set( Style::A_HEIGHT, 529 );
		}
	);

	AddStyle(
		"RightRightFrame", { "RightFrame" }, SH() {
			s->SetTexture( Style::A_TEXTURE, "console_x2_a.pcx", 747, 1, 761, 529 );
			s->Set( Style::A_RIGHT, -23 );
			s->Set( Style::A_WIDTH, 15 );
			s->Set( Style::A_TOP, 85 );
		}
	);

	AddStyle(
		"RightLeftFrame", { "RightFrame" }, SH() {
			s->SetTexture( Style::A_TEXTURE, "console_x2_a.pcx", 738, 1, 745, 529 );
			s->Set( Style::A_RIGHT, -8 );
			s->Set( Style::A_WIDTH, 8 );
		}
	);

	// specific popups

	AddStyle(
		"PleaseDontGoImage", SH() {

			// TODO: fix z index bugs
			s->Set( Style::A_Z_INDEX, 0.6f );

			s->Set( Style::A_WIDTH, 200 );
			s->Set( Style::A_HEIGHT, 120 );
			s->Set( Style::A_LEFT, 8 );
			s->Set( Style::A_TOP, 36 );

			s->SetTexture( Style::A_TEXTURE, "space_sm.pcx" );

		}
	);

	AddStyle(
		"PleaseDontGoText", SH() {

			s->Set( Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_RIGHT );

			// TODO: fix z index bugs
			s->Set( Style::A_Z_INDEX, 0.6f );

			s->Set( Style::A_HEIGHT, 32 );
			s->Set( Style::A_RIGHT, 96 );
			s->Set( Style::A_TOP, 80 );

			s->SetFont( Style::A_FONT, "arialn.ttf", 28 );
			s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 92, 124, 188 ) );

		}
	);

	AddStyle(
		"PleaseDontGoSound", SH() {
			s->SetSound( Style::A_SOUND, "CPU please don't go.wav" );
			s->Set( Style::A_SOUND_AUTOPLAY );
			s->Set( Style::A_SOUND_AUTOSTOP );
			s->Set( Style::A_SOUND_VOLUME, 0.5f );
		}
	);

	AddStyle(
		"Label", SH() {
			s->SetFont( Style::A_FONT, "arialn.ttf", 18 );
			s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 120, 164, 212 ) );
			s->Set( Style::A_HEIGHT, 18 );
			s->Set( Style::A_ALIGN, UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
			s->Set( Style::A_Z_INDEX, 0.6f );
		}
	);
}

}
}
}
}
