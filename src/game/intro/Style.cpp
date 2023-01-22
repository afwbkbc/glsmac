#include "Style.h"

namespace game {
namespace intro {

void Style::AddStyles() {
	
	AddStyle( "IntroLogo", SH() {
		s->Set( ui::theme::Style::A_ALIGN, ui::object::UIObject::ALIGN_CENTER );
		s->SetTexture( ui::theme::Style::A_TEXTURE, "logo.pcx" );
		s->Set( ui::theme::Style::A_STRETCH_TEXTURE );
	});
	
}

}
}
