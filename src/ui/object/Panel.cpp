#include "Panel.h"

#include "engine/Engine.h"

namespace ui {
namespace object {

Panel::Panel( const string& class_name ) : UIContainer( class_name ) {
	
}

void Panel::Create() {
	UIContainer::Create();
	
	for ( auto& b : m_surfaces ) {
		NEW( b.second, Surface );
		b.second->ForwardStyleAttribute( b.first, Style::A_TEXTURE );
	}
	
	m_surfaces[ Style::A_TEXTURE_BORDER_LEFT ]->SetAlign( UIObject::ALIGN_LEFT );
	m_surfaces[ Style::A_TEXTURE_BORDER_TOP ]->SetAlign( UIObject::ALIGN_TOP );
	m_surfaces[ Style::A_TEXTURE_BORDER_RIGHT ]->SetAlign( UIObject::ALIGN_RIGHT );
	m_surfaces[ Style::A_TEXTURE_BORDER_BOTTOM ]->SetAlign( UIObject::ALIGN_BOTTOM );
	m_surfaces[ Style::A_TEXTURE_BORDER_CORNER_LT ]->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
	m_surfaces[ Style::A_TEXTURE_BORDER_CORNER_RT ]->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
	m_surfaces[ Style::A_TEXTURE_BORDER_CORNER_RB ]->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	m_surfaces[ Style::A_TEXTURE_BORDER_CORNER_LB ]->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_BOTTOM );
	
	for (auto& b : m_surfaces) {
		AddChild( b.second );
	}
	
}

void Panel::Destroy() {

	for (auto& b : m_surfaces) {
		RemoveChild( b.second );
	}
	
	UIContainer::Destroy();
}

void Panel::Align() {
	UIContainer::Align();
	
	for (auto& b : m_surfaces) {
		bool is_corner =
			b.first == Style::A_TEXTURE_BORDER_CORNER_LT ||
			b.first == Style::A_TEXTURE_BORDER_CORNER_RT ||
			b.first == Style::A_TEXTURE_BORDER_CORNER_RB ||
			b.first == Style::A_TEXTURE_BORDER_CORNER_LB
		;
		if ( b.first == Style::A_TEXTURE_BACK ) {
			b.second->SetPadding( m_border_width );
		}
		if (
			b.first == Style::A_TEXTURE_BORDER_LEFT ||
			b.first == Style::A_TEXTURE_BORDER_RIGHT ||
			is_corner
		) {
			b.second->SetWidth( m_border_width );
		}
		if (
			b.first == Style::A_TEXTURE_BORDER_TOP ||
			b.first == Style::A_TEXTURE_BORDER_BOTTOM ||
			is_corner
		) {
			b.second->SetHeight( m_border_width );
		}
		if (
			b.first == Style::A_TEXTURE_BORDER_LEFT ||
			b.first == Style::A_TEXTURE_BORDER_RIGHT
		) {
			b.second->SetTop( m_border_width );
			b.second->SetBottom( m_border_width );
		}
		if (
			b.first == Style::A_TEXTURE_BORDER_TOP ||
			b.first == Style::A_TEXTURE_BORDER_BOTTOM
		) {
			b.second->SetLeft( m_border_width );
			b.second->SetRight( m_border_width );
		}
	}
}

void Panel::SetBorderWidth( const size_t border_width ) {
	if ( border_width != m_border_width ) {
		m_border_width = border_width;
		Realign();
	}
}

void Panel::ApplyStyle() {
	UIContainer::ApplyStyle();
	
	if ( Has( Style::A_BORDER_WIDTH ) ) {
		SetBorderWidth( Get( Style::A_BORDER_WIDTH ) );
	}

}

}
}
