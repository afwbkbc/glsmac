#include "Rectangle.h"

#include "ui/UI.h"
#include "types/mesh/Rectangle.h"

namespace ui {
namespace geometry {

Rectangle::Rectangle( const UI* const ui, Geometry* const parent )
	: Geometry( ui, parent, GT_RECTANGLE ) {

}

void Rectangle::SetMesh( types::mesh::Rectangle* const mesh ) {
	m_mesh = mesh;
	UpdateImpl();
}

void Rectangle::UpdateImpl() {
	if ( m_mesh ) {
		m_mesh->SetCoords(
			m_ui->ClampXY(
				{
					m_area.left,
					m_area.top,
				}
			),
			m_ui->ClampXY(
				{
					m_area.right,
					m_area.bottom,
				}
			),
			0.5f
		);
	}
}

}
}
