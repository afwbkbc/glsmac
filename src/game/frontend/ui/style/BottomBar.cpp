#include "BottomBar.h"

#include "ui/theme/Style.h"

namespace game {
namespace frontend {
namespace ui {
namespace style {

void BottomBar::AddStyles() {

	SetPrefix( "BB" );

	// frame

	AddStyle(
		"", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_BOTTOM );
			s->Set( ::ui::A_HEIGHT, 256 );
			s->Set( ::ui::A_Z_INDEX, 0.8f );
		}
	);

	AddStyle(
		"Frame", SH() {

		}
	);

	AddStyle(
		"FrameLeft", { "Frame" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_BOTTOM | ::ui::ALIGN_LEFT );
			s->Set( ::ui::A_WIDTH, 356 );
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE2_A, 0, 0, 356, 256 );
		}
	);

	AddStyle(
		"FrameMiddle", { "Frame" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_BOTTOM );
			s->Set( ::ui::A_LEFT, 356 );
			s->Set( ::ui::A_RIGHT, 520 );
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE2_A, 357, 0, ( 1024 - 521 ), 256 );
		}
	);

	AddStyle(
		"FrameRight", { "Frame" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_BOTTOM | ::ui::ALIGN_RIGHT );
			s->Set( ::ui::A_WIDTH, 520 );
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE2_A, ( 1024 - 520 ), 0, 1023, 256 );
		}
	);

	AddStyle(
		"FrameBackground", SH() {
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_INTERFACE, 86, 665, 109, 688 );
		}
	);

	AddStyle(
		"SectionOuter", SH() {
			s->Set( ::ui::A_BORDER_SIZE, 3 );
			s->SetTexture( ::ui::A_TEXTURE_LEFT, resource::PCX_INTERFACE, 40, 275, 42, 298 );
			s->SetTexture( ::ui::A_TEXTURE_LEFTTOP, resource::PCX_INTERFACE, 40, 268, 42, 270 );
			s->SetTexture( ::ui::A_TEXTURE_TOP, resource::PCX_INTERFACE, 47, 268, 70, 270 );
			s->SetTexture( ::ui::A_TEXTURE_RIGHTTOP, resource::PCX_INTERFACE, 75, 268, 77, 270 );
			s->SetTexture( ::ui::A_TEXTURE_RIGHT, resource::PCX_INTERFACE, 75, 275, 77, 298 );
			s->SetTexture( ::ui::A_TEXTURE_RIGHTBOTTOM, resource::PCX_INTERFACE, 75, 303, 77, 305 );
			s->SetTexture( ::ui::A_TEXTURE_BOTTOM, resource::PCX_INTERFACE, 47, 303, 70, 305 );
			s->SetTexture( ::ui::A_TEXTURE_LEFTBOTTOM, resource::PCX_INTERFACE, 40, 303, 42, 305 );
			s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_INTERFACE, 86, 665, 109, 688 );
		}
	);

	AddStyle(
		"SectionInner", SH() {
			s->Set( ::ui::A_BORDER_SIZE, 3 );
			s->SetTexture( ::ui::A_TEXTURE_LEFT, resource::PCX_INTERFACE, 1, 275, 3, 298 );
			s->SetTexture( ::ui::A_TEXTURE_LEFTTOP, resource::PCX_INTERFACE, 1, 268, 3, 270 );
			s->SetTexture( ::ui::A_TEXTURE_TOP, resource::PCX_INTERFACE, 8, 268, 31, 270 );
			s->SetTexture( ::ui::A_TEXTURE_RIGHTTOP, resource::PCX_INTERFACE, 36, 268, 38, 270 );
			s->SetTexture( ::ui::A_TEXTURE_RIGHT, resource::PCX_INTERFACE, 36, 275, 38, 298 );
			s->SetTexture( ::ui::A_TEXTURE_RIGHTBOTTOM, resource::PCX_INTERFACE, 36, 303, 38, 305 );
			s->SetTexture( ::ui::A_TEXTURE_BOTTOM, resource::PCX_INTERFACE, 8, 303, 31, 305 );
			s->SetTexture( ::ui::A_TEXTURE_LEFTBOTTOM, resource::PCX_INTERFACE, 1, 303, 3, 305 );
			s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_INTERFACE, 86, 548, 109, 571 );
		}
	);

	// buttons

	AddStyle(
		"Button", SH() {
			s->Set( ::ui::A_WIDTH, 108 );
			s->Set( ::ui::A_HEIGHT, 16 );

			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALNB, 16 );

			if ( s->Is( ::ui::M_ACTIVE ) || s->Is( ::ui::M_SELECTED ) ) {
				s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_CONSOLE_X2_A, 1, 247, 106, 260 );
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 0, 0, 0 ) );
			}
			else if ( s->Is( ::ui::M_HOVER ) ) {
				s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_CONSOLE_X2_A, 1, 232, 106, 245 );
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 0, 0, 0 ) );
			}
			else {
				s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_CONSOLE_X2_A, 1, 217, 106, 230 );
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 228, 104, 24 ) );
			}

		}
	);

	AddStyle(
		"ButtonMenu", { "Button" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT );
			s->Set( ::ui::A_LEFT, 10 );
			s->Set( ::ui::A_TOP, 19 );
			s->SetSound( ::ui::A_BUTTON_CLICK_SOUND, resource::WAV_AMENU2 );
			s->Set( ::ui::A_SOUND_VOLUME, 0.4 );
		}
	);

	AddStyle(
		"ButtonCommlink", { "Button" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_RIGHT );
			s->Set( ::ui::A_RIGHT, 10 );
			s->Set( ::ui::A_TOP, 21 );
			s->SetSound( ::ui::A_BUTTON_CLICK_SOUND, resource::WAV_MMENU );
			s->Set( ::ui::A_SOUND_VOLUME, 0.2 );
		}
	);

	// centered block (used to center narrow bottom bar contents on high resolutions
	AddStyle(
		"CenteredBlock", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_CENTER );
			s->Set( ::ui::A_WIDTH, 460 );
		}
	);

	// unit preview

	AddStyle(
		"ObjectPreview", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT );
			s->Set( ::ui::A_TOP, 58 );
			s->Set( ::ui::A_BOTTOM, 6 );
			s->Set( ::ui::A_LEFT, 5 );
			s->Set( ::ui::A_WIDTH, 133 );
		}
	);

	AddStyle(
		"ObjectPreviewObject", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_HCENTER );
			s->Set( ::ui::A_TOP, 12 );
			s->Set( ::ui::A_WIDTH, 100 ); // 80
			s->Set( ::ui::A_HEIGHT, 75 ); // 60 // 48
		}
	);
	AddStyle(
		"ObjectPreviewBadge", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT );
			s->Set( ::ui::A_LEFT, 6 );
			s->Set( ::ui::A_TOP, 6 );
			s->Set( ::ui::A_WIDTH, 23 );
			s->Set( ::ui::A_HEIGHT, 30 );
		}
	);
	AddStyle(
		"ObjectPreviewHealthbar", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT );
			s->Set( ::ui::A_LEFT, 12 );
			s->Set( ::ui::A_TOP, 12 );
			s->Set( ::ui::A_WIDTH, 3 );
			s->Set( ::ui::A_HEIGHT, 22 );
		}
	);
	AddStyle(
		"ObjectPreviewLabel", SH() {
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALN, 14 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 116, 156, 56 ) );
			s->Set( ::ui::A_HEIGHT, 17 );
			s->Set( ::ui::A_WIDTH, 131 );
		}
	);
	AddStyle(
		"ObjectPreviewLabelHeader", SH() {
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALNB, 16 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 116, 156, 56 ) );
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_HCENTER );
			s->Set( ::ui::A_HEIGHT, 17 );
		}
	);
	AddStyle(
		"ObjectPreviewLabelLeft", { "ObjectPreviewLabel" }, SH() {
			s->Set( ::ui::A_LEFT, 6 );
			s->Set( ::ui::A_TEXT_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT );
		}
	);
	AddStyle(
		"ObjectPreviewLabelCenter", { "ObjectPreviewLabel" }, SH() {
			s->Set( ::ui::A_TEXT_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_HCENTER );
		}
	);

	// tile preview

	AddStyle(
		"TilePreview", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT );
			s->Set( ::ui::A_TOP, 58 );
			s->Set( ::ui::A_BOTTOM, 6 );
			s->Set( ::ui::A_LEFT, 136 );
			s->Set( ::ui::A_WIDTH, 108 );
		}
	);

	AddStyle(
		"TilePreviewImage", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_HCENTER );
			s->Set( ::ui::A_TOP, 2 );
			s->Set( ::ui::A_WIDTH, 84 );
			s->Set( ::ui::A_HEIGHT, 52 );
		}
	);

	AddStyle(
		"TilePreviewSprite", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_HCENTER );
			s->Set( ::ui::A_TOP, 2 );
			s->Set( ::ui::A_WIDTH, 100 );
			s->Set( ::ui::A_HEIGHT, 68 );
		}
	);

	AddStyle(
		"TilePreviewText", SH() {
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALN, 14 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 116, 156, 56 ) );
			s->Set( ::ui::A_HEIGHT, 15 );
		}
	);

	AddStyle(
		"TilePreviewTextLine", { "TilePreviewText" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT );
			s->Set( ::ui::A_LEFT, 3 );
		}
	);

	AddStyle(
		"TilePreviewTextFooter", { "TilePreviewText" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_BOTTOM | ::ui::ALIGN_RIGHT );
			s->Set( ::ui::A_RIGHT, 2 );
			s->Set( ::ui::A_BOTTOM, 6 );
		}
	);

	// stuff at center

	AddStyle(
		"MiddleArea", SH() {
			s->Set( ::ui::A_LEFT, 262 );
			s->Set( ::ui::A_TOP, 67 );
			s->Set( ::ui::A_RIGHT, 262 );
			s->Set( ::ui::A_BOTTOM, 85 );
		}
	);

	AddStyle(
		"MiddleAreaButton", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_RIGHT );
			s->Set( ::ui::A_RIGHT, 3 );
			s->Set( ::ui::A_WIDTH, 19 );
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALNB, 16 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 0, 0, 0 ) );
			if ( s->Is( ::ui::M_HOVER ) || s->Is( ::ui::M_SELECTED ) ) {
				s->SetTexture( ::ui::A_TEXTURE_LEFT, resource::PCX_INTERFACE, 40, 587, 40, 610 );
				s->SetTexture( ::ui::A_TEXTURE_TOP, resource::PCX_INTERFACE, 47, 580, 70, 580 );
				s->SetTexture( ::ui::A_TEXTURE_RIGHT, resource::PCX_INTERFACE, 77, 587, 77, 610 );
				s->SetTexture( ::ui::A_TEXTURE_BOTTOM, resource::PCX_INTERFACE, 47, 617, 70, 617 );
			}
			else {
				s->SetTexture( ::ui::A_TEXTURE_LEFT, resource::PCX_INTERFACE, 8, 587, 8, 610 );
				s->SetTexture( ::ui::A_TEXTURE_TOP, resource::PCX_INTERFACE, 8, 588, 31, 588 );
				s->SetTexture( ::ui::A_TEXTURE_RIGHT, resource::PCX_INTERFACE, 31, 587, 31, 610 );
				s->SetTexture( ::ui::A_TEXTURE_BOTTOM, resource::PCX_INTERFACE, 8, 610, 31, 610 );
			}
			if ( s->Is( ::ui::M_ACTIVE ) || s->Is( ::ui::M_SELECTED ) ) {
				s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_INTERFACE, 86, 587, 109, 610 );
			}
			else {
				s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_INTERFACE, 47, 587, 70, 610 );
			}
		}
	);

	AddStyle(
		"MiddleAreaPage", SH() {
			s->Set( ::ui::A_LEFT, 2 );
			s->Set( ::ui::A_TOP, 2 );
			s->Set( ::ui::A_RIGHT, 26 );
			s->Set( ::ui::A_BOTTOM, 2 );
		}
	);

	// messages page

	AddStyle(
		"MiddleAreaMessages", { "MiddleAreaPage" }, SH() {

		}
	);

	AddStyle(
		"MiddleAreaMessagesList", { "SectionInner" }, SH() {

			s->Set( ::ui::A_LEFT, 0 );
			s->Set( ::ui::A_TOP, 0 );
			s->Set( ::ui::A_RIGHT, 0 );
			s->Set( ::ui::A_BOTTOM, 0 );

			// items
			s->Set( ::ui::A_ITEM_HEIGHT, 17 ); // TODO: auto-size?
			s->Set( ::ui::A_ITEM_MARGIN, 0 ); // TODO: fix alignment if non-zero
			s->Set( ::ui::A_TEXT_ALIGN, ::ui::ALIGN_LEFT );
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALN, 16 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 60, 148, 124 ) );
		}
	);

	// toolbar page

	AddStyle(
		"MiddleAreaToolbar", { "MiddleAreaPage" }, SH() {

		}
	);

	const auto f_toolbar_button_texture = []( ::ui::theme::Style* s, const size_t tx, const size_t ty, const size_t xsize, const size_t ysize ) -> void {
		if ( s->Is( ::ui::M_ACTIVE ) || s->Is( ::ui::M_SELECTED ) ) {
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_INTERFACE, tx, ty + ( ysize + 1 ) * 2, tx + xsize - 1, ty + ( ysize - 1 ) + ( ysize + 1 ) * 2 );
		}
		else if ( s->Is( ::ui::M_HOVER ) ) {
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_INTERFACE, tx, ty + ( ysize + 1 ), tx + xsize - 1, ty + ( ysize - 1 ) + ( ysize + 1 ) );
		}
		else {
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_INTERFACE, tx, ty, tx + xsize - 1, ty + ( ysize - 1 ) );
		}
	};

	const auto f_toolbar_tool_button_texture = [ f_toolbar_button_texture ]( ::ui::theme::Style* s, const size_t tx, const size_t ty ) -> void {
		f_toolbar_button_texture( s, tx, ty, 43, 18 );
	};
	AddStyle(
		"MiddleAreaToolbarButton Tool", SH() {
			s->Set( ::ui::A_WIDTH, 43 );
			s->Set( ::ui::A_HEIGHT, 18 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Elevations", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 350, 245 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Moisture", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 702, 302 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Forest", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 438, 302 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Sensor Array", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 658, 302 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Monolith", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 526, 359 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Rockiness", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 394, 245 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Farm", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 614, 245 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Road", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 482, 302 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Fungus", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 350, 359 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Jungle", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 570, 359 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Rivers", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 438, 245 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Soil Enricher", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 658, 245 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Mag Tube", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 526, 302 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Condenser", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 394, 359 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Dunes", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 614, 359 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Resources", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 482, 245 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Mine", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 350, 302 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Bunker", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 570, 302 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Echelon Mirror", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 438, 359 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Uranium", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 658, 359 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Unity Pods", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 526, 245 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Solar Collector", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 394, 302 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Airbase", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 614, 302 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Thermal Borehole", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 482, 359 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Tool Geothermal", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
			f_toolbar_tool_button_texture( s, 350, 416 );
		}
	);

	AddStyle(
		"MiddleAreaToolbarButton Brush", SH() {
			s->Set( ::ui::A_WIDTH, 24 );
			s->Set( ::ui::A_HEIGHT, 24 );
		}
	);
	const auto f_toolbar_brush_button_texture = [ f_toolbar_button_texture ]( ::ui::theme::Style* s, const size_t tx, const size_t ty ) -> void {
		f_toolbar_button_texture( s, tx, ty, 24, 24 );
	};
	AddStyle(
		"MiddleAreaToolbarButton Brush Dot", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
			f_toolbar_brush_button_texture( s, 465, 438 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Brush Cross", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
			f_toolbar_brush_button_texture( s, 490, 438 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Brush Square 3x3", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
			f_toolbar_brush_button_texture( s, 515, 438 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Brush Square 5x5", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
			f_toolbar_brush_button_texture( s, 540, 438 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Brush Square 7x7", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
			f_toolbar_brush_button_texture( s, 565, 438 );
		}
	);
	AddStyle(
		"MiddleAreaToolbarButton Brush Square 9x9", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
			f_toolbar_brush_button_texture( s, 590, 438 );
		}
	);

	AddStyle(
		"MiddleAreaToolbarToolInfo", SH() {
			s->Set( ::ui::A_LEFT, 277 );
			s->Set( ::ui::A_WIDTH, 185 );
		}
	);

	AddStyle(
		"MiddleAreaToolbarInfoLabel", SH() {
			s->Set( ::ui::A_TEXT_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT );
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALN, 14 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 184, 192, 96 ) );
		}
	);

	// info panels page

	AddStyle(
		"MiddleAreaInfoPanels", { "MiddleAreaPage" }, SH() {

		}
	);

	AddStyle(
		"MiddleAreaInfoPanel", SH() {

		}
	);

	AddStyle(
		"MiddleAreaInfoPanelLabel", SH() {
			s->Set( ::ui::A_TEXT_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT );
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALN, 14 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 184, 192, 96 ) );
		}
	);

	// units list at bottom

	AddStyle(
		"ObjectsList", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_BOTTOM );
			s->Set( ::ui::A_BOTTOM, 6 );
			s->Set( ::ui::A_LEFT, 242 );
			s->Set( ::ui::A_RIGHT, 257 );
			s->Set( ::ui::A_HEIGHT, 60 );
		}
	);

	AddStyle(
		"ObjectsListItem", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_LEFT );
			s->Set( ::ui::A_TOP, 0 );
			s->Set( ::ui::A_WIDTH, 68 );
			s->Set( ::ui::A_HEIGHT, 46 );
		}
	);

	AddStyle(
		"ObjectsListSelection", SH() {
			s->Set( ::ui::A_LEFT, -6 );
			s->Set( ::ui::A_TOP, -6 );
			s->Set( ::ui::A_BOTTOM, -3 );
			s->Set( ::ui::A_RIGHT, -6 );
			s->Set( ::ui::A_BORDER_SIZE, 1 );
			types::Color c;
			if ( s->Is( ::ui::M_ACTIVE ) ) {
				c = types::Color::FromRGB( 56, 160, 198 );
			}
			else if ( s->Is( ::ui::M_HOVER ) ) {
				c = types::Color::FromRGB( 28, 80, 99 );
			}
			else {
				c = types::Color::FromRGBA( 0, 0, 0, 0 );
			}
			s->SetColorTexture( ::ui::A_TEXTURE_LEFT, c );
			s->SetColorTexture( ::ui::A_TEXTURE_TOP, c );
			s->SetColorTexture( ::ui::A_TEXTURE_RIGHT, c );
			s->SetColorTexture( ::ui::A_TEXTURE_BOTTOM, c );
		}
	);

	AddStyle(
		"ObjectsListPreviewUnit", SH() {
			s->Set( ::ui::A_LEFT, 6 );
			s->Set( ::ui::A_TOP, -6 );
			s->Set( ::ui::A_WIDTH, 60 );
			s->Set( ::ui::A_HEIGHT, 36 );
		}
	);
	AddStyle(
		"ObjectsListPreviewUnitBadge", SH() {
			s->Set( ::ui::A_LEFT, 0 );
			s->Set( ::ui::A_TOP, -3 );
			s->Set( ::ui::A_WIDTH, 20 );
			s->Set( ::ui::A_HEIGHT, 26 );
		}
	);
	AddStyle(
		"ObjectsListPreviewUnitHealthbar", SH() {
			s->Set( ::ui::A_LEFT, 5 );
			s->Set( ::ui::A_TOP, 2 );
			s->Set( ::ui::A_WIDTH, 3 );
			s->Set( ::ui::A_HEIGHT, 20 );
		}
	);
	AddStyle(
		"ObjectsListPreviewBase", SH() {
			s->Set( ::ui::A_LEFT, 2 );
			s->Set( ::ui::A_TOP, -6 );
			s->Set( ::ui::A_WIDTH, 60 );
			s->Set( ::ui::A_HEIGHT, 40 );
		}
	);
	AddStyle(
		"ObjectsListPreviewLabel", SH() {
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALNB, 12 );
			s->Set( ::ui::A_TEXT_ALIGN, ::ui::ALIGN_BOTTOM | ::ui::ALIGN_HCENTER );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 235, 235, 235 ) );
		}
	);

	// minimap

	AddStyle(
		"MiniMap", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_RIGHT );
			s->Set( ::ui::A_TOP, 58 );
			s->Set( ::ui::A_BOTTOM, 6 );
			s->Set( ::ui::A_RIGHT, 6 );
			s->Set( ::ui::A_WIDTH, 238 );
		}
	);

	AddStyle(
		"TurnCompleteButton", SH() {

			const auto f_buttonstyle = [ &s ]( const size_t ox, const size_t oy ) -> void {
				s->SetTexture( ::ui::A_TEXTURE_LEFT, resource::PCX_CONSOLE_X2_A, 0 + ox, 6 + oy, 5 + ox, 18 + oy );
				s->SetTexture( ::ui::A_TEXTURE_LEFTTOP, resource::PCX_CONSOLE_X2_A, 0 + ox, 0 + oy, 5 + ox, 5 + oy );
				s->SetTexture( ::ui::A_TEXTURE_TOP, resource::PCX_CONSOLE_X2_A, 6 + ox, 0 + oy, 181 + ox, 5 + oy );
				s->SetTexture( ::ui::A_TEXTURE_RIGHTTOP, resource::PCX_CONSOLE_X2_A, 182 + ox, 0 + oy, 187 + ox, 5 + oy );
				s->SetTexture( ::ui::A_TEXTURE_RIGHT, resource::PCX_CONSOLE_X2_A, 182 + ox, 6 + oy, 187 + ox, 18 + oy );
				s->SetTexture( ::ui::A_TEXTURE_RIGHTBOTTOM, resource::PCX_CONSOLE_X2_A, 182 + ox, 19 + oy, 187 + ox, 24 + oy );
				s->SetTexture( ::ui::A_TEXTURE_BOTTOM, resource::PCX_CONSOLE_X2_A, 6 + ox, 19 + oy, 181 + ox, 24 + oy );
				s->SetTexture( ::ui::A_TEXTURE_LEFTBOTTOM, resource::PCX_CONSOLE_X2_A, 0 + ox, 19 + oy, 5 + ox, 24 + oy );
				s->SetTexture( ::ui::A_TEXTURE_BACK, resource::PCX_CONSOLE_X2_A, 6 + ox, 6 + oy, 182 + ox, 19 + oy );
			};

			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_HCENTER );
			s->Set( ::ui::A_TOP, 1 );
			s->Set( ::ui::A_LEFT, 1 );
			s->Set( ::ui::A_RIGHT, 1 );
			s->Set( ::ui::A_HEIGHT, 25 );

			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALNB, 16 );

			s->Set( ::ui::A_BORDER_SIZE, 6 );

			if ( s->Is( ::ui::M_ACTIVE ) ) {
				f_buttonstyle( 193, 55 );
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 0, 0, 0 ) );
			}
			else if ( s->Is( ::ui::M_HOVER ) ) {
				f_buttonstyle( 2, 55 );
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 228, 104, 24 ) );
			}
			else {
				f_buttonstyle( 2, 1 );
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 228, 104, 24 ) );
			}

			s->SetSound( ::ui::A_BUTTON_CLICK_SOUND, resource::WAV_OK );
			s->Set( ::ui::A_SOUND_VOLUME, 0.5f );
		}
	);
	AddStyle(
		"TurnCompleteSound", SH() {
			s->SetSound( ::ui::A_SOUND, resource::WAV_TURN_COMPLETE );
			s->Set( ::ui::A_SOUND_AUTOSTOP );
			s->Set( ::ui::A_SOUND_VOLUME, 0.5f );
		}
	);

	AddStyle(
		"MinimapImage", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_HCENTER );
			s->Set( ::ui::A_TOP, 29 );
			s->Set( ::ui::A_WIDTH, 224 );
			s->Set( ::ui::A_HEIGHT, 112 );
		}
	);

	AddStyle(
		"MinimapBottomBar", { "SectionInner" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_BOTTOM | ::ui::ALIGN_LEFT );
			s->Set( ::ui::A_BOTTOM, -2 );
			s->Set( ::ui::A_LEFT, -2 );
			s->Set( ::ui::A_WIDTH, 122 );
			s->Set( ::ui::A_HEIGHT, 41 );
		}
	);

	AddStyle(
		"MinimapBottomBarLabel", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_LEFT );
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALNB, 15 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 60, 148, 124 ) );
			s->Set( ::ui::A_LEFT, 5 );
			s->Set( ::ui::A_HEIGHT, 15 );
		}
	);

	// side menus
	AddStyle(
		"Menu", SH() {
			s->Set( ::ui::A_WIDTH, 136 );
		}
	);

	AddStyle(
		"MenuBackground", SH() {
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_INTERFACE, 439, 64, 458, 83 );
		}
	);

	AddStyle(
		"MenuFrame", SH() {
			s->Set( ::ui::A_Z_INDEX, 0.3f ); // TODO: why doesn't it work correctly without?
		}
	);

	AddStyle(
		"LeftMenuTopFrame", { "MenuFrame" }, SH() {
			s->Set( ::ui::A_HEIGHT, 5 );
			s->Set( ::ui::A_WIDTH, 139 );
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP );
			s->Set( ::ui::A_TOP, -4 );
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE2_A, 0, 0, 138, 4 );
		}
	);

	AddStyle(
		"RightMenuTopFrame", { "MenuFrame" }, SH() {
			s->Set( ::ui::A_HEIGHT, 6 );
			s->Set( ::ui::A_WIDTH, 139 );
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_TOP | ::ui::ALIGN_RIGHT );
			s->Set( ::ui::A_TOP, -5 );
			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE2_A, 885, 1, 1023, 6 );
		}
	);

	AddStyle(
		"MenuBottomFrame", { "MenuFrame" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_BOTTOM );
			s->Set( ::ui::A_BOTTOM, -4 );
		}
	);

	AddStyle(
		"MenuButton", SH() {
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALNB, 17 );
			s->Set( ::ui::A_TEXT_LEFT, 4 );
			if ( s->Is( ::ui::M_ACTIVE ) || s->Is( ::ui::M_SELECTED ) ) {
				s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE_X2_A, 1, 249, 106, 258 );
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 0, 0, 0 ) );
			}
			else if ( s->Is( ::ui::M_HOVER ) ) {
//			s->SetTexture( ::ui::A_TEXTURE, resource::PCX_INTERFACE, 68, 65, 134, 82 );
				s->SetTexture( ::ui::A_TEXTURE, resource::PCX_CONSOLE_X2_A, 1, 219, 106, 228 );
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 228, 104, 24 ) );
			}
			else {
				s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 176, 212, 235 ) );
			}
		}
	);

	AddStyle(
		"LeftMenuButton", { "MenuButton" }, SH() {
			s->Set( ::ui::A_TEXT_ALIGN, ::ui::ALIGN_LEFT | ::ui::ALIGN_VCENTER );
		}
	);

	AddStyle(
		"RightMenuButton", { "MenuButton" }, SH() {
			s->Set( ::ui::A_TEXT_ALIGN, ::ui::ALIGN_RIGHT | ::ui::ALIGN_VCENTER );
		}
	);

	AddStyle(
		"LeftMenu", { "Menu" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_BOTTOM | ::ui::ALIGN_LEFT );
		}
	);

	AddStyle(
		"RightMenu", { "Menu" }, SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_BOTTOM | ::ui::ALIGN_RIGHT );
		}
	);

	AddStyle(
		"MessageLabel", SH() {
			s->Set( ::ui::A_ALIGN, ::ui::ALIGN_LEFT | ::ui::ALIGN_TOP );
			s->Set( ::ui::A_LEFT, 228 );
			s->Set( ::ui::A_TOP, 10 );
			s->Set( ::ui::A_HEIGHT, 20 );
			s->SetFont( ::ui::A_FONT, resource::TTF_ARIALNB, 18 );
			s->SetColor( ::ui::A_TEXT_COLOR, types::Color::FromRGB( 232, 212, 140 ) );
		}
	);
}

}
}
}
}
