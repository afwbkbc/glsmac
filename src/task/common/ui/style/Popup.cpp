#include "Popup.h"

#include "ui/theme/Style.h"

namespace task {
namespace style {

void Popup::AddStyles() {

	// TODO: multilevel includes	
	AddStyle(
		"PopupBorder", SH() {
			// frame
			{
				const std::unordered_map< ui::attribute_type_t, std::vector< size_t > > textures = {
					{ ui::A_TEXTURE_BACK,   { 86,  353, 109, 376 } },
					{ ui::A_TEXTURE_LEFT,   { 79,  431, 79,  454 } },
					{ ui::A_TEXTURE_TOP,    { 86,  307, 109, 307 } },
					{ ui::A_TEXTURE_RIGHT,  { 116, 431, 116, 454 } },
					{ ui::A_TEXTURE_BOTTOM, { 86,  461, 109, 461 } },
				};
				for ( auto& texture : textures ) {
					s->SetTexture( texture.first, resource::PCX_INTERFACE, texture.second[ 0 ], texture.second[ 1 ], texture.second[ 2 ], texture.second[ 3 ] );
				}
			}
		}
	);

	AddStyle(
		"PopupFrame", SH() {

			// frame
			{
				const std::unordered_map< ui::attribute_type_t, std::vector< size_t > > textures = (
					s->Is( ui::M_HIGHLIGHT )
						?
						std::unordered_map< ui::attribute_type_t, std::vector< size_t > >{
							{ ui::A_TEXTURE_BACK,   { 86, 353, 109, 376 } },
							{ ui::A_TEXTURE_LEFT,   { 40, 626, 40,  649 } },
							{ ui::A_TEXTURE_TOP,    { 47, 619, 70,  619 } },
							{ ui::A_TEXTURE_RIGHT,  { 77, 626, 77,  649 } },
							{ ui::A_TEXTURE_BOTTOM, { 47, 656, 70,  656 } },
						}
						:
						std::unordered_map< ui::attribute_type_t, std::vector< size_t > >{
							{ ui::A_TEXTURE_BACK,   { 86,  353, 109, 376 } },
							{ ui::A_TEXTURE_LEFT,   { 79,  431, 79,  454 } },
							{ ui::A_TEXTURE_TOP,    { 86,  307, 109, 307 } },
							{ ui::A_TEXTURE_RIGHT,  { 116, 431, 116, 454 } },
							{ ui::A_TEXTURE_BOTTOM, { 86,  461, 109, 461 } },
						}
				);
				for ( auto& texture : textures ) {
					s->SetTexture( texture.first, resource::PCX_INTERFACE, texture.second[ 0 ], texture.second[ 1 ], texture.second[ 2 ], texture.second[ 3 ] );
				}
			}

			// header
			{
				const std::unordered_map< ui::attribute_type_t, std::vector< size_t > > textures = {
					{ ui::A_HEADER_TEXTURE_BACK,   { 86,  314, 109, 337 } },
					{ ui::A_HEADER_TEXTURE_LEFT,   { 79,  431, 79,  454 } },
					{ ui::A_HEADER_TEXTURE_TOP,    { 86,  307, 109, 307 } },
					{ ui::A_HEADER_TEXTURE_RIGHT,  { 116, 431, 116, 454 } },
					{ ui::A_HEADER_TEXTURE_BOTTOM, { 86,  461, 109, 461 } },
				};
				for ( auto& texture : textures ) {
					s->SetTexture( texture.first, resource::PCX_INTERFACE, texture.second[ 0 ], texture.second[ 1 ], texture.second[ 2 ], texture.second[ 3 ] );
				}
			}
			s->SetColor( ui::A_HEADER_TEXTCOLOR, types::Color::FromRGB( 109, 126, 178 ) );
		}
	);

	AddStyle(
		"PopupWindow", { "PopupFrame" }, SH() {
			s->SetFont( ui::A_HEADER_FONT, resource::TTF_ARIALNB, 18 );
			s->Set( ui::A_HEADER_HEIGHT, 22 );
		}
	);

	AddStyle(
		"PopupSection", { "PopupFrame" }, SH() {
			s->SetFont( ui::A_HEADER_FONT, resource::TTF_ARIALNB, 16 );
			s->Set( ui::A_HEADER_HEIGHT, 20 );
		}
	);

	AddStyle(
		"PopupLabel", SH() {
			s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 109, 126, 178 ) );
			s->SetFont( ui::A_FONT, resource::TTF_ARIALNB, 18 );
		}
	);

