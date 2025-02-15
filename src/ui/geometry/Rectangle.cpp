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

void Rectangle::SetStretched() {
	if ( m_tile_dimensions.x || m_tile_dimensions.y ) {
		m_tile_dimensions = {};
		NeedUpdate();
	}
}

void Rectangle::SetTiled( const types::Vec2< size_t >& dimensions ) {
	if ( m_tile_dimensions != dimensions ) {
		m_tile_dimensions = dimensions;
		NeedUpdate();
	}
}

void Rectangle::UpdateImpl() {
	if ( m_mesh ) {
		const auto top_left = m_ui->ClampXY(
			{
				m_area.left,
				m_area.top,
			}
		);
		const auto bottom_right = m_ui->ClampXY(
			{
				m_area.right,
				m_area.bottom,
			}
		);
		if ( m_tile_dimensions.x && m_tile_dimensions.y ) {
			m_mesh->SetCoordsTiled( top_left, bottom_right, m_tile_dimensions, 0.5f );
		}
		else {
			m_mesh->SetCoords( top_left, bottom_right, 0.5f );
		}
	}
}

}
}
