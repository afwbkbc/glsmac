#include "InfoPanels.h"

namespace task {
namespace game {
namespace ui {

void InfoPanels::Create() {
	MiddleAreaPage::Create();

	SetMargin( -2 );
}

void InfoPanels::Destroy() {
	for ( auto& panel : m_panels ) {
		RemoveChild( panel );
	}
	m_panels.clear();

	MiddleAreaPage::Destroy();
}

void InfoPanels::Align() {
	MiddleAreaPage::Align();

	// determine number of info panels from available width
	const auto w = m_object_area.width;
	const size_t panels_count = floor( w / s_min_info_panel_width );
	const size_t panel_width = floor( w / panels_count );

	// delete excessive panels
	while ( m_panels.size() > panels_count ) {
		RemoveChild( m_panels.back() );
		m_panels.pop_back();
	}

	// add missing panels
	while ( m_panels.size() < panels_count ) {
		NEWV( panel, InfoPanel, m_game );
		panel->SetTop( m_info_panel_margin.y );
		panel->SetBottom( m_info_panel_margin.y );
		AddChild( panel );
		m_panels.push_back( panel );
	}

	// resize / realign panels
	ssize_t left = ( w - panels_count * panel_width ) / 2; // symmetrical offset if width doesn't divide perfectly
	for ( auto& panel : m_panels ) {
		panel->SetWidth( (ssize_t)panel_width - m_info_panel_margin.x * 2 );
		panel->SetLeft( left + m_info_panel_margin.x );
		left += panel_width;
	}

}

}
}
}
