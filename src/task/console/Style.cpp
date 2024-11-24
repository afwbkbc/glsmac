#include "Style.h"

#include "ui/theme/Style.h"

namespace task {
namespace console {

void Style::AddStyles() {

	SetPrefix( "Console" );

	AddStyle(
		"", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_TOP );
			s->Set( ui::A_TOP, 0 );
			s->Set( ui::A_LEFT, 0 );
			s->Set( ui::A_RIGHT, 0 );
			s->Set( ui::A_Z_INDEX, 0.9f );
		}
	);

	AddStyle(
		"Background", SH() {
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_INTERFACE, 86, 665, 109, 688 );
		}
	);

	AddStyle(
		"Border", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_BOTTOM );
			s->Set( ui::A_BOTTOM, 1 );
			s->Set( ui::A_LEFT, 0 );
			s->Set( ui::A_RIGHT, 0 );
			s->Set( ui::A_HEIGHT, 6 );
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_INTERFACE, 358, 839, 418, 844 );
		}
	);

	AddStyle(
		"Prompt", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_LEFT | ui::ALIGN_BOTTOM );
			s->Set( ui::A_BOTTOM, 13 );
			s->Set( ui::A_LEFT, 4 );
			s->Set( ui::A_RIGHT, 0 );
			s->Set( ui::A_HEIGHT, 24 );
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALNB, 18 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 255, 255, 255 ) );
			s->Set( ui::A_TEXT_ALIGN, ui::ALIGN_LEFT | ui::ALIGN_BOTTOM );
		}
	);

	AddStyle(
		"Input", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_LEFT | ui::ALIGN_BOTTOM );
			s->Set( ui::A_BOTTOM, 7 );
			s->Set( ui::A_LEFT, 16 );
			s->Set( ui::A_RIGHT, 0 );
			s->Set( ui::A_HEIGHT, 24 );
			s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_INTERFACE, 86, 470, 109, 493 );
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALN, 18 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 255, 255, 255 ) );
			s->Set( ui::A_TEXT_ALIGN, ui::ALIGN_LEFT | ui::ALIGN_BOTTOM );
		}
	);

	AddStyle(
		"History", SH() {
			s->Set( ui::A_LEFT, 0 );
			s->Set( ui::A_TOP, 0 );
			s->Set( ui::A_RIGHT, 0 );
			s->Set( ui::A_BOTTOM, 32 );
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALN, 18 );
			s->Set( ui::A_ITEM_HEIGHT, 22 );
			s->Set( ui::A_TEXT_ALIGN, ui::ALIGN_LEFT );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 192, 192, 192 ) );
		}
	);

}

}
}
