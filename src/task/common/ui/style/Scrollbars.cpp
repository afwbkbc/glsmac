#include "Scrollbars.h"

namespace task {
namespace style {

void Scrollbars::AddStyles() {
	
	SetPrefix( "Scrollbar" );
	
	AddStyle( "Vertical", SH() {
		s->SetColorTexture( Style::A_TEXTURE_BACK, Color( 0.0f, 0.0f, 0.0f, 1.0f ) );
	});
	
	AddStyle( "VerticalThick", { "Vertical" }, SH() {
		const auto tc = Color::RGB( 152, 24, 228 );
		s->Set( Style::A_WIDTH, 18 );
		
		if ( s->Is( Style::M_ACTIVE ) ) {
			s->SetTextureTC( Style::A_TEXTURE_TOP, "Icons.pcx", 157, 4, 174, 21, tc );
			s->SetTextureTC( Style::A_TEXTURE_CENTER, "Icons.pcx", 104, 101, 121, 118, tc, TextureLoader::LT_CONTRAST, 2.0f );
			s->SetTextureTC( Style::A_TEXTURE_BOTTOM, "Icons.pcx", 157, 29, 174, 46, tc );
		}
		else if ( s->Is( Style::M_HOVER ) ) {
			s->SetTextureTC( Style::A_TEXTURE_TOP, "Icons.pcx", 132, 4, 149, 21, tc );
			s->SetTextureTC( Style::A_TEXTURE_CENTER, "Icons.pcx", 104, 101, 121, 118, tc, TextureLoader::LT_CONTRAST, 1.5f );
			s->SetTextureTC( Style::A_TEXTURE_BOTTOM, "Icons.pcx", 132, 29, 149, 46, tc );
		}
		else {
			s->SetTextureTC( Style::A_TEXTURE_TOP, "Icons.pcx", 107, 4, 124, 21, tc );
			s->SetTextureTC( Style::A_TEXTURE_CENTER, "Icons.pcx", 104, 101, 121, 118, tc );
			s->SetTextureTC( Style::A_TEXTURE_BOTTOM, "Icons.pcx", 107, 29, 124, 46, tc );
		}
	});
}

}
}
