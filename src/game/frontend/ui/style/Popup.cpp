#include "Popup.h"

#include "ui/theme/Style.h"

namespace game {
namespace frontend {
namespace ui {
namespace style {

void Popup::AddStyles() {

	SetPrefix( "WP" );

	// general style

	AddStyle(
		"", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_HCENTER | ::ui::ALIGN_BOTTOM );
			s->Set( ::ui::A_Z_INDEX, 0.7f );
		}
	);

	// TODO: multilevel inheritance
	AddStyle(
		"SectionOuter", SH() {
			s->Set( ::ui::A_BORDER_SIZE, 1 );
			s->Set( ::ui::A_LEFT, -3 ); // TODO: why are gaps created without it?
			s->Set( ::ui::A_RIGHT, -3 ); // TODO: why are gaps created without it?
			s->SetTexture( ::ui::A_TEXTURE_LEFT, resource::PCX_INTERFACE, 40, 275, 40, 298 );
			s->SetTexture( ::ui::A_TEXTURE_TOP, resource::PCX_INTERFACE, 47, 268, 70, 268 );
			s->SetTexture( ::ui::A_TEXTURE_RIGHT, resource::PCX_INTERFACE, 77, 275, 77, 298 );
			s->SetTexture( ::ui::A_TEXTURE_BOTTOM, resource::PCX_INTERFACE, 47, 305, 70, 305 );
			s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_INTERFACE, 86, 665, 109, 688 );
		}
	);

	AddStyle(
		"Frame", SH() {
			s->Set( ::ui::A_BORDER_SIZE, 3 );
			s->SetTexture( ::ui::A_TEXTURE_LEFT, resource::PCX_INTERFACE, 1, 275, 3, 298 );
			s->SetTexture( ::ui::A_TEXTURE_LEFTTOP, resource::PCX_INTERFACE, 1, 268, 3, 270 );
			s->SetTexture( ::ui::A_TEXTURE_TOP, resource::PCX_INTERFACE, 8, 268, 31, 270 );
			s->SetTexture( ::ui::A_TEXTURE_RIGHTTOP, resource::PCX_INTERFACE, 36, 268, 38, 270 );
			s->SetTexture( ::ui::A_TEXTURE_RIGHT, resource::PCX_INTERFACE, 36, 275, 38, 298 );
			s->SetTexture( ::ui::A_TEXTURE_RIGHTBOTTOM, resource::PCX_INTERFACE, 36, 303, 38, 305 );
			s->SetTexture( ::ui::A_TEXTURE_BOTTOM, resource::PCX_INTERFACE, 8, 303, 31, 305 );
			s->SetTexture( ::ui::A_TEXTURE_LEFTBOTTOM, resource::PCX_INTERFACE, 1, 303, 3, 305 );
			s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_INTERFACE, 86, 548, 109, 571 );
		}
	);

	AddStyle(
		"InnerFrame", { "Frame" }, SH() {
			s->Set( ::ui::A_Z_INDEX, 0.6f );
		}
	);

	AddStyle(
		"SectionInner", { "Frame" }, SH() {
			s->Set( ::ui::A_LEFT, 1 );
			s->Set( ::ui::A_TOP, 1 );
			s->Set( ::ui::A_RIGHT, 1 );
			s->Set( ::ui::A_BOTTOM, 1 );

			s->SetFont( ::ui::A_HEADER_FONT, resource::TTF_ARIALNB, 18 );
			s->Set( ::ui::A_HEADER_HEIGHT, 22 );
			s->SetColor( ::ui::A_HEADER_TEXTCOLOR, types::Color::FromRGB( 71, 144, 161 ) );
			s->SetTexture( ::ui::A_HEADER_TEXTURE_LEFT, resource::PCX_INTERFACE, 40, 275, 40, 298 );
			s->SetTexture( ::ui::A_HEADER_TEXTURE_TOP, resource::PCX_INTERFACE, 47, 268, 70, 268 );
			s->SetTexture( ::ui::A_HEADER_TEXTURE_RIGHT, resource::PCX_INTERFACE, 77, 275, 77, 298 );
			s->SetTexture( ::ui::A_HEADER_TEXTURE_BOTTOM, resource::PCX_INTERFACE, 47, 305, 70, 305 );
		}
	);

	AddStyle(
		"LeftFrame", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_LEFT | ::ui::ALIGN_TOP );
			s->Set( ::ui::A_HEIGHT, 529 );
		}
	);

	AddStyle(
		"LeftLeftFrame", { "LeftFrame" }, SH() {
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE_X2_A, 713, 1, 727, 529 );
			s->Set( ::ui::A_LEFT, -23 );
			s->Set( ::ui::A_WIDTH, 15 );
			s->Set( ::ui::A_TOP, 85 );
		}
	);

	AddStyle(
		"LeftRightFrame", { "LeftFrame" }, SH() {
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE_X2_A, 729, 1, 736, 529 );
			s->Set( ::ui::A_LEFT, -8 );
			s->Set( ::ui::A_WIDTH, 8 );
		}
	);

	AddStyle(
		"RightFrame", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_RIGHT | ::ui::ALIGN_TOP );
			s->Set( ::ui::A_HEIGHT, 529 );
		}
	);

	AddStyle(
		"RightRightFrame", { "RightFrame" }, SH() {
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE_X2_A, 747, 1, 761, 529 );
			s->Set( ::ui::A_RIGHT, -23 );
			s->Set( ::ui::A_WIDTH, 15 );
			s->Set( ::ui::A_TOP, 85 );
		}
	);

	AddStyle(
		"RightLeftFrame", { "RightFrame" }, SH() {
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE_X2_A, 738, 1, 745, 529 );
			s->Set( ::ui::A_RIGHT, -8 );
			s->Set( ::ui::A_WIDTH, 8 );
		}
	);

	// specific popups

	AddStyle(
		"PleaseDontGoImage", SH() {

			// TODO: fix z index bugs
			s->Set( ::ui::A_Z_INDEX, 0.6f );

			s->Set( ::ui::A_WIDTH, 200 );
			s->Set( ::ui::A_HEIGHT, 120 );
			s->Set( ::ui::A_LEFT, 8 );
			s->Set( ::ui::A_TOP, 36 );

			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_SPACE_SM );

		}
	);

	AddStyle(
		"PleaseDontGoText", SH() {

			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_RIGHT );

			// TODO: fix z index bugs
			s->Set( ::ui::A_Z_INDEX, 0.6f );

			s->Set( ::ui::A_HEIGHT, 32 );
			s->Set( ::ui::A_RIGHT, 96 );
			s->Set( ::ui::A_TOP, 80 );

			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALN, 28 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 92, 124, 188 ) );

		}
	);

	AddStyle(
		"PleaseDontGoSound", SH() {
			s->SetSound( ::ui::A_SOUND, resource::WAV_PLS_DONT_GO );
			s->Set( ::ui::A_SOUND_AUTOPLAY );
			s->Set( ::ui::A_SOUND_AUTOSTOP );
			s->Set( ::ui::A_SOUND_VOLUME, 0.5f );
		}
	);

	AddStyle(
		"Label", SH() {
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALN, 18 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 120, 164, 212 ) );
			s->Set( ::ui::A_HEIGHT, 18 );
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_HCENTER | ::ui::ALIGN_TOP );
			s->Set( ::ui::A_Z_INDEX, 0.6f );
		}
	);
}

}
}
}
}
