#include "Style.h"

#include "ui_legacy/theme/Style.h"

namespace task {
namespace console {

void Style::AddStyles() {

	SetPrefix( "Console" );

	AddStyle(
		"", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_TOP );
			s->Set( ui_legacy::A_TOP, 0 );
			s->Set( ui_legacy::A_LEFT, 0 );
			s->Set( ui_legacy::A_RIGHT, 0 );
			s->Set( ui_legacy::A_Z_INDEX, 0.9f );
		}
	);

	AddStyle(
		"Background", SH() {
			s->SetTexture( ::ui_legacy::A_TEXTURE, resource::PCX_INTERFACE, 86, 665, 109, 688 );
		}
	);

	AddStyle(
		"Border", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_BOTTOM );
			s->Set( ui_legacy::A_BOTTOM, 1 );
			s->Set( ui_legacy::A_LEFT, 0 );
			s->Set( ui_legacy::A_RIGHT, 0 );
			s->Set( ui_legacy::A_HEIGHT, 6 );
			s->SetTexture( ::ui_legacy::A_TEXTURE, resource::PCX_INTERFACE, 358, 839, 418, 844 );
		}
	);

	AddStyle(
		"Prompt", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_BOTTOM );
			s->Set( ui_legacy::A_BOTTOM, 13 );
			s->Set( ui_legacy::A_LEFT, 4 );
			s->Set( ui_legacy::A_RIGHT, 0 );
			s->Set( ui_legacy::A_HEIGHT, 24 );
			s->SetFont( ::ui_legacy::A_FONT, resource::TTF_ARIALNB, 18 );
			s->SetColor( ::ui_legacy::A_TEXT_COLOR, types::Color::FromRGB( 255, 255, 255 ) );
			s->Set( ui_legacy::A_TEXT_ALIGN, ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_BOTTOM );
		}
	);

	AddStyle(
		"Input", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_BOTTOM );
			s->Set( ui_legacy::A_BOTTOM, 7 );
			s->Set( ui_legacy::A_LEFT, 16 );
			s->Set( ui_legacy::A_RIGHT, 0 );
			s->Set( ui_legacy::A_HEIGHT, 24 );
			s->SetTexture( ::ui_legacy::A_TEXTURE_BACK, resource::PCX_INTERFACE, 86, 470, 109, 493 );
			s->SetFont( ::ui_legacy::A_FONT, resource::TTF_ARIALN, 18 );
			s->SetColor( ::ui_legacy::A_TEXT_COLOR, types::Color::FromRGB( 255, 255, 255 ) );
			s->Set( ui_legacy::A_TEXT_ALIGN, ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_BOTTOM );
		}
	);

	AddStyle(
		"History", SH() {
			s->Set( ui_legacy::A_LEFT, 0 );
			s->Set( ui_legacy::A_TOP, 0 );
			s->Set( ui_legacy::A_RIGHT, 0 );
			s->Set( ui_legacy::A_BOTTOM, 32 );
			s->SetFont( ::ui_legacy::A_FONT, resource::TTF_ARIALN, 18 );
			s->Set( ui_legacy::A_ITEM_HEIGHT, 22 );
			s->Set( ui_legacy::A_TEXT_ALIGN, ui_legacy::ALIGN_LEFT );
			s->SetColor( ::ui_legacy::A_TEXT_COLOR, types::Color::FromRGB( 192, 192, 192 ) );
		}
	);

}

}
}
