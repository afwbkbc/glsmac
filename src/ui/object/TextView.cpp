#include "TextView.h"

#include "engine/Engine.h"

namespace ui {
namespace object {

void TextView::Create() {
	ScrollView::Create();
	
	ApplyStyleIfNeeded(); // TODO: move to UIContainer?
	
	for ( auto& line : m_lines ) {
		AddLabel( line );
	}
}

void TextView::Align() {
	
	size_t total_height = m_item_align.margin;
	for ( auto& label : m_labels ) {
		AlignLabel( label, total_height );
		total_height += label->GetHeight();
	}
	
	ScrollView::Align();
}

void TextView::Destroy() {
	for ( auto& label : m_labels ) {
		RemoveChild( label );
	}
	m_labels.clear();
	
	ScrollView::Destroy();
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

void TextView::AddLine( const std::string& text ) {
	m_lines.push_back( text );
	if ( m_created ) {
		AddLabel( text );
	}
	LimitLines();
}

void TextView::RemoveLine( const size_t index ) {
	ASSERT( index < m_lines.size(), "line out of bounds" );
	m_lines.erase( m_lines.begin() + index );
	if ( m_created ) {
		RemoveLabel( index );
	}
}

void TextView::ApplyStyle() {
	ScrollView::ApplyStyle();
	
	if ( Has( Style::A_ITEM_WIDTH ) ) {
		SetItemWidth( Get( Style::A_ITEM_WIDTH ) );
	}
	if ( Has( Style::A_ITEM_HEIGHT ) ) {
		SetItemHeight( Get( Style::A_ITEM_HEIGHT ) );
	}
	if ( Has( Style::A_ITEM_MARGIN ) ) {
		SetItemMargin( Get( Style::A_ITEM_MARGIN ) );
	}
}

void TextView::AddLabel( const std::string& text ) {
	//Log( "Adding line \"" + text + "\"" );
	NEWV( label, Label );
		label->ForwardStyleAttributesV( m_forwarded_style_attributes );
		label->SetText( text );
		AlignLabel( label, !m_labels.empty()
			? m_labels.back()->GetTop() + m_labels.back()->GetHeight() + m_item_align.margin
			: 0
		);
	AddChild( label );
	m_labels.push_back( label );
}

void TextView::AlignLabel( Label* label, const size_t top ) {
	label->SetWidth( m_item_align.width );
	label->SetHeight( m_item_align.height + m_item_align.margin );
	label->SetLeft( m_item_align.margin );
	label->SetTop( top );
}

void TextView::RemoveLabel( const size_t index ) {
	ASSERT( index < m_labels.size(), "label out of bounds" );
	const auto& label = m_labels.at( index );
	//Log( "Removing line \"" + label->GetText() + "\"" );
	const auto h = label->GetHeight() + m_item_align.margin;
	for ( auto i = index ; i < m_labels.size() ; i++ ) {
		const auto& label = m_labels.at( i );
		label->SetTop( label->GetTop() - h );
	}
	RemoveChild( label );
	m_labels.erase( m_labels.begin() + index );
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
