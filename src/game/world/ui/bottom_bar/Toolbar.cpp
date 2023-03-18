#include "Toolbar.h"

namespace game {
namespace world {
namespace ui {

void Toolbar::Create() {
	MiddleAreaPage::Create();
	
	SetMargin( -3 );
	
	// TODO: split to separate panels/sections?
	
	{ // tool buttons
		const Vec2< size_t > offset = { 0, 0 };
		uint8_t bx = 0, by = 0;
		for ( auto t = ET_NONE + 1 ; t < ET_MAX ; t++ ) {
			const auto tool = (editing_tool_t) t;
			ASSERT( m_tool_names.find( tool ) != m_tool_names.end(), "tool name not found" );
			const std::string& tool_name = m_tool_names.at( tool );
			NEWV( button, ::ui::object::SimpleButton, "MapBottomBarMiddleAreaToolbarButton Tool " + tool_name );
				button->SetLeft( offset.x + bx * s_tool_button_width );
				button->SetTop( offset.y + by * s_tool_button_height );
				button->On( UIEvent::EV_MOUSE_DOWN, EH( this, tool ) {
					SelectTool( tool );
					return true;
				});
			AddChild( button );
			m_tool_buttons.push_back( button );

			// horizontal, then vertical
			if ( ++bx >= s_tool_buttons_in_row ) {
				bx = 0;
				by++;
			}
		}
		SelectTool( ET_ELEVATIONS );
	}
	
	{ // brush buttons
		const Vec2< size_t > offset = { 230, 10 };
		uint8_t bx = 0, by = 0;
		for ( auto b = EB_NONE + 1 ; b < EB_MAX ; b++ ) {
			const auto brush = (editing_brush_t) b;
			ASSERT( m_brush_names.find( brush ) != m_brush_names.end(), "brush name not found" );
			const std::string& brush_name = m_brush_names.at( brush );
			NEWV( button, ::ui::object::SimpleButton, "MapBottomBarMiddleAreaToolbarButton Brush " + brush_name );
				button->SetLeft( offset.x + bx * s_brush_button_width );
				button->SetTop( offset.y + by * s_brush_button_height );
				button->On( UIEvent::EV_MOUSE_DOWN, EH( this, brush ) {
					SelectBrush( brush );
					return true;
				});
			AddChild( button );
			m_brush_buttons.push_back( button );

			// vertical, then horizontal
			if ( ++by >= s_brush_buttons_in_column ) {
				by = 0;
				bx++;
			}
		}
		SelectBrush( EB_DOT );
	}
}

void Toolbar::Destroy() {
	
	SelectTool( ET_NONE );
	for ( auto& button : m_tool_buttons ) {
		RemoveChild( button );
	}
	m_tool_buttons.clear();
	
	SelectBrush( EB_NONE );
	for ( auto& button : m_brush_buttons ) {
		RemoveChild( button );
	}
	m_brush_buttons.clear();

	MiddleAreaPage::Destroy();
}

void Toolbar::Align() {
	MiddleAreaPage::Align();
		
}

void Toolbar::SelectTool( editing_tool_t tool ) {
	
	if ( m_active_tool != tool ) {
		m_active_tool = tool;
		
		Log( "Selecting tool: " + m_tool_names.at( m_active_tool ) );

		if ( m_active_tool_button ) {
			m_active_tool_button->RemoveStyleModifier( Style::M_SELECTED );
		}
		if ( tool == ET_NONE ) {
			m_active_tool_button = nullptr;
		}
		else {
			m_active_tool_button = m_tool_buttons.at( m_active_tool - 1 ); // because there's no button for ET_NONE
			m_active_tool_button->AddStyleModifier( Style::M_SELECTED );
		}
	}
}

void Toolbar::SelectBrush( editing_brush_t brush ) {
	
	if ( m_active_brush != brush ) {
		m_active_brush = brush;
		
		Log( "Selecting brush: " + m_brush_names.at( m_active_brush ) );

		if ( m_active_brush_button ) {
			m_active_brush_button->RemoveStyleModifier( Style::M_SELECTED );
		}
		if ( brush == EB_NONE ) {
			m_active_brush_button = nullptr;
		}
		else {
			m_active_brush_button = m_brush_buttons.at( m_active_brush - 1 ); // because there's no button for EB_NONE
			m_active_brush_button->AddStyleModifier( Style::M_SELECTED );
		}
	}
}

}
}
}
