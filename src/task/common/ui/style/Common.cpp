#include "Common.h"

#include "ui/theme/Style.h"

namespace task {
namespace style {

void Common::AddStyles() {

	AddStyle(
		"FPSCounter", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_LEFT | ui::ALIGN_TOP );
			s->Set( ui::A_LEFT, 0 );
			s->Set( ui::A_TOP, 0 );
			s->SetFont( ui::A_FONT, "arialnb.ttf", 18 );
			s->SetColor( ui::A_TEXT_COLOR, types::Color( 0.5f, 1.0f, 0.3f, 0.7f ) );
			s->Set( ui::A_Z_INDEX, 0.9f );
		}
	);

}

}
}
