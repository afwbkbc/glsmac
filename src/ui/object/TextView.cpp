#include "TextView.h"

#include "engine/Engine.h"

#define DOUBLECLICK_MAX_MS 1000

namespace ui {
namespace object {

void TextView::Create() {
	ScrollView::Create();
	
	ApplyStyleIfNeeded(); // TODO: move to UIContainer?
	
	size_t index = 0;
	for ( auto& line : m_lines ) {
		AddItem( index, line );
		index++;
	}
}

void TextView::Align() {
	ScrollView::Align();
	
	m_item_align.width = m_object_area.width - m_item_align.margin;
	if ( m_type == TT_SIMPLE ) {
		m_item_align.width -= m_item_align.left + m_item_align.right;
	}
	size_t total_height = m_item_align.margin;
	for ( auto& item : m_items ) {
		AlignItem( item, total_height );
		total_height += item->GetHeight();
	}
	
}

void TextView::Destroy() {
	Clear();
	
	ScrollView::Destroy();
}

void TextView::SetType( const textview_type_t type ) {
	ASSERT( m_lines.empty(), "can't change type on non-empty text view" );
	if ( m_type != type ) {
		m_type = type;
	}
}

void TextView::SetLinesLimit( const size_t lines_limit ) {
	m_lines_limit = lines_limit;
	LimitLines();
}

void TextView::SetItemWidth( const coord_t item_width ) {
	m_item_align.width = item_width;
	Realign();
}

void TextView::SetItemHeight( const coord_t item_height ) {
	m_item_align.height = item_height;
	Realign();
	SetScrollSpeed( m_item_align.height + m_item_align.margin );
}

void TextView::SetItemMargin( const coord_t item_margin ) {
	m_item_align.margin = item_margin;
	Realign();
	SetScrollSpeed( m_item_align.height + m_item_align.margin );
}

void TextView::SetTextLeft( const coord_t text_left ) {
	m_item_align.left = text_left;
	Realign();
}

void TextView::Clear() {
	for ( auto& item : m_items ) {
		RemoveChild( item );
	}
	m_items.clear();
	m_lines.clear();
	
	if ( m_type == TT_EXTENDED ) {
		m_lines_indices.clear();
		m_current_index = -1;
		if ( m_active_textline ) {
			m_active_textline = nullptr;
			UIEvent::event_data_t d = {};
			d.value.list_item.text_ptr = nullptr;
			Trigger( UIEvent::EV_CHANGE, &d );
		}
	}
}

void TextView::AddLine( const std::string& text, const std::string& line_class ) {
	ASSERT( line_class.empty() || m_type == TT_EXTENDED, "can't assign line class for simple text view" );
	const ssize_t index = m_lines.size();
	m_lines.push_back({
		text,
		line_class
	});
	if ( m_type == TT_EXTENDED ) {
		m_lines_indices[ text ] = index;
	}
	if ( m_created ) {
		AddItem( index, m_lines.back() );
	}
	LimitLines();
}

void TextView::RemoveLine( const size_t index ) {
	ASSERT( index < m_lines.size(), "line out of bounds" );
	if ( m_type == TT_EXTENDED ) {
		const auto it = m_lines_indices.find( m_lines.at( index ).text );
		if ( it != m_lines_indices.end() ) {
			m_lines_indices.erase( it );
		}
	}
	m_lines.erase( m_lines.begin() + index );
	if ( m_created ) {
		RemoveItem( index );
	}
}

const std::string& TextView::GetSelectedText() const {
	ASSERT( m_type == TT_EXTENDED, "can only get value in extended text view" );
	if ( m_active_textline ) {
		return m_active_textline->GetText();
	}
	else {
		return m_empty_value;
	}
}

void TextView::SelectLine( const std::string& line ) {
	ASSERT( m_type == TT_EXTENDED, "can only select line in extended text view" );
	const auto it = m_lines_indices.find( line );
	if ( it != m_lines_indices.end() ) {
		SelectItem( it->second );
	}
}

void TextView::SelectFirstLine() {
	ASSERT( m_type == TT_EXTENDED, "can only select line in extended text view" );
	if ( !m_items.empty() ) {
		SelectItem( 0 );
	}
}

void TextView::SelectLastLine() {
	ASSERT( m_type == TT_EXTENDED, "can only select line in extended text view" );
	if ( !m_items.empty() ) {
		SelectItem( m_items.size() - 1 );
	}
}

void TextView::SelectPreviousLine() {
	ASSERT( m_type == TT_EXTENDED, "can only select line in extended text view" );
	if ( !m_items.empty() ) {
		if ( m_current_index > 0 ) {
			SelectItem( m_current_index - 1 );
		}
	}
}

void TextView::SelectNextLine() {
	ASSERT( m_type == TT_EXTENDED, "can only select line in extended text view" );
	if ( !m_items.empty() ) {
		if ( m_current_index < m_items.size() - 1 ) {
			SelectItem( m_current_index + 1 );
		}
	}
}

// TODO: determine from actual items in ScrollView
#define LINES_PER_PAGE 20

void TextView::SelectPreviousPage() {
	ASSERT( m_type == TT_EXTENDED, "can only select page in extended text view" );
	if ( !m_items.empty() ) {
		if ( m_current_index >= LINES_PER_PAGE ) {
			SelectItem( m_current_index - LINES_PER_PAGE );
		}
		else {
			SelectFirstLine();
		}
	}
}

void TextView::SelectNextPage() {
	ASSERT( m_type == TT_EXTENDED, "can only select page in extended text view" );
	if ( !m_items.empty() ) {
		if ( m_current_index + LINES_PER_PAGE <= m_items.size() - 1 ) {
			SelectItem( m_current_index + LINES_PER_PAGE );
		}
		else {
			SelectLastLine();
		}
	}
}

void TextView::ApplyStyle() {
	ScrollView::ApplyStyle();
	
	if ( Has( Style::A_ITEM_WIDTH ) ) {
		ASSERT( false, "A_ITEM_WIDTH not implemented yet" );
		SetItemWidth( Get( Style::A_ITEM_WIDTH ) );
	}
	if ( Has( Style::A_ITEM_HEIGHT ) ) {
		SetItemHeight( Get( Style::A_ITEM_HEIGHT ) );
	}
	if ( Has( Style::A_ITEM_MARGIN ) ) {
		SetItemMargin( Get( Style::A_ITEM_MARGIN ) );
	}
	if ( Has( Style::A_TEXT_LEFT ) ) {
		SetTextLeft( Get( Style::A_TEXT_LEFT ) );
	}
}

void TextView::SelectItem( const ssize_t index ) {
	ASSERT( m_type == TT_EXTENDED, "can only select line in extended text view" );
	ASSERT( index >= 0, "index can't be negative" );
	ASSERT( index < m_items.size(), "index overflow" );
	
	auto* textline = (TextLine*) m_items[ index ];
	ASSERT( textline, "textline is null" );
	
	if ( m_active_textline != textline ) {
		if ( m_active_textline ) {
			m_active_textline->RemoveStyleModifier( Style::M_SELECTED );
		}
		m_current_index = index;
		m_active_textline = textline;
		ScrollToObjectMaybe( m_active_textline );
		m_active_textline->AddStyleModifier( Style::M_SELECTED );
		UIEvent::event_data_t d = {};
		d.value.list_item.text_ptr = textline->GetTextPtr();
		Trigger( UIEvent::EV_CHANGE, &d );
	}
}

void TextView::AddItem( const size_t index, const line_t& line ) {
	//Log( "Adding line \"" + line.text + "\"" );
	UIObject* item = nullptr;
	switch ( m_type ) {
		case TT_SIMPLE: {
			NEWV( label, Label );
				label->SetText( line.text );
			item = label;
			break;
		}
		case TT_EXTENDED: {
			NEWV( textline, TextLine, line.line_class );
				textline->SetText( line.text );
				textline->On( UIEvent::EV_MOUSE_DOWN, EH( this, index, textline ) {
					if ( data->mouse.button == UIEvent::M_LEFT ) {
						bool is_double_click = false;
						if ( m_active_textline == textline && m_maybe_doubleclick ) {
							if ( !m_doubleclick_timer.HasTicked() ) {
								is_double_click = true;
							}
							m_doubleclick_timer.Stop();
							m_maybe_doubleclick = false;
						}
						else {
							m_doubleclick_timer.SetTimeout( DOUBLECLICK_MAX_MS );
							m_maybe_doubleclick = true;
						}
						if ( m_active_textline != textline ) {
							SelectItem( index );
						}
						if ( is_double_click ) {
							UIEvent::event_data_t d = {};
							d.value.list_item.text_ptr = textline->GetTextPtr();
							Trigger( UIEvent::EV_SELECT, &d );
						}
						return true;
					}
					return false;
				});
			item = textline;
			item->ForwardStyleAttributesV( m_forwarded_style_attributes_ext );
			break;
		}
		default: {
			THROW( "unknown textview type " + std::to_string( m_type ) );
		}
	}
	item->ForwardStyleAttributesV( m_forwarded_style_attributes );
	AlignItem( item, !m_items.empty()
		? m_items.back()->GetTop() + m_items.back()->GetHeight() + m_item_align.margin
		: 0
	);
	AddChild( item );
	m_items.push_back( item );
	if ( m_type == TT_EXTENDED ) {
		if ( m_current_index < 0 ) {
			SelectItem( index );
		}
	}
}

void TextView::AlignItem( UIObject* item, const size_t top ) {
	if ( m_type == TT_SIMPLE ) {
		item->SetLeft( m_item_align.left + m_item_align.margin );
	}
	item->SetWidth( m_item_align.width );
	item->SetHeight( m_item_align.height + m_item_align.margin );
	item->SetTop( top );
}

void TextView::RemoveItem( const size_t index ) {
	ASSERT( index < m_items.size(), "item out of bounds" );
	const auto& item = m_items.at( index );
	//Log( "Removing line \"" + item->GetText() + "\"" );
	const auto h = item->GetHeight() + m_item_align.margin;
	for ( auto i = index ; i < m_items.size() ; i++ ) {
		const auto& item = m_items.at( i );
		item->SetTop( item->GetTop() - h );
	}
	if ( m_active_textline == item ) {
		m_current_index = -1;
		m_active_textline = nullptr;
		UIEvent::event_data_t d = {};
		d.value.list_item.text_ptr = nullptr;
		Trigger( UIEvent::EV_CHANGE, &d );
	}
	RemoveChild( item );
	m_items.erase( m_items.begin() + index );
}

void TextView::LimitLines() {
	ASSERT( m_lines_limit > 0, "lines limit must be higher than 0" );
	if ( m_lines_limit ) {
		while ( m_lines.size() > m_lines_limit ) {
			RemoveLine( 0 );
		}
	}
}

}
}
