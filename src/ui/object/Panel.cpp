#include "Panel.h"

#include "engine/Engine.h"

namespace ui {
namespace object {

Panel::Panel( const string& class_name ) : UIContainer( class_name ) {
	
}

void Panel::Create() {
	UIContainer::Create();
	
	size_t bw = 1;
	
	for (auto& b : m_borders) {
		NEW( b.second, Surface );
		bool is_corner =
			b.first == Style::A_TEXTURE_BORDER_CORNER_LT ||
			b.first == Style::A_TEXTURE_BORDER_CORNER_RT ||
			b.first == Style::A_TEXTURE_BORDER_CORNER_RB ||
			b.first == Style::A_TEXTURE_BORDER_CORNER_LB
		;
		if ( b.first == Style::A_TEXTURE_BACK ) {
			b.second->SetPadding( bw );
		}
		if (
			b.first == Style::A_TEXTURE_BORDER_LEFT ||
			b.first == Style::A_TEXTURE_BORDER_RIGHT ||
			is_corner
		) {
			b.second->SetWidth( bw );
		}
		if (
			b.first == Style::A_TEXTURE_BORDER_TOP ||
			b.first == Style::A_TEXTURE_BORDER_BOTTOM ||
			is_corner
		) {
			b.second->SetHeight( bw );
		}
		if (
			b.first == Style::A_TEXTURE_BORDER_LEFT ||
			b.first == Style::A_TEXTURE_BORDER_RIGHT
		) {
			b.second->SetTop( bw );
			b.second->SetBottom( bw );
		}
		if (
			b.first == Style::A_TEXTURE_BORDER_TOP ||
			b.first == Style::A_TEXTURE_BORDER_BOTTOM
		) {
			b.second->SetLeft( bw );
			b.second->SetRight( bw );
		}
	}
	
	m_borders[ Style::A_TEXTURE_BORDER_LEFT ]->SetAlign( UIObject::ALIGN_LEFT );
	m_borders[ Style::A_TEXTURE_BORDER_TOP ]->SetAlign( UIObject::ALIGN_TOP );
	m_borders[ Style::A_TEXTURE_BORDER_LEFT ]->SetAlign( UIObject::ALIGN_RIGHT );
	m_borders[ Style::A_TEXTURE_BORDER_BOTTOM ]->SetAlign( UIObject::ALIGN_BOTTOM );
	m_borders[ Style::A_TEXTURE_BORDER_CORNER_LT ]->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
	m_borders[ Style::A_TEXTURE_BORDER_CORNER_RT ]->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
	m_borders[ Style::A_TEXTURE_BORDER_CORNER_RB ]->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	m_borders[ Style::A_TEXTURE_BORDER_CORNER_LB ]->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_BOTTOM );
	
	for (auto& b : m_borders) {
		AddChild( b.second );
	}
}

void Panel::Destroy() {
//	RemoveChild( m_background );

	for (auto& b : m_borders) {
		RemoveChild( b.second );
	}
	
	UIContainer::Destroy();
}

void Panel::ApplyStyle() {
	UIContainer::ApplyStyle();

	for (auto& b : m_borders) {
		if ( Has( b.first ) ) {
			const auto* texture = (Texture*)GetObject( b.first );
			b.second->SetTexture( texture );
		}
	}
}

}
}
