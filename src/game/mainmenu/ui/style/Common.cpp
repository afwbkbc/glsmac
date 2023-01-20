#include "Common.h"

namespace game {
namespace mainmenu {
namespace style {

void Common::AddStyles() {
	
	AddStyle( "MainMenuBackground", SH() {
		s->Set( Style::A_ALIGN, UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
		s->SetTexture( Style::A_TEXTURE, "xopeningb.pcx" );
		s->Set( Style::A_STRETCH_TEXTURE );
	});
	
	AddStyle( "MainMenuMusic", SH() {
		s->SetSound( Style::A_SOUND, "opening menu.wav" );
		s->Set( Style::A_SOUND_AUTOPLAY );
		s->Set( Style::A_SOUND_REPEATABLE );
		s->Set( Style::A_SOUND_VOLUME, 0.8 );
	});
}

}
}
}
