#pragma once

/* rectangle area with border and children objects */

#include "UIContainer.h"

#include "Surface.h"

namespace ui {
namespace object {

CHILD_CLASS( Panel, UIContainer )

	void SetBackgroundColor( const types::Color &color ) {
		m_colors.background = color;
		Redraw();
	}

	void SetBorderColor( const types::Color &color ) {
		m_colors.border = color;

		if ( m_created )
			DrawBorders();
	}

	void SetBorderWidth( const unsigned char border_width ) {
		m_border_width = border_width;

		if ( m_created )
			AlignBorders();
	}

	virtual void Create() {
		UIContainer::Create();

		AddChild( m_background = new Surface );

		m_borders.left = new Surface;
		m_borders.left->SetAlign( ALIGN_LEFT );
		m_borders.top = new Surface;
		m_borders.top->SetAlign( ALIGN_TOP );
		m_borders.right = new Surface;
		m_borders.right->SetAlign( ALIGN_RIGHT );
		m_borders.bottom = new Surface;
		m_borders.bottom->SetAlign( ALIGN_BOTTOM );

		AlignBorders();
		DrawBorders();

		AddChild( m_borders.left );
		AddChild( m_borders.top );
		AddChild( m_borders.right );
		AddChild( m_borders.bottom );
	}

	virtual void Destroy() {
		RemoveChild( m_background );

		RemoveChild( m_borders.left );
		RemoveChild( m_borders.top );
		RemoveChild( m_borders.right );
		RemoveChild( m_borders.bottom );

		UIContainer::Destroy();
	}

	virtual void Draw() {
		UIContainer::Draw();

		m_background->SetColor( m_colors.background );
	}

	/*const coord_t GetLeft() const {
		return m_coords.top_left.x;
	}
	virtual const coord_t GetTop() const = 0;
	virtual const coord_t GetRight() const = 0;
	virtual const coord_t GetBottom() const = 0;*/
protected:
	struct {
		types::Color background;
		types::Color border;
	} m_colors;

	Surface *m_background;

	unsigned char m_border_width = 0;
	struct {
		Surface *left;
		Surface *top;
		Surface *right;
		Surface *bottom;
	} m_borders;
private:
	void AlignBorders() {
		m_borders.left->SetWidth( m_border_width );
		m_borders.top->SetHeight( m_border_width );
		m_borders.right->SetWidth( m_border_width );
		m_borders.bottom->SetHeight( m_border_width );
	}
	void DrawBorders() {
		m_borders.left->SetColor( m_colors.border );
		m_borders.top->SetColor( m_colors.border );
		m_borders.right->SetColor( m_colors.border );
		m_borders.bottom->SetColor( m_colors.border );
	}
};

} /* namespace object */
} /* namespace ui */
