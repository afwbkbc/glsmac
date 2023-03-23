#include "Common.h"

namespace game {
namespace style {

void Common::AddStyles() {
	
	AddStyle( "FPSCounter", SH() {
		s->Set( Style::A_ALIGN, UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		s->Set( Style::A_LEFT, 0 );
		s->Set( Style::A_TOP, 0 );
		s->SetFont( Style::A_FONT, "arialnb.ttf", 18 );
		s->SetColor( Style::A_TEXT_COLOR, Color( 0.5f, 1.0f, 0.3f, 0.7f ) );
	});
	
}

}
}
