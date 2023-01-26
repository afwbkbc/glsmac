#include "Style.h"

using namespace ui;

namespace game {
namespace intro {

void Style::AddStyles() {
	
	AddStyle( "IntroLogo", SH() {
		s->Set( theme::Style::A_ALIGN, ui::object::UIObject::ALIGN_CENTER );
		s->SetTexture( theme::Style::A_TEXTURE, "logo.pcx" );
		s->Set( theme::Style::A_STRETCH_TEXTURE );
		s->Set( theme::Style::A_KEEP_TEXTURE_ASPECT_RATIO );
	});
	
}

}
}
