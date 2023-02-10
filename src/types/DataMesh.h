#pragma once

#include "Mesh.h"

#include <vector>

#include "Vec3.h"
#include "Color.h"

namespace types {

CLASS( DataMesh, Mesh )
	
	static const uint8_t VERTEX_DATA_SIZE = 1; // data value
	static const uint8_t VERTEX_SIZE = Mesh::VERTEX_COORD_SIZE + VERTEX_DATA_SIZE;
	static const uint8_t SURFACE_SIZE = 3; // triangles

	typedef uint32_t data_t;
	
	DataMesh( const size_t vertex_count, const size_t surface_count );
	
	Mesh::index_t AddVertex( const Vec3 &coord, const data_t data );
	void SetVertex( const Mesh::index_t index, const Vec3 &coord, const data_t data );
	
	void SetVertexData( const Mesh::index_t index, const data_t data );

};

}
