#pragma once

#include <vector>
#include <unordered_map>

#include "Geometry.h"

namespace types::mesh {
class Mesh;
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

	void SetMesh( types::mesh::Mesh* const mesh );
	void SetActor( scene::actor::Mesh* const actor );
	void SetStretched();
	void SetTiled( const types::Vec2< size_t >& dimensions );

	void AddActor( scene::actor::Mesh* const actor );
	void AddMesh( types::mesh::Mesh* const mesh );
	void Clear();

protected:
	void UpdateImpl() override;

private:

	types::mesh::Mesh* m_mesh = nullptr;
	scene::actor::Mesh* m_actor = nullptr;
	types::Vec2< size_t > m_tile_dimensions = {};

	std::vector< scene::actor::Mesh* > m_actors = {};
	std::vector< types::mesh::Mesh* > m_meshes = {};

	std::unordered_map< const types::mesh::Render*, types::mesh::Render* > m_render_mesh_originals = {};
	std::unordered_map< const types::mesh::Render*, area_t > m_render_mesh_original_areas = {};

	void InitMesh( types::mesh::Mesh* const mesh );

	void UpdateActor( scene::actor::Mesh* const actor );
	void UpdateMesh( types::mesh::Mesh* const mesh );

};

}
}
