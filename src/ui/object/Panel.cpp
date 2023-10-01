#include "Panel.h"

#include "engine/Engine.h"

namespace ui {
namespace object {

Panel::Panel( const std::string& class_name )
	: UIContainer( class_name ) {

}

void Panel::Create() {
	UIContainer::Create();

	for ( auto& b : m_surfaces ) {
		NEW( b.second, Surface );
		b.second->SetOverflow( UIObject::OVERFLOW_VISIBLE_ALWAYS );
		b.second->ForwardStyleAttribute( b.first, Style::A_TEXTURE );
	}

	m_surfaces[ Style::A_TEXTURE_LEFT ]->SetAlign( UIObject::ALIGN_LEFT );
	m_surfaces[ Style::A_TEXTURE_TOP ]->SetAlign( UIObject::ALIGN_TOP );
	m_surfaces[ Style::A_TEXTURE_RIGHT ]->SetAlign( UIObject::ALIGN_RIGHT );
	m_surfaces[ Style::A_TEXTURE_BOTTOM ]->SetAlign( UIObject::ALIGN_BOTTOM );
	m_surfaces[ Style::A_TEXTURE_LEFTTOP ]->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
	m_surfaces[ Style::A_TEXTURE_RIGHTTOP ]->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
	m_surfaces[ Style::A_TEXTURE_RIGHTBOTTOM ]->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	m_surfaces[ Style::A_TEXTURE_LEFTBOTTOM ]->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_BOTTOM );

	for ( auto& b : m_surfaces ) {
		AddChild( b.second );
	}

}

void Panel::Destroy() {

	for ( auto& b : m_surfaces ) {
		RemoveChild( b.second );
	}

	UIContainer::Destroy();
}

void Panel::Align() {
	UIContainer::Align();

	for ( auto& b : m_surfaces ) {
		bool is_corner =
			b.first == Style::A_TEXTURE_LEFTTOP ||
				b.first == Style::A_TEXTURE_RIGHTTOP ||
				b.first == Style::A_TEXTURE_RIGHTBOTTOM ||
				b.first == Style::A_TEXTURE_LEFTBOTTOM;
		if ( b.first == Style::A_TEXTURE_BACK ) {
			b.second->SetMargin( m_border_size );
			b.second->Maximize();
		}
		if (
			b.first == Style::A_TEXTURE_LEFT ||
				b.first == Style::A_TEXTURE_RIGHT ||
				is_corner
			) {
			b.second->SetWidth( m_border_size );
		}
		if (
			b.first == Style::A_TEXTURE_TOP ||
				b.first == Style::A_TEXTURE_BOTTOM ||
				is_corner
			) {
			b.second->SetHeight( m_border_size );
		}
		if (
			b.first == Style::A_TEXTURE_LEFT ||
				b.first == Style::A_TEXTURE_RIGHT
			) {
			b.second->SetTop( m_border_size );
			b.second->SetBottom( m_border_size );
		}
		if (
			b.first == Style::A_TEXTURE_TOP ||
				b.first == Style::A_TEXTURE_BOTTOM
			) {
			b.second->SetLeft( m_border_size );
			b.second->SetRight( m_border_size );
		}
	}
}

void Panel::SetBorderWidth( const size_t border_size ) {
	if ( border_size != m_border_size ) {
		m_border_size = border_size;
		Realign();
	}
}

const size_t Panel::GetBorderSize() const {
	return m_border_size;
}

void Panel::ApplyStyle() {
	UIContainer::ApplyStyle();

	if ( Has( Style::A_BORDER_SIZE ) ) {
		SetBorderWidth( Get( Style::A_BORDER_SIZE ) );
	}

}

}
}
