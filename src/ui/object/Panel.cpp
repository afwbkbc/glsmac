#include "Panel.h"

#include "engine/Engine.h"

#include "Surface.h"

namespace ui {
namespace object {

Panel::Panel( const std::string& class_name )
	: UIContainer( class_name ) {

}

void Panel::Create() {
	UIContainer::Create();

	for ( auto& b : m_surfaces ) {
		NEW( b.second, ui::object::Surface );

		// some days ago it was added, why? check if anything is broken now
		//b.second->SetOverflow( OVERFLOW_VISIBLE_ALWAYS );

		b.second->ForwardStyleAttribute( b.first, A_TEXTURE );
	}

	m_surfaces[ A_TEXTURE_LEFT ]->SetAlign( ALIGN_LEFT );
	m_surfaces[ A_TEXTURE_TOP ]->SetAlign( ALIGN_TOP );
	m_surfaces[ A_TEXTURE_RIGHT ]->SetAlign( ALIGN_RIGHT );
	m_surfaces[ A_TEXTURE_BOTTOM ]->SetAlign( ALIGN_BOTTOM );
	m_surfaces[ A_TEXTURE_LEFTTOP ]->SetAlign( ALIGN_LEFT | ALIGN_TOP );
	m_surfaces[ A_TEXTURE_RIGHTTOP ]->SetAlign( ALIGN_RIGHT | ALIGN_TOP );
	m_surfaces[ A_TEXTURE_RIGHTBOTTOM ]->SetAlign( ALIGN_RIGHT | ALIGN_BOTTOM );
	m_surfaces[ A_TEXTURE_LEFTBOTTOM ]->SetAlign( ALIGN_LEFT | ALIGN_BOTTOM );

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
			b.first == A_TEXTURE_LEFTTOP ||
				b.first == A_TEXTURE_RIGHTTOP ||
				b.first == A_TEXTURE_RIGHTBOTTOM ||
				b.first == A_TEXTURE_LEFTBOTTOM;
		if ( b.first == A_TEXTURE_BACK ) {
			b.second->SetMargin( m_border_size );
			b.second->Maximize();
		}
		if (
			b.first == A_TEXTURE_LEFT ||
				b.first == A_TEXTURE_RIGHT ||
				is_corner
			) {
			b.second->SetWidth( m_border_size );
		}
		if (
			b.first == A_TEXTURE_TOP ||
				b.first == A_TEXTURE_BOTTOM ||
				is_corner
			) {
			b.second->SetHeight( m_border_size );
		}
		if (
			b.first == A_TEXTURE_LEFT ||
				b.first == A_TEXTURE_RIGHT
			) {
			b.second->SetTop( m_border_size );
			b.second->SetBottom( m_border_size );
		}
		if (
			b.first == A_TEXTURE_TOP ||
				b.first == A_TEXTURE_BOTTOM
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

	if ( Has( A_BORDER_SIZE ) ) {
		SetBorderWidth( Get( A_BORDER_SIZE ) );
	}

}

}
}
