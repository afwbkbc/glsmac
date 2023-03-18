#include "Toolbar.h"

namespace game {
namespace world {
namespace ui {

void Toolbar::Create() {
	MiddleAreaPage::Create();
	
	SetMargin( -3 );
	
	uint8_t bx = 0, by = 0;
	for ( auto t = ET_NONE + 1 ; t < ET_MAX ; t++ ) {
		const auto tool = (editing_tool_t) t;
		ASSERT( m_tool_names.find( tool ) != m_tool_names.end(), "tool name not found" );
		const std::string& tool_name = m_tool_names.at( tool );
		NEWV( button, ::ui::object::SimpleButton, "MapBottomBarMiddleAreaToolbarButton " + tool_name );
			button->SetLeft( bx * s_tool_button_width );
			button->SetTop( by * s_tool_button_height );
			button->On( UIEvent::EV_MOUSE_DOWN, EH( this, tool ) {
				SelectTool( tool );
				return true;
			});
		AddChild( button );
		m_tool_buttons.push_back( button );
		
		if ( ++bx >= s_tool_buttons_in_row ) {
			bx = 0;
			by++;
		}
	}
}

void Toolbar::Destroy() {
	
	SelectTool( ET_NONE );
	for ( auto& button : m_tool_buttons ) {
		RemoveChild( button );
	}
	m_tool_buttons.clear();
	
	MiddleAreaPage::Destroy();
}

void Toolbar::Align() {
	MiddleAreaPage::Align();
		
}

void Toolbar::SelectTool( editing_tool_t tool ) {
	
	if ( m_active_tool != tool ) {
		m_active_tool = tool;
		
		Log( "Selecting tool: " + ( tool == ET_NONE ? "None" : m_tool_names.at( m_active_tool ) ) );

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

}
}
}
