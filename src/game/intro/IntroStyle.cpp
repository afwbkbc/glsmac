#include "IntroStyle.h"

namespace game {
namespace intro {

void IntroStyle::AddStyles() {
	
	AddStyle( "IntroLogo", SH() {
		s->Set( Style::A_ALIGN, ui::object::UIObject::ALIGN_CENTER );
		s->SetTexture( Style::A_TEXTURE, "logo.pcx" );
		s->Set( Style::A_STRETCH_TEXTURE );
	});
	
}

}
}
