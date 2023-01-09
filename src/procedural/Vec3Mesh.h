#pragma once

#include <vector>

#include "Mesh.h"
#include "Vec3MeshVertex.h"
#include "scene/mesh/Vec3Mesh.h"

namespace procedural {

class Vec3Mesh : public Mesh {
public:
	~Vec3Mesh();
	void Clear();
	virtual scene::mesh::Vec3Mesh *Generate();
	Vec3MeshVertex *AddVertex( Vec3MeshVertex *vertex );
protected:
	std::vector<Vec3MeshVertex *> m_vertices;
};

} /* namespace procedural */
