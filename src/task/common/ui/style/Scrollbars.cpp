#include "Scrollbars.h"

#include "ui/theme/Style.h"

namespace task {
namespace style {

void Scrollbars::AddStyles() {

	SetPrefix( "Scrollbar" );

	// vertical
	AddStyle(
		"Vertical", SH() {
			s->SetColorTexture( ui::A_TEXTURE_BACK, types::Color( 0.0f, 0.0f, 0.0f, 1.0f ) );
		}
	);
	AddStyle(
		"VerticalThick", { "Vertical" }, SH() {
			s->Set( ui::A_WIDTH, 18 );

			if ( s->Is( ui::M_ACTIVE ) ) {
				s->SetTexture( ui::A_TEXTURE_TOP, "Icons.pcx", 157, 4, 174, 21 );
				s->SetTexture( ui::A_TEXTURE_CENTER, "Icons.pcx", 104, 101, 121, 118, ui::LT_CONTRAST, 2.0f );
				s->SetTexture( ui::A_TEXTURE_BOTTOM, "Icons.pcx", 157, 29, 174, 46 );
			}
			else if ( s->Is( ui::M_HOVER ) ) {
				s->SetTexture( ui::A_TEXTURE_TOP, "Icons.pcx", 132, 4, 149, 21 );
				s->SetTexture( ui::A_TEXTURE_CENTER, "Icons.pcx", 104, 101, 121, 118, ui::LT_CONTRAST, 1.5f );
				s->SetTexture( ui::A_TEXTURE_BOTTOM, "Icons.pcx", 132, 29, 149, 46 );
			}
			else {
				s->SetTexture( ui::A_TEXTURE_TOP, "Icons.pcx", 107, 4, 124, 21 );
				s->SetTexture( ui::A_TEXTURE_CENTER, "Icons.pcx", 104, 101, 121, 118 );
				s->SetTexture( ui::A_TEXTURE_BOTTOM, "Icons.pcx", 107, 29, 124, 46 );
			}
		}
	);

	// horizontal inline
	AddStyle(
		"HorizontalArrow", SH() {
			s->Set( ui::A_WIDTH, 12 );
			s->Set( ui::A_HEIGHT, 42 );
		}
	);
	AddStyle(
		"HorizontalLeftArrow", { "HorizontalArrow" }, SH() {
			if ( s->Is( ui::M_ACTIVE ) || s->Is( ui::M_HIGHLIGHT ) ) {
				s->SetTexture( ui::A_TEXTURE, "interface.pcx", 423, 153, 434, 194 );
			}
			else if ( s->Is( ui::M_HOVER ) ) {
				s->SetTexture( ui::A_TEXTURE, "interface.pcx", 410, 153, 421, 194 );
			}
			else {
				s->SetTexture( ui::A_TEXTURE, "interface.pcx", 397, 153, 408, 194 );
			}
			s->Set( ui::A_ALIGN, ui::ALIGN_LEFT | ui::ALIGN_VCENTER );
			s->Set( ui::A_LEFT, 3 );
		}
	);
	AddStyle(
		"HorizontalRightArrow", { "HorizontalArrow" }, SH() {
			if ( s->Is( ui::M_ACTIVE ) || s->Is( ui::M_HIGHLIGHT ) ) {
				s->SetTexture( ui::A_TEXTURE, "interface.pcx", 423, 196, 434, 237 );
			}
			else if ( s->Is( ui::M_HOVER ) ) {
				s->SetTexture( ui::A_TEXTURE, "interface.pcx", 410, 196, 421, 237 );
			}
			else {
				s->SetTexture( ui::A_TEXTURE, "interface.pcx", 397, 196, 408, 237 );
			}
			s->Set( ui::A_ALIGN, ui::ALIGN_RIGHT | ui::ALIGN_VCENTER );
			s->Set( ui::A_RIGHT, 3 );
		}
	);

}

}
}
