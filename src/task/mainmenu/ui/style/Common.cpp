#include "Common.h"

#include "ui/theme/Style.h"

namespace task {
namespace mainmenu {
namespace style {

void Common::AddStyles() {

	AddStyle(
		"MainMenuBackground", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_RIGHT | ui::ALIGN_BOTTOM );
			s->SetTexture( ui::A_TEXTURE, "openinga.pcx" );
			s->Set( ui::A_STRETCH_TEXTURE );
		}
	);

	AddStyle(
		"MainMenuMusic", SH() {
			s->SetSound( ui::A_SOUND, "opening menu.wav" );
			s->Set( ui::A_SOUND_AUTOPLAY );
			s->Set( ui::A_SOUND_REPEATABLE );
			s->Set( ui::A_SOUND_VOLUME, 0.8 );
		}
	);

	AddStyle(
		"MainMenuGLSMACLogo", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_RIGHT | ui::ALIGN_TOP );
			s->SetFont( ui::A_FONT, "arialnb.ttf", 14 );
			s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGBA( 127, 192, 192, 127 ) );
			s->Set( ui::A_TOP, 3 ); // TODO: fix text width determination
		}
	);

	AddStyle(
		"MainMenuCustomizeMapPreview", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_RIGHT | ui::ALIGN_VCENTER );
			s->Set( ui::A_TOP, 0 );
			s->Set( ui::A_BOTTOM, 0 );
			s->Set( ui::A_RIGHT, 0 );
		}
	);

	AddStyle(
		"MainMenuCustomizeMapMoons", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_LEFT | ui::ALIGN_TOP );
			s->Set( ui::A_TOP, 0 );
			s->Set( ui::A_LEFT, 0 );
		}
	);

}

}
}
}
