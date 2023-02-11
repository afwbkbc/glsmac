#pragma once

#include <vector>

#include "Mesh.h"

using namespace std;

namespace types {
namespace mesh {

CLASS( Data, Mesh )
	
	static const uint8_t VERTEX_DATA_SIZE = 1; // data value
	static const uint8_t VERTEX_SIZE = VERTEX_COORD_SIZE + VERTEX_DATA_SIZE;

	typedef uint32_t data_t;
	
	Data( const size_t vertex_count, const size_t surface_count );
	
	Mesh::index_t AddVertex( const Vec3 &coord, const data_t data );
	void SetVertex( const Mesh::index_t index, const Vec3 &coord, const data_t data );
	
	void SetVertexData( const Mesh::index_t index, const data_t data );

};

}
}
