#pragma once

#include <vector>
#include <unordered_map>

#include "Geometry.h"

namespace types::mesh {
class Render;
}

namespace scene::actor {
class Mesh;
}

namespace ui {
namespace geometry {

class Rectangle : public Geometry {
public:
	Rectangle( const UI* const ui, Geometry* const parent );
	virtual ~Rectangle();

	void SetStretched();
	void SetTiled( const types::Vec2< size_t >& dimensions );

	void AddActor( scene::actor::Mesh* const actor );
	void AddMesh(  // managed by geometry
		types::mesh::Render* const mesh,
		const bool keep_tex = false,
		const types::Vec2< float >& scale = { 1.0f, 1.0f },
		const types::Vec2< float >& offset = { 0.0f, 0.0f }
	);
	void Clear();

protected:
	void UpdateImpl() override;

private:

	types::Vec2< size_t > m_tile_dimensions = {};

	std::vector< scene::actor::Mesh* > m_actors = {};
	std::vector< types::mesh::Render* > m_meshes = {};

	std::unordered_map< const types::mesh::Render*, types::mesh::Render* > m_render_mesh_originals = {};
	std::unordered_map< const types::mesh::Render*, area_t > m_render_mesh_original_areas = {};

	std::unordered_map< const types::mesh::Render*, bool > m_mesh_keep_tex = {};
	std::unordered_map< const types::mesh::Render*, types::Vec2< float > > m_mesh_scales = {};
	std::unordered_map< const types::mesh::Render*, types::Vec2< float > > m_mesh_offsets = {};

	void InitMesh( types::mesh::Render* const mesh, const bool keep_tex, const types::Vec2< float >& scale, const types::Vec2< float >& offset );

	void UpdateActor( scene::actor::Mesh* const actor );
	void UpdateMesh( types::mesh::Render* const mesh );

};

}
}
