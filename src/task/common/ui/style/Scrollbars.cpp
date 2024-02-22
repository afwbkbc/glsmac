#include "Scrollbars.h"

namespace task {
namespace style {

void Scrollbars::AddStyles() {

	SetPrefix( "Scrollbar" );

	// vertical
	AddStyle(
		"Vertical", SH() {
			s->SetColorTexture( Style::A_TEXTURE_BACK, Color( 0.0f, 0.0f, 0.0f, 1.0f ) );
		}
	);
	AddStyle(
		"VerticalThick", { "Vertical" }, SH() {
			s->Set( Style::A_WIDTH, 18 );

			if ( s->Is( Style::M_ACTIVE ) ) {
				s->SetTexture( Style::A_TEXTURE_TOP, "Icons.pcx", 157, 4, 174, 21 );
				s->SetTexture( Style::A_TEXTURE_CENTER, "Icons.pcx", 104, 101, 121, 118, TextureLoader::LT_CONTRAST, 2.0f );
				s->SetTexture( Style::A_TEXTURE_BOTTOM, "Icons.pcx", 157, 29, 174, 46 );
			}
			else if ( s->Is( Style::M_HOVER ) ) {
				s->SetTexture( Style::A_TEXTURE_TOP, "Icons.pcx", 132, 4, 149, 21 );
				s->SetTexture( Style::A_TEXTURE_CENTER, "Icons.pcx", 104, 101, 121, 118, TextureLoader::LT_CONTRAST, 1.5f );
				s->SetTexture( Style::A_TEXTURE_BOTTOM, "Icons.pcx", 132, 29, 149, 46 );
			}
			else {
				s->SetTexture( Style::A_TEXTURE_TOP, "Icons.pcx", 107, 4, 124, 21 );
				s->SetTexture( Style::A_TEXTURE_CENTER, "Icons.pcx", 104, 101, 121, 118 );
				s->SetTexture( Style::A_TEXTURE_BOTTOM, "Icons.pcx", 107, 29, 124, 46 );
			}
		}
	);

	// horizontal inline
	AddStyle(
		"HorizontalArrow", SH() {
			s->Set( Style::A_WIDTH, 12 );
			s->Set( Style::A_HEIGHT, 42 );
		}
	);
	AddStyle(
		"HorizontalLeftArrow", { "HorizontalArrow" }, SH() {
			if ( s->Is( Style::M_ACTIVE ) || s->Is( Style::M_HIGHLIGHT ) ) {
				s->SetTexture( Style::A_TEXTURE, "interface.pcx", 423, 153, 434, 194 );
			}
			else if ( s->Is( Style::M_HOVER ) ) {
				s->SetTexture( Style::A_TEXTURE, "interface.pcx", 410, 153, 421, 194 );
			}
			else {
				s->SetTexture( Style::A_TEXTURE, "interface.pcx", 397, 153, 408, 194 );
			}
			s->Set( Style::A_ALIGN, UIObject::ALIGN_LEFT | UIObject::ALIGN_VCENTER );
			s->Set( Style::A_LEFT, 3 );
		}
	);
	AddStyle(
		"HorizontalRightArrow", { "HorizontalArrow" }, SH() {
			if ( s->Is( Style::M_ACTIVE ) || s->Is( Style::M_HIGHLIGHT ) ) {
				s->SetTexture( Style::A_TEXTURE, "interface.pcx", 423, 196, 434, 237 );
			}
			else if ( s->Is( Style::M_HOVER ) ) {
				s->SetTexture( Style::A_TEXTURE, "interface.pcx", 410, 196, 421, 237 );
			}
			else {
				s->SetTexture( Style::A_TEXTURE, "interface.pcx", 397, 196, 408, 237 );
			}
			s->Set( Style::A_ALIGN, UIObject::ALIGN_RIGHT | UIObject::ALIGN_VCENTER );
			s->Set( Style::A_RIGHT, 3 );
		}
	);

}

}
}
