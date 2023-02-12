#include "StyleSheet.h"

namespace ui {
namespace theme {

StyleSheet::~StyleSheet() {
	for ( auto& style : m_styles ) {
		DELETE( style.second );
	}
}

void StyleSheet::AddStyle( const std::string& style_name, const Style::includes_t& includes, const Style::style_handler_t handler ) {
	ASSERT( m_styles.find( style_name ) == m_styles.end(), "style '" + style_name + "' already exists" );
	NEWV( style, Style, style_name, this );
	style->SetIncludes( includes );
	style->SetStyleHandler( handler );
	m_styles[ style_name ] = style;
	m_styles_order.push_back( style_name );
}
void StyleSheet::AddStyle( const std::string& style_name, const Style::style_handler_t handler ) {
	AddStyle( style_name, {}, handler );
}

// vector because styles loading order matters
const std::vector< Style* > StyleSheet::GetStyles() {
	if ( !m_is_initialized ) {
		AddStyles();
		m_is_initialized = true;
	}
	std::vector< Style* > result;
	for ( auto& s : m_styles_order ) {
		result.push_back( m_styles.at( s ) );
	}
	return result;
}

Style* StyleSheet::GetStylePtr( const std::string& style_name ) const {
	auto it = m_styles.find( style_name );
	ASSERT( it != m_styles.end(), "included style '" + style_name + "' not found" );
	return it->second;
}

}
}
