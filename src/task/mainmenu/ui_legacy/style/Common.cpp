#include "Common.h"

#include "ui_legacy/theme/Style.h"

namespace task {
namespace mainmenu {
namespace style {

void Common::AddStyles() {

	AddStyle(
		"MainMenuBackground", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_RIGHT | ui_legacy::ALIGN_BOTTOM );
			s->SetTexture( ui_legacy::A_TEXTURE, resource::PCX_OPENINGA );
			s->Set( ui_legacy::A_STRETCH_TEXTURE );
		}
	);

	AddStyle(
		"MainMenuMusic", SH() {
			s->SetSound( ui_legacy::A_SOUND, resource::WAV_OPENING_MENU );
			s->Set( ui_legacy::A_SOUND_AUTOPLAY );
			s->Set( ui_legacy::A_SOUND_REPEATABLE );
			s->Set( ui_legacy::A_SOUND_VOLUME, 0.8 );
		}
	);

	AddStyle(
		"MainMenuGLSMACLogo", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_RIGHT | ui_legacy::ALIGN_TOP );
			s->SetFont( ui_legacy::A_FONT, resource::TTF_ARIALNB, 14 );
			s->SetColor( ui_legacy::A_TEXT_COLOR, types::Color::FromRGBA( 127, 192, 192, 127 ) );
			s->Set( ui_legacy::A_TOP, 3 ); // TODO: fix text width determination
		}
	);

	AddStyle(
		"MainMenuCustomizeMapPreview", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_RIGHT | ui_legacy::ALIGN_VCENTER );
			s->Set( ui_legacy::A_TOP, 0 );
			s->Set( ui_legacy::A_BOTTOM, 0 );
			s->Set( ui_legacy::A_RIGHT, 0 );
		}
	);

	AddStyle(
		"MainMenuCustomizeMapMoons", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_TOP );
			s->Set( ui_legacy::A_TOP, 0 );
			s->Set( ui_legacy::A_LEFT, 0 );
		}
	);

}

}
}
}
