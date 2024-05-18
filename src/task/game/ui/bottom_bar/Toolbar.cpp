#include "Toolbar.h"

#include "task/game/Game.h"
#include "CenteredBlock.h"
#include "ui/object/SimpleButton.h"
#include "BBSection.h"
#include "ui/object/Label.h"

namespace task {
namespace game {
namespace ui {

void Toolbar::Create() {
	MiddleAreaPage::Create();

	SetMargin( -3 );

	NEW( m_centered_block, CenteredBlock );
	AddChild( m_centered_block );

	// TODO: put everything into style?

	// TODO: split to separate panels/sections?

	{ // tool buttons
		const types::Vec2< size_t > offset = {
			0,
			0
		};
		uint8_t bx = 0, by = 0;
		for ( auto t = ::game::map_editor::TT_NONE + 1 ; t < ::game::map_editor::TT_MAX ; t++ ) {
			const auto tool = (::game::map_editor::tool_type_t)t;
			ASSERT( m_tool_names.find( tool ) != m_tool_names.end(), "tool name not found" );
			const std::string& tool_name = m_tool_names.at( tool );
			NEWV( button, ::ui::object::SimpleButton, "BBMiddleAreaToolbarButton Tool " + tool_name );
			button->SetLeft( offset.x + bx * s_tool_button_width );
			button->SetTop( offset.y + by * s_tool_button_height );
			button->On(
				::ui::event::EV_MOUSE_DOWN, EH( this, tool ) {
					SelectTool( tool );
					return true;
				}
			);
			m_centered_block->AddChild( button );
			m_tool_buttons.push_back( button );

			// horizontal, then vertical
			if ( ++bx >= s_tool_buttons_in_row ) {
				bx = 0;
				by++;
			}
		}
	}

	{ // brush buttons
		const types::Vec2< size_t > offset = {
			225,
			10
		};
		uint8_t bx = 0, by = 0;
		for ( auto b = ::game::map_editor::BT_NONE + 1 ; b < ::game::map_editor::BT_MAX ; b++ ) {
			const auto brush = (::game::map_editor::brush_type_t)b;
			ASSERT( m_brush_names.find( brush ) != m_brush_names.end(), "brush name not found" );
			const std::string& brush_name = m_brush_names.at( brush );
			NEWV( button, ::ui::object::SimpleButton, "BBMiddleAreaToolbarButton Brush " + brush_name );
			button->SetLeft( offset.x + bx * s_brush_button_width );
			button->SetTop( offset.y + by * s_brush_button_height );
			button->On(
				::ui::event::EV_MOUSE_DOWN, EH( this, brush ) {
					SelectBrush( brush );
					return true;
				}
			);
			m_centered_block->AddChild( button );
			m_brush_buttons.push_back( button );

			// vertical, then horizontal
			if ( ++by >= s_brush_buttons_in_column ) {
				by = 0;
				bx++;
			}
		}
	}

	{ // tool info
		NEW( m_tool_info.body, BBSection, m_game, "MiddleAreaToolbarToolInfo" );
		m_centered_block->AddChild( m_tool_info.body );

		const size_t offset = 4;
		uint8_t by = 0;
		for ( auto t = TI_NONE + 1 ; t < TI_MAX ; t++ ) {
			const auto ti = (tool_info_line_t)t;
			NEWV( label, ::ui::object::Label, "BBMiddleAreaToolbarInfoLabel" );
			label->SetLeft( offset );
			label->SetTop( offset + by * s_tool_info_line_height );
			m_tool_info.body->AddChild( label );
			m_tool_info.labels[ ti ] = label;

			by++;
		}
	}

	// initialize
	UpdateMapFileName();
	m_tool_info.labels[ TI_MAP_SIZE ]->SetText(
		"Map Size: " +
			std::to_string( m_game->GetMapWidth() ) +
			"x" +
			std::to_string( m_game->GetMapHeight() )
	);
	m_tool_info.labels[ TI_MODE ]->SetText( "PLAY mode (No Scroll Lock)" );

	SelectTool( ::game::map_editor::TT_ELEVATIONS );
	SelectBrush( ::game::map_editor::BT_DOT );
}

void Toolbar::Destroy() {

	{ // tools
		SelectTool( ::game::map_editor::TT_NONE );
		for ( auto& button : m_tool_buttons ) {
			m_centered_block->RemoveChild( button );
		}
		m_tool_buttons.clear();
	}

	{ // brushes
		SelectBrush( ::game::map_editor::BT_NONE );
		for ( auto& button : m_brush_buttons ) {
			m_centered_block->RemoveChild( button );
		}
		m_brush_buttons.clear();
	}

	{ // tool info
		for ( auto& label : m_tool_info.labels ) {
			m_tool_info.body->RemoveChild( label.second );
		}
		m_centered_block->RemoveChild( m_tool_info.body );
	}

	RemoveChild( m_centered_block );

	MiddleAreaPage::Destroy();
}

void Toolbar::Align() {
	MiddleAreaPage::Align();

	if ( m_tool_info.body ) {
		// TODO: implement in Style ?
		if ( m_object_area.width >= 460 ) {
			m_tool_info.body->Show();
			m_centered_block->SetAlign( ::ui::ALIGN_CENTER );
		}
		else {
			m_tool_info.body->Hide();
			m_centered_block->SetAlign( ::ui::ALIGN_LEFT );
		}
	}
}

void Toolbar::UpdateMapFileName() {
	ASSERT( m_tool_info.labels.find( TI_FILE ) != m_tool_info.labels.end(), "file line not found" );
	m_tool_info.labels[ TI_FILE ]->SetText( "File: " + m_game->GetMapFilename() );
}

void Toolbar::SelectTool( ::game::map_editor::tool_type_t tool ) {

	if ( m_game->GetEditorTool() != tool ) {

		Log( "Selecting tool: " + m_tool_names.at( tool ) );

		m_game->SetEditorTool( tool );

		if ( m_active_tool_button ) {
			m_active_tool_button->RemoveStyleModifier( ::ui::M_SELECTED );
		}
		if ( tool == ::game::map_editor::TT_NONE ) {
			m_active_tool_button = nullptr;
		}
		else {
			m_active_tool_button = m_tool_buttons.at( tool - 1 ); // because there's no button for ::game::map_editor::TT_NONE
			m_active_tool_button->AddStyleModifier( ::ui::M_SELECTED );
		}

		m_tool_info.labels[ TI_TOOL ]->SetText( "Editing: " + m_tool_names.at( tool ) );
	}
}

void Toolbar::SelectBrush( ::game::map_editor::brush_type_t brush ) {

	if ( m_game->GetEditorBrush() != brush ) {

		Log( "Selecting brush: " + m_brush_names.at( brush ) );

		m_game->SetEditorBrush( brush );

		if ( m_active_brush_button ) {
			m_active_brush_button->RemoveStyleModifier( ::ui::M_SELECTED );
		}
		if ( brush == ::game::map_editor::BT_NONE ) {
			m_active_brush_button = nullptr;
		}
		else {
			m_active_brush_button = m_brush_buttons.at( brush - 1 ); // because there's no button for ::game::map_editor::BT_NONE
			m_active_brush_button->AddStyleModifier( ::ui::M_SELECTED );
		}

		m_tool_info.labels[ TI_BRUSH ]->SetText( "Brush: " + m_brush_names.at( brush ) );
	}
}

}
}
}
