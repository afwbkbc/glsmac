#include "Rectangle.h"

#include "ui/UI.h"
#include "types/mesh/Rectangle.h"
#include "scene/actor/Mesh.h"

namespace ui {
namespace geometry {

Rectangle::Rectangle( const UI* const ui, Geometry* const parent )
	: Geometry( ui, parent, GT_RECTANGLE ) {

}

void Rectangle::SetActorAndMesh( scene::actor::Mesh* const actor, types::mesh::Rectangle* const mesh ) {
	m_actor = actor;
	m_mesh = mesh;
	UpdateImpl();
}

void Rectangle::UpdateImpl() {
	const float zindex = 0.5f; // TODO
	if ( m_actor && m_mesh ) {
		m_actor->SetPosition(
			{
				0, //m_ui->ClampX( m_effective_area.left ),
				0, //m_ui->ClampY( m_effective_area.top ),
				zindex,
			}
		);
		m_mesh->SetCoords(
			m_ui->ClampXY(
				{
					m_area.left - m_effective_area.left,
					m_area.top - m_effective_area.top,
				}
			),
			m_ui->ClampXY(
				{
					m_effective_area.width,
					m_effective_area.height,
				}
			),
			/*{
				-1,
				-1
			},
			{
				1,
				1
			},*/
			zindex
		);
	}
}

}
}
