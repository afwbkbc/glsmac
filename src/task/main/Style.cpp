#include "Style.h"

#include "ui_legacy/theme/Style.h"

namespace task {
namespace intro {

void Style::AddStyles() {

	AddStyle(
		"IntroLogo", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_CENTER );
			s->SetTexture( ui_legacy::A_TEXTURE, resource::PCX_LOGO );
			s->Set( ui_legacy::A_STRETCH_TEXTURE );
			s->Set( ui_legacy::A_KEEP_TEXTURE_ASPECT_RATIO );
		}
	);

}

}
}
