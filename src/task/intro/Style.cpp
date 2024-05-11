#include "Style.h"

#include "ui/theme/Style.h"

namespace task {
namespace intro {

void Style::AddStyles() {

	AddStyle(
		"IntroLogo", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_CENTER );
			s->SetTexture( ui::A_TEXTURE, "logo.pcx" );
			s->Set( ui::A_STRETCH_TEXTURE );
			s->Set( ui::A_KEEP_TEXTURE_ASPECT_RATIO );
		}
	);

}

}
}
