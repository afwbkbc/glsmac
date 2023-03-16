#pragma once

#include <vector>

#include "ScrollView.h"

#include "Label.h"

namespace ui {
namespace object {

CLASS( TextView, ScrollView )
	
	TextView( const std::string& class_name = "" ) : ScrollView( class_name ) {}

	void Create();
	void Align();
	void Destroy();

	void SetLinesLimit( const size_t lines_limit );
	
	void SetItemWidth( const coord_t item_width );
	void SetItemHeight( const coord_t item_height );
	void SetItemMargin( const coord_t item_margin );
	
	void AddLine( const std::string& text );
	void RemoveLine( const size_t index );
	
protected:
	void ApplyStyle();
	
private:
	
	size_t m_lines_limit = 100;
	
	const std::vector< Style::attribute_type_t > m_forwarded_style_attributes = {
		Style::A_FONT,
		Style::A_TEXTCOLOR,
		Style::A_TEXTALIGN,
	};
	
	std::vector< std::string > m_lines = {};
	
	std::vector< Label* > m_labels = {};
	void AddLabel( const std::string& text );
	void AlignLabel( Label* label, const size_t top );
	void RemoveLabel( const size_t index );
	
	struct {
		coord_t width;
		coord_t height;
		coord_t margin;
	} m_item_align = {};
	
	void LimitLines();
};
	
}
}
