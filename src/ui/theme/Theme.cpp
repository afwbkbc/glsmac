#include "Theme.h"

#include "Style.h"
#include "StyleSheet.h"

namespace ui {
namespace theme {

void Theme::AddStyle( Style* style ) {
	ASSERT( !m_is_finalized, "adding style to finalized theme" );
	ASSERT( m_styles.find( style->GetClassName() ) == m_styles.end(), "style '" + style->GetClassName() + "' already exists" );
	m_styles[ style->GetStyleName() ] = style;
	m_styles_order.push_back( style->GetStyleName() );
}

void Theme::AddStyleSheet( StyleSheet* stylesheet ) {
	ASSERT( !m_is_finalized, "adding style to finalized theme" );
	for ( auto& style : stylesheet->GetStyles() ) {
		AddStyle( style );
	}
}

const Style* Theme::GetStyle( const std::string style_class ) const {
	ASSERT( m_is_finalized, "getting style from non-finalized theme" );
	auto it = m_styles.find( style_class );
	if ( it == m_styles.end() ) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

void Theme::Finalize() {
	if ( !m_is_finalized ) {
		for ( auto& style : m_styles_order ) {
			m_styles.at( style )->Initialize();
		}
		m_is_finalized = true;
	}
}

}
}
