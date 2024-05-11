#pragma once

#include "Mesh.h"

namespace types {
namespace mesh {

CLASS( Data, Mesh )

	static constexpr uint8_t VERTEX_DATA_SIZE = 1; // data value
	static constexpr uint8_t VERTEX_SIZE = VERTEX_COORD_SIZE + VERTEX_DATA_SIZE;

	Data( const size_t vertex_count, const size_t surface_count );

	index_t AddVertex( const types::Vec3& coord, const data_t data );
	void SetVertex( const index_t index, const types::Vec3& coord, const data_t data );

	void SetVertexData( const index_t index, const data_t data );

};

}
}
