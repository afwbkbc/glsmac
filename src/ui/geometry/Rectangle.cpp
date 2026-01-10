#include "Rectangle.h"

#include <cmath>

#include "ui/UI.h"
#include "types/mesh/Rectangle.h"
#include "types/mesh/Render.h"
#include "scene/actor/Mesh.h"

namespace ui {
namespace geometry {

Rectangle::Rectangle( const UI* const ui, Geometry* const parent )
	: Geometry( ui, parent, GT_RECTANGLE ) {}

Rectangle::~Rectangle() {
	Clear();
}

void Rectangle::SetMesh( types::mesh::Mesh* const mesh ) {
	if ( mesh != m_mesh ) {
		m_mesh = mesh;
		InitMesh( m_mesh );
		UpdateImpl();
	}
}

void Rectangle::SetActor( scene::actor::Mesh* const actor ) {
	if ( actor != m_actor ) {
		m_actor = actor;
		UpdateImpl();
	}
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

void Rectangle::AddActor( scene::actor::Mesh* const actor ) {
	m_actors.push_back( actor );
	UpdateActor( actor );
}

void Rectangle::AddMesh( types::mesh::Mesh* const mesh ) {
	m_meshes.push_back( mesh );
	InitMesh( mesh );
	UpdateMesh( mesh );
}

void Rectangle::Clear() {
	m_actors.clear();
	m_meshes.clear();
	for ( const auto& it : m_render_mesh_originals ) {
		delete it.second;
	}
	m_render_mesh_originals.clear();
}

void Rectangle::UpdateImpl() {
	if ( m_mesh ) {
		UpdateMesh( m_mesh );
	}
	if ( m_actor ) {
		UpdateActor( m_actor );
	}
	for ( const auto& mesh : m_meshes ) {
		UpdateMesh( mesh );
	}
	for ( const auto& actor : m_actors ) {
		UpdateActor( actor );
	}
}

void Rectangle::InitMesh( types::mesh::Mesh* const mesh ) {
	if ( mesh && mesh->GetType() == types::mesh::Mesh::MT_RENDER ) {
		const auto* const m = (types::mesh::Render*)mesh;
		const auto& it = m_render_mesh_originals.find( m );
		if ( it != m_render_mesh_originals.end() ) {
			delete it->second;
		}

		// need to save original because we'll scale active mesh from it
		m_render_mesh_originals.insert_or_assign( m, new types::mesh::Render( *m ) );

		// calculate and store original size (for future scaling)
		const auto sz = m->GetVertexCount();

		area_t area = {};
		types::Vec3 coord = {};
		for ( size_t i = 0 ; i < sz ; i++ ) {
			m->GetVertexCoord( i, &coord );
			if ( coord.x < area.left ) {
				area.left = coord.x;
			}
			if ( coord.x > area.right ) {
				area.right = coord.x;
			}
			if ( coord.y < area.top ) {
				area.top = coord.y;
			}
			if ( coord.y > area.bottom ) {
				area.bottom = coord.y;
			}
		}
		m_render_mesh_original_areas.insert_or_assign( m, area );
	}
}

void Rectangle::UpdateActor( scene::actor::Mesh* const actor ) {
	actor->SetPositionZ( m_area.zindex );
}

void Rectangle::UpdateMesh( types::mesh::Mesh* const mesh ) {
	switch ( mesh->GetType() ) {
		case types::mesh::Mesh::MT_RECTANGLE: {
			auto* m = (types::mesh::Rectangle*)mesh;
			const auto top_left = m_ui->ClampXY(
				{
					std::round( m_area.left ),
					std::round( m_area.top ),
				}
			);
			const auto bottom_right = m_ui->ClampXY(
				{
					std::round( m_area.right ),
					std::round( m_area.bottom ),
				}
			);
			if ( m_tile_dimensions.x && m_tile_dimensions.y ) {
				m->SetCoordsTiled( top_left, bottom_right, m_tile_dimensions, m_area.zindex );
			}
			else {
				m->SetCoords( top_left, bottom_right, m_area.zindex );
			}
			break;
		}
		case types::mesh::Mesh::MT_RENDER: {
			auto* m = (types::mesh::Render*)mesh;

			ASSERT( m_render_mesh_originals.find( m ) != m_render_mesh_originals.end(), "render mesh original not set" );
			const auto* const original = m_render_mesh_originals.at( m );
			ASSERT( m_render_mesh_original_areas.find( m ) != m_render_mesh_original_areas.end(), "render mesh original size not set" );
			const auto& original_area = m_render_mesh_original_areas.at( m );

			const auto count = mesh->GetVertexCount();
			types::Vec3 coord = {};
			types::Vec2< float > tex_coord = {};

			util::Clamper< float > clamp_x(
				{
					{
						original_area.left,
						original_area.right,
					},
					{
						m_area.left,
						m_area.right,
					}
				}
			);
			util::Clamper< float > clamp_y(
				{
					{
						original_area.top,
						original_area.bottom,
					},
					{
						m_area.top,
						m_area.bottom,
					}
				}
			);

			for ( size_t i = 0 ; i < count ; i++ ) {
				original->GetVertexCoord( i, &coord );
				coord.z = 0.5f; // doesn't do anything currently
				coord.x = m_ui->ClampX( clamp_x.Clamp( coord.x ) );
				coord.y = m_ui->ClampY( clamp_y.Clamp( coord.y ) );
				m->SetVertexCoord( i, coord );
			}
			break;
		}
		default: {
			ASSERT( false, "unsupported mesh type: " + std::to_string( m_mesh->GetType() ) );
		}
	}
}

}
}
