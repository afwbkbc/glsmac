#include "Common.h"

#include "ui_legacy/theme/Style.h"

namespace task {
namespace style {

void Common::AddStyles() {

	AddStyle(
		"FPSCounter", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_TOP );
			s->Set( ui_legacy::A_LEFT, 0 );
			s->Set( ui_legacy::A_TOP, 0 );
			s->SetFont( ui_legacy::A_FONT, resource::TTF_ARIALNB, 18 );
			s->SetColor( ui_legacy::A_TEXT_COLOR, types::Color( 0.5f, 1.0f, 0.3f, 0.7f ) );
			s->Set( ui_legacy::A_Z_INDEX, 0.9f );
		}
	);

}

}
}
