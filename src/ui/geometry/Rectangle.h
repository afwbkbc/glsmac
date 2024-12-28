#pragma once

#include "Geometry.h"

namespace types::mesh {
class Rectangle;
}

namespace scene::actor {
class Mesh;
}

namespace ui {
namespace geometry {

class Rectangle : public Geometry {
public:
	Rectangle( const UI* const ui, Geometry* const parent );

	void SetActorAndMesh( scene::actor::Mesh* const actor, types::mesh::Rectangle* const mesh );

protected:
	void UpdateImpl() override;

private:

	types::mesh::Rectangle* m_mesh = nullptr;
	scene::actor::Mesh* m_actor = nullptr;

};

}
}
