#include "Popup.h"

namespace task {
namespace style {

void Popup::AddStyles() {

	// TODO: multilevel includes	
	AddStyle(
		"PopupBorder", SH()
	{
		// frame
		{
			const std::unordered_map< Style::attribute_type_t, std::vector< size_t > > textures = {
				{ Style::A_TEXTURE_BACK,   { 86,  353, 109, 376 } },
				{ Style::A_TEXTURE_LEFT,   { 79,  431, 79,  454 } },
				{ Style::A_TEXTURE_TOP,    { 86,  307, 109, 307 } },
				{ Style::A_TEXTURE_RIGHT,  { 116, 431, 116, 454 } },
				{ Style::A_TEXTURE_BOTTOM, { 86,  461, 109, 461 } },
			};
			for ( auto& texture : textures ) {
				s->SetTexture( texture.first, "interface.pcx", texture.second[ 0 ], texture.second[ 1 ], texture.second[ 2 ], texture.second[ 3 ] );
			}
		}
	}
	);

	AddStyle(
		"PopupFrame", SH()
	{

		// frame
		{
			const std::unordered_map< Style::attribute_type_t, std::vector< size_t > > textures = (
				s->Is( Style::M_HIGHLIGHT )
					?
					std::unordered_map< Style::attribute_type_t, std::vector< size_t > >{
						{ Style::A_TEXTURE_BACK,   { 86, 353, 109, 376 } },
						{ Style::A_TEXTURE_LEFT,   { 40, 626, 40,  649 } },
						{ Style::A_TEXTURE_TOP,    { 47, 619, 70,  619 } },
						{ Style::A_TEXTURE_RIGHT,  { 77, 626, 77,  649 } },
						{ Style::A_TEXTURE_BOTTOM, { 47, 656, 70,  656 } },
					}
					:
					std::unordered_map< Style::attribute_type_t, std::vector< size_t > >{
						{ Style::A_TEXTURE_BACK,   { 86,  353, 109, 376 } },
						{ Style::A_TEXTURE_LEFT,   { 79,  431, 79,  454 } },
						{ Style::A_TEXTURE_TOP,    { 86,  307, 109, 307 } },
						{ Style::A_TEXTURE_RIGHT,  { 116, 431, 116, 454 } },
						{ Style::A_TEXTURE_BOTTOM, { 86,  461, 109, 461 } },
					}
			);
			for ( auto& texture : textures ) {
				s->SetTexture( texture.first, "interface.pcx", texture.second[ 0 ], texture.second[ 1 ], texture.second[ 2 ], texture.second[ 3 ] );
			}
		}

		// header
		{
			const std::unordered_map< Style::attribute_type_t, std::vector< size_t > > textures = {
				{ Style::A_HEADER_TEXTURE_BACK,   { 86,  314, 109, 337 } },
				{ Style::A_HEADER_TEXTURE_LEFT,   { 79,  431, 79,  454 } },
				{ Style::A_HEADER_TEXTURE_TOP,    { 86,  307, 109, 307 } },
				{ Style::A_HEADER_TEXTURE_RIGHT,  { 116, 431, 116, 454 } },
				{ Style::A_HEADER_TEXTURE_BOTTOM, { 86,  461, 109, 461 } },
			};
			for ( auto& texture : textures ) {
				s->SetTexture( texture.first, "interface.pcx", texture.second[ 0 ], texture.second[ 1 ], texture.second[ 2 ], texture.second[ 3 ] );
			}
		}
		s->SetColor( Style::A_HEADER_TEXTCOLOR, Color::FromRGB( 109, 126, 178 ) );
	}
	);

	AddStyle(
		"PopupWindow", { "PopupFrame" }, SH()
	{
		s->SetFont( Style::A_HEADER_FONT, "arialnb.ttf", 18 );
		s->Set( Style::A_HEADER_HEIGHT, 22 );
	}
	);

	AddStyle(
		"PopupSection", { "PopupFrame" }, SH()
	{
		s->SetFont( Style::A_HEADER_FONT, "arialnb.ttf", 16 );
		s->Set( Style::A_HEADER_HEIGHT, 20 );
	}
	);

	AddStyle(
		"PopupLabel", SH()
	{
		s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 109, 126, 178 ) );
		s->SetFont( Style::A_FONT, "arialnb.ttf", 18 );
	}
	);