	AddStyle(
		"PopupInput", {
			"PopupBorder"
		}, SH() {
			s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 159, 196, 198 ) );
			s->SetFont( ui::A_FONT, resource::TTF_ARIALNB, 18 );
			s->Set( ui::A_HEIGHT, 22 );
		}
	);

	AddStyle(
		"PopupButton", SH() {
			std::unordered_map< ui::attribute_type_t, std::vector< size_t > > textures = {};

			// borders
			if ( s->Is( ui::M_ACTIVE ) || s->Is( ui::M_SELECTED ) ) {
				textures = {
					{ ui::A_TEXTURE_BACK,   { 68,  170, 134, 187 } },
					{ ui::A_TEXTURE_LEFT,   { 64,  170, 64,  187 } },
					{ ui::A_TEXTURE_TOP,    { 68,  169, 134, 169 } },
					{ ui::A_TEXTURE_RIGHT,  { 138, 170, 138, 187 } },
					{ ui::A_TEXTURE_BOTTOM, { 68,  188, 134, 188 } },
				};
			}
			else {
				if ( s->Is( ui::M_HOVER ) ) {
					textures = {
						{ ui::A_TEXTURE_LEFT,   { 64,  149, 64,  166 } },
						{ ui::A_TEXTURE_TOP,    { 68,  148, 134, 148 } },
						{ ui::A_TEXTURE_RIGHT,  { 138, 149, 138, 166 } },
						{ ui::A_TEXTURE_BOTTOM, { 68,  167, 134, 167 } },
					};
				}
				else {
					textures = {
						{ ui::A_TEXTURE_LEFT,   { 64,  128, 64,  145 } },
						{ ui::A_TEXTURE_TOP,    { 68,  127, 134, 127 } },
						{ ui::A_TEXTURE_RIGHT,  { 138, 128, 138, 145 } },
						{ ui::A_TEXTURE_BOTTOM, { 68,  146, 134, 146 } },
					};
				}
				textures[ ui::A_TEXTURE_BACK ] = {
					68,
					149,
					134,
					166
				};
			}

			for ( auto& texture : textures ) {
				s->SetTexture( texture.first, resource::PCX_INTERFACE, texture.second[ 0 ], texture.second[ 1 ], texture.second[ 2 ], texture.second[ 3 ] );
			}

			if ( s->Is( ui::M_HOVER ) && !s->Is( ui::M_ACTIVE ) && !s->Is( ui::M_SELECTED ) ) {
				s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 164, 176, 232 ) );
			}
			else {
				s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 13, 23, 36 ) );
			}
		}
	);

	AddStyle(
		"PopupButtonList", {
			"PopupButton"
		}, SH() {
			s->Set( ui::A_LEFT, 3 );
			s->Set( ui::A_RIGHT, 3 );
			//s->Set( ui::A_TOP, 3 );
			s->Set( ui::A_BOTTOM, 3 );

			s->SetFont( ui::A_FONT, resource::TTF_ARIALNB, 16 );
			s->Set( ui::A_TEXT_ALIGN, ui::ALIGN_LEFT );
		}
	);

	AddStyle(
		"PopupButtonOkCancel", {
			"PopupButton"
		}, SH() {
			s->Set( ui::A_WIDTH, 230 );
			s->Set( ui::A_HEIGHT, 20 );

			s->SetFont( ui::A_FONT, resource::TTF_ARIALNB, 18 );
			s->SetSound( ui::A_BUTTON_CLICK_SOUND, resource::WAV_OK );
			s->Set( ui::A_SOUND_VOLUME, 0.5 );
		}
	);

	AddStyle(
		"PopupText", SH() {
			s->SetFont( ui::A_FONT, resource::TTF_ARIALNB, 18 );
			s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 129, 146, 198 ) );
		}
	);

	AddStyle(
		"Popup", { "PopupFrame" }, SH() {
			s->Set( ui::A_Z_INDEX, 0.8f );
		}
	);

	// TODO: fix overlay fbo blending
	/*AddStyle( "PopupBackground", SH() {
		s->Set( ui::A_Z_INDEX, 0.7f );
		s->SetColorTexture( ui::A_TEXTURE, types::Color( 0.0f, 0.0f, 0.0f, 0.5f ) );
	});*/

	AddStyle(
		"PopupTextList", { "PopupText" }, SH() {
			s->Set( ui::A_ITEM_HEIGHT, 17 ); // TODO: auto-size?
			s->Set( ui::A_ITEM_MARGIN, 0 ); // TODO: fix alignment if non-zero
			s->Set( ui::A_TEXT_ALIGN, ui::ALIGN_LEFT );
		}
	);

	AddStyle(
		"PopupFileList", SH() {
			s->Set( ui::A_RIGHT, 4 );
			s->Set( ui::A_BOTTOM, 30 ); // to fit text input
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALN, 16 );
			s->Set( ui::A_ITEM_HEIGHT, 17 ); // TODO: auto-size?
		}
	);

	AddStyle(
		"PopupFileListInput", { "PopupBorder" }, SH() {
			s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 92, 124, 188 ) );
			s->SetColor( ui::A_HINT_COLOR, types::Color::FromRGBA( 92, 124, 188, 127 ) );
			s->SetFont( ui::A_FONT, resource::TTF_ARIALN, 18 );
			s->Set( ui::A_HEIGHT, 22 );
			s->Set( ui::A_ALIGN, ui::ALIGN_HCENTER | ui::ALIGN_BOTTOM );
			s->Set( ui::A_BOTTOM, 2 );
			s->Set( ui::A_LEFT, 4 );
			s->Set( ui::A_RIGHT, 4 );
		}
	);

	AddStyle(
		"PopupFileListItem", SH() {
			s->Set( ui::A_TEXT_ALIGN, ui::ALIGN_LEFT | ui::ALIGN_VCENTER );
			s->Set( ui::A_TEXT_LEFT, 20 );

			if ( s->Is( ui::M_SELECTED ) ) {
				s->SetTexture( ui::A_ITEM_TEXTURE, resource::PCX_INTERFACE, 68, 169, 134, 188 );
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 13, 23, 36 ) );
			}
			else if ( s->Is( ui::M_HOVER ) ) {
				s->SetTexture( ui::A_ITEM_TEXTURE, resource::PCX_INTERFACE, 68, 149, 134, 166, ui::LT_CONTRAST, 0.7f );
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 120, 164, 212 ) );
			}
			else {
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 92, 124, 188 ) );

				// TODO: fix styles where texture is absent in normal modifier and remove this hack
				s->SetColorTexture( ui::A_ITEM_TEXTURE, types::Color( 0.0f, 0.0f, 0.0f, 0.0f ) );
			}

			s->Set( ui::A_ITEM_ICON_WIDTH, 16 );
		}
	);

	AddStyle(
		"PopupFileListItemFile", { "PopupFileListItem" }, SH() {
			s->SetTexture( ui::A_ITEM_ICON_TEXTURE, resource::PCX_JACKAL, 18, 1, 33, 16 );
		}
	);

	AddStyle(
		"PopupFileListItemDir", { "PopupFileListItem" }, SH() {
			s->SetTexture( ui::A_ITEM_ICON_TEXTURE, resource::PCX_JACKAL, 1, 1, 16, 16 );
		}
	);

	AddStyle(
		"PopupFileListItemDirUp", { "PopupFileListItem" }, SH() {
			s->SetTexture( ui::A_ITEM_ICON_TEXTURE, resource::PCX_JACKAL, 35, 1, 50, 16 );
		}
	);

	// TODO: multilevel includes
	AddStyle(
		"DefaultPopupFrame", {
			"PopupFrame"
		}, SH() {
			s->SetFont( ui::A_HEADER_FONT, resource::TTF_ARIALNB, 18 );
			s->Set( ui::A_HEADER_HEIGHT, 22 );
			s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 109, 126, 178 ) );
			s->SetFont( ui::A_FONT, resource::TTF_ARIALNB, 20 );
		}
	);

	AddStyle(
		"DefaultPopupLabel", SH() {
			s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 109, 126, 178 ) );
			s->SetFont( ui::A_FONT, resource::TTF_ARIALNB, 20 );
		}
	);

	AddStyle(
		"DefaultPopupButton", {
			"PopupButton"
		}, SH() {
			s->Set( ui::A_WIDTH, 234 );
			s->Set( ui::A_HEIGHT, 20 );

			s->SetFont( ui::A_FONT, resource::TTF_ARIALNB, 18 );
			s->SetSound( ui::A_BUTTON_CLICK_SOUND, resource::WAV_OK );
			s->Set( ui::A_SOUND_VOLUME, 0.5 );
		}
	);

	AddStyle(
		"PopupDropdown", {
			"PopupFrame"
		}, SH() {
			s->Set( ui::A_HEIGHT, 24 );
			s->SetFont( ui::A_FONT, resource::TTF_ARIALNB, 18 );
			s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 255, 255, 255 ) );
			s->Set( ui::A_TEXT_ALIGN, ui::ALIGN_LEFT | ui::ALIGN_TOP );
			s->Set( ui::A_TEXT_LEFT, 4 );
			s->Set( ui::A_PADDING, 2 );

		}
	);

	AddStyle(
		"PopupDropdownOpenClose", SH() {
			s->Set( ui::A_WIDTH, 7 );
			s->Set( ui::A_HEIGHT, 13 );
			s->Set( ui::A_ALIGN, ui::A_RIGHT );
			s->Set( ui::A_TOP, 3 );
			s->Set( ui::A_RIGHT, 3 );

			if ( s->Is( ui::M_ACTIVE ) ) {
				s->SetTexture( ui::A_TEXTURE, resource::PCX_INTERFACE, 550, 537, 556, 549 );
			}
			else if ( s->Is( ui::M_HOVER ) ) {
				s->SetTexture( ui::A_TEXTURE, resource::PCX_INTERFACE, 542, 537, 548, 549 );
			}
			else {
				s->SetTexture( ui::A_TEXTURE, resource::PCX_INTERFACE, 534, 537, 540, 549 );
			}
		}
	);

	AddStyle(
		"PopupDropdownChoices", SH() {

			s->Set( ui::A_ITEM_MARGIN, 0 );
			s->Set( ui::A_ITEM_HEIGHT, 19 );
			s->Set( ui::A_BORDER_SIZE, 0 );

			s->SetFont( ui::A_FONT, resource::TTF_ARIALNB, 17 );
			s->Set( ui::A_TEXT_ALIGN, ui::ALIGN_LEFT | ui::ALIGN_TOP );

			if ( s->Is( ui::M_HOVER ) ) {
				s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 53, 61, 115 ) );
				s->SetTexture( ui::A_TEXTURE_BACK, resource::PCX_INTERFACE, 68, 169, 134, 188 );
			}
			else {
				s->SetColor( ui::A_TEXT_COLOR, types::Color::FromRGB( 176, 212, 235 ) );
				s->SetTexture( ui::A_TEXTURE_BACK, resource::PCX_INTERFACE, 68, 127, 134, 146 );
			}
		}
	);

}

}
}
