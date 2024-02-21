#pragma once

#include <vector>
#include <map>

#include "ScrollView.h"

#include "Label.h"
#include "TextLine.h"

#include "util/Timer.h"

namespace ui {
namespace object {

CLASS( TextView, ScrollView )

	enum textview_type_t {
		TT_SIMPLE, // faster, but provides only label
		TT_EXTENDED // slower. provides label, background, icon and custom styles, tracks selections and emits EV_CHANGE
	};

	TextView( const std::string& class_name = "" )
		: ScrollView( ST_VERTICAL, class_name ) {}

	void Create() override;
	void Align() override;
	void Destroy() override;

	void SetType( const textview_type_t type );
	void SetLinesLimit( const size_t lines_limit );

	void SetItemWidth( const coord_t item_width );
	void SetItemHeight( const coord_t item_height );
	void SetItemMargin( const coord_t item_margin );
	void SetTextLeft( const coord_t text_left );

	void Clear();
	void AddLine( const std::string& text, const std::string& line_class = "" );
	void RemoveLine( const size_t index );

	// for TT_EXTENDED
	const std::string& GetSelectedText() const;
	void SelectLine( const std::string& line );
	void SelectFirstLine();
	void SelectLastLine();
	void SelectPreviousLine();
	void SelectNextLine();
	void SelectPreviousPage();
	void SelectNextPage();
	size_t SelectByMask( std::string mask ); // returns number of matched characters

protected:
	void ApplyStyle() override;

private:

	typedef ssize_t index_t;

	textview_type_t m_type = TT_SIMPLE;
	size_t m_lines_limit = 200;

	const std::vector< Style::attribute_type_t > m_forwarded_style_attributes = {
		Style::A_FONT,
		Style::A_TEXT_COLOR,
		Style::A_TEXT_ALIGN,
	};

	const std::vector< Style::attribute_type_t > m_forwarded_style_attributes_ext = {
		Style::A_TEXT_LEFT,
		Style::A_ITEM_TEXTURE,
		Style::A_ITEM_ICON_TEXTURE,
	};

	struct line_t {
		std::string text;
		std::string line_class;
	};
	std::vector< line_t > m_lines = {};
	typedef std::map< std::string, index_t > lines_indices_t;
	lines_indices_t m_lines_indices = {}; // ordered because of SelectByMask()
	lines_indices_t m_lines_indices_ci = {}; // uppercase lines for case-insensitive matches

	std::vector< UIObject* > m_items = {}; // Label objects for TT_SIMPLE, TextLine objects for TT_EXTENDED

	// for TT_EXTENDED
	const std::string m_empty_value = "";
	TextLine* m_active_textline = nullptr;
	bool m_maybe_doubleclick = false;
	util::Timer m_doubleclick_timer;
	void SelectItem( const index_t index );

	index_t m_current_index = -1;
	void AddItem( const size_t index, const line_t& line );
	void AlignItem( UIObject* item, const size_t top );
	void RemoveItem( const size_t index );

	struct {
		coord_t width = 0;
		coord_t height = 0;
		coord_t margin = 0;
		coord_t left = 0;
		coord_t right = 0;
	} m_item_align = {};

	void LimitLines();
};

}
}