	AddStyle(
		"PopupInput", {
			"PopupBorder"
		}, SH()
	{
		s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 159, 196, 198 ) );
		s->SetFont( Style::A_FONT, "arialnb.ttf", 18 );
		s->Set( Style::A_HEIGHT, 22 );
	}
	);

	AddStyle(
		"PopupButton", SH()
	{
		std::unordered_map< Style::attribute_type_t, std::vector< size_t > > textures = {};

		// borders
		if ( s->Is( Style::M_ACTIVE ) || s->Is( Style::M_SELECTED ) ) {
			textures = {
				{ Style::A_TEXTURE_BACK,   { 68,  170, 134, 187 } },
				{ Style::A_TEXTURE_LEFT,   { 64,  170, 64,  187 } },
				{ Style::A_TEXTURE_TOP,    { 68,  169, 134, 169 } },
				{ Style::A_TEXTURE_RIGHT,  { 138, 170, 138, 187 } },
				{ Style::A_TEXTURE_BOTTOM, { 68,  188, 134, 188 } },
			};
		}
		else {
			if ( s->Is( Style::M_HOVER ) ) {
				textures = {
					{ Style::A_TEXTURE_LEFT,   { 64,  149, 64,  166 } },
					{ Style::A_TEXTURE_TOP,    { 68,  148, 134, 148 } },
					{ Style::A_TEXTURE_RIGHT,  { 138, 149, 138, 166 } },
					{ Style::A_TEXTURE_BOTTOM, { 68,  167, 134, 167 } },
				};
			}
			else {
				textures = {
					{ Style::A_TEXTURE_LEFT,   { 64,  128, 64,  145 } },
					{ Style::A_TEXTURE_TOP,    { 68,  127, 134, 127 } },
					{ Style::A_TEXTURE_RIGHT,  { 138, 128, 138, 145 } },
					{ Style::A_TEXTURE_BOTTOM, { 68,  146, 134, 146 } },
				};
			}
			textures[ Style::A_TEXTURE_BACK ] = {
				68,
				149,
				134,
				166
			};
		}

		for ( auto& texture : textures ) {
			s->SetTexture( texture.first, "interface.pcx", texture.second[ 0 ], texture.second[ 1 ], texture.second[ 2 ], texture.second[ 3 ] );
		}

		if ( s->Is( Style::M_HOVER ) && !s->Is( Style::M_ACTIVE ) && !s->Is( Style::M_SELECTED ) ) {
			s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 164, 176, 232 ) );
		}
		else {
			s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 13, 23, 36 ) );
		}
	}
	);

	AddStyle(
		"PopupButtonList", {
			"PopupButton"
		}, SH()
	{
		s->Set( Style::A_LEFT, 3 );
		s->Set( Style::A_RIGHT, 3 );
		//s->Set( Style::A_TOP, 3 );
		s->Set( Style::A_BOTTOM, 3 );

		s->SetFont( Style::A_FONT, "arialnb.ttf", 16 );
		s->Set( Style::A_TEXT_ALIGN, UIObject::ALIGN_LEFT );
	}
	);

	AddStyle(
		"PopupButtonOkCancel", {
			"PopupButton"
		}, SH()
	{
		s->Set( Style::A_WIDTH, 230 );
		s->Set( Style::A_HEIGHT, 20 );

		s->SetFont( Style::A_FONT, "arialnb.ttf", 18 );
		s->SetSound( Style::A_BUTTON_CLICK_SOUND, "ok.wav" );
		s->Set( Style::A_SOUND_VOLUME, 0.5 );
	}
	);

	AddStyle(
		"PopupText", SH()
	{
		s->SetFont( Style::A_FONT, "arialnb.ttf", 18 );
		s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 129, 146, 198 ) );
	}
	);

	AddStyle(
		"Popup", { "PopupFrame" }, SH()
	{
		s->Set( Style::A_Z_INDEX, 0.8f );
	}
	);

	// TODO: fix overlay fbo blending
	/*AddStyle( "PopupBackground", SH() {
		s->Set( Style::A_Z_INDEX, 0.7f );
		s->SetColorTexture( Style::A_TEXTURE, Color( 0.0f, 0.0f, 0.0f, 0.5f ) );
	});*/

	AddStyle(
		"PopupTextList", { "PopupText" }, SH()
	{
		s->Set( Style::A_ITEM_HEIGHT, 17 ); // TODO: auto-size?
		s->Set( Style::A_ITEM_MARGIN, 0 ); // TODO: fix alignment if non-zero
		s->Set( Style::A_TEXT_ALIGN, UIObject::ALIGN_LEFT );
	}
	);

	AddStyle(
		"PopupFileList", SH()
	{
		s->Set( Style::A_RIGHT, 4 );
		s->Set( Style::A_BOTTOM, 30 ); // to fit text input
		s->SetFont( ::Style::A_FONT, "arialn.ttf", 16 );
		s->Set( Style::A_ITEM_HEIGHT, 17 ); // TODO: auto-size?
	}
	);

	AddStyle(
		"PopupFileListInput", { "PopupBorder" }, SH()
	{
		s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 92, 124, 188 ) );
		s->SetColor( Style::A_HINT_COLOR, Color::FromRGBA( 92, 124, 188, 127 ) );
		s->SetFont( Style::A_FONT, "arialn.ttf", 18 );
		s->Set( Style::A_HEIGHT, 22 );
		s->Set( Style::A_ALIGN, UIObject::ALIGN_HCENTER | UIObject::ALIGN_BOTTOM );
		s->Set( Style::A_BOTTOM, 2 );
		s->Set( Style::A_LEFT, 4 );
		s->Set( Style::A_RIGHT, 4 );
	}
	);

	AddStyle(
		"PopupFileListItem", SH()
	{
		s->Set( Style::A_TEXT_ALIGN, UIObject::ALIGN_LEFT | UIObject::ALIGN_VCENTER );
		s->Set( Style::A_TEXT_LEFT, 20 );

		if ( s->Is( Style::M_SELECTED ) ) {
			s->SetTexture( Style::A_ITEM_TEXTURE, "interface.pcx", 68, 169, 134, 188 );
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 13, 23, 36 ) );
		}
		else if ( s->Is( Style::M_HOVER ) ) {
			s->SetTexture( Style::A_ITEM_TEXTURE, "interface.pcx", 68, 149, 134, 166, TextureLoader::LT_CONTRAST, 0.7f );
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 120, 164, 212 ) );
		}
		else {
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 92, 124, 188 ) );

			// TODO: fix styles where texture is absent in normal modifier and remove this hack
			s->SetColorTexture( Style::A_ITEM_TEXTURE, Color( 0.0f, 0.0f, 0.0f, 0.0f ) );
		}

		s->Set( Style::A_ITEM_ICON_WIDTH, 16 );
	}
	);

	AddStyle(
		"PopupFileListItemFile", { "PopupFileListItem" }, SH()
	{
		s->SetTexture( Style::A_ITEM_ICON_TEXTURE, "Jackal.pcx", 18, 1, 33, 16 );
	}
	);

	AddStyle(
		"PopupFileListItemDir", { "PopupFileListItem" }, SH()
	{
		s->SetTexture( Style::A_ITEM_ICON_TEXTURE, "Jackal.pcx", 1, 1, 16, 16 );
	}
	);

	AddStyle(
		"PopupFileListItemDirUp", { "PopupFileListItem" }, SH()
	{
		s->SetTexture( Style::A_ITEM_ICON_TEXTURE, "Jackal.pcx", 35, 1, 50, 16 );
	}
	);

	// TODO: multilevel includes
	AddStyle(
		"DefaultPopupFrame", {
			"PopupFrame"
		}, SH()
	{
		s->SetFont( Style::A_HEADER_FONT, "arialnb.ttf", 18 );
		s->Set( Style::A_HEADER_HEIGHT, 22 );
		s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 109, 126, 178 ) );
		s->SetFont( Style::A_FONT, "arialnb.ttf", 20 );
	}
	);

	AddStyle(
		"DefaultPopupLabel", SH()
	{
		s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 109, 126, 178 ) );
		s->SetFont( Style::A_FONT, "arialnb.ttf", 20 );
	}
	);

	AddStyle(
		"DefaultPopupButton", {
			"PopupButton"
		}, SH()
	{
		s->Set( Style::A_WIDTH, 234 );
		s->Set( Style::A_HEIGHT, 20 );

		s->SetFont( Style::A_FONT, "arialnb.ttf", 18 );
		s->SetSound( Style::A_BUTTON_CLICK_SOUND, "ok.wav" );
		s->Set( Style::A_SOUND_VOLUME, 0.5 );
	}
	);

	AddStyle(
		"PopupDropdown", {
			"PopupFrame"
		}, SH()
	{
		s->Set( Style::A_HEIGHT, 24 );
		s->SetFont( Style::A_FONT, "arialnb.ttf", 18 );
		s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 255, 255, 255 ) );
		s->Set( Style::A_TEXT_ALIGN, UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		s->Set( Style::A_TEXT_LEFT, 4 );
		s->Set( Style::A_PADDING, 2 );

	}
	);

	AddStyle(
		"PopupDropdownOpenClose", SH()
	{
		s->Set( Style::A_WIDTH, 7 );
		s->Set( Style::A_HEIGHT, 13 );
		s->Set( Style::A_ALIGN, Style::A_RIGHT );
		s->Set( Style::A_TOP, 3 );
		s->Set( Style::A_RIGHT, 3 );

		if ( s->Is( Style::M_ACTIVE ) ) {
			s->SetTexture( Style::A_TEXTURE, "interface.pcx", 550, 537, 556, 549 );
		}
		else if ( s->Is( Style::M_HOVER ) ) {
			s->SetTexture( Style::A_TEXTURE, "interface.pcx", 542, 537, 548, 549 );
		}
		else {
			s->SetTexture( Style::A_TEXTURE, "interface.pcx", 534, 537, 540, 549 );
		}
	}
	);

	AddStyle(
		"PopupDropdownChoices", SH()
	{

		s->Set( Style::A_ITEM_MARGIN, 0 );
		s->Set( Style::A_ITEM_HEIGHT, 19 );
		s->Set( Style::A_BORDER_SIZE, 0 );

		s->SetFont( Style::A_FONT, "arialnb.ttf", 17 );
		s->Set( Style::A_TEXT_ALIGN, UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );

		if ( s->Is( Style::M_HOVER ) ) {
			s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 53, 61, 115 ) );
			s->SetTexture( Style::A_TEXTURE_BACK, "interface.pcx", 68, 169, 134, 188 );
		}
		else {
			s->SetColor( Style::A_TEXT_COLOR, Color::FromRGB( 176, 212, 235 ) );
			s->SetTexture( Style::A_TEXTURE_BACK, "interface.pcx", 68, 127, 134, 146 );
		}
	}
	);

}

}
}
