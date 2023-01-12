#pragma once

#include <vector>
#include <cstddef>

#include "types/Vec3.h"

namespace procedural {

class Vec3MeshVertex;

class Vec3MeshVertex {
public:
	Vec3MeshVertex( types::Vec3 position );
	void ConnectToVertex( Vec3MeshVertex *vertex );

	types::Vec3 m_position = { 0.0, 0.0, 0.0 };
	size_t m_index = 0;
	std::vector<Vec3MeshVertex *> m_out = {};
};

} /* namespace procedural */
