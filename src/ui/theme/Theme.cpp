#include "Theme.h"

namespace ui {
namespace theme {

Theme::~Theme() {
	for ( auto& it : m_styles ) {
		delete it.second;
	}
}

void Theme::AddStyle( Style* style ) {
#if DEBUG
	if ( m_is_finalized ) {
		throw ThemeError( "adding style to finalized theme" );
	}
	if ( m_styles.find( style->GetClassName() ) != m_styles.end() ) {
		throw ThemeError( "style '" + style->GetClassName() + "' already exists" );
	}
#endif
	m_styles[ style->GetClassName() ] = style;
}

const Style* Theme::GetStyle( const string style_class ) const {
#if DEBUG
	if ( !m_is_finalized ) {
		throw ThemeError( "getting style from non-finalized theme" );
	}
	if ( m_styles.find( style_class ) == m_styles.end() ) {
		throw ThemeError( "style '" + style_class + "' does not exist" );
	}
#endif
	return m_styles.at( style_class );
}

void Theme::Finalize() {
#if DEBUG
	if ( m_is_finalized ) {
		throw ThemeError( "theme already finalized" );
	}
#endif
	for ( auto& style : m_styles ) {
		style.second->Initialize();
	}
	m_is_finalized = true;
}

}
}
