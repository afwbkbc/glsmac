#include "SlidingMenu.h"

#include "ui_legacy/theme/Style.h"

namespace task {
namespace mainmenu {
namespace style {

void SlidingMenu::AddStyles() {

	AddStyle(
		"SlidingMenuBlock", SH() {
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_RIGHT | ui_legacy::ALIGN_BOTTOM );
			s->Set( ui_legacy::A_WIDTH, 354 );
			s->Set( ui_legacy::A_BOTTOM, 16 );
		}
	);

	AddStyle(
		"SlidingMenuItem", SH() {
			s->Set( ui_legacy::A_LEFT, 0 );
			s->Set( ui_legacy::A_WIDTH, 300 );
			s->Set( ui_legacy::A_HEIGHT, 46 );
			s->Set( ui_legacy::A_ALIGN, ui_legacy::ALIGN_BOTTOM | ui_legacy::ALIGN_LEFT );
		}
	);

	AddStyle(
		"SlidingMenuItemBody", SH() {
			s->Set( ui_legacy::A_LEFT, 11 );
			s->Set( ui_legacy::A_TOP, 6 );
			s->Set( ui_legacy::A_WIDTH, 284 );
			s->Set( ui_legacy::A_HEIGHT, 36 );
			s->SetTexture(
				ui_legacy::A_TEXTURE,
				resource::PCX_PALETTE,
				450, 28, 450, 28,
				ui_legacy::LT_ALPHA,
				0.8
			);
		}
	);

	AddStyle(
		"SlidingMenuItemBottom", SH() {
			s->Set( ui_legacy::A_LEFT, 6 );
			s->Set( ui_legacy::A_TOP, 40 );
			s->SetTexture(
				ui_legacy::A_TEXTURE,
				resource::PCX_CONSOLE_X,
				699, 1, 706, 293,
				ui_legacy::LT_ROTATE |
					ui_legacy::LT_FLIPV |
					ui_legacy::LT_CONTRAST,
				0.2
			);
			s->Set( ui_legacy::A_SIZE_FROM_TEXTURE );
		}
	);

	AddStyle(
		"SlidingMenuItemLeft", SH() {
			s->Set( ui_legacy::A_LEFT, 6 );
			s->Set( ui_legacy::A_TOP, 6 );
			s->SetTexture(
				ui_legacy::A_TEXTURE,
				resource::PCX_INTERFACE,
				472, 756, 476, 790,
				ui_legacy::LT_CONTRAST,
				0.3
			);
			s->Set( ui_legacy::A_SIZE_FROM_TEXTURE );
		}
	);

	AddStyle(
		"SlidingMenuItemLowerTop", SH() {
			s->Set( ui_legacy::A_LEFT, 0 );
			s->Set( ui_legacy::A_TOP, -2 );
			s->SetTexture(
				ui_legacy::A_TEXTURE,
				resource::PCX_CONSOLE_X,
				708, 1, 715, 354,
				ui_legacy::LT_ROTATE |
					ui_legacy::LT_FLIPV |
					ui_legacy::LT_CONTRAST,
				0.8
			);
			s->Set( ui_legacy::A_SIZE_FROM_TEXTURE );
		}
	);

	AddStyle(
		"SlidingMenuItemRight", SH() {
			s->Set( ui_legacy::A_LEFT, 294 );
			s->Set( ui_legacy::A_TOP, 6 );
			s->SetTexture(
				ui_legacy::A_TEXTURE,
				resource::PCX_INTERFACE,
				299, 756, 303, 790,
				ui_legacy::LT_CONTRAST,
				0.2
			);
			s->Set( ui_legacy::A_SIZE_FROM_TEXTURE );

		}
	);

	AddStyle(
		"SlidingMenuItemUpperTop", SH() {
			s->Set( ui_legacy::A_LEFT, 200 );
			s->Set( ui_legacy::A_TOP, -8 );
			s->SetTexture(
				ui_legacy::A_TEXTURE,
				resource::PCX_CONSOLE_X,
				708, 1, 715, 354,
				ui_legacy::LT_ROTATE |
					ui_legacy::LT_FLIPV |
					ui_legacy::LT_CONTRAST,
				1.5
			);
			s->Set( ui_legacy::A_SIZE_FROM_TEXTURE );
		}
	);

	AddStyle(
		"SlidingMenuItemText", SH() {
			s->SetFont( ui_legacy::A_FONT, resource::TTF_ARIALNB, 20 );
		}
	);

	AddStyle(
		"SlidingMenuButtonText", {
			"SlidingMenuItemText"
		}, SH() {
			if ( s->Is( ui_legacy::M_SELECTED ) ) {
				s->SetColor( ui_legacy::A_TEXT_COLOR, types::Color::FromRGBA( 193, 240, 204, 255 ) );
			}
			else {
				s->SetColor( ui_legacy::A_TEXT_COLOR, types::Color::FromRGBA( 94, 162, 137, 255 ) );
			}
		}
	);

	AddStyle(
		"SlidingMenuTitleText", {
			"SlidingMenuItemText"
		}, SH() {
			s->SetColor( ui_legacy::A_TEXT_COLOR, types::Color::FromRGBA( 230, 243, 233, 255 ) );
		}
	);

	AddStyle(
		"SlidingMenuSound", SH() {
			s->SetSound( ui_legacy::A_SOUND, resource::WAV_MENU_OUT );
			s->Set( ui_legacy::A_SOUND_AUTOPLAY );
			s->Set( ui_legacy::A_SOUND_START_DELAY, 60 );
			s->Set( ui_legacy::A_SOUND_VOLUME, 0.7 );
		}
	);

}

}
}
}
