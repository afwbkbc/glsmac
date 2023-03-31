#include "Common.h"

namespace game {
namespace mainmenu {
namespace style {

void Common::AddStyles() {
	
	AddStyle( "MainMenuBackground", SH() {
		s->Set( Style::A_ALIGN, UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
		s->SetTexture( Style::A_TEXTURE, "openinga.pcx" );
		s->Set( Style::A_STRETCH_TEXTURE );
	});
	
	AddStyle( "MainMenuMusic", SH() {
		s->SetSound( Style::A_SOUND, "opening menu.wav" );
		s->Set( Style::A_SOUND_AUTOPLAY );
		s->Set( Style::A_SOUND_REPEATABLE );
		s->Set( Style::A_SOUND_VOLUME, 0.8 );
	});
	
	AddStyle( "MainMenuGLSMACLogo", SH() {
		s->Set( Style::A_ALIGN, UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
		s->SetFont( Style::A_FONT, "arialnb.ttf", 14 );
		s->SetColor( Style::A_TEXT_COLOR, Color::FromRGBA( 127, 192, 192, 127 ) );
		s->Set( Style::A_TOP, 3 ); // TODO: fix text width determination
	});
	
	AddStyle( "MainMenuCustomizeMapPreview", SH() {
		s->Set( Style::A_ALIGN, UIObject::ALIGN_RIGHT | UIObject::ALIGN_VCENTER );
		s->Set( Style::A_TOP, 0 );
		s->Set( Style::A_BOTTOM, 0 );
		s->Set( Style::A_RIGHT, 0 );
	});
	
	AddStyle( "MainMenuCustomizeMapMoons", SH() {
		s->Set( Style::A_ALIGN, UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		s->Set( Style::A_TOP, 0 );
		s->Set( Style::A_LEFT, 0 );
	});
	
}

}
}
}
