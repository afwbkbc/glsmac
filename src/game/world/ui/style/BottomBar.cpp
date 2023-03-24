#include "BottomBar.h"

namespace game {
namespace world {
namespace ui {
namespace style {

void BottomBar::AddStyles() {
	
	SetPrefix( "MapBottomBar" );
	
	// frame
	
	AddStyle( "", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM );
		s->Set( ::Style::A_HEIGHT, 256 );
	});
	
	AddStyle( "Frame", SH() {
		
	});
	
	AddStyle( "FrameLeft", { "Frame" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_WIDTH, 356 );
		s->SetTextureTC( ::Style::A_TEXTURE, "console2_A.pcx", 0, 0, 356, 256, Color::RGB( 100, 16, 156 ) );
	});
	
	AddStyle( "FrameMiddle", { "Frame" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM );
		s->Set( ::Style::A_LEFT, 356 );
		s->Set( ::Style::A_RIGHT, 520 );
		s->SetTextureTC( ::Style::A_TEXTURE, "console2_A.pcx", 357, 0, ( 1024 - 521 ), 256, Color::RGB( 100, 16, 156 ) );
	});
	
	AddStyle( "FrameRight", { "Frame" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_WIDTH, 520 );
		s->SetTextureTC( ::Style::A_TEXTURE, "console2_A.pcx", ( 1024 - 520 ), 0, 1023, 256, Color::RGB( 100, 16, 156 ) );
	});
	
	AddStyle( "FrameBackground", SH() {
		s->SetTexture( ::Style::A_TEXTURE, "interface.pcx", 86, 665, 109, 688 );
	});
	
	AddStyle( "SectionOuter", SH() {
		s->Set( ::Style::A_BORDER_SIZE, 3 );
		s->SetTexture( ::Style::A_TEXTURE_LEFT, "interface.pcx", 40, 275, 42, 298 );
		s->SetTexture( ::Style::A_TEXTURE_LEFTTOP, "interface.pcx", 40, 268, 42, 270 );
		s->SetTexture( ::Style::A_TEXTURE_TOP, "interface.pcx", 47, 268, 70, 270 );
		s->SetTexture( ::Style::A_TEXTURE_RIGHTTOP, "interface.pcx", 75, 268, 77, 270 );
		s->SetTexture( ::Style::A_TEXTURE_RIGHT, "interface.pcx", 75, 275, 77, 298 );
		s->SetTexture( ::Style::A_TEXTURE_RIGHTBOTTOM, "interface.pcx", 75, 303, 77, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BOTTOM, "interface.pcx", 47, 303, 70, 305 );
		s->SetTexture( ::Style::A_TEXTURE_LEFTBOTTOM, "interface.pcx", 40, 303, 42, 305 );
	});
	
	AddStyle( "SectionInner", SH() {
		s->Set( ::Style::A_BORDER_SIZE, 3 );
		s->SetTexture( ::Style::A_TEXTURE_LEFT, "interface.pcx", 1, 275, 3, 298 );
		s->SetTexture( ::Style::A_TEXTURE_LEFTTOP, "interface.pcx", 1, 268, 3, 270 );
		s->SetTexture( ::Style::A_TEXTURE_TOP, "interface.pcx", 8, 268, 31, 270 );
		s->SetTexture( ::Style::A_TEXTURE_RIGHTTOP, "interface.pcx", 36, 268, 38, 270 );
		s->SetTexture( ::Style::A_TEXTURE_RIGHT, "interface.pcx", 36, 275, 38, 298 );
		s->SetTexture( ::Style::A_TEXTURE_RIGHTBOTTOM, "interface.pcx", 36, 303, 38, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BOTTOM, "interface.pcx", 8, 303, 31, 305 );
		s->SetTexture( ::Style::A_TEXTURE_LEFTBOTTOM, "interface.pcx", 1, 303, 3, 305 );
		s->SetTexture( ::Style::A_TEXTURE_BACK, "interface.pcx", 86, 548, 109, 571 );
	});
	
	// buttons
	
	AddStyle( "Button", SH() {
		s->Set( ::Style::A_WIDTH, 108 );
		s->Set( ::Style::A_HEIGHT, 16 );
		
		s->SetFont( ::Style::A_FONT, "arialnb.ttf", 16 );
		
		if ( s->Is( ::Style::M_ACTIVE ) || s->Is( ::Style::M_SELECTED ) ) {
			s->SetTexture( ::Style::A_TEXTURE_BACK, "console_x2_a.pcx", 1, 247, 106, 260 );
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 0, 0, 0 ) );
		}
		else if ( s->Is( ::Style::M_HOVER ) ) {
			s->SetTexture( ::Style::A_TEXTURE_BACK, "console_x2_a.pcx", 1, 232, 106, 245 );
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 0, 0, 0 ) );
		}
		else {
			s->SetTexture( ::Style::A_TEXTURE_BACK, "console_x2_a.pcx", 1, 217, 106, 230 );
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 228, 104, 24 ) );
		}

	});
	
	AddStyle( "ButtonMenu", { "Button" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_LEFT, 10 );
		s->Set( ::Style::A_TOP, 19 );
		s->SetSound( ::Style::A_BUTTON_CLICK_SOUND, "amenu2.wav" );
		s->Set( ::Style::A_SOUND_VOLUME, 0.4 );
	});
	
	AddStyle( "ButtonCommlink", { "Button" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_RIGHT, 10 );
		s->Set( ::Style::A_TOP, 21 );
		s->SetSound( ::Style::A_BUTTON_CLICK_SOUND, "mmenu.wav" );
		s->Set( ::Style::A_SOUND_VOLUME, 0.2 );
	});
	
	// centered block (used to center narrow bottom bar contents on high resolutions
	AddStyle( "CenteredBlock", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_CENTER );
		s->Set( ::Style::A_WIDTH, 460 );
	});
	
	// unit preview
	
	AddStyle( "UnitPreview", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_TOP, 58 );
		s->Set( ::Style::A_BOTTOM, 6 );
		s->Set( ::Style::A_LEFT, 5 );
		s->Set( ::Style::A_WIDTH, 133 );
	});
	
	// tile preview
	
	AddStyle( "TilePreview", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_TOP, 58 );
		s->Set( ::Style::A_BOTTOM, 6 );
		s->Set( ::Style::A_LEFT, 136 );
		s->Set( ::Style::A_WIDTH, 108 );
	});

	AddStyle( "TilePreviewImage", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_HCENTER );
		s->Set( ::Style::A_TOP, 2 );
		s->Set( ::Style::A_WIDTH, 84 );
		s->Set( ::Style::A_HEIGHT, 52 );
	});
	
	AddStyle( "TilePreviewSprite", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_HCENTER );
		s->Set( ::Style::A_TOP, 2 );
		s->Set( ::Style::A_WIDTH, 100 );
		s->Set( ::Style::A_HEIGHT, 68 );
	});
	
	AddStyle( "TilePreviewText", SH() {
		s->SetFont( ::Style::A_FONT, "arialn.ttf", 14 );
		s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 116, 156, 56 ) );
		s->Set( ::Style::A_HEIGHT, 15 );
	});
	
	AddStyle( "TilePreviewTextLine", { "TilePreviewText" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_LEFT, 3 );
	});
	
	AddStyle( "TilePreviewTextFooter", { "TilePreviewText" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_RIGHT, 2 );
		s->Set( ::Style::A_BOTTOM, 6 );
	});

	// stuff at center
	
	AddStyle( "MiddleArea", SH() {
		s->Set( ::Style::A_LEFT, 262 );
		s->Set( ::Style::A_TOP, 67 );
		s->Set( ::Style::A_RIGHT, 262 );
		s->Set( ::Style::A_BOTTOM, 85 );
	});
	
	AddStyle( "MiddleAreaButton", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_RIGHT, 3 );
		s->Set( ::Style::A_WIDTH, 19 );
		s->SetFont( ::Style::A_FONT, "arialnb.ttf", 16 );
		s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 0, 0, 0 ) );
		if ( s->Is( ::Style::M_HOVER ) || s->Is( ::Style::M_SELECTED ) ) {
			s->SetTexture( ::Style::A_TEXTURE_LEFT, "interface.pcx", 40, 587, 40, 610 );
			s->SetTexture( ::Style::A_TEXTURE_TOP, "interface.pcx", 47, 580, 70, 580 );
			s->SetTexture( ::Style::A_TEXTURE_RIGHT, "interface.pcx", 77, 587, 77, 610 );
			s->SetTexture( ::Style::A_TEXTURE_BOTTOM, "interface.pcx", 47, 617, 70, 617 );
		}
		else {
			s->SetTexture( ::Style::A_TEXTURE_LEFT, "interface.pcx", 8, 587, 8, 610 );
			s->SetTexture( ::Style::A_TEXTURE_TOP, "interface.pcx", 8, 588, 31, 588 );
			s->SetTexture( ::Style::A_TEXTURE_RIGHT, "interface.pcx", 31, 587, 31, 610 );
			s->SetTexture( ::Style::A_TEXTURE_BOTTOM, "interface.pcx", 8, 610, 31, 610 );
		}
		if ( s->Is( ::Style::M_ACTIVE ) || s->Is( ::Style::M_SELECTED ) ) {
			s->SetTexture( ::Style::A_TEXTURE_BACK, "interface.pcx", 86, 587, 109, 610 );
		}
		else {
			s->SetTexture( ::Style::A_TEXTURE_BACK, "interface.pcx", 47, 587, 70, 610 );
		}
	});
	
	AddStyle( "MiddleAreaPage", SH() {
		s->Set( ::Style::A_LEFT, 2 );
		s->Set( ::Style::A_TOP, 2 );
		s->Set( ::Style::A_RIGHT, 26 );
		s->Set( ::Style::A_BOTTOM, 2 );
	});
	
	// messages page
	
	AddStyle( "MiddleAreaMessages", { "MiddleAreaPage" }, SH() {
		
	});
	
	AddStyle( "MiddleAreaMessagesList", { "SectionInner" }, SH() {
		
		s->Set( ::Style::A_LEFT, 0 );
		s->Set( ::Style::A_TOP, 0 );
		s->Set( ::Style::A_RIGHT, 0 );
		s->Set( ::Style::A_BOTTOM, 0 );
		
		// items
		s->Set( Style::A_ITEM_HEIGHT, 17 ); // TODO: auto-size?
		s->Set( Style::A_ITEM_MARGIN, 0 ); // TODO: fix alignment if non-zero
		s->Set( Style::A_TEXT_ALIGN, UIObject::ALIGN_LEFT );
		s->SetFont( ::Style::A_FONT, "arialn.ttf", 16 );
		s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 60, 148, 124 ) );
	});
	
	// toolbar page
	
	AddStyle( "MiddleAreaToolbar", { "MiddleAreaPage" }, SH() {
		
	});
	
	const auto f_toolbar_button_texture = [] ( Style* s, const size_t tx, const size_t ty, const size_t xsize, const size_t ysize ) -> void {
		if ( s->Is( ::Style::M_ACTIVE ) || s->Is( ::Style::M_SELECTED ) ) {
			s->SetTexture( ::Style::A_TEXTURE, "interface.pcx", tx, ty + ( ysize + 1 ) * 2, tx + xsize - 1, ty + ( ysize - 1 ) + ( ysize + 1 ) * 2 );
		}
		else if ( s->Is( ::Style::M_HOVER ) ) {
			s->SetTexture( ::Style::A_TEXTURE, "interface.pcx", tx, ty + ( ysize + 1 ), tx + xsize - 1, ty + ( ysize - 1 ) + ( ysize + 1 ) );
		}
		else {
			s->SetTexture( ::Style::A_TEXTURE, "interface.pcx", tx, ty, tx + xsize - 1, ty + ( ysize - 1 ) );
		}
	};	
	
	const auto f_toolbar_tool_button_texture = [ f_toolbar_button_texture ] ( Style* s, const size_t tx, const size_t ty ) -> void {
		f_toolbar_button_texture( s, tx, ty, 43, 18 );
	};
	AddStyle( "MiddleAreaToolbarButton Tool", SH() {
		s->Set( ::Style::A_WIDTH, 43 );
		s->Set( ::Style::A_HEIGHT, 18 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Elevations", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 350, 245 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Moisture", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 702, 302 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Forest", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 438, 302 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Sensor Array", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 658, 302 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Monolith", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 526, 359 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Rockiness", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 394, 245 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Farm", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 614, 245 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Road", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 482, 302 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Fungus", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 350, 359 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Jungle", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 570, 359 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Rivers", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 438, 245 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Soil Enricher", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 658, 245 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Mag Tube", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 526, 302 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Condenser", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 394, 359 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Dunes", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 614, 359 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Resources", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 482, 245 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Mine", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 350, 302 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Bunker", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 570, 302 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Echelon Mirror", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 438, 359 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Uranium", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 658, 359 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Unity Pods", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 526, 245 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Solar Collector", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 394, 302 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Airbase", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 614, 302 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Thermal Borehole", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 482, 359 );
	});
	AddStyle( "MiddleAreaToolbarButton Tool Geothermal", { "MiddleAreaToolbarButton Tool" }, SH( f_toolbar_tool_button_texture ) {
		f_toolbar_tool_button_texture( s, 350, 416 );
	});
	
	AddStyle( "MiddleAreaToolbarButton Brush", SH() {
		s->Set( ::Style::A_WIDTH, 24 );
		s->Set( ::Style::A_HEIGHT, 24 );
	});
	const auto f_toolbar_brush_button_texture = [ f_toolbar_button_texture ] ( Style* s, const size_t tx, const size_t ty ) -> void {
		f_toolbar_button_texture( s, tx, ty, 24, 24 );
	};
	AddStyle( "MiddleAreaToolbarButton Brush Dot", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
		f_toolbar_brush_button_texture( s, 465, 438 );
	});
	AddStyle( "MiddleAreaToolbarButton Brush Cross", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
		f_toolbar_brush_button_texture( s, 490, 438 );
	});
	AddStyle( "MiddleAreaToolbarButton Brush Square 3x3", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
		f_toolbar_brush_button_texture( s, 515, 438 );
	});
	AddStyle( "MiddleAreaToolbarButton Brush Square 5x5", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
		f_toolbar_brush_button_texture( s, 540, 438 );
	});
	AddStyle( "MiddleAreaToolbarButton Brush Square 7x7", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
		f_toolbar_brush_button_texture( s, 565, 438 );
	});
	AddStyle( "MiddleAreaToolbarButton Brush Square 9x9", { "MiddleAreaToolbarButton Brush" }, SH( f_toolbar_brush_button_texture ) {
		f_toolbar_brush_button_texture( s, 590, 438 );
	});
	
	AddStyle( "MiddleAreaToolbarToolInfo", SH() {
		s->Set( ::Style::A_LEFT, 277 );
		s->Set( ::Style::A_WIDTH, 185 );
	});
	
	AddStyle( "MiddleAreaToolbarInfoLabel", SH() {
		s->Set( ::Style::A_TEXT_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->SetFont( ::Style::A_FONT, "arialn.ttf", 14 );
		s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 184, 192, 96 ) );
	});
	
	// info panels page
	
	AddStyle( "MiddleAreaInfoPanels", { "MiddleAreaPage" }, SH() {
		
	});
	
	AddStyle( "MiddleAreaInfoPanel", SH() {
		
	});
	
	AddStyle( "MiddleAreaInfoPanelLabel", SH() {
		s->Set( ::Style::A_TEXT_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->SetFont( ::Style::A_FONT, "arialn.ttf", 14 );
		s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 184, 192, 96 ) );
	});
	
	// units list at bottom
	
	AddStyle( "UnitsList", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM );
		s->Set( ::Style::A_BOTTOM, 6 );
		s->Set( ::Style::A_LEFT, 242 );
		s->Set( ::Style::A_RIGHT, 257 );
		s->Set( ::Style::A_HEIGHT, 60 );
	});
	
	// minimap
	
	AddStyle( "MiniMap", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_RIGHT );
		s->Set( ::Style::A_TOP, 58 );
		s->Set( ::Style::A_BOTTOM, 6 );
		s->Set( ::Style::A_RIGHT, 6 );
		s->Set( ::Style::A_WIDTH, 238 );
	});

	AddStyle( "MinimapTurnCompleteButton", SH() {
		
		const auto f_buttonstyle = [ &s ] ( const size_t ox, const size_t oy ) -> void {
			const std::string t = "console_x2_a.pcx";
			s->SetTexture( ::Style::A_TEXTURE_LEFT, t, 0 + ox, 6 + oy, 5 + ox, 18 + oy );
			s->SetTexture( ::Style::A_TEXTURE_LEFTTOP, t, 0 + ox, 0 + oy, 5 + ox, 5 + oy );
			s->SetTexture( ::Style::A_TEXTURE_TOP, t, 6 + ox, 0 + oy, 181 + ox, 5 + oy );
			s->SetTexture( ::Style::A_TEXTURE_RIGHTTOP, t, 182 + ox, 0 + oy, 187 + ox, 5 + oy );
			s->SetTexture( ::Style::A_TEXTURE_RIGHT, t, 182 + ox, 6 + oy, 187 + ox, 18 + oy );
			s->SetTexture( ::Style::A_TEXTURE_RIGHTBOTTOM, t, 182 + ox, 19 + oy, 187 + ox, 24 + oy );
			s->SetTexture( ::Style::A_TEXTURE_BOTTOM, t, 6 + ox, 19 + oy, 181 + ox, 24 + oy );
			s->SetTexture( ::Style::A_TEXTURE_LEFTBOTTOM, t, 0 + ox, 19 + oy, 5 + ox, 24 + oy );
			s->SetTexture( ::Style::A_TEXTURE_BACK, t, 6 + ox, 6 + oy, 182 + ox, 19 + oy );
		};
	
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_HCENTER );
		s->Set( ::Style::A_TOP, 1 );
		s->Set( ::Style::A_LEFT, 1 );
		s->Set( ::Style::A_RIGHT, 1 );
		s->Set( ::Style::A_HEIGHT, 25 );
		
		s->SetFont( ::Style::A_FONT, "arialnb.ttf", 16 );
		
		s->Set( ::Style::A_BORDER_SIZE, 6 );
		
		if ( s->Is( ::Style::M_ACTIVE ) ) {
			f_buttonstyle( 193, 55 );
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 0, 0, 0 ) );
		}
		else if ( s->Is( ::Style::M_HOVER ) ) {
			f_buttonstyle( 2, 55 );
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 228, 104, 24 ) );
		}
		else {
			f_buttonstyle( 2, 1 );
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 228, 104, 24 ) );
		}

		s->SetSound( ::Style::A_BUTTON_CLICK_SOUND, "ok.wav" );
		s->Set( ::Style::A_SOUND_VOLUME, 0.5f );
	});
	
	AddStyle( "MinimapImage", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_HCENTER );
		s->Set( ::Style::A_TOP, 29 );
		s->Set( ::Style::A_WIDTH, 224 );
		s->Set( ::Style::A_HEIGHT, 112 );
	});
	
	AddStyle( "MinimapBottomBar", { "SectionInner" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
		s->Set( ::Style::A_BOTTOM, -2 );
		s->Set( ::Style::A_LEFT, -2 );
		s->Set( ::Style::A_WIDTH, 122 );
		s->Set( ::Style::A_HEIGHT, 41 );
	});
	
	AddStyle( "MinimapBottomBarLabel", SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
		s->SetFont( ::Style::A_FONT, "arialnb.ttf", 15 );
		s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 60, 148, 124 ) );
		s->Set( ::Style::A_LEFT, 5 );
		s->Set( ::Style::A_HEIGHT, 15 );
	});
	
	// side menus
	AddStyle( "Menu", SH() {
		s->Set( ::Style::A_WIDTH, 136 );
	});
	
	AddStyle( "MenuBackground", SH() {
		s->SetTexture( ::Style::A_TEXTURE, "interface.pcx", 439, 64, 458, 83 );
	});
	
	AddStyle( "MenuButton", SH() {
		s->SetFont( ::Style::A_FONT, "arialnb.ttf", 17 );
		s->Set( Style::A_TEXT_ALIGN, UIObject::ALIGN_LEFT | UIObject::ALIGN_VCENTER );
		s->Set( Style::A_TEXT_LEFT, 4 );
		if ( s->Is( ::Style::M_ACTIVE ) || s->Is( ::Style::M_SELECTED ) ) {
			s->SetTexture( ::Style::A_TEXTURE, "console_x2_a.pcx", 1, 249, 106, 258 );
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 0, 0, 0 ) );
		}
		else if ( s->Is( ::Style::M_HOVER ) ) {
//			s->SetTexture( ::Style::A_TEXTURE, "interface.pcx", 68, 65, 134, 82 );
			s->SetTexture( ::Style::A_TEXTURE, "console_x2_a.pcx", 1, 219, 106, 228 );
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 228, 104, 24 ) );
		}
		else {
			s->SetColor( ::Style::A_TEXT_COLOR, Color::FromRGB( 176, 212, 235 ) );			
		}
	});
	
	AddStyle( "LeftMenu", { "Menu" }, SH() {
		s->Set( ::Style::A_ALIGN, UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
	});
}

}
}
}
}
