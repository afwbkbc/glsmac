#include "SlidingMenu.h"

#include "ui/theme/Style.h"

namespace task {
namespace mainmenu {
namespace style {

void SlidingMenu::AddStyles() {

	AddStyle(
		"SlidingMenuBlock", SH() {
			s->Set( ui::A_ALIGN, ui::ALIGN_RIGHT | ui::ALIGN_BOTTOM );
			s->Set( ui::A_WIDTH, 354 );
			s->Set( ui::A_BOTTOM, 16 );
		}
	);

	AddStyle(
		"SlidingMenuItem", SH() {
			s->Set( ui::A_LEFT, 0 );
			s->Set( ui::A_WIDTH, 300 );
			s->Set( ui::A_HEIGHT, 46 );
			s->Set( ui::A_ALIGN, ui::ALIGN_BOTTOM | ui::ALIGN_LEFT );
		}
	);

	AddStyle(
		"SlidingMenuItemBody", SH() {
			s->Set( ui::A_LEFT, 11 );
			s->Set( ui::A_TOP, 6 );
			s->Set( ui::A_WIDTH, 284 );
			s->Set( ui::A_HEIGHT, 36 );
			s->SetTexture(
				ui::A_TEXTURE,
				"palette.pcx",
				450, 28, 450, 28,
				ui::LT_ALPHA,
				0.8
			);
		}
	);

	AddStyle(
		"SlidingMenuItemBottom", SH() {
			s->Set( ui::A_LEFT, 6 );
			s->Set( ui::A_TOP, 40 );
			s->SetTexture(
				ui::A_TEXTURE,
				"console_x.pcx",
				699, 1, 706, 293,
				ui::LT_ROTATE |
					ui::LT_FLIPV |
					ui::LT_CONTRAST,
				0.2
			);
			s->Set( ui::A_SIZE_FROM_TEXTURE );
		}
	);

	AddStyle(
		"SlidingMenuItemLeft", SH() {
			s->Set( ui::A_LEFT, 6 );
			s->Set( ui::A_TOP, 6 );
			s->SetTexture(
				ui::A_TEXTURE,
				"interface.pcx",
				472, 756, 476, 790,
				ui::LT_CONTRAST,
				0.3
			);
			s->Set( ui::A_SIZE_FROM_TEXTURE );
		}
	);

	AddStyle(
		"SlidingMenuItemLowerTop", SH() {
			s->Set( ui::A_LEFT, 0 );
			s->Set( ui::A_TOP, -2 );
			s->SetTexture(
				ui::A_TEXTURE,
				"console_x.pcx",
				708, 1, 715, 354,
				ui::LT_ROTATE |
					ui::LT_FLIPV |
					ui::LT_CONTRAST,
				0.8
			);
			s->Set( ui::A_SIZE_FROM_TEXTURE );
		}
	);

	AddStyle(
		"SlidingMenuItemRight", SH() {
			s->Set( ui::A_LEFT, 294 );
			s->Set( ui::A_TOP, 6 );
			s->SetTexture(
				ui::A_TEXTURE,
				"interface.pcx",
				299, 756, 303, 790,
				ui::LT_CONTRAST,
				0.2
			);
			s->Set( ui::A_SIZE_FROM_TEXTURE );

		}
	);

	AddStyle(
		"SlidingMenuItemUpperTop", SH() {
			s->Set( ui::A_LEFT, 200 );
			s->Set( ui::A_TOP, -8 );
			s->SetTexture(
				ui::A_TEXTURE,
				"console_x.pcx",
				708, 1, 715, 354,
				ui::LT_ROTATE |
					ui::LT_FLIPV |
					ui::LT_CONTRAST,
				1.5
			);
			s->Set( ui::A_SIZE_FROM_TEXTURE );
		}
	);

	AddStyle(
		"SlidingMenuItemText", SH() {
			s->SetFont( ui::A_FONT, "arialnb.ttf", 20 );
		}
	);

	AddStyle(
		"SlidingMenuButtonText", {
			"SlidingMenuItemText"
		}, SH() {
			if ( s->Is( ui::M_SELECTED ) ) {
				s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGBA( 193, 240, 204, 255 ) );
			}
			else {
				s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGBA( 94, 162, 137, 255 ) );
			}
		}
	);

	AddStyle(
		"SlidingMenuTitleText", {
			"SlidingMenuItemText"
		}, SH() {
			s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGBA( 230, 243, 233, 255 ) );
		}
	);

	AddStyle(
		"SlidingMenuSound", SH() {
			s->SetSound( ui::A_SOUND, "menu out.wav" );
			s->Set( ui::A_SOUND_AUTOPLAY );
			s->Set( ui::A_SOUND_START_DELAY, 60 );
			s->Set( ui::A_SOUND_VOLUME, 0.7 );
		}
	);

}

}
}
}
